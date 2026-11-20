/* ====================================================================
 *  MENU: usuario y clave, menus, pantallas de configuracion y timbre
 *  Navegacion con 4 botones: UP, DOWN, SELECT, BACK
 *  Cada caja del mapa del sistema es un estado.
 *  Lo que se edita se cambia directamente en los datos (BACK solo sale).
 * ====================================================================
 */

#include "MENU.h"
#include "../LCD/LCD_LIB.h"
#include "../DS18B20_LIB/DS18B20_LIB.h"
#include "../BOTONES/BOTONES.h"
#include "../RELOJ/RELOJ.h"
#include "../BUZZER/BUZZER.h"

// ---------- Estados (cada caja del mapa del sistema) ----------
#define ESTADO_PRINCIPAL         0
#define ESTADO_CLAVE             1
#define ESTADO_MENU              2
#define ESTADO_HORARIOS          3
#define ESTADO_DESCANSOS         4
#define ESTADO_JORNADA           5
#define ESTADO_FECHA_HORA        6
#define ESTADO_EDIT_HORA         7    // configurar HORA X (horarios normales)
#define ESTADO_EDIT_DESCANSO     8    // configurar DESCANSO X
#define ESTADO_EDIT_FECHA        9    // fecha del reloj
#define ESTADO_EDIT_HORA_RELOJ   10   // hora del reloj
#define ESTADO_JORNADA_HORARIOS  11   // lista de horarios especiales
#define ESTADO_EDIT_HORA_ESP     12   // configurar HORA X de la jornada especial
#define ESTADO_EDIT_FECHA_ESP    13   // fecha de la jornada especial
#define ESTADO_USUARIO           14   // escribir el usuario (4 letras)
#define ESTADO_GUARDANDO         15   // mensaje "GUARDANDO..."
#define ESTADO_CLAVE_MAL         16   // mensaje "DATOS INCORRECTOS"

// ---------- Opciones del menu de configuracion ----------
#define OPC_HORARIOS    0
#define OPC_DESCANSOS   1
#define OPC_JORNADA     2
#define OPC_FECHA_HORA  3

// ---------- Ultima opcion de cada lista (la primera es 0) ----------
#define ULTIMA_MENU        3
#define ULTIMA_HORARIOS    5      // HORA 1 a HORA 6
#define ULTIMA_DESCANSOS   2
#define ULTIMA_JORNADA     1
#define ULTIMA_FECHA_HORA  1

// ---------- Los mensajes duran 150 vueltas x 10 ms = 1.5 s ----------
#define VUELTAS_MENSAJE  150

// ---------- Sonidos del timbre: cuantos pitidos y cuanto dura cada uno (en vueltas de 10 ms) ----------
#define HORA_PITIDOS      3      // cambio de hora
#define HORA_LARGO        50     // 0.5 s
#define DESC_INI_PITIDOS  1      // empieza un descanso
#define DESC_INI_LARGO    150    // 1.5 s
#define DESC_FIN_PITIDOS  2      // termina un descanso
#define DESC_FIN_LARGO    60     // 0.6 s
#define ERROR_PITIDOS     3      // usuario o clave incorrectos
#define ERROR_LARGO       10     // 0.1 s

// ---------- Datos que se configuran (valores iniciales de prueba) ----------
char     usuario[4]     = {'A', 'D', 'M', 'N'};   // usuario de prueba: ADMN
uint8_t  clave[4]       = {1, 2, 3, 4};

uint8_t  horario_h[6]   = {7, 8, 9, 10, 11, 12};
uint8_t  horario_m[6]   = {0, 0, 0, 0, 0, 0};
uint8_t  horario_on[6]  = {1, 1, 1, 1, 1, 1};     // 1 = suena, 0 = anulada

uint8_t  descanso_ih[3] = {8, 10, 12};
uint8_t  descanso_im[3] = {50, 40, 30};
uint8_t  descanso_fh[3] = {9, 10, 12};
uint8_t  descanso_fm[3] = {5, 55, 45};
uint8_t  descanso_on[3] = {1, 1, 1};              // 1 = suena, 0 = anulado

uint8_t  esp_dia        = 25;
uint8_t  esp_mes        = 9;
uint16_t esp_anio       = 2026;
uint8_t  especial_h[6]  = {7, 7, 8, 9, 10, 11};
uint8_t  especial_m[6]  = {0, 45, 30, 15, 0, 0};
uint8_t  especial_on[6] = {1, 1, 1, 1, 1, 1};     // 1 = suena, 0 = anulado

