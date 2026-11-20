#ifndef RELOJ_H_
#define RELOJ_H_

#include <avr/io.h>

void    reloj_avanzar_segundo(void);       // se llama una vez cada segundo
void    reloj_pausar(uint8_t pausa);       // 1 = el reloj no avanza (se usa mientras se edita su hora o fecha), 0 = sigue
uint8_t reloj_pausado(void);

// Leer la hora y la fecha actuales
uint8_t  reloj_hora(void);
uint8_t  reloj_minuto(void);
uint8_t  reloj_segundo(void);
uint8_t  reloj_dia(void);
uint8_t  reloj_mes(void);
uint16_t reloj_anio(void);

// Cambiar un campo del reloj (arriba = 1 sube, arriba = 0 baja; al llegar al limite da la vuelta)
// campo: 0 hora, 1 minuto, 2 segundo, 3 dia, 4 mes, 5 anio (2000 a 2099)
void    reloj_ajustar(uint8_t campo, uint8_t arriba);

uint8_t dias_del_mes(uint8_t m, uint16_t a);

// Sirven para cualquier valor que va de 0 a "maximo" y da la vuelta
uint8_t subir(uint8_t valor, uint8_t maximo);
uint8_t bajar(uint8_t valor, uint8_t maximo);

#endif /* RELOJ_H_ */
