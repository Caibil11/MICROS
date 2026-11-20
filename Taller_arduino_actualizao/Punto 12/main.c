#include <avr/io.h>

int main(void)
{
	uint8_t entrada;             // lectura del puerto B (A y B juntos)
	uint8_t A;                   // primer numero (4 bits)
	uint8_t B;                   // segundo numero (4 bits)
	uint8_t operacion;           // numero que forman PC0, PC1 y PC2
	uint8_t resultado;           // resultado (maximo 15 x 15 = 225, cabe en 8 bits)

	DDRB  = 0b00000000;          // PORTB: entradas
	PORTB = 0b11111111;          // pull-up interno en los 8 bits
	DDRC  = 0b00000000;          // PORTC: entradas
	PORTC = 0b00000111;          // pull-up interno en PC0, PC1 y PC2
	DDRD  = 0b11111111;          // PORTD: 8 salidas (el resultado en binario)

	while (1)
	{
		entrada = PINB;                        // leemos A y B
		entrada = ~entrada;                    // con pull-up, ON = 0, por eso se invierte
		A = entrada & 0b00001111;              // nibble bajo
		B = entrada >> 4;                      // nibble alto

		operacion = PINC & 0b00000111;         // leemos solo PC0, PC1 y PC2
		operacion = (~operacion) & 0b00000111; // se invierte igual que antes

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

		PORTD = resultado;                     // el resultado en binario
	}
}