// ---------- Variables del menu ----------
uint8_t  estado     = ESTADO_PRINCIPAL;
uint8_t  seleccion  = 0;       // opcion resaltada en la lista actual
uint8_t  tecla      = BTN_NINGUNO;
uint8_t  redibujar  = 1;       // 1 = hay que volver a dibujar la pantalla

char     letra[4]   = {'A', 'A', 'A', 'A'};   // usuario que se esta escribiendo
uint8_t  digito[4]  = {0, 0, 0, 0};           // clave que se esta escribiendo
uint8_t  pos        = 0;                      // en cual letra o digito va (0 a 3)

uint8_t  indice     = 0;       // cual HORA o DESCANSO se esta editando
uint8_t  campo      = 0;       // cual campo se esta editando (hora, minutos...)

uint8_t  espera           = 0;                   // vueltas que le faltan al mensaje en pantalla
uint8_t  estado_siguiente = ESTADO_PRINCIPAL;    // a donde se va cuando termina el mensaje

int16_t  temp_decimas = 0;     // temperatura x 10  (26.0 C -> 260)
uint8_t  hay_temp     = 0;     // 0 = aun no hay medida, 1 = medida buena, 2 = no responde el sensor


// ---------- Funciones auxiliares ----------

void borrar_usuario(void){
	pos = 0;
	for (uint8_t i = 0; i < 4; i++) {
		letra[i] = 'A';
	}
}

void borrar_clave(void){
	pos = 0;
	for (uint8_t i = 0; i < 4; i++) {
		digito[i] = 0;
	}
}

// Mueve la seleccion con UP y DOWN. "ultima" es la ultima opcion de la lista.
void mover_seleccion(uint8_t ultima){
	if (tecla == BTN_UP && seleccion > 0) {
		seleccion--;
	}
	if (tecla == BTN_DOWN && seleccion < ultima) {
		seleccion++;
	}
}

// Pone un mensaje en pantalla por 1.5 s (sin detener el programa) y despues pasa al estado "siguiente"
void poner_mensaje(uint8_t mensaje, uint8_t siguiente){
	estado           = mensaje;
	estado_siguiente = siguiente;
	espera           = VUELTAS_MENSAJE;
}

// Cambia SI por NO (o NO por SI)
uint8_t cambiar_activa(uint8_t valor){
	if (valor == 1) {
		return 0;
	}
	return 1;
}

// Cambia un dato de la fecha de la jornada especial (campo 0 = dia, 1 = mes, 2 = anio)
void ajustar_fecha_esp(uint8_t arriba){

	if (campo == 0) {
		if (arriba) {
			if (esp_dia >= dias_del_mes(esp_mes, esp_anio)) esp_dia = 1;
			else                                            esp_dia++;
		} else {
			if (esp_dia <= 1) esp_dia = dias_del_mes(esp_mes, esp_anio);
			else              esp_dia--;
		}
	}
	if (campo == 1) {
		if (arriba) {
			if (esp_mes == 12) esp_mes = 1;
			else               esp_mes++;
		} else {
			if (esp_mes <= 1) esp_mes = 12;
			else              esp_mes--;
		}
	}
	if (campo == 2) {
		if (arriba) {
			if (esp_anio >= 2099) esp_anio = 2000;
			else                  esp_anio++;
		} else {
			if (esp_anio <= 2000) esp_anio = 2099;
			else                  esp_anio--;
		}
	}
	// si el dia no existe en ese mes (ej. 31 de abril), baja al ultimo dia
	if (esp_dia > dias_del_mes(esp_mes, esp_anio)) {
		esp_dia = dias_del_mes(esp_mes, esp_anio);
	}
}


// ---------- Cada segundo: temperatura y timbre ----------

// Lee la medida que se pidio hace 1 s y pide la siguiente (el sensor tarda ~750 ms en medir)
void actualizar_temperatura(void){
	float temp;

	temp = ds18b20_leer_temp();
	ds18b20_iniciar_conv();

	if (temp < -900) {                  // el sensor no respondio
		hay_temp = 2;
	} else {
		temp_decimas = temp * 10;
		hay_temp = 1;
	}
}

