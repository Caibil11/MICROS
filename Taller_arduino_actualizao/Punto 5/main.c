#define F_CPU 16000000UL        // Arduino UNO (ATmega328P): 16 MHz. Debe ser igual al clock del micro en Proteus
#include <avr/io.h>
#include <util/delay.h>

// Cada numero es lo que se muestra en los LEDs en ese paso
uint8_t seq1[8]  = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};     // un LED corre hacia la izquierda
uint8_t seq2[14] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
                    0x40, 0x20, 0x10, 0x08, 0x04, 0x02};                 // ida y vuelta
uint8_t seq3[2]  = {0xAA, 0x55};                                         // LEDs alternados
uint8_t seq4[8]  = {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};     // se van llenando

int main(void)
{
	uint8_t seleccion;           // numero que forman las 4 entradas
	uint8_t anterior = 255;      // la seleccion anterior (255 = ninguna todavia)
	uint8_t paso = 0;            // en que paso de la secuencia va

	DDRD  = 0xFF;                // PORTD: 8 salidas (los LEDs)
	PORTD = 0x00;

	DDRB  &= 0xF0;               // PB0 a PB3: entradas
	PORTB |= 0x0F;               // pull-up interno en PB0 a PB3 (el DIP en ON conecta el pin a GND)

	while (1)
	{
		// Con pull-up, un interruptor en ON lee 0, por eso invertimos (~) y
		// nos quedamos solo con los 4 bits de abajo
		seleccion = ~PINB & 0x0F;

		// Si cambiaron la seleccion, la secuencia nueva empieza desde el principio
		if (seleccion != anterior) {
			paso = 0;
			anterior = seleccion;
		}

		switch (seleccion) {

			case 1:
				PORTD = seq1[paso];
				paso++;
				if (paso >= 8) {
					paso = 0;
				}
				break;

			case 2:
				PORTD = seq2[paso];
				paso++;
				if (paso >= 14) {
					paso = 0;
				}
				break;

			case 3:
				PORTD = seq3[paso];
				paso++;
				if (paso >= 2) {
					paso = 0;
				}
				break;

			case 4:
				PORTD = seq4[paso];
				paso++;
				if (paso >= 8) {
					paso = 0;
				}
				break;

			default:                   // cualquier otro numero: todo apagado
				PORTD = 0x00;
				break;
		}

		_delay_ms(30);                // tiempo entre un paso y el siguiente
	}
}