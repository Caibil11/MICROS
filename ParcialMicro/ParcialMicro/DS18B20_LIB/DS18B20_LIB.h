

#ifndef DS18B20_LIB_H_
#define DS18B20_LIB_H_


#ifndef F_CPU
#define F_CPU 20000000UL
#endif
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

//DEFINIR EL PUERTO Y EL PIN DEL SENSOR
#define DS18B20_PORT  PORTA
#define DS18B20_DDR   DDRA
#define DS18B20_PIN   PINA
#define DS18B20_DQ    PINA6

//DEFINIRMOS LOS PROTOTIPOS DE FUNCION

void ds18b20_init(void);
void ds18b20_write(uint8_t byte);
uint8_t ds18b20_read(void);
float ds18b20_get_temp(void);          // pide la medida y ESPERA 750 ms (bloquea el programa)

// Version sin bloqueo: se pide la medida, y 1 segundo despues se lee el resultado
#define DS18B20_SIN_SENSOR  -1000.0     // lo que devuelve ds18b20_leer_temp() si el sensor no responde
uint8_t ds18b20_iniciar_conv(void);    // devuelve 1 si el sensor respondio, 0 si no
float   ds18b20_leer_temp(void);



#endif