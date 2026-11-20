#define F_CPU 20000000UL        // ATmega1284P: 20 MHz. Debe ser igual al "Clock Frequency" del micro en Proteus
#include <avr/io.h>
#include <util/delay.h>

// Posiciones del registro de 16 bits (1 = LED encendido)
#define BIT_0     0b0000000000000001      // solo el bit 0
#define BIT_15    0b1000000000000000      // solo el bit 15
#define PAR_BAJO  0b0000000000000011      // los bits 0 y 1
#define PAR_ALTO  0b1100000000000000      // los bits 14 y 15

// Limites del tiempo entre movimientos (en milisegundos)
#define TIEMPO_MIN   50
#define TIEMPO_MAX   1000
#define PASO_TIEMPO  50

uint16_t registro = 0;          // el registro de 16 bits (sin signo, para que al ir a la derecha entren ceros)
uint16_t tiempo = 300;          // tiempo entre un movimiento y el siguiente

int main(void)
{
	uint16_t i;
	uint8_t modo;                // numero que forman las 3 entradas
	uint8_t anterior = 255;      // el modo anterior (255 = ninguno todavia)
	uint8_t botones;

	DDRA = 0xFF;                 // PORTA: 8 salidas
	DDRB = 0xFF;                 // PORTB: 8 salidas
	DDRC  = 0b00000000;          // PORTC: entradas
	PORTC = 0b00000111;          // pull-up interno en PC0, PC1 y PC2
	DDRD  = 0b00000000;          // PORTD: entradas
	PORTD = 0b00000011;          // pull-up interno en PD0 y PD1

	while (1)
	{
		modo = PINC & 0b00000111;              // leemos solo PC0, PC1 y PC2
		modo = (~modo) & 0b00000111;           // con pull-up, ON = 0, por eso se invierte

		// Si cambiaron el modo, el registro se vacia y arranca de nuevo
		if (modo != anterior) {
			anterior = modo;
			registro = 0;
		}

		// Mostramos el registro: PORTA la parte baja y PORTB la parte alta
		PORTA = registro;
		PORTB = registro >> 8;

		// Esperamos "tiempo" milisegundos y mientras tanto revisamos los botones
		for (i = 0; i < tiempo; i++) {
			_delay_ms(1);

			botones = PIND & 0b00000011;       // 3 = ninguno, 2 = PD0, 1 = PD1

			if (botones == 2) {                // PD0: mas tiempo
				if (tiempo < TIEMPO_MAX) {
					tiempo = tiempo + PASO_TIEMPO;
				}
				_delay_ms(200);
			}

			if (botones == 1) {                // PD1: menos tiempo
				if (tiempo > TIEMPO_MIN) {
					tiempo = tiempo - PASO_TIEMPO;
				}
				_delay_ms(200);
			}
		}

		// Siguiente movimiento del registro (si esta en 0, empieza desde su posicion inicial)
		switch (modo) {

			case 1:                            // un LED hacia la izquierda
				if (registro == 0) {
					registro = BIT_0;
				}
				else {
					registro = registro << 1;
				}
				break;

			case 2:                            // un LED hacia la derecha
				if (registro == 0) {
					registro = BIT_15;
				}
				else {
					registro = registro >> 1;
				}
				break;

			case 3:                            // dos LEDs hacia la izquierda
				if (registro == 0) {
					registro = PAR_BAJO;
				}
				else {
					registro = registro << 1;
				}
				break;

			case 4:                            // dos LEDs hacia la derecha
				if (registro == 0) {
					registro = PAR_ALTO;
				}
				else {
					registro = registro >> 1;
				}
				break;

			default:                           // 0 y 5 a 7: detenido
				registro = 0;
				break;
		}
	}
}