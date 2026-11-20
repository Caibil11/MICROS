/*
 * Conversores.c
 *
 * Created: 16/10/2026 9:39:25 a. m.
 * Author : dylan
 */ 
#define F_CPU 20000000UL 
#include <avr/io.h> 
#include <util/delay.h> 
#include "ADC/ADC.h" 

int conver=0; 

int main(void) { 
	ADC_init(); 
	DDRB = 0xFF; 
	DDRD = 0xFF; 
	
	while (1) { 
		conver = ADC_read(0); 
		PORTB = conver; 
		PORTD = conver>>8; 
		_delay_ms(50); 
		} 
	}