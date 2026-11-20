#include "BOTONES.h"

// Guarda el boton que estaba presionado en la vuelta anterior
uint8_t boton_anterior = BTN_NINGUNO;

// Devuelve el boton que ACABAN de presionar.
// Si no hay ninguno, o si lo siguen manteniendo presionado, devuelve BTN_NINGUNO.
// Se debe llamar una vez por vuelta del while(1) de main (con su _delay_ms de 10 ms,
// que ademas sirve de antirrebote).
uint8_t boton_leer(void){

	uint8_t actual = BTN_NINGUNO;

	if      ( (_BTN_PIN & (1<<_BTN_UP_PIN))     == 0 ) actual = BTN_UP;
	else if ( (_BTN_PIN & (1<<_BTN_DOWN_PIN))   == 0 ) actual = BTN_DOWN;
	else if ( (_BTN_PIN & (1<<_BTN_SELECT_PIN)) == 0 ) actual = BTN_SELECT;
	else if ( (_BTN_PIN & (1<<_BTN_BACK_PIN))   == 0 ) actual = BTN_BACK;

	// Solo avisamos si es un boton nuevo (antes no estaba presionado)
	if (actual != BTN_NINGUNO && actual != boton_anterior) {
		boton_anterior = actual;
		return actual;
	}

	boton_anterior = actual;
	return BTN_NINGUNO;
}