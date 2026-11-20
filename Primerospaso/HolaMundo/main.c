/*
 * HolaMundo.c
 *
 * Created: 4/09/2026 9:43:40 a. m.
 * Author : Jatemon
 */ 

#define F_CPU 20000000UL
#include <avr/io.h>
#include <util/delay.h>
/*#include <avr/interrupt.h>*/

int main(void)
{
	//SETUP
	/*DDRA =0xFF;
	DDRA = 255;
	DDRA = 0377;*/
	
	DDRA = 0b11111111; //Configuramos el PTA como salida;
	DDRB = 0b00000000; //PB0 como entrada;
	int status = 0;
	
	while (1){ //LOOP
		
		if (PINB & (1 << PB0))
		{
			status++;
			if (status > 3)
			status = 1;
			while (PINB & (1 << PB0)); // antirrebote
		}
		
		//Secuencia 1
		if( status == 1 ){
			
			PORTA = 0b10101010;
			for (int i = 0; i < 300; i++){
				_delay_ms(1);
				if (PINB & (1 << PB0)) break;
			}
			
			PORTA = 0b01010101;
			for (int i = 0; i < 300; i++){
				_delay_ms(1);
				if (PINB & (1 << PB0)) break;
			}
			
			PORTA = 0b10011001;
			for (int i = 0; i < 200; i++){
				_delay_ms(1);
				if (PINB & (1 << PB0)) break;
			}
			
			PORTA = 0b01100110;
			for (int i = 0; i < 400; i++){
				_delay_ms(1);
				if (PINB & (1 << PB0)) break;
			}
		}
		
		//Secuencia 2;
		if( status == 2){
			
			PORTA = 0b11110000;
			for (int i = 0; i < 150; i++){
				_delay_ms(1);
				if (PINB & (1 << PB0)) break;
			}
			
			PORTA = 0b00001111;
			for (int i = 0; i < 150; i++){
				_delay_ms(1);
				if (PINB & (1 << PB0)) break;
			}
			
			PORTA = 0b01100110;
			for (int i = 0; i < 200; i++){
				_delay_ms(1);
				if (PINB & (1 << PB0)) break;
			}
			
			PORTA = 0b10011001;
			for (int i = 0; i < 200; i++){
				_delay_ms(1);
				if (PINB & (1 << PB0)) break;
			}
		}
		
		//Secuencia 3
		if (status == 3)
		{
			PORTA = 0b11001100;
			for (int i = 0; i < 200; i++){
				_delay_ms(1);
				if (PINB & (1 << PB0)) break;
			}
			
			PORTA = 0b00110011;
			for (int i = 0; i < 200; i++){
				_delay_ms(1);
				if (PINB & (1 << PB0)) break;
			}
			
			PORTA = 0b10010010;
			for (int i = 0; i < 100; i++){
				_delay_ms(1);
				if (PINB & (1 << PB0)) break;
			}
			
			PORTA = 0b01101101;
			for (int i = 0; i < 100; i++){
				_delay_ms(1);
				if (PINB & (1 << PB0)) break;
			}
		}
	}
}