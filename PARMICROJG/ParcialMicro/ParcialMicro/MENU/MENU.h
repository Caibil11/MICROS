#ifndef MENU_H_
#define MENU_H_

#include <avr/io.h>

// Datos que se configuran desde el menu.
// (en la siguiente etapa el timbre los va a leer para hacer sonar el buzzer)
extern uint8_t  clave[4];          // clave de 4 digitos

extern uint8_t  horario_h[5];      // HORA 1 a HORA 5: hora
extern uint8_t  horario_m[5];      // HORA 1 a HORA 5: minutos

extern uint8_t  descanso_ih[3];    // DESCANSO 1 a 3: hora de inicio
extern uint8_t  descanso_im[3];    // DESCANSO 1 a 3: minutos de inicio
extern uint8_t  descanso_fh[3];    // DESCANSO 1 a 3: hora de fin
extern uint8_t  descanso_fm[3];    // DESCANSO 1 a 3: minutos de fin

extern uint8_t  esp_dia;           // fecha de la jornada especial
extern uint8_t  esp_mes;
extern uint16_t esp_anio;
extern uint8_t  especial_h[5];     // horarios de la jornada especial: hora
extern uint8_t  especial_m[5];     // horarios de la jornada especial: minutos

// Se llama una vez por cada vuelta del while(1) de main
void menu_actualizar(uint8_t boton, uint8_t paso_un_segundo);

#endif /* MENU_H_ */