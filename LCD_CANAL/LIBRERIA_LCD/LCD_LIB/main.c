/*
 * LCD_LIB.c
 *
 * Created: 27/09/2026 7:34:11 p. m.
 * Author : Daniel Millán
 */ 
#define  F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include "LCD/LCD_LIB.h"



int main(void)
{
  
lcd_init();
lcd_printf("temp=%d hum=%d" ,25,30);
lcd_puts("dakdkakdbask");
   
    while(1) 
    {
		

    }
}

