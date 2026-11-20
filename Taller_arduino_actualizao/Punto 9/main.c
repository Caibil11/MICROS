#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	uint8_t botones;             // lectura de los 4 pulsadores
	uint8_t led = 0;             // cual LED esta prendido: 0 = ninguno, 1 = D1, 2 = D2, 3 = D3

	DDRB  = 0b00000111;          // PB0, PB1 y PB2: salidas (los LEDs)
	PORTB = 0b00000000;          // todos apagados

	DDRC  = 0b00000000;          // PORTC: entradas
	PORTC = 0b00001111;          // pull-up interno en PC0 a PC3

	while (1)
	{
		botones = PINC & 0b00001111;       // leemos solo PC0 a PC3. Con pull-up, presionado = 0

		switch (botones) {

			case 14:                       // 1110 : solo P1 presionado
				if (led == 0) {            // solo si no hay otro LED prendido
					led = 1;
					PORTB = 0b00000001;
				}
				break;

			case 13:                       // 1101 : solo P2 presionado
				if (led == 0) {
					led = 2;
					PORTB = 0b00000010;
				}
				break;

			case 11:                       // 1011 : solo P3 presionado
				if (led == 0) {
					led = 3;
					PORTB = 0b00000100;
				}
				break;

			case 7:                        // 0111 : solo P4 presionado
				led = 0;                   // apaga el LED que este prendido
				PORTB = 0b00000000;
				break;

			default:                       // ninguno presionado, o varios a la vez: no pasa nada
				break;
		}
	}
}

