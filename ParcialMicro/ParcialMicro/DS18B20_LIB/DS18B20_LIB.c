#include "DS18B20_LIB.h"

uint8_t ds18b20_reset(void);
void ds18b20_write_bit(uint8_t bit);
uint8_t ds18b20_read_bit(void);


void ds18b20_init(void){
	DS18B20_DDR  |= (1<<DS18B20_DQ); //CONFIGRUAR EL PIN DQ COMO SALIDA
	DS18B20_PORT |= (1<<DS18B20_DQ); //ESTABLECER EL ESTADO LOGICO EN ALTO
}

// Devuelve 1 si el sensor respondio con el pulso de presencia, 0 si no hay sensor
uint8_t ds18b20_reset(void){
	uint8_t espera   = 0;
	uint8_t presente = 1;

	DS18B20_DDR  |=  (1<<DS18B20_DQ); //CONFIGRUAR EL PIN DQ COMO SALIDA
	DS18B20_PORT &= ~(1<<DS18B20_DQ); //ESTABLECER EL ESTADO LOGICO EN BAJO
	_delay_us(480);
	DS18B20_PORT |= (1<<DS18B20_DQ); //ESTABLECER EL ESTADO LOGICO EN ALTO
	DS18B20_DDR &= ~(1<<DS18B20_DQ); //CONFIGRUAR EL PIN DQ COMO ENTRADA
	_delay_us(60);

	//ESPERA EL PULSO DE PRESENCIA (maximo 100 us: si no hay sensor no se queda pegado aqui)
	while ((DS18B20_PIN & (1<<DS18B20_DQ)) && espera < 100) {
		_delay_us(1);
		espera++;
	}
	if (espera == 100) {
		presente = 0;
	}
	_delay_us(480);
	return presente;
}

void ds18b20_write_bit(uint8_t bit){
	DS18B20_DDR  |=  (1<<DS18B20_DQ); //CONFIGRUAR EL PIN DQ COMO SALIDA
	DS18B20_PORT &= ~(1<<DS18B20_DQ); //ESTABLECER EL ESTADO LOGICO EN BAJO
	_delay_us(1);
	if(bit){
		DS18B20_PORT |= (1<<DS18B20_DQ);
	}
	_delay_us(60);
	DS18B20_PORT |= (1<<DS18B20_DQ); //REESTABLECER EL BUS
}

uint8_t ds18b20_read_bit(void){
	uint8_t bit = 0;
	DS18B20_DDR |= (1<<DS18B20_DQ);  //CONFIGRUAR EL PIN DQ COMO SALIDA
	DS18B20_PORT &= ~(1<<DS18B20_DQ); //ESTABLECER EL ESTADO LOGICO EN BAJO
	_delay_us(1);
	
	DS18B20_DDR &= ~(1<<DS18B20_DQ); //CONFIGRUAR EL PIN DQ COMO SALIDA
	_delay_us(14);
	
	if( DS18B20_PIN & (1<<DS18B20_DQ) ){
		bit = 1;
	}
	_delay_us(45);
	return bit;
}

void ds18b20_write(uint8_t byte){
	
	for(uint8_t i=0; i<=7; i++){
		ds18b20_write_bit(byte & 0x01);
		byte >>=1;
	}
}

uint8_t ds18b20_read(){
	uint8_t  data=0;
	for( uint8_t i=0; i<=7; i++){
		data >>=1;
		if( ds18b20_read_bit() ){
			data |= 0x80;
		}
	}
	return data;
}

float ds18b20_get_temp(void){
	uint8_t temp_lsb, temp_msb;
	int16_t temp;//Con uint16_t: antes veiamos es 65535 x 0.0625 = 4095.9
	//	Con int16_t:-1X 0.0625=-0.0625
	
	ds18b20_reset();
	ds18b20_write(0xCC); //SKIP ROOM
	ds18b20_write(0x44); //START CONVER
	
	_delay_ms(750);                 //la conversion de 12 bits tarda hasta 750 ms
	
	ds18b20_reset();
	ds18b20_write(0xCC);
	ds18b20_write(0xBE);  //LEER LA MEMORIA
	
	temp_lsb = ds18b20_read();
	temp_msb = ds18b20_read();
	
	temp = (temp_msb << 8) | temp_lsb;
	
	return temp*0.0625;
	
	
}


// ---------- Version sin bloqueo ----------

// Le dice al sensor que empiece a medir. La medida queda lista en unos 750 ms.
uint8_t ds18b20_iniciar_conv(void){

	if (ds18b20_reset() == 0) {
		return 0;
	}
	ds18b20_write(0xCC); //SKIP ROM
	ds18b20_write(0x44); //START CONVERSION
	return 1;
}

// Lee la ultima medida que termino el sensor
float ds18b20_leer_temp(void){
	uint8_t temp_lsb, temp_msb;
	int16_t temp;

	if (ds18b20_reset() == 0) {
		return DS18B20_SIN_SENSOR;
	}
	ds18b20_write(0xCC);
	ds18b20_write(0xBE);  //LEER LA MEMORIA

	temp_lsb = ds18b20_read();
	temp_msb = ds18b20_read();

	temp = (temp_msb << 8) | temp_lsb;

	return temp*0.0625;
}
