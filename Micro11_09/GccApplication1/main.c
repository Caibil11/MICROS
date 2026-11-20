#include <avr/io.h>
#define F_CPU 20000000UL
#include <util/delay.h>
#include <stdint.h>

uint8_t decoCC[10]={191,134,219,207,230,237,253,135,255,231};
uint8_t decoAC[10]={64,121,36,48,25,18,2,120,0,24};
	
int main(void)
{
	DDRA = 0xFF;
	PORTA = decoCC[5];
   
    while (1)
	{
	 
		
    }
   
}
