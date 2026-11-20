#define F_CPU 1000000UL         // 1 MHz para simular rapido en Proteus (en el Arduino real serian 16 MHz).
                                // Debe ser igual al "Clock Frequency" del micro en Proteus
#include <avr/io.h>
#include <util/delay.h>

// Pines de cada LED
#define V1_ROJO      PD2
#define V1_AMARILLO  PD3
#define V1_VERDE     PD4

#define V2_ROJO      PD5
#define V2_AMARILLO  PD6
#define V2_VERDE     PD7

#define V3_ROJO      PB0
#define V3_AMARILLO  PB1
#define V3_VERDE     PB2

// Cuanto dura cada luz (en segundos)
#define TIEMPO_VERDE     2
#define TIEMPO_AMARILLO  1

// Espera la cantidad de segundos que le pidan
void esperar(uint8_t segundos)
{
	for (uint8_t i = 0; i < segundos; i++) {
		_delay_ms(1000);
	}
}

int main(void)
{
	DDRD = 0xFF;                 // PORTD: salidas (vias 1 y 2)
	DDRB = 0xFF;                 // PORTB: salidas (via 3)

	while (1)
	{
		// ----- Via 1 en verde -----
		PORTD = (1 << V1_VERDE) | (1 << V2_ROJO);
		PORTB = (1 << V3_ROJO);
		esperar(TIEMPO_VERDE);

		// ----- Via 1 en amarillo -----
		PORTD = (1 << V1_AMARILLO) | (1 << V2_ROJO);
		PORTB = (1 << V3_ROJO);
		esperar(TIEMPO_AMARILLO);

		// ----- Via 2 en verde -----
		PORTD = (1 << V1_ROJO) | (1 << V2_VERDE);
		PORTB = (1 << V3_ROJO);
		esperar(TIEMPO_VERDE);

		// ----- Via 2 en amarillo -----
		PORTD = (1 << V1_ROJO) | (1 << V2_AMARILLO);
		PORTB = (1 << V3_ROJO);
		esperar(TIEMPO_AMARILLO);

		// ----- Via 3 en verde -----
		PORTD = (1 << V1_ROJO) | (1 << V2_ROJO);
		PORTB = (1 << V3_VERDE);
		esperar(TIEMPO_VERDE);

		// ----- Via 3 en amarillo -----
		PORTD = (1 << V1_ROJO) | (1 << V2_ROJO);
		PORTB = (1 << V3_AMARILLO);
		esperar(TIEMPO_AMARILLO);
	}
}