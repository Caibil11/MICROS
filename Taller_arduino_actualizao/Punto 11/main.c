#define F_CPU 1000000UL        // Arduino UNO (ATmega328P): 16 MHz. Debe ser igual al clock del micro en Proteus
#include <avr/io.h>
#include <util/delay.h>

// Tabla del 0 al 7 para catodo comun (1 = segmento encendido). Bit 0 = a ... bit 6 = g
uint8_t decoCC[8] = {0b00111111, 0b00000110, 0b01011011, 0b01001111,
                     0b01100110, 0b01101101, 0b01111101, 0b00000111};

int main(void)
{
	uint8_t botones;             // lectura de los 4 pulsadores (15 = ninguno presionado)
	uint8_t posicion = 0;        // bit en el que esta ubicado el usuario (0 a 7)
	uint8_t leds = 0;            // estado de los 8 LEDs

	DDRD  = 0b11111111;          // PORTD: 8 salidas (los LEDs)
	DDRB  = 0b01111111;          // PB0 a PB6: segmentos del display (salidas)
	DDRC  = 0b00000000;          // PORTC: entradas
	PORTC = 0b00001111;          // pull-up interno en PC0 a PC3

	while (1)
	{
		PORTD = leds;                        // mostramos los LEDs
		PORTB = decoCC[posicion];            // mostramos la posicion en el display

		botones = PINC & 0b00001111;         // leemos solo PC0 a PC3. Con pull-up, presionado = 0

		if (botones != 15) {                 // se presiono algun pulsador
			_delay_ms(20);                   // antirrebote
			botones = PINC & 0b00001111;     // volvemos a leer para confirmar

			switch (botones) {

				case 14:                     // 1110 : SEL+
					if (posicion < 7) {
						posicion++;
					}
					break;

				case 13:                     // 1101 : SEL-
					if (posicion > 0) {
						posicion--;
					}
					break;

				case 11:                     // 1011 : ENCENDER
					leds |= (1 << posicion);
					break;

				case 7:                      // 0111 : APAGAR
					leds &= ~(1 << posicion);
					break;

				default:                     // varios a la vez: no pasa nada
					break;
			}

			while (botones != 15) {          // esperamos a que lo suelten
				botones = PINC & 0b00001111;
			}
			_delay_ms(20);
		}
	}
}