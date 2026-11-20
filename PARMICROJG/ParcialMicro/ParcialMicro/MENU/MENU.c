/* ====================================================================
 *  MENU: clave, menus y pantallas de configuracion
 *  Navegacion con 4 botones: UP, DOWN, SELECT, BACK
 *  Cada caja del mapa del sistema es un estado.
 * ====================================================================
 */

#include "MENU.h"
#include "../LCD/LCD_LIB.h"
#include "../DS18B20_LIB/DS18B20_LIB.h"
#include "../BOTONES/BOTONES.h"
#include "../RELOJ/RELOJ.h"

// ---------- Estados (cada caja del mapa del sistema) ----------
#define ESTADO_PRINCIPAL         0
#define ESTADO_CLAVE             1
#define ESTADO_MENU              2
#define ESTADO_HORARIOS          3
#define ESTADO_DESCANSOS         4
#define ESTADO_JORNADA           5
#define ESTADO_FECHA_HORA        6
#define ESTADO_EDIT_HORA         7    // configurar HORA X (normal o especial)
#define ESTADO_EDIT_DESCANSO     8    // configurar DESCANSO X
#define ESTADO_EDIT_FECHA        9    // fecha del reloj o fecha de la jornada especial
#define ESTADO_EDIT_HORA_RELOJ   10   // hora del reloj
#define ESTADO_JORNADA_HORARIOS  11   // lista de horarios especiales
#define ESTADO_GUARDANDO         12   // mensaje "GUARDANDO..."
#define ESTADO_CLAVE_MAL         13   // mensaje "CLAVE INCORRECTA"

// ---------- Opciones del menu de configuracion ----------
#define OPC_HORARIOS    0
#define OPC_DESCANSOS   1
#define OPC_JORNADA     2
#define OPC_FECHA_HORA  3

// ---------- Ultima opcion de cada lista (la primera es 0) ----------
#define ULTIMA_MENU        3
#define ULTIMA_HORARIOS    4
#define ULTIMA_DESCANSOS   2
#define ULTIMA_JORNADA     1
#define ULTIMA_FECHA_HORA  1

// Cuanto dura un mensaje: cada vuelta del while(1) dura ~10 ms,
// 150 vueltas = 1.5 segundos
#define TIEMPO_MENSAJE  150

// ---------- Datos que se configuran (valores iniciales de prueba) ----------
uint8_t  clave[4]       = {1, 2, 3, 4};

uint8_t  horario_h[5]   = {7, 8, 9, 10, 11};
uint8_t  horario_m[5]   = {0, 0, 0, 0, 0};

uint8_t  descanso_ih[3] = {8, 10, 12};
uint8_t  descanso_im[3] = {50, 40, 30};
uint8_t  descanso_fh[3] = {9, 10, 12};
uint8_t  descanso_fm[3] = {5, 55, 45};

uint8_t  esp_dia        = 25;
uint8_t  esp_mes        = 9;
uint16_t esp_anio       = 2026;
uint8_t  especial_h[5]  = {7, 7, 8, 9, 10};
uint8_t  especial_m[5]  = {0, 45, 30, 15, 0};

// ---------- Variables del menu ----------
uint8_t  estado     = ESTADO_PRINCIPAL;
uint8_t  seleccion  = 0;       // opcion resaltada en la lista actual
uint8_t  tecla      = BTN_NINGUNO;
uint8_t  redibujar  = 1;       // 1 = hay que volver a dibujar la pantalla

uint8_t  digito[4]  = {0, 0, 0, 0};   // clave que se esta escribiendo
uint8_t  pos        = 0;              // en cual digito va (0 a 3)

uint8_t  indice     = 0;       // cual HORA o DESCANSO se esta editando
uint8_t  campo      = 0;       // cual campo se esta editando (hora, minutos...)
uint8_t  especial   = 0;       // 0 = horarios normales, 1 = horarios especiales
uint8_t  modo_fecha = 0;       // 0 = fecha del reloj, 1 = fecha de la jornada especial

