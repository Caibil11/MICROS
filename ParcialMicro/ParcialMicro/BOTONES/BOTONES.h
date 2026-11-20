#ifndef BOTONES_H_
#define BOTONES_H_

#include <avr/io.h>

// Los botones estan entre el pin y GND (con resistencia a VCC).
// Sin presionar = 1, presionado = 0.
#define _BTN_DDR         DDRB
#define _BTN_PORT        PORTB
#define _BTN_PIN         PINB

#define _BTN_UP_PIN      0     // PB0
#define _BTN_DOWN_PIN    1     // PB1
#define _BTN_SELECT_PIN  2     // PB2
#define _BTN_BACK_PIN    3     // PB3

// Lo que devuelve boton_leer()
#define BTN_NINGUNO  0
#define BTN_UP       1
#define BTN_DOWN     2
#define BTN_SELECT   3
#define BTN_BACK     4

void    boton_init(void);
uint8_t boton_leer(void);

#endif /* BOTONES_H_ */