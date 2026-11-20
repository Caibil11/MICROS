#include "RELOJ.h"

// Valores iniciales. Cambialos por la fecha y hora con la que quieras arrancar.
static uint8_t  hora    = 10;
static uint8_t  minuto  = 35;
static uint8_t  segundo = 42;
static uint8_t  dia     = 22;
static uint8_t  mes     = 9;
static uint16_t anio    = 2026;

static uint8_t  pausado = 0;     // 1 = el reloj esta detenido

uint8_t  reloj_hora(void)    { return hora; }
uint8_t  reloj_minuto(void)  { return minuto; }
uint8_t  reloj_segundo(void) { return segundo; }
uint8_t  reloj_dia(void)     { return dia; }
uint8_t  reloj_mes(void)     { return mes; }
uint16_t reloj_anio(void)    { return anio; }

// Cuantos dias tiene el mes m del anio a
uint8_t dias_del_mes(uint8_t m, uint16_t a){

	uint8_t dias[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	// Febrero tiene 29 dias si el anio es multiplo de 4 (sirve de 2001 a 2099)
	if (m == 2 && a % 4 == 0) {
		return 29;
	}
	return dias[m - 1];
}

void reloj_pausar(uint8_t pausa){
	pausado = pausa;
}

uint8_t reloj_pausado(void){
	return pausado;
}

// Suma un segundo y arregla minutos, horas, dia, mes y anio si hace falta
void reloj_avanzar_segundo(void){

	if (pausado == 1) {
		return;
	}

	segundo++;
	if (segundo == 60) { segundo = 0; minuto++; }
	if (minuto  == 60) { minuto  = 0; hora++;   }
	if (hora    == 24) { hora    = 0; dia++;    }

	if (dia > dias_del_mes(mes, anio)) { dia = 1; mes++; }
	if (mes > 12)                      { mes = 1; anio++; }
	if (anio > 2099)                   { anio = 2000; }     // mismo limite que al editar la fecha
}

// valor + 1, o 0 si ya estaba en el maximo
uint8_t subir(uint8_t valor, uint8_t maximo){
	if (valor >= maximo) {
		return 0;
	}
	return valor + 1;
}

// valor - 1, o el maximo si ya estaba en 0
uint8_t bajar(uint8_t valor, uint8_t maximo){
	if (valor == 0) {
		return maximo;
	}
	return valor - 1;
}

void reloj_ajustar(uint8_t campo, uint8_t arriba){

	switch (campo) {

		case 0:
			if (arriba) hora = subir(hora, 23);
			else        hora = bajar(hora, 23);
			break;

		case 1:
			if (arriba) minuto = subir(minuto, 59);
			else        minuto = bajar(minuto, 59);
			break;

		case 2:
			if (arriba) segundo = subir(segundo, 59);
			else        segundo = bajar(segundo, 59);
			break;

		case 3:                                              // dia: de 1 al ultimo del mes
			if (arriba) {
				if (dia >= dias_del_mes(mes, anio)) dia = 1;
				else                                dia++;
			} else {
				if (dia <= 1) dia = dias_del_mes(mes, anio);
				else          dia--;
			}
			break;

		case 4:                                              // mes: de 1 a 12
			if (arriba) {
				if (mes == 12) mes = 1;
				else           mes++;
			} else {
				if (mes <= 1) mes = 12;
				else          mes--;
			}
			break;

		case 5:                                              // anio: de 2000 a 2099
			if (arriba) {
				if (anio >= 2099) anio = 2000;
				else              anio++;
			} else {
				if (anio <= 2000) anio = 2099;
				else              anio--;
			}
			break;
	}

	// si el dia no existe en ese mes (ej. 31 de abril), baja al ultimo dia
	if (dia > dias_del_mes(mes, anio)) {
		dia = dias_del_mes(mes, anio);
	}
}
