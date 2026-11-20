#ifndef RELOJ_H_
#define RELOJ_H_

#include <avr/io.h>

// Hora y fecha actuales (main las puede leer y cambiar directamente)
extern uint8_t  hora;
extern uint8_t  minuto;
extern uint8_t  segundo;
extern uint8_t  dia;
extern uint8_t  mes;
extern uint16_t anio;

void    reloj_avanzar_segundo(void);       // se llama una vez cada segundo
uint8_t dias_del_mes(uint8_t m, uint16_t a);

#endif /* RELOJ_H_ */