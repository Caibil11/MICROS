#ifndef MENU_H_
#define MENU_H_

#include <avr/io.h>

// Se llama una vez por cada vuelta del while(1) de main.
// Aqui adentro estan los datos que se configuran (clave, horarios, descansos, jornada especial)
// y tambien se revisa cada segundo si le toca sonar al timbre.
void menu_actualizar(uint8_t boton, uint8_t paso_un_segundo);

#endif /* MENU_H_ */
