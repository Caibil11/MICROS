#include <avr/io.h>
#define F_CPU 20000000UL
#include <util/delay.h>
int main(void)
{
   DDRA =0xFF;
   DDRB &= ~(1<<0);
   
   PORTA = 0xAA;
   PORTA = ~0xAA;
   _delay_ms(1000);
   
   //PORTA ^=  ~((1<<7)|(1<<2)|(1<<3)|(1<<6));
   //PORTA ^=  ((1<<3)|(1<<4));
   
    while (1)
	{
		
		if ((PINB & (1<<0) == 0)
		{
			
		}
	   /*PORTA ^=0xFF;
	   _delay_ms(500);*/
		
    }
   
  // PORTA = PORTA | (1<<PORTA6); // Prender un solo Bit en el puerto 
  //PORTA = PORTA |( (1<<PORTA6) | (1<<PORTA5) | (1<<PORTA4)); //Prender los bits que se me de la gana;
   
   /*for (i = 0; i < 7; i++)
   {
	   // PORTA=PORTA | 0b10000001;
	   PORTA = PORTA | (1<<i); //Llenar los bits
	   _delay_ms(1000);
	   
	   /*PORTA = (1<<i); // Se desplaze el bit
	   _delay_ms(1000);
	   
   }
   */
	   
	   
   
  
}

