#define F_CPU 16000000UL        // Arduino UNO (ATmega328P): 16 MHz. Debe ser igual al clock del micro en Proteus
#include <avr/io.h>
#include <util/delay.h>

#define IN1       PB0
#define IN2       PB1
#define DECENAS   PB4
#define UNIDADES  PB5

// Tabla del 0 al 9 para catodo comun (1 = segmento encendido). Bit 0 = a ... bit 6 = g
uint8_t decoCC[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

// Muestra el numero en los dos displays. Cada vuelta dura ~10 ms (5 ms cada display).
// En catodo comun un display se enciende poniendo su pin comun en 0.
void mostrar(uint8_t numero, uint8_t vueltas)
{
	uint8_t decenas  = numero / 10;
	uint8_t unidades = numero % 10;

	for (uint8_t i = 0; i < vueltas; i++) {

		// Decenas
		PORTB |= (1 << DECENAS) | (1 << UNIDADES);    // apagamos los dos displays
		PORTD = decoCC[decenas];                       // ponemos los segmentos
		PORTB &= ~(1 << DECENAS);                      // encendemos las decenas
		_delay_ms(5);

		// Unidades
		PORTB |= (1 << DECENAS) | (1 << UNIDADES);
		PORTD = decoCC[unidades];
		PORTB &= ~(1 << UNIDADES);
		_delay_ms(5);
	}
}

int main(void)
{
	uint8_t numero = 0;          // numero que se muestra (00 a 99)
	uint8_t subiendo = 1;        // ultima accion: 1 = ascendente, 0 = descendente
	uint8_t paso;                // de cuanto en cuanto cuenta (1 o 2)
	uint8_t in1;
	uint8_t in2;

	DDRD = 0xFF;                                      // PD0 a PD6: segmentos (salidas)
	DDRB |= (1 << DECENAS) | (1 << UNIDADES);         // pines comunes de los displays: salidas
	DDRB &= ~((1 << IN1) | (1 << IN2));               // IN1 e IN2: entradas
	PORTB |= (1 << IN1) | (1 << IN2);                 // pull-up interno en IN1 e IN2

	while (1)
	{
		mostrar(numero, 40);                          // muestra el numero ~400 ms

		// Con pull-up, un interruptor en ON lee 0
		in1 = 0;
		in2 = 0;
		if ((PINB & (1 << IN1)) == 0) {
			in1 = 1;
		}
		if ((PINB & (1 << IN2)) == 0) {
			in2 = 1;
		}

		if (in1 == 1 && in2 == 1) {
			// PAUSA: el numero no cambia
		}
		else {
			if (in1 == 1 && in2 == 0) {
				subiendo = 1;
			}
			if (in1 == 0 && in2 == 1) {
				subiendo = 0;
			}

			paso = 1;
			if (in1 == 0 && in2 == 0) {
				paso = 2;                             // 0 0: sigue la ultima accion de 2 en 2
			}

			if (subiendo == 1) {
				numero = numero + paso;
				if (numero > 99) {
					numero = numero - 100;            // despues del 99 sigue el 00
				}
			}
			else {
				if (numero >= paso) {
					numero = numero - paso;
				}
				else {
					numero = numero + 100 - paso;     // antes del 00 viene el 99
				}
			}
		}
	}
}