#define F_CPU 16000000L   // Debe coincidir con Clock Frequency en Proteus
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#define LEDS_DDR    DDRD
#define LEDS_PORT   PORTD
#define TIEMPO_MS   500   // Tiempo que dura cada paso
/* ---------- Definicion de pines ---------- */
#define SALIDA_1      PB0
#define SALIDA_2      PB1
#define ENTRADA_INA   PD2
#define ENTRADA_INB   PD3

/* ---------- Estados del motor ---------- */
#define ESTADO_FRENO      0
#define ESTADO_DERECHA    1
#define ESTADO_IZQUIERDA  2

/* Tiempo de espera antes de invertir el giro (ms) */
#define TIEMPO_MUERTO_MS  500

/* ---------- Configuracion de pines ---------- */
void configurar_pines(void)
{
	/* Salidas: PB0 y PB1 */
	DDRB |= (1 << SALIDA_1) | (1 << SALIDA_2);

	/* Arrancar siempre en estado seguro (freno) */
	PORTB &= ~((1 << SALIDA_1) | (1 << SALIDA_2));

	/* Entradas: PD2 y PD3 */
	DDRD &= ~((1 << ENTRADA_INA) | (1 << ENTRADA_INB));

	/* Pull-up interno DESACTIVADO (se usan resistencias externas) */
	PORTD &= ~((1 << ENTRADA_INA) | (1 << ENTRADA_INB));
}

/* ---------- Acciones del motor ---------- */
void motor_frenar(void)
{
	PORTB &= ~((1 << SALIDA_1) | (1 << SALIDA_2));   // 0 0
}

void motor_derecha(void)
{
	PORTB |=  (1 << SALIDA_1);                       // 1
	PORTB &= ~(1 << SALIDA_2);                       // 0
}

void motor_izquierda(void)
{
	PORTB &= ~(1 << SALIDA_1);                       // 0
	PORTB |=  (1 << SALIDA_2);                       // 1
}

/* ---------- Lectura de entradas ---------- */
/* Devuelve 0..3 -> bit1 = INA, bit0 = INB */
unsigned char leer_entradas(void)
{
	unsigned char ina = (PIND & (1 << ENTRADA_INA)) ? 1 : 0;
	unsigned char inb = (PIND & (1 << ENTRADA_INB)) ? 1 : 0;
	return (ina << 1) | inb;
}

/* Lectura con antirrebote: dos lecturas separadas 20 ms deben coincidir */
unsigned char leer_entradas_estables(unsigned char valor_anterior)
{
	unsigned char lectura1 = leer_entradas();
	_delay_ms(20);
	unsigned char lectura2 = leer_entradas();

	if (lectura1 == lectura2)
	return lectura2;
	return valor_anterior;       // Si no coinciden, se ignora el cambio
}

/* Convierte la combinacion INA/INB en una accion */
unsigned char decidir_estado(unsigned char entradas)
{
	if (entradas == 0b01) return ESTADO_DERECHA;     // INA=0, INB=1
	if (entradas == 0b10) return ESTADO_IZQUIERDA;   // INA=1, INB=0
	return ESTADO_FRENO;                             // 00 y 11
}

/* Aplica el estado, pasando por freno si se invierte el giro */
void aplicar_estado(unsigned char actual, unsigned char nuevo)
{
	if (nuevo == actual)
	return;

	/* Si va de derecha a izquierda (o viceversa): frenar y esperar */
	if (actual != ESTADO_FRENO && nuevo != ESTADO_FRENO)
	{
		motor_frenar();
		_delay_ms(TIEMPO_MUERTO_MS);
	}

	if (nuevo == ESTADO_DERECHA)        motor_derecha();
	else if (nuevo == ESTADO_IZQUIERDA) motor_izquierda();
	else                                motor_frenar();
}

/* ---------- Programa principal ---------- */
int main(void)
{
	unsigned char entradas = 0b11;
	unsigned char estado_actual = ESTADO_FRENO;
	unsigned char estado_nuevo;

	configurar_pines();

	while (1)
	{
		entradas = leer_entradas_estables(entradas);
		estado_nuevo = decidir_estado(entradas);

		aplicar_estado(estado_actual, estado_nuevo);
		estado_actual = estado_nuevo;
	}
}
