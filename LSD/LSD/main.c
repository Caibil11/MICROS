#define F_CPU 20000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "LCD/LCD_LIB.h"

int main(void){
	
	// Inicializamos la LCD
	lcd_init();
	
	// Escribimos en la fila 1
	lcd_set_cursor(1,1);
	lcd_puts("Hola Mundo");
	
	// Escribimos en la fila 2
	lcd_set_cursor(2,1);
	lcd_puts("ATmega1284p");
	
	// Escribimos en la fila 3
	lcd_set_cursor(3,1);
	lcd_puts("LCD 20x4 OK");
	
    while (1) 
    {
    }
}

