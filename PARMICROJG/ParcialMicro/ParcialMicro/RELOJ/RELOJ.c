#include "RELOJ.h"

// Valores iniciales. Cambialos por la fecha y hora con la que quieras arrancar.
uint8_t  hora    = 10;
uint8_t  minuto  = 35;
uint8_t  segundo = 42;
uint8_t  dia     = 22;
uint8_t  mes     = 9;
uint16_t anio    = 2026;

// Cuantos dias tiene el mes m del anio a
uint8_t dias_del_mes(uint8_t m, uint16_t a){

	uint8_t dias[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	// Febrero tiene 29 dias si el anio es multiplo de 4 (sirve de 2001 a 2099)
	if (m == 2 && a % 4 == 0) {
		return 29;
	}
	return dias[m - 1];
}

// Suma un segundo y arregla minutos, horas, dia, mes y anio si hace falta
void reloj_avanzar_segundo(void){

	segundo++;
	if (segundo == 60) { segundo = 0; minuto++; }
	if (minuto  == 60) { minuto  = 0; hora++;   }
	if (hora    == 24) { hora    = 0; dia++;    }

	if (dia > dias_del_mes(mes, anio)) { dia = 1; mes++; }
	if (mes > 12)                      { mes = 1; anio++; }
}