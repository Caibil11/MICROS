#ifndef BUZZER_H_
#define BUZZER_H_

#include <avr/io.h>

// Buzzer activo entre el pin y GND: 1 = suena, 0 = silencio.
// (En fisico va con una resistencia de 1k a la base de un NPN y el buzzer en el colector.)
#define _BUZZER_DDR    DDRC
#define _BUZZER_PORT   PORTC
#define _BUZZER_PIN    0      // PC0

// Pausa entre pitidos, en vueltas del while(1) (30 vueltas x 10 ms = 0.3 s)
#define BUZZER_PAUSA   30

void buzzer_init(void);
void buzzer_pitar(uint8_t pitidos, uint8_t largo);   // largo = duracion de cada pitido en vueltas de 10 ms
uint8_t buzzer_sonando(void);                         // 1 si todavia le faltan pitidos por hacer
void buzzer_detener(void);                           // apaga el buzzer y cancela los pitidos que faltan
void buzzer_actualizar(void);                        // se llama una vez por vuelta del while(1)

#endif /* BUZZER_H_ */
