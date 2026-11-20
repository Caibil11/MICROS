#define F_CPU 20000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#include "Display7/display_7seg.h"

int main(void){
	
	Display7Seg_Init();
	
	while(1){
		for(int i=0; i<=9; i++){
			set_value_decoCC(i);
			_delay_ms(100);
		}
	}
}


	/*while (1)
	{	for (int i=0; i<=9; i++)
	{
		PORTA = decoCC[i];
		_delay_ms(100);*/
		
	

