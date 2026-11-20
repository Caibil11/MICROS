#include "BUZZER.h"

static uint8_t pitidos   = 0;   // pitidos que faltan por hacer (0 = buzzer en reposo)
static uint8_t largo     = 0;   // duracion de cada pitido (en vueltas)
static uint8_t cuenta    = 0;   // vueltas que faltan para cambiar de pitido a pausa (o al reves)
static uint8_t encendido = 0;   // 1 = esta sonando, 0 = esta en la pausa

void buzzer_init(void){
	_BUZZER_DDR  |=  (1 << _BUZZER_PIN);    // el pin es salida
	_BUZZER_PORT &= ~(1 << _BUZZER_PIN);    // empieza en silencio
}

// Ejemplo: buzzer_pitar(3, 50) = 3 pitidos de 0.5 s cada uno
void buzzer_pitar(uint8_t veces, uint8_t duracion){
	if (veces == 0 || duracion == 0) {
		return;
	}
	pitidos   = veces;
	largo     = duracion;
	cuenta    = duracion;
	encendido = 1;
	_BUZZER_PORT |= (1 << _BUZZER_PIN);
}

void buzzer_detener(void){
	pitidos   = 0;
	encendido = 0;
	_BUZZER_PORT &= ~(1 << _BUZZER_PIN);
}

uint8_t buzzer_sonando(void){
	if (pitidos == 0) {
		return 0;
	}
	return 1;
}

void buzzer_actualizar(void){
	if (pitidos == 0) {
		return;
	}

	cuenta--;
	if (cuenta != 0) {
		return;
	}

	if (encendido == 1) {                       // termino un pitido
		_BUZZER_PORT &= ~(1 << _BUZZER_PIN);
		encendido = 0;
		pitidos--;
		cuenta = BUZZER_PAUSA;
	} else {                                    // termino la pausa: sigue el otro pitido
		_BUZZER_PORT |= (1 << _BUZZER_PIN);
		encendido = 1;
		cuenta = largo;
	}
}