// Valores que se estan editando (hasta que se guarden no cambian los de verdad)
uint8_t  v1 = 0, v2 = 0, v3 = 0, v4 = 0;
uint16_t v_anio = 2026;

uint8_t  espera           = 0;                 // vueltas que faltan para quitar el mensaje
uint8_t  estado_siguiente = ESTADO_PRINCIPAL;  // a donde se va cuando termina el mensaje


// ---------- Funciones auxiliares ----------

// Sube un valor. Si pasa del maximo vuelve al minimo.
uint8_t subir(uint8_t valor, uint8_t minimo, uint8_t maximo){
	if (valor >= maximo) {
		return minimo;
	}
	return valor + 1;
}

// Baja un valor. Si pasa del minimo vuelve al maximo.
uint8_t bajar(uint8_t valor, uint8_t minimo, uint8_t maximo){
	if (valor <= minimo) {
		return maximo;
	}
	return valor - 1;
}

// Si el dia no existe en ese mes (ej. 31 de abril), lo baja al ultimo dia del mes
void ajustar_dia(void){
	if (v1 > dias_del_mes(v2, v_anio)) {
		v1 = dias_del_mes(v2, v_anio);
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

// Escribe "^" debajo del campo que se esta editando.
// Al terminar deja el cursor en la fila 4, listo para escribir la ayuda.
void flecha(uint8_t columna){
	lcd_set_cursor(3, columna);
	lcd_write('^');
	lcd_set_cursor(4, 1);
}

void mostrar_hora_fecha(void){
	lcd_set_cursor(1, 1);
	lcd_printf("%02d:%02d:%02d  %02d/%02d/%04d", hora, minuto, segundo, dia, mes, anio);
}

void mostrar_temperatura(void){
	float   temp;
	int16_t decimas;      // temperatura x 10  (26.0 C -> 260)
	char    signo = ' ';

	temp = ds18b20_get_temp();

	decimas = temp * 10;
	if (decimas < 0) {
		signo = '-';
		decimas = -decimas;
	}

	lcd_set_cursor(3, 1);
	lcd_printf("Temp: %c%d.%d C   ", signo, decimas / 10, decimas % 10);
}

void dibujar_principal(void){
	lcd_clear();
	mostrar_hora_fecha();
	mostrar_temperatura();
	lcd_set_cursor(4, 1);
	lcd_puts("SELECT: menu");
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
			lcd_write('0' + digito[i]);
		} else {
			lcd_write('_');
		}
		lcd_write(' ');
	}
	flecha(1 + 2 * pos);
}

void dibujar_clave_mal(void){
	lcd_clear();
	lcd_set_cursor(1, 1);
	lcd_puts("CLAVE INCORRECTA");
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

// Lista HORA 1 a HORA 5 (sirve para los horarios normales y los especiales).
// La LCD tiene 4 filas: si la seleccion llega a HORA 5, la lista se corre una fila.
void dibujar_horarios(void){
	uint8_t inicio = 0;
	if (seleccion > 3) {
		inicio = 1;
	}
	lcd_clear();
	for (uint8_t fila = 0; fila < 4; fila++) {
		marcador(fila + 1, inicio + fila);
		lcd_printf("HORA %d", inicio + fila + 1);
	}
}

void dibujar_descansos(void){
	lcd_clear();
	marcador(1, 0);  lcd_puts("DESCANSO 1");
	marcador(2, 1);  lcd_puts("DESCANSO 2");
	marcador(3, 2);  lcd_puts("DESCANSO 3");
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

// Configurar HORA X:   HORA: 10:30
void dibujar_edit_hora(void){
	lcd_clear();
	lcd_set_cursor(1, 1);
	if (especial == 0) {
		lcd_printf("CONFIGURAR HORA %d", indice + 1);
	} else {
		lcd_printf("HORA ESPECIAL %d", indice + 1);
	}
	lcd_set_cursor(2, 1);
	lcd_printf("HORA: %02d:%02d", v1, v2);

	if (campo == 0) {
		flecha(7);
		lcd_puts("SELECT: minutos");
	} else {
		flecha(10);
		lcd_puts("SELECT: guardar");
	}
}

// Configurar DESCANSO X:   INICIO: 10:40   FIN: 10:55
void dibujar_edit_descanso(void){
	lcd_clear();
	lcd_set_cursor(1, 1);
	lcd_printf("DESCANSO %d", indice + 1);
	lcd_set_cursor(2, 1);
	lcd_printf("INICIO: %02d:%02d", v1, v2);
	lcd_set_cursor(3, 1);
	lcd_printf("FIN:    %02d:%02d", v3, v4);

	lcd_set_cursor(4, 1);
	if      (campo == 0) lcd_puts("Campo: hora inicio");
	else if (campo == 1) lcd_puts("Campo: min inicio");
	else if (campo == 2) lcd_puts("Campo: hora fin");
	else                 lcd_puts("Campo: min fin");
}

// Fecha del reloj o fecha de la jornada especial:   FECHA: 22/09/2026
void dibujar_edit_fecha(void){
	lcd_clear();
	lcd_set_cursor(1, 1);
	if (modo_fecha == 0) {
		lcd_puts("CONFIGURAR FECHA");
	} else {
		lcd_puts("JORNADA ESPECIAL");
	}
	lcd_set_cursor(2, 1);
	lcd_printf("FECHA: %02d/%02d/%04d", v1, v2, v_anio);

	if (campo == 0) {
		flecha(8);
		lcd_puts("SELECT: mes");
	} else if (campo == 1) {
		flecha(11);
		lcd_puts("SELECT: anio");
	} else {
		flecha(14);
		lcd_puts("SELECT: guardar");
	}
}

// Hora del reloj:   HORA: 10:35:42
void dibujar_edit_hora_reloj(void){
	lcd_clear();
	lcd_set_cursor(1, 1);
	lcd_puts("CONFIGURAR HORA");
	lcd_set_cursor(2, 1);
	lcd_printf("HORA: %02d:%02d:%02d", v1, v2, v3);

	if (campo == 0) {
		flecha(7);
		lcd_puts("SELECT: minutos");
	} else if (campo == 1) {
		flecha(10);
		lcd_puts("SELECT: segundos");
	} else {
		flecha(13);
		lcd_puts("SELECT: guardar");
	}
}

// Dibuja la pantalla que corresponde al estado actual
void dibujar_pantalla(void){
	switch (estado) {
		case ESTADO_PRINCIPAL:         dibujar_principal();       break;
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
		case ESTADO_EDIT_DESCANSO:     dibujar_edit_descanso();   break;
		case ESTADO_EDIT_FECHA:        dibujar_edit_fecha();      break;
		case ESTADO_EDIT_HORA_RELOJ:   dibujar_edit_hora_reloj(); break;
	}
}


// ---------- Programa del menu: se llama una vez por vuelta del while(1) ----------
void menu_actualizar(uint8_t boton, uint8_t paso_un_segundo)
{
	tecla = boton;

	// ---------- Que pasa segun el estado y el boton ----------
	switch (estado) {

		case ESTADO_PRINCIPAL:
			if (tecla == BTN_SELECT) {
				borrar_clave();
				estado = ESTADO_CLAVE;
			}
			break;

		// UP / DOWN cambian el digito, SELECT pasa al siguiente.
		// En el ultimo digito, SELECT revisa la clave.
		case ESTADO_CLAVE:
			if (tecla == BTN_UP) {
				digito[pos] = subir(digito[pos], 0, 9);
			}
			if (tecla == BTN_DOWN) {
				digito[pos] = bajar(digito[pos], 0, 9);
			}
			if (tecla == BTN_SELECT) {
				if (pos < 3) {
					pos++;
				} else {
					if (digito[0] == clave[0] && digito[1] == clave[1] &&
					    digito[2] == clave[2] && digito[3] == clave[3]) {
						estado = ESTADO_MENU;
						seleccion = 0;
					} else {
						borrar_clave();
						estado = ESTADO_CLAVE_MAL;
						estado_siguiente = ESTADO_CLAVE;   // vuelve a pedir la clave
						espera = TIEMPO_MENSAJE;
					}
				}
			}
			if (tecla == BTN_BACK) {
				estado = ESTADO_PRINCIPAL;
			}
			break;

		// Mientras sale un mensaje se ignoran los botones
		case ESTADO_CLAVE_MAL:
		case ESTADO_GUARDANDO:
			espera--;
			if (espera == 0) {
				estado = estado_siguiente;
				redibujar = 1;
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

		// ----- HORARIOS: lista de HORA 1 a HORA 5 -----
		case ESTADO_HORARIOS:
			mover_seleccion(ULTIMA_HORARIOS);
			if (tecla == BTN_SELECT) {
				especial = 0;
				indice   = seleccion;
				campo    = 0;
				v1 = horario_h[indice];
				v2 = horario_m[indice];
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
				v1 = descanso_ih[indice];
				v2 = descanso_im[indice];
				v3 = descanso_fh[indice];
				v4 = descanso_fm[indice];
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
					modo_fecha = 1;
					campo  = 0;
					v1     = esp_dia;
					v2     = esp_mes;
					v_anio = esp_anio;
					estado = ESTADO_EDIT_FECHA;
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
				especial = 1;
				indice   = seleccion;
				campo    = 0;
				v1 = especial_h[indice];
				v2 = especial_m[indice];
				estado = ESTADO_EDIT_HORA;
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
					modo_fecha = 0;
					v1     = dia;
					v2     = mes;
					v_anio = anio;
					estado = ESTADO_EDIT_FECHA;
				} else {                               // HORA
					v1 = hora;
					v2 = minuto;
					v3 = segundo;
					estado = ESTADO_EDIT_HORA_RELOJ;
				}
			}
			if (tecla == BTN_BACK) {
				estado = ESTADO_MENU;
				seleccion = OPC_FECHA_HORA;
			}
			break;

		// ----- Configurar HORA X: campo 0 = horas, campo 1 = minutos -----
		case ESTADO_EDIT_HORA:
			if (tecla == BTN_UP) {
				if (campo == 0) v1 = subir(v1, 0, 23);
				else            v2 = subir(v2, 0, 59);
			}
			if (tecla == BTN_DOWN) {
				if (campo == 0) v1 = bajar(v1, 0, 23);
				else            v2 = bajar(v2, 0, 59);
			}
			if (tecla == BTN_SELECT) {
				if (campo == 0) {
					campo = 1;
				} else {
					if (especial == 0) {
						horario_h[indice] = v1;
						horario_m[indice] = v2;
						estado_siguiente = ESTADO_HORARIOS;
					} else {
						especial_h[indice] = v1;
						especial_m[indice] = v2;
						estado_siguiente = ESTADO_JORNADA_HORARIOS;
					}
					seleccion = indice;
					estado = ESTADO_GUARDANDO;
					espera = TIEMPO_MENSAJE;
				}
			}
			if (tecla == BTN_BACK) {                   // cancelar sin guardar
				if (especial == 0) estado = ESTADO_HORARIOS;
				else               estado = ESTADO_JORNADA_HORARIOS;
				seleccion = indice;
			}
			break;

		// ----- Configurar DESCANSO X: hora inicio, min inicio, hora fin, min fin -----
		case ESTADO_EDIT_DESCANSO:
			if (tecla == BTN_UP) {
				if (campo == 0) v1 = subir(v1, 0, 23);
				if (campo == 1) v2 = subir(v2, 0, 59);
				if (campo == 2) v3 = subir(v3, 0, 23);
				if (campo == 3) v4 = subir(v4, 0, 59);
			}
			if (tecla == BTN_DOWN) {
				if (campo == 0) v1 = bajar(v1, 0, 23);
				if (campo == 1) v2 = bajar(v2, 0, 59);
				if (campo == 2) v3 = bajar(v3, 0, 23);
				if (campo == 3) v4 = bajar(v4, 0, 59);
			}
			if (tecla == BTN_SELECT) {
				if (campo < 3) {
					campo++;
				} else {
					descanso_ih[indice] = v1;
					descanso_im[indice] = v2;
					descanso_fh[indice] = v3;
					descanso_fm[indice] = v4;
					seleccion = indice;
					estado_siguiente = ESTADO_DESCANSOS;
					estado = ESTADO_GUARDANDO;
					espera = TIEMPO_MENSAJE;
				}
			}
			if (tecla == BTN_BACK) {                   // cancelar sin guardar
				estado = ESTADO_DESCANSOS;
				seleccion = indice;
			}
			break;

		// ----- Configurar FECHA: campo 0 = dia, 1 = mes, 2 = anio (2000 a 2099) -----
		case ESTADO_EDIT_FECHA:
			if (tecla == BTN_UP) {
				if (campo == 0) v1 = subir(v1, 1, dias_del_mes(v2, v_anio));
				if (campo == 1) v2 = subir(v2, 1, 12);
				if (campo == 2) {
					if (v_anio >= 2099) v_anio = 2000;
					else                v_anio++;
				}
				ajustar_dia();
			}
			if (tecla == BTN_DOWN) {
				if (campo == 0) v1 = bajar(v1, 1, dias_del_mes(v2, v_anio));
				if (campo == 1) v2 = bajar(v2, 1, 12);
				if (campo == 2) {
					if (v_anio <= 2000) v_anio = 2099;
					else                v_anio--;
				}
				ajustar_dia();
			}
			if (tecla == BTN_SELECT) {
				if (campo < 2) {
					campo++;
				} else {
					if (modo_fecha == 0) {             // fecha del reloj
						dia  = v1;
						mes  = v2;
						anio = v_anio;
						estado_siguiente = ESTADO_FECHA_HORA;
					} else {                           // fecha de la jornada especial
						esp_dia  = v1;
						esp_mes  = v2;
						esp_anio = v_anio;
						estado_siguiente = ESTADO_JORNADA;
					}
					seleccion = 0;
					estado = ESTADO_GUARDANDO;
					espera = TIEMPO_MENSAJE;
				}
			}
			if (tecla == BTN_BACK) {                   // cancelar sin guardar
				if (modo_fecha == 0) estado = ESTADO_FECHA_HORA;
				else                 estado = ESTADO_JORNADA;
				seleccion = 0;
			}
			break;

		// ----- Configurar HORA del reloj: horas, minutos, segundos -----
		case ESTADO_EDIT_HORA_RELOJ:
			if (tecla == BTN_UP) {
				if (campo == 0) v1 = subir(v1, 0, 23);
				if (campo == 1) v2 = subir(v2, 0, 59);
				if (campo == 2) v3 = subir(v3, 0, 59);
			}
			if (tecla == BTN_DOWN) {
				if (campo == 0) v1 = bajar(v1, 0, 23);
				if (campo == 1) v2 = bajar(v2, 0, 59);
				if (campo == 2) v3 = bajar(v3, 0, 59);
			}
			if (tecla == BTN_SELECT) {
				if (campo < 2) {
					campo++;
				} else {
					hora    = v1;
					minuto  = v2;
					segundo = v3;
					seleccion = 1;
					estado_siguiente = ESTADO_FECHA_HORA;
					estado = ESTADO_GUARDANDO;
					espera = TIEMPO_MENSAJE;
				}
			}
			if (tecla == BTN_BACK) {                   // cancelar sin guardar
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
