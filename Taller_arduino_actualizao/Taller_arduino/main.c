/*
 * Taller_arduino.c
 *
 * Created: 15/10/2026 8:20:49 p. m.
 * Author : dylan
 */ 
#define F_CPU 16000000UL   // Debe coincidir con Clock Frequency en Proteus
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#define LEDS_DDR    DDRD
#define LEDS_PORT   PORTD
#define TIEMPO_MS   10   // Tiempo que dura cada paso

int main(void)
{
	DDRD  = 0xFF;          // PORTB como salida
	PORTD = 0x00;          // LEDs apagados

	uint8_t mascara = 0x01;   // Máscara inicial: solo RB0 encendido
	uint8_t sentido = 0;      // 0 = hacia RB7 (izquierda), 1 = hacia RB0 (derecha)

	while (1)
	{
		PORTD = mascara;      // Aplicamos la máscara a los LEDs
		_delay_ms(TIEMPO_MS);

		if (sentido == 0)
		{
			if (mascara == 0x80)     // Llegó a RB7
			{
				sentido = 1;
				mascara >>= 1;
			}
			else
			{
				mascara <<= 1;       // Desplaza la máscara a la izquierda
			}
		}
		else
		{
			if (mascara == 0x01)     // Llegó a RB0
			{
				sentido = 0;
				mascara <<= 1;
			}
			else
			{
				mascara >>= 1;       // Desplaza la máscara a la derecha
			}
		}
	}
}


/*int main(void)
{
	// SETUP
	LEDS_DDR  = 0xFF;   // PORTB completo como salida
	LEDS_PORT = 0x00;   // LEDs apagados al inicio

	// LOOP
	while (1)
	{
		// Ida: RB0 -> RB7
		for (uint8_t i = 0; i < 8; i++)
		{
			LEDS_PORT = (1 << i);
			_delay_ms(TIEMPO_MS);
		}

		// Regreso: RB6 -> RB1 (los extremos ya se mostraron, así no se repiten)
		for (int8_t i = 6; i >= 1; i--)
		{
			LEDS_PORT = (1 << i);
			_delay_ms(TIEMPO_MS);
		}
	}
}*/