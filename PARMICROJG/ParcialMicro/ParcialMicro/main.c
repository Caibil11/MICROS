/* ====================================================================
 *  SISTEMA DE TIMBRE ESCOLAR - ATmega1284P
 *  Menus con 4 botones: UP, DOWN, SELECT, BACK
 *  Pantalla principal con hora, fecha y temperatura (DS18B20)
 *  El menu (clave y configuracion) esta en MENU/MENU.c
 * ====================================================================
 */

#define F_CPU 20000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "LCD/LCD_LIB.h"
#include "DS18B20_LIB/DS18B20_LIB.h"
#include "Botones/BOTONES.h"
#include "Reloj/RELOJ.h"
#include "MENU/MENU.h"

// ---------- Tiempo ----------
// Cada vuelta del while(1) dura ~10 ms. 100 vueltas = 1 segundo.
// Si el reloj se atrasa o se adelanta, se ajusta este numero.
#define VUELTAS_POR_SEGUNDO  100

// ---------- Variables ----------
uint8_t boton           = BTN_NINGUNO;
uint8_t vueltas         = 0;   // cuenta las vueltas del while(1)
uint8_t paso_un_segundo = 0;   // 1 cuando acaba de pasar un segundo


int main(void)
{
	lcd_init();
	lcd_disable_blink();
	lcd_disable_cursor();
	ds18b20_init();

	while (1) {

		boton = boton_leer();

		// El menu decide que pantalla toca y la dibuja
		menu_actualizar(boton, paso_un_segundo);

		// ---------- Reloj: cada vuelta dura ~10 ms ----------
		_delay_ms(10);
		vueltas++;
		paso_un_segundo = 0;
		if (vueltas >= VUELTAS_POR_SEGUNDO) {
			vueltas = 0;
			reloj_avanzar_segundo();
			paso_un_segundo = 1;
		}
	}
}