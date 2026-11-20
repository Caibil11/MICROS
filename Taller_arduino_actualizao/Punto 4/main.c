#define F_CPU 20000000UL        // ATmega1284P: 20 MHz. Debe ser igual al "Clock Frequency" del micro en Proteus
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	uint8_t boton;               // lectura del boton: 1 = suelto, 0 = presionado (por el pull-up)
	uint8_t pulsaciones = 0;     // cuantas veces lo han presionado
	uint8_t estado = 0;          // estado de la salida PB7: 0 = apagada, 1 = encendida

	DDRB  = 0b10000000;          // PB7 salida, PB0 entrada (y las demas entradas)
	PORTB = 0b00000001;          // pull-up interno en PB0, PB7 apagada

	while (1)
	{
		boton = PINB & 0b00000001;           // leemos solo PB0

		if (boton == 0) {                    // el boton esta presionado
			_delay_ms(20);                   // antirrebote

			while (boton == 0) {             // esperamos a que lo suelten
				boton = PINB & 0b00000001;
			}
			_delay_ms(20);

			pulsaciones++;                   // contamos una pulsacion

			if (pulsaciones == 3) {          // a la tercera, cambiamos la salida
				pulsaciones = 0;

				if (estado == 0) {
					estado = 1;
					PORTB = 0b10000001;      // PB7 encendida (PB0 sigue con su pull-up)
				}
				else {
					estado = 0;
					PORTB = 0b00000001;      // PB7 apagada
				}
			}
		}
	}
}