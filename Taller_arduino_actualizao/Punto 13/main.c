#define F_CPU 16000000UL        // Arduino UNO (ATmega328P): 16 MHz. Debe ser igual al clock del micro en Proteus
#include <avr/io.h>
#include <util/delay.h>

// Tabla del 0 al 9 para catodo comun (1 = segmento encendido). Bit 0 = a ... bit 6 = g
uint8_t decoCC[10] = {0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110,
                      0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111};

int main(void)
{
	uint8_t entrada;             // lectura del puerto B (A y B juntos)
	uint8_t A;                   // primer numero (4 bits)
	uint8_t B;                   // segundo numero (4 bits)
	uint8_t operacion;           // numero que forman PC0, PC1 y PC2
	uint8_t resultado;           // resultado (0 a 225)
	uint8_t centenas;
	uint8_t decenas;
	uint8_t unidades;

	DDRB  = 0b00000000;          // PORTB: entradas
	PORTB = 0b11111111;          // pull-up interno en los 8 bits
	DDRC  = 0b00111000;          // PC0 a PC2 entradas; PC3 a PC5 salidas (pines comunes de los displays)
	PORTC = 0b00111111;          // pull-up en PC0 a PC2 y displays apagados
	DDRD  = 0b11111111;          // PD0 a PD6: segmentos (salidas)

	while (1)
	{
		entrada = PINB;                        // leemos A y B
		entrada = ~entrada;                    // con pull-up, ON = 0, por eso se invierte
		A = entrada & 0b00001111;              // nibble bajo
		B = entrada >> 4;                      // nibble alto

		operacion = PINC & 0b00000111;         // leemos solo PC0, PC1 y PC2
		operacion = (~operacion) & 0b00000111;

		switch (operacion) {

			case 0:                            // sumar
				resultado = A + B;
				break;

			case 1:                            // restar
				if (A >= B) {
					resultado = A - B;
				}
				else {
					resultado = 0;             // no mostramos negativos
				}
				break;

			case 2:                            // multiplicar
				resultado = A * B;
				break;

			case 3:                            // dividir
				if (B != 0) {
					resultado = A / B;
				}
				else {
					resultado = 0;             // no se puede dividir entre 0
				}
				break;

			default:                           // 4 a 7: sin operacion
				resultado = 0;
				break;
		}

		// Separamos el resultado en sus tres digitos
		centenas = resultado / 100;
		decenas  = (resultado / 10) % 10;
		unidades = resultado % 10;

		// Mostramos los tres displays, uno por uno (catodo comun: se enciende con 0).
		// Los bits PC0 a PC2 se dejan en 1 para no quitar el pull-up de las entradas.
		PORTC = 0b00111111;                    // apagamos los tres displays
		PORTD = decoCC[centenas];
		PORTC = 0b00110111;                    // encendemos las centenas (PC3 = 0)
		_delay_ms(1);

		PORTC = 0b00111111;
		PORTD = decoCC[decenas];
		PORTC = 0b00101111;                    // encendemos las decenas (PC4 = 0)
		_delay_ms(1);

		PORTC = 0b00111111;
		PORTD = decoCC[unidades];
		PORTC = 0b00011111;                    // encendemos las unidades (PC5 = 0)
		_delay_ms(1);
	}
}