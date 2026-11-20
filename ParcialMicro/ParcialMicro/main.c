/* ====================================================================
 *  SISTEMA DE TIMBRE ESCOLAR - ATmega1284P
 *  Menus con 4 botones: UP, DOWN, SELECT, BACK
 *  Pantalla principal con hora, fecha y temperatura (DS18B20)
 *  El menu (usuario, clave, configuracion y timbre) esta en MENU/MENU.c
 *  El buzzer (BUZZER/BUZZER.c) suena:
 *     - al presionar cualquier boton (pitido corto)
 *     - cuando los datos de acceso son incorrectos (3 pitidos rapidos, lo pide MENU.c)
 *     - cuando llega la hora de un horario, inicio o fin de descanso (lo pide MENU.c)
 * ====================================================================
 */

#define F_CPU 20000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "LCD/LCD_LIB.h"
#include "DS18B20_LIB/DS18B20_LIB.h"
#include "BOTONES/BOTONES.h"
#include "RELOJ/RELOJ.h"
#include "BUZZER/BUZZER.h"
#include "MENU/MENU.h"

// ---------- Tiempo ----------
// Cada vuelta del while(1) dura ~10 ms, mas lo que tarda en dibujar la LCD y leer el sensor
// (unos 28 ms cada segundo en la pantalla principal). Por eso no son 100 vueltas sino 97:
// 97 x 10 ms + 28 ms = ~998 ms.
// Si el reloj se atrasa o se adelanta, se ajusta este numero.
#define VUELTAS_POR_SEGUNDO  97

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
	ds18b20_iniciar_conv();    // la primera medida queda lista para cuando pase el primer segundo
	boton_init();
	buzzer_init();

	buzzer_pitar(1, 15);       // pitido de arranque: confirma que el buzzer funciona

	while (1) {

		boton = boton_leer();

		// Pitido corto al presionar un boton (si no esta sonando el timbre, para no cortarlo)
		if (boton != BTN_NINGUNO && buzzer_sonando() == 0) {
			buzzer_pitar(1, 15);
		}

		// El menu decide que pantalla toca, la dibuja y revisa si suena el timbre
		menu_actualizar(boton, paso_un_segundo);

		// El buzzer avanza un paso de sus pitidos
		buzzer_actualizar();

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