// Hace sonar el buzzer cuando la hora actual coincide con un horario o un descanso que este activo.
// Solo se revisa en el segundo 0 de cada minuto, para que suene una vez.
// Si hoy es la fecha de la jornada especial, se usan los horarios especiales.
void revisar_timbre(void){
	uint8_t i;
	uint8_t h = reloj_hora();
	uint8_t m = reloj_minuto();
	uint8_t es_especial = 0;

	if (reloj_segundo() != 0) {
		return;
	}

	if (reloj_dia() == esp_dia && reloj_mes() == esp_mes && reloj_anio() == esp_anio) {
		es_especial = 1;
	}

	// ----- HORA 1 a HORA 6 (3 pitidos) -----
	for (i = 0; i < 6; i++) {
		if (es_especial == 1) {
			if (especial_on[i] == 1 && especial_h[i] == h && especial_m[i] == m) {
				buzzer_pitar(HORA_PITIDOS, HORA_LARGO);
				return;
			}
		} else {
			if (horario_on[i] == 1 && horario_h[i] == h && horario_m[i] == m) {
				buzzer_pitar(HORA_PITIDOS, HORA_LARGO);
				return;
			}
		}
	}

	// ----- DESCANSO 1 a 3: inicio (1 pitido largo) y fin (2 pitidos) -----
	for (i = 0; i < 3; i++) {
		if (descanso_on[i] == 0) {           // descanso anulado: no suena
			continue;
		}
		if (descanso_ih[i] == h && descanso_im[i] == m) {
			buzzer_pitar(DESC_INI_PITIDOS, DESC_INI_LARGO);
			return;
		}
		if (descanso_fh[i] == h && descanso_fm[i] == m) {
			buzzer_pitar(DESC_FIN_PITIDOS, DESC_FIN_LARGO);
			return;
		}
	}
}


// ---------- Funciones para dibujar ----------

// Escribe ">" si la opcion esta seleccionada, o un espacio si no
void marcador(uint8_t fila, uint8_t opcion){
	lcd_set_cursor(fila, 1);
	if (seleccion == opcion) {
		lcd_write('>');
	} else {
		lcd_write(' ');
	}
}

// Escribe SI o NO segun el valor (1 = SI, 0 = NO)
void escribir_si_no(uint8_t valor){
	if (valor == 1) {
		lcd_puts("SI");
	} else {
		lcd_puts("NO");
	}
}

// Una fila de la lista de horarios:  >HORA 1  07:00      SI
// (sirve para los horarios normales y para los especiales)
void fila_horario(uint8_t fila, uint8_t i){
	uint8_t h, m, on;
	if (estado == ESTADO_HORARIOS) {
		h  = horario_h[i];
		m  = horario_m[i];
		on = horario_on[i];
	} else {
		h  = especial_h[i];
		m  = especial_m[i];
		on = especial_on[i];
	}
	marcador(fila, i);
	lcd_printf("HORA %d  %02d:%02d", i + 1, h, m);
	lcd_set_cursor(fila, 18);
	escribir_si_no(on);
}

// Una fila de la lista de descansos:  >DESCANSO 1        SI
void fila_descanso(uint8_t fila, uint8_t i){
	marcador(fila, i);
	lcd_printf("DESCANSO %d", i + 1);
	lcd_set_cursor(fila, 18);
	escribir_si_no(descanso_on[i]);
}

void mostrar_hora_fecha(void){
	lcd_set_cursor(1, 1);
	lcd_printf("%02d:%02d:%02d  %02d/%02d/%04d", reloj_hora(), reloj_minuto(), reloj_segundo(),
	           reloj_dia(), reloj_mes(), reloj_anio());
}

void mostrar_temperatura(void){
	int16_t decimas = temp_decimas;
	char    signo   = ' ';

	lcd_set_cursor(3, 1);

	if (hay_temp == 0) {
		lcd_puts("Temp: --.- C        ");
	} else if (hay_temp == 2) {
		lcd_puts("Temp: sin sensor    ");
	} else {
		if (decimas < 0) {
			signo = '-';
			decimas = -decimas;
		}
		lcd_printf("Temp: %c%d.%d C        ", signo, decimas / 10, decimas % 10);
	}
}

void dibujar_principal(void){
	lcd_clear();
	mostrar_hora_fecha();
	mostrar_temperatura();
	lcd_set_cursor(4, 1);
	lcd_puts("SELECT: menu");
}

// Las letras ya escritas se ven, la actual tambien y las que faltan salen como "_"
void dibujar_usuario(void){
	lcd_clear();
	lcd_set_cursor(1, 1);
	lcd_puts("INGRESE USUARIO:");
	lcd_set_cursor(2, 1);
	for (uint8_t i = 0; i < 4; i++) {
		if (i <= pos) {
			lcd_write(letra[i]);
		} else {
			lcd_write('_');
		}
		lcd_write(' ');
	}
	lcd_set_cursor(3, 1 + 2 * pos);   // "^" debajo de la letra actual
	lcd_write('^');
	lcd_set_cursor(4, 1);
	lcd_puts("SELECT: siguiente");
}

// Los digitos ya escritos salen como "*", el actual con su numero, los demas "_"
void dibujar_clave(void){
	lcd_clear();
	lcd_set_cursor(1, 1);
	lcd_puts("INGRESE CLAVE:");
	lcd_set_cursor(2, 1);
	for (uint8_t i = 0; i < 4; i++) {
		if (i < pos) {
			lcd_write('*');
		} else if (i == pos) {
			lcd_printf("%d", digito[i]);
		} else {
			lcd_write('_');
		}
		lcd_write(' ');
	}
	lcd_set_cursor(3, 1 + 2 * pos);   // "^" debajo del digito actual
	lcd_write('^');
	lcd_set_cursor(4, 1);
	lcd_printf("Usuario: %c%c%c%c", letra[0], letra[1], letra[2], letra[3]);
}

void dibujar_clave_mal(void){
	lcd_clear();
	lcd_set_cursor(1, 1);
	lcd_puts("DATOS INCORRECTOS");
	lcd_set_cursor(2, 1);
	lcd_puts("Intente de nuevo...");
}

void dibujar_guardando(void){
	lcd_clear();
	lcd_set_cursor(2, 1);
	lcd_puts("GUARDANDO...");
}

void dibujar_menu(void){
	lcd_clear();
	marcador(1, 0);  lcd_puts("HORARIOS");
	marcador(2, 1);  lcd_puts("DESCANSOS");
	marcador(3, 2);  lcd_puts("JORNADA ESPECIAL");
	marcador(4, 3);  lcd_puts("FECHA / HORA");
}

// Lista HORA 1 a HORA 6 (sirve para los horarios normales y los especiales).
// La LCD tiene 4 filas: cuando la seleccion pasa de HORA 4, la lista se va corriendo.
void dibujar_horarios(void){
	uint8_t primero = 0;                 // numero de la primera HORA que se ve (0 = HORA 1)

	if (seleccion > 3) {
		primero = seleccion - 3;
	}
	lcd_clear();
	for (uint8_t k = 0; k < 4; k++) {
		fila_horario(k + 1, primero + k);
	}
}

void dibujar_descansos(void){
	lcd_clear();
	fila_descanso(1, 0);
	fila_descanso(2, 1);
	fila_descanso(3, 2);
}

void dibujar_jornada(void){
	lcd_clear();
	marcador(1, 0);  lcd_puts("FECHA");
	marcador(2, 1);  lcd_puts("HORARIOS ESPECIALES");
}

void dibujar_fecha_hora(void){
	lcd_clear();
	marcador(1, 0);  lcd_puts("FECHA");
	marcador(2, 1);  lcd_puts("HORA");
}

// Configurar HORA X (normal o de la jornada especial):   HORA: 10:30 ACT: SI
void dibujar_edit_hora(void){
	uint8_t h;
	uint8_t m;
	uint8_t on;

	lcd_clear();
	lcd_set_cursor(1, 1);
	if (estado == ESTADO_EDIT_HORA) {
		lcd_printf("CONFIGURAR HORA %d", indice + 1);
		h  = horario_h[indice];
		m  = horario_m[indice];
		on = horario_on[indice];
	} else {
		lcd_printf("HORA ESPECIAL %d", indice + 1);
		h  = especial_h[indice];
		m  = especial_m[indice];
		on = especial_on[indice];
	}
	lcd_set_cursor(2, 1);
	lcd_printf("HORA: %02d:%02d ACT: ", h, m);
	escribir_si_no(on);

	if (campo == 0) {
		lcd_set_cursor(3, 7);          // "^" debajo del campo que se edita
		lcd_write('^');
		lcd_set_cursor(4, 1);
		lcd_puts("SELECT: minutos");
	} else if (campo == 1) {
		lcd_set_cursor(3, 10);
		lcd_write('^');
		lcd_set_cursor(4, 1);
		lcd_puts("SELECT: activa");
	} else {
		lcd_set_cursor(3, 18);
		lcd_write('^');
		lcd_set_cursor(4, 1);
		lcd_puts("SELECT: guardar");
	}
}

// Configurar DESCANSO X:   INICIO: 10:40   FIN: 10:55
void dibujar_edit_descanso(void){
	lcd_clear();
	lcd_set_cursor(1, 1);
	lcd_printf("DESCANSO %d  ACT: ", indice + 1);
	escribir_si_no(descanso_on[indice]);
	lcd_set_cursor(2, 1);
	lcd_printf("INICIO: %02d:%02d", descanso_ih[indice], descanso_im[indice]);
	lcd_set_cursor(3, 1);
	lcd_printf("FIN:    %02d:%02d", descanso_fh[indice], descanso_fm[indice]);

	lcd_set_cursor(4, 1);
	if      (campo == 0) lcd_puts("Campo: hora inicio");
	else if (campo == 1) lcd_puts("Campo: min inicio");
	else if (campo == 2) lcd_puts("Campo: hora fin");
	else if (campo == 3) lcd_puts("Campo: min fin");
	else                 lcd_puts("Campo: activa");
}

// Fecha del reloj o de la jornada especial:   FECHA: 22/09/2026
void dibujar_edit_fecha(void){
	lcd_clear();
	lcd_set_cursor(1, 1);
	if (estado == ESTADO_EDIT_FECHA) {
		lcd_puts("CONFIGURAR FECHA");
		lcd_set_cursor(2, 1);
		lcd_printf("FECHA: %02d/%02d/%04d", reloj_dia(), reloj_mes(), reloj_anio());
	} else {
		lcd_puts("JORNADA ESPECIAL");
		lcd_set_cursor(2, 1);
		lcd_printf("FECHA: %02d/%02d/%04d", esp_dia, esp_mes, esp_anio);
	}

	if (campo == 0) {
		lcd_set_cursor(3, 8);          // "^" debajo del campo que se edita
		lcd_write('^');
		lcd_set_cursor(4, 1);
		lcd_puts("SELECT: mes");
	} else if (campo == 1) {
		lcd_set_cursor(3, 11);
		lcd_write('^');
		lcd_set_cursor(4, 1);
		lcd_puts("SELECT: anio");
	} else {
		lcd_set_cursor(3, 14);
		lcd_write('^');
		lcd_set_cursor(4, 1);
		lcd_puts("SELECT: guardar");
	}
}

// Hora del reloj:   HORA: 10:35:42
void dibujar_edit_hora_reloj(void){
	lcd_clear();
	lcd_set_cursor(1, 1);
	lcd_puts("CONFIGURAR HORA");
	lcd_set_cursor(2, 1);
	lcd_printf("HORA: %02d:%02d:%02d", reloj_hora(), reloj_minuto(), reloj_segundo());

	if (campo == 0) {
		lcd_set_cursor(3, 7);
		lcd_write('^');
		lcd_set_cursor(4, 1);
		lcd_puts("SELECT: minutos");
	} else if (campo == 1) {
		lcd_set_cursor(3, 10);
		lcd_write('^');
		lcd_set_cursor(4, 1);
		lcd_puts("SELECT: segundos");
	} else {
		lcd_set_cursor(3, 13);
		lcd_write('^');
		lcd_set_cursor(4, 1);
		lcd_puts("SELECT: guardar");
	}
}

// Dibuja la pantalla que corresponde al estado actual
void dibujar_pantalla(void){
	switch (estado) {
		case ESTADO_PRINCIPAL:         dibujar_principal();       break;
		case ESTADO_USUARIO:           dibujar_usuario();         break;
		case ESTADO_CLAVE:             dibujar_clave();           break;
		case ESTADO_CLAVE_MAL:         dibujar_clave_mal();       break;
		case ESTADO_GUARDANDO:         dibujar_guardando();       break;
		case ESTADO_MENU:              dibujar_menu();            break;
		case ESTADO_HORARIOS:          dibujar_horarios();        break;
		case ESTADO_JORNADA_HORARIOS:  dibujar_horarios();        break;
		case ESTADO_DESCANSOS:         dibujar_descansos();       break;
		case ESTADO_JORNADA:           dibujar_jornada();         break;
		case ESTADO_FECHA_HORA:        dibujar_fecha_hora();      break;
		case ESTADO_EDIT_HORA:         dibujar_edit_hora();       break;
		case ESTADO_EDIT_HORA_ESP:     dibujar_edit_hora();       break;
		case ESTADO_EDIT_DESCANSO:     dibujar_edit_descanso();   break;
		case ESTADO_EDIT_FECHA:        dibujar_edit_fecha();      break;
		case ESTADO_EDIT_FECHA_ESP:    dibujar_edit_fecha();      break;
		case ESTADO_EDIT_HORA_RELOJ:   dibujar_edit_hora_reloj(); break;
	}
}


// ---------- Programa del menu: se llama una vez por vuelta del while(1) ----------
void menu_actualizar(uint8_t boton, uint8_t paso_un_segundo)
{
	tecla = boton;

	// El reloj se detiene mientras se edita su hora (para no perseguir un numero que cambia)
	if (estado == ESTADO_EDIT_HORA_RELOJ) {
		reloj_pausar(1);
	} else {
		reloj_pausar(0);
	}

	// ---------- Cada segundo: se lee la temperatura y se revisa el timbre ----------
	if (paso_un_segundo == 1) {
		actualizar_temperatura();
		if (reloj_pausado() == 0) {
			revisar_timbre();
		}
	}

	// ---------- Mensajes de 1.5 s: los botones no hacen nada mientras se muestran ----------
	if (estado == ESTADO_GUARDANDO || estado == ESTADO_CLAVE_MAL) {
		tecla = BTN_NINGUNO;
		espera--;
		if (espera == 0) {
			estado    = estado_siguiente;
			redibujar = 1;
		}
	}

	// ---------- Que pasa segun el estado y el boton ----------
	switch (estado) {

		case ESTADO_PRINCIPAL:
			if (tecla == BTN_SELECT) {
				borrar_usuario();
				borrar_clave();
				estado = ESTADO_USUARIO;
			}
			break;

		// UP / DOWN cambian la letra (A a Z), SELECT pasa a la siguiente.
		// En la ultima letra, SELECT pasa a pedir la clave.
		case ESTADO_USUARIO:
			if (tecla == BTN_UP) {
				if (letra[pos] == 'Z') letra[pos] = 'A';
				else                   letra[pos]++;
			}
			if (tecla == BTN_DOWN) {
				if (letra[pos] == 'A') letra[pos] = 'Z';
				else                   letra[pos]--;
			}
			if (tecla == BTN_SELECT) {
				if (pos < 3) {
					pos++;
				} else {
					pos = 0;
					estado = ESTADO_CLAVE;
				}
			}
			if (tecla == BTN_BACK) {
				estado = ESTADO_PRINCIPAL;
			}
			break;

		// UP / DOWN cambian el digito, SELECT pasa al siguiente.
		// En el ultimo digito, SELECT revisa el usuario y la clave.
		case ESTADO_CLAVE:
			if (tecla == BTN_UP) {
				digito[pos] = subir(digito[pos], 9);
			}
			if (tecla == BTN_DOWN) {
				digito[pos] = bajar(digito[pos], 9);
			}
			if (tecla == BTN_SELECT) {
				if (pos < 3) {
					pos++;
				} else {
					if (letra[0] == usuario[0] && letra[1] == usuario[1] &&
					    letra[2] == usuario[2] && letra[3] == usuario[3] &&
					    digito[0] == clave[0] && digito[1] == clave[1] &&
					    digito[2] == clave[2] && digito[3] == clave[3]) {
						estado = ESTADO_MENU;
						seleccion = 0;
					} else {
						borrar_usuario();
						borrar_clave();
						buzzer_pitar(ERROR_PITIDOS, ERROR_LARGO);
						poner_mensaje(ESTADO_CLAVE_MAL, ESTADO_USUARIO);   // vuelve a pedir el usuario
					}
				}
			}
			if (tecla == BTN_BACK) {
				estado = ESTADO_PRINCIPAL;
			}
			break;

		case ESTADO_MENU:
			mover_seleccion(ULTIMA_MENU);
			if (tecla == BTN_SELECT) {
				if (seleccion == OPC_HORARIOS)   estado = ESTADO_HORARIOS;
				if (seleccion == OPC_DESCANSOS)  estado = ESTADO_DESCANSOS;
				if (seleccion == OPC_JORNADA)    estado = ESTADO_JORNADA;
				if (seleccion == OPC_FECHA_HORA) estado = ESTADO_FECHA_HORA;
				seleccion = 0;
			}
			if (tecla == BTN_BACK) {
				estado = ESTADO_PRINCIPAL;
			}
			break;

		// ----- HORARIOS: lista de HORA 1 a HORA 6 -----
		case ESTADO_HORARIOS:
			mover_seleccion(ULTIMA_HORARIOS);
			if (tecla == BTN_SELECT) {
				indice = seleccion;
				campo  = 0;
				estado = ESTADO_EDIT_HORA;
			}
			if (tecla == BTN_BACK) {
				estado = ESTADO_MENU;
				seleccion = OPC_HORARIOS;
			}
			break;

		// ----- DESCANSOS: lista de DESCANSO 1 a 3 -----
		case ESTADO_DESCANSOS:
			mover_seleccion(ULTIMA_DESCANSOS);
			if (tecla == BTN_SELECT) {
				indice = seleccion;
				campo  = 0;
				estado = ESTADO_EDIT_DESCANSO;
			}
			if (tecla == BTN_BACK) {
				estado = ESTADO_MENU;
				seleccion = OPC_DESCANSOS;
			}
			break;

		// ----- JORNADA ESPECIAL: FECHA / HORARIOS ESPECIALES -----
		case ESTADO_JORNADA:
			mover_seleccion(ULTIMA_JORNADA);
			if (tecla == BTN_SELECT) {
				if (seleccion == 0) {                  // FECHA
					campo  = 0;
					estado = ESTADO_EDIT_FECHA_ESP;
				} else {                               // HORARIOS ESPECIALES
					estado = ESTADO_JORNADA_HORARIOS;
					seleccion = 0;
				}
			}
			if (tecla == BTN_BACK) {
				estado = ESTADO_MENU;
				seleccion = OPC_JORNADA;
			}
			break;

		// Lista de horarios especiales (se configuran igual que HORARIOS)
		case ESTADO_JORNADA_HORARIOS:
			mover_seleccion(ULTIMA_HORARIOS);
			if (tecla == BTN_SELECT) {
				indice = seleccion;
				campo  = 0;
				estado = ESTADO_EDIT_HORA_ESP;
			}
			if (tecla == BTN_BACK) {
				estado = ESTADO_JORNADA;
				seleccion = 1;
			}
			break;

		// ----- FECHA / HORA: del reloj -----
		case ESTADO_FECHA_HORA:
			mover_seleccion(ULTIMA_FECHA_HORA);
			if (tecla == BTN_SELECT) {
				campo = 0;
				if (seleccion == 0) {                  // FECHA
					estado = ESTADO_EDIT_FECHA;
				} else {                               // HORA
					estado = ESTADO_EDIT_HORA_RELOJ;
				}
			}
			if (tecla == BTN_BACK) {
				estado = ESTADO_MENU;
				seleccion = OPC_FECHA_HORA;
			}
			break;

		// ----- Configurar HORA X: campo 0 = horas, 1 = minutos, 2 = activa (SI / NO) -----
		case ESTADO_EDIT_HORA:
			if (tecla == BTN_UP) {
				if (campo == 0) horario_h[indice]  = subir(horario_h[indice], 23);
				if (campo == 1) horario_m[indice]  = subir(horario_m[indice], 59);
				if (campo == 2) horario_on[indice] = cambiar_activa(horario_on[indice]);
			}
			if (tecla == BTN_DOWN) {
				if (campo == 0) horario_h[indice]  = bajar(horario_h[indice], 23);
				if (campo == 1) horario_m[indice]  = bajar(horario_m[indice], 59);
				if (campo == 2) horario_on[indice] = cambiar_activa(horario_on[indice]);
			}
			if (tecla == BTN_SELECT) {
				if (campo < 2) {
					campo++;
				} else {
					seleccion = indice;
					poner_mensaje(ESTADO_GUARDANDO, ESTADO_HORARIOS);
				}
			}
			if (tecla == BTN_BACK) {
				estado = ESTADO_HORARIOS;
				seleccion = indice;
			}
			break;

		// ----- Configurar HORA X de la jornada especial (igual que HORA X) -----
		case ESTADO_EDIT_HORA_ESP:
			if (tecla == BTN_UP) {
				if (campo == 0) especial_h[indice]  = subir(especial_h[indice], 23);
				if (campo == 1) especial_m[indice]  = subir(especial_m[indice], 59);
				if (campo == 2) especial_on[indice] = cambiar_activa(especial_on[indice]);
			}
			if (tecla == BTN_DOWN) {
				if (campo == 0) especial_h[indice]  = bajar(especial_h[indice], 23);
				if (campo == 1) especial_m[indice]  = bajar(especial_m[indice], 59);
				if (campo == 2) especial_on[indice] = cambiar_activa(especial_on[indice]);
			}
			if (tecla == BTN_SELECT) {
				if (campo < 2) {
					campo++;
				} else {
					seleccion = indice;
					poner_mensaje(ESTADO_GUARDANDO, ESTADO_JORNADA_HORARIOS);
				}
			}
			if (tecla == BTN_BACK) {
				estado = ESTADO_JORNADA_HORARIOS;
				seleccion = indice;
			}
			break;

		// ----- Configurar DESCANSO X: hora inicio, min inicio, hora fin, min fin, activa -----
		case ESTADO_EDIT_DESCANSO:
			if (tecla == BTN_UP) {
				if (campo == 0) descanso_ih[indice] = subir(descanso_ih[indice], 23);
				if (campo == 1) descanso_im[indice] = subir(descanso_im[indice], 59);
				if (campo == 2) descanso_fh[indice] = subir(descanso_fh[indice], 23);
				if (campo == 3) descanso_fm[indice] = subir(descanso_fm[indice], 59);
				if (campo == 4) descanso_on[indice] = cambiar_activa(descanso_on[indice]);
			}
			if (tecla == BTN_DOWN) {
				if (campo == 0) descanso_ih[indice] = bajar(descanso_ih[indice], 23);
				if (campo == 1) descanso_im[indice] = bajar(descanso_im[indice], 59);
				if (campo == 2) descanso_fh[indice] = bajar(descanso_fh[indice], 23);
				if (campo == 3) descanso_fm[indice] = bajar(descanso_fm[indice], 59);
				if (campo == 4) descanso_on[indice] = cambiar_activa(descanso_on[indice]);
			}
			if (tecla == BTN_SELECT) {
				if (campo < 4) {
					campo++;
				} else {
					seleccion = indice;
					poner_mensaje(ESTADO_GUARDANDO, ESTADO_DESCANSOS);
				}
			}
			if (tecla == BTN_BACK) {
				estado = ESTADO_DESCANSOS;
				seleccion = indice;
			}
			break;

		// ----- Configurar FECHA del reloj: campo 0 = dia, 1 = mes, 2 = anio (2000 a 2099) -----
		// (en reloj_ajustar el dia es el campo 3, el mes el 4 y el anio el 5)
		case ESTADO_EDIT_FECHA:
			if (tecla == BTN_UP) {
				reloj_ajustar(campo + 3, 1);
			}
			if (tecla == BTN_DOWN) {
				reloj_ajustar(campo + 3, 0);
			}
			if (tecla == BTN_SELECT) {
				if (campo < 2) {
					campo++;
				} else {
					seleccion = 0;
					poner_mensaje(ESTADO_GUARDANDO, ESTADO_FECHA_HORA);
				}
			}
			if (tecla == BTN_BACK) {
				estado = ESTADO_FECHA_HORA;
				seleccion = 0;
			}
			break;

		// ----- Configurar FECHA de la jornada especial -----
		case ESTADO_EDIT_FECHA_ESP:
			if (tecla == BTN_UP) {
				ajustar_fecha_esp(1);
			}
			if (tecla == BTN_DOWN) {
				ajustar_fecha_esp(0);
			}
			if (tecla == BTN_SELECT) {
				if (campo < 2) {
					campo++;
				} else {
					seleccion = 0;
					poner_mensaje(ESTADO_GUARDANDO, ESTADO_JORNADA);
				}
			}
			if (tecla == BTN_BACK) {
				estado = ESTADO_JORNADA;
				seleccion = 0;
			}
			break;

		// ----- Configurar HORA del reloj: horas, minutos, segundos -----
		case ESTADO_EDIT_HORA_RELOJ:
			if (tecla == BTN_UP) {
				reloj_ajustar(campo, 1);
			}
			if (tecla == BTN_DOWN) {
				reloj_ajustar(campo, 0);
			}
			if (tecla == BTN_SELECT) {
				if (campo < 2) {
					campo++;
				} else {
					seleccion = 1;
					poner_mensaje(ESTADO_GUARDANDO, ESTADO_FECHA_HORA);
				}
			}
			if (tecla == BTN_BACK) {
				estado = ESTADO_FECHA_HORA;
				seleccion = 1;
			}
			break;
	}

	// Si se presiono un boton, hay que redibujar la pantalla
	if (tecla != BTN_NINGUNO) {
		redibujar = 1;
	}

	// ---------- Dibujo ----------
	if (redibujar == 1) {
		dibujar_pantalla();
		redibujar = 0;
	}
	else if (estado == ESTADO_PRINCIPAL && paso_un_segundo == 1) {
		mostrar_hora_fecha();
		mostrar_temperatura();
	}
}
