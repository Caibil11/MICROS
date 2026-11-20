#define F_CPU 16000000UL        // Arduino UNO (ATmega328P): 16 MHz. Debe ser igual al clock del micro en Proteus
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	uint8_t botones;             // lectura de los pulsadores: 3 = ninguno, 2 = P1, 1 = P2, 0 = los dos
	uint8_t led = 0;             // estado del LED: 0 = apagado, 1 = encendido

	DDRB  = 0b00000100;          // PB2 salida (el LED), PB0 y PB1 entradas
	PORTB = 0b00000011;          // pull-up interno en PB0 y PB1, LED apagado

	while (1)
	{
		botones = PINB & 0b00000011;         // leemos solo PB0 y PB1. Con pull-up, presionado = 0

		if (botones != 3) {                  // se presiono algun pulsador
			_delay_ms(20);                   // antirrebote

			while (botones != 3) {           // esperamos a que lo suelten
				botones = PINB & 0b00000011;
			}
			_delay_ms(20);

			if (led == 0) {                  // cambiamos el estado del LED
				led = 1;
				PORTB = 0b00000111;          // LED encendido (los pull-up siguen activos)
			}
			else {
				led = 0;
				PORTB = 0b00000011;          // LED apagado
			}
		}
	}
}