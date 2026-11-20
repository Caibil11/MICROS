/* ====================================================================
 *  CALCULADORA DE 4 BITS - ATmega1284p
 *  Operaciones: Suma, Resta, Multiplicacion, Division, AND, OR, NOT
 *  Salida: display de 3 digitos (centenas, decenas, unidades) multiplexado
 * ====================================================================
 */

#define F_CPU 20000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

/* --------------------------------------------------------------------
 *  1. TABLA DE SEGMENTOS (catodo comun)
 * -------------------------------------------------------------------- */
static const uint8_t decoCC[10] = {191, 134, 219, 207, 230, 237, 253, 135, 255, 231};

#define segA 0   // PD0
#define segB 1   // PD1
#define segC 2   // PD2
#define segD 3   // PD3
#define segE 4   // PD4
#define segF 5   // PD5
#define segG 6   // PD6

#define DIG_CENTENAS 4   // PA4 -> digito mas a la izquierda (posicion "1" del display)
#define DIG_DECENAS  5   // PA5 -> posicion "2"
#define DIG_UNIDADES 6   // PA6 -> posicion "3"
#define MASK_DIGITOS ((1<<DIG_UNIDADES)|(1<<DIG_DECENAS)|(1<<DIG_CENTENAS))

/* --------------------------------------------------------------------
 *  2. FUNCIONES DEL DISPLAY
 * -------------------------------------------------------------------- */

static void mostrar_digito(uint8_t num)
{
    uint8_t val = decoCC[num];

    if (val & (1<<0)) PORTD |= (1<<segA); else PORTD &= ~(1<<segA);
    if (val & (1<<1)) PORTD |= (1<<segB); else PORTD &= ~(1<<segB);
    if (val & (1<<2)) PORTD |= (1<<segC); else PORTD &= ~(1<<segC);
    if (val & (1<<3)) PORTD |= (1<<segD); else PORTD &= ~(1<<segD);
    if (val & (1<<4)) PORTD |= (1<<segE); else PORTD &= ~(1<<segE);
    if (val & (1<<5)) PORTD |= (1<<segF); else PORTD &= ~(1<<segF);
    if (val & (1<<6)) PORTD |= (1<<segG); else PORTD &= ~(1<<segG);
}

// Apaga los 3 digitos (todos en ALTO, ya que el encendido es activo en bajo)
static inline void apagar_todos(void)
{
    PORTA |= MASK_DIGITOS;
}

// Multiplexa los 3 digitos. Solo UNO se pone en bajo (encendido) a la vez.
static void mostrar_resultado(uint8_t unidades, uint8_t decenas, uint8_t centenas, uint16_t ciclos)
{
    for (uint16_t i = 0; i < ciclos; i++) {

        apagar_todos();
        mostrar_digito(unidades);
        PORTA &= ~(1<<DIG_UNIDADES);   // pone SOLO este pin en bajo -> se enciende
        _delay_ms(4);

        apagar_todos();
        mostrar_digito(decenas);
        PORTA &= ~(1<<DIG_DECENAS);
        _delay_ms(4);

        apagar_todos();
        mostrar_digito(centenas);
        PORTA &= ~(1<<DIG_CENTENAS);
        _delay_ms(4);
    }
    apagar_todos();
}

/* --------------------------------------------------------------------
 *  3. LECTURA DE ENTRADAS
 * -------------------------------------------------------------------- */
static void leer_entradas(uint8_t *A, uint8_t *B, uint8_t *op)
{
    *A  = PINA & 0x0F;
    *B  = PINB & 0x0F;
    *op = PINC & 0x07;
}

/* --------------------------------------------------------------------
 *  4. CALCULO
 * -------------------------------------------------------------------- */
static uint8_t calcular(uint8_t A, uint8_t B, uint8_t op, uint8_t *error)
{
    uint16_t r = 0;
    *error = 0;

    switch (op) {
        case 0: r = A + B; break;
        case 1: r = (A >= B) ? (uint16_t)(A - B) : 0; break;
        case 2: r = (uint16_t)A * B; break;
        case 3:
            if (B == 0) { *error = 1; r = 0; }
            else r = A / B;
            break;
        case 4: r = A & B; break;
        case 5: r = A | B; break;
        case 6: r = (~A) & 0x0F; break;
        default: r = 0; break;
    }
    return (uint8_t)(r & 0xFF);
}

/* --------------------------------------------------------------------
 *  5. PROGRAMA PRINCIPAL
 * -------------------------------------------------------------------- */
int main(void)
{
    DDRA = (DDRA & 0x0F) | 0x70;  // PA0-3 entrada, PA4-6 salida (digitos)
    DDRB &= 0xF0;
    DDRC &= 0xF8;
    DDRD |= 0x7F;

    apagar_todos();               // arranca con los 3 digitos apagados

    uint8_t A, B, op, resultado, error;
    uint8_t unidades, decenas, centenas;

    while (1) {
        leer_entradas(&A, &B, &op);
        resultado = calcular(A, B, op, &error);

        if (error) {
            unidades = decenas = centenas = 0;
        } else {
            unidades = resultado % 10;
            decenas  = (resultado / 10) % 10;
            centenas = (resultado / 100) % 10;
        }

        mostrar_resultado(unidades, decenas, centenas, 15);
    }
}