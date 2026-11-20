#define F_CPU 20000000UL
#include <avr/io.h>
#include <util/delay.h>

//SE INCLUYEN LAS LIBRERIAS
#include "ADC/ADC_LIB.h"
#include "LCD/LCD_LIB.h"

//MAPIAR MIS SENSORES, ACTUADORES, INDICADORES
#define LED_DDR			DDRB
#define LED_PORT		PORTB
#define LED_UBI			2

#define MOTOR_DDR		DDRB
#define MOTOR_PORT		PORTB
#define MOTOR_UBI		3

#define LM35			0
#define MCP9700A		1	
#define POT				2
#define LDR				3

#define BTN_SEL_DDR     DDRD 
#define BTN_SEL_UBI     0
#define BTN_SEL_PIN     PIND

#define BTN_UP_DDR      DDRD
#define BTN_UP_UBI      1
#define BTN_UP_PIN      PIND

#define BTN_DOWN_DDR    DDRD
#define BTN_DOWN_UBI    2
#define BTN_DOWN_PIN    PIND

void menuSensores(void);
void menuActuadores(void);
void menuPpal(void);

enum{
	mmppal,
	menuactua,
	menusens
};

float lm35value=150.0;
float potvalue;
float mcpvalue=150.0;
float ldrvalue;

int conver, op=1, current_op=mmppal;

int main(void)
{
    ADC_Init();
	lcd_init();
	lcd_disable_cursor();
	lcd_disable_blink();
	
	//CONFIGURAMOS LOS BTN COMO ENTRADA
	BTN_SEL_DDR  &= ~(1<<BTN_SEL_UBI);
	BTN_DOWN_DDR &= ~(1<<BTN_DOWN_UBI);
	BTN_UP_DDR   &= ~(1<<BTN_UP_UBI);
	
	//CONFIGURAOMS EL MOTOR Y LED COMO SALIDA
	LED_DDR   |= (1<<LED_UBI);
	MOTOR_DDR |= (1<<MOTOR_UBI);
	
    while(1){
		
		switch(current_op){
			case mmppal:    menuPpal(); break;
			case menuactua: menuActuadores(); break;
			case menusens:  menuSensores(); break;
		}
		
		_delay_ms(50);
		
    }
}

void menuPpal(void){
	lcd_set_cursor(1,1); lcd_printf("     MENU PPAL      ");
	lcd_set_cursor(2,1); lcd_printf("1) Menu Sensores");
	lcd_set_cursor(3,1); lcd_printf("2) Menu Actuadores");
	lcd_set_cursor(4,1); lcd_printf("op: %d              ", op);
	
	switch(op){
		case 1: lcd_set_cursor(2,17); lcd_write('<');
		lcd_set_cursor(3,19); lcd_write(' ');
		break;
		case 2: lcd_set_cursor(2,17); lcd_write(' ');
		lcd_set_cursor(3,19); lcd_write('<');
		break;
	}
	
	if( (BTN_UP_PIN & (1<<BTN_UP_UBI) ) != 0 ){
		while( (BTN_UP_PIN & (1<<BTN_UP_UBI) ) != 0 );
		op++;
		if(op>2) op=1;
	}
	if( (BTN_DOWN_PIN & (1<<BTN_DOWN_UBI) ) != 0 ){
		while( (BTN_DOWN_PIN & (1<<BTN_DOWN_UBI) ) != 0 );
		op--;
		if(op<1) op=2;
	}
	
	if( (BTN_SEL_PIN & (1<<BTN_SEL_UBI) ) != 0 ){
		while( (BTN_SEL_PIN & (1<<BTN_SEL_UBI) ) != 0 );
		lcd_clear();
		 
		switch(op){
			case 2:  current_op=menuactua;op=1;   break;
			case 1:  current_op=menusens; op=1;break;
		}
		
	}
}

void menuSensores(void){
	
	conver = ADC_Read(LM35);
	lm35value =  150.0/308.0*conver;
	conver = ADC_Read(MCP9700A);
	mcpvalue =  165.0/337.92*conver-50.0;
	conver = ADC_Read(LDR);
	ldrvalue = conver/1024.0*100;
	conver = ADC_Read(POT);
	potvalue = conver/1024.0*5.0;
	lcd_set_cursor(1,1); lcd_printf("      SENSORES      ");
	lcd_set_cursor(2,1); lcd_printf("T1:%5.1f T2: %5.1f",lm35value,mcpvalue);
	lcd_set_cursor(3,1); lcd_printf("ldr:%5.1f%",ldrvalue);
	lcd_set_cursor(4,1); lcd_printf("POT:%5.1fV      BACK",potvalue);
	
	if( (BTN_SEL_PIN & (1<<BTN_SEL_UBI) ) != 0){
		while((BTN_SEL_PIN & (1<<BTN_SEL_UBI) ) != 0);
		lcd_clear();
		current_op = mmppal;
		op=1;
	}
}

void menuActuadores(void){
	lcd_set_cursor(1,1); lcd_printf("      ACTUADORES    ");
	lcd_set_cursor(2,1); lcd_printf("1) M.ON  2) M.OFF   ");
	lcd_set_cursor(3,1); lcd_printf("3) L.ON  4) L.OFF   ");
	lcd_set_cursor(4,1); lcd_printf("OP: %d   5) Back    ",op);
	
	switch(op){
		case 1: lcd_set_cursor(2,8);  lcd_write('<');
				lcd_set_cursor(2,18); lcd_write(' ');
				lcd_set_cursor(3,8);  lcd_write(' ');
				lcd_set_cursor(3,18); lcd_write(' ');
				lcd_set_cursor(4,19); lcd_write(' ');
				break;
		case 2: lcd_set_cursor(2,8);  lcd_write(' ');
				lcd_set_cursor(2,18); lcd_write('<');
				lcd_set_cursor(3,8);  lcd_write(' ');
				lcd_set_cursor(3,18); lcd_write(' ');
				lcd_set_cursor(4,19); lcd_write(' ');
				break;
		case 3: lcd_set_cursor(2,8);  lcd_write(' ');
				lcd_set_cursor(2,18); lcd_write(' ');
				lcd_set_cursor(3,8);  lcd_write('<');
				lcd_set_cursor(3,18); lcd_write(' ');
				lcd_set_cursor(4,19); lcd_write(' ');
				break;
		case 4: lcd_set_cursor(2,8);  lcd_write(' ');
				lcd_set_cursor(2,18); lcd_write(' ');
				lcd_set_cursor(3,8);  lcd_write(' ');
				lcd_set_cursor(3,18); lcd_write('<');
				lcd_set_cursor(4,19); lcd_write(' ');
				break;	
		case 5: lcd_set_cursor(2,8);  lcd_write(' ');
				lcd_set_cursor(2,18); lcd_write(' ');
				lcd_set_cursor(3,8);  lcd_write(' ');
				lcd_set_cursor(3,18); lcd_write(' ');
				lcd_set_cursor(4,19); lcd_write('<');
				break;
		
	}
	if( (BTN_UP_PIN & (1<<BTN_UP_UBI) ) != 0 ){
		while( (BTN_UP_PIN & (1<<BTN_UP_UBI) ) != 0 );
		op++;
		if(op>5) op=1;
	}
	if( (BTN_DOWN_PIN & (1<<BTN_DOWN_UBI) ) != 0 ){
		while( (BTN_DOWN_PIN & (1<<BTN_DOWN_UBI) ) != 0 );
		op--;
		if(op<1) op=5;
	}
	
	if( (BTN_SEL_PIN & (1<<BTN_SEL_UBI) ) != 0 ){
		while( (BTN_SEL_PIN & (1<<BTN_SEL_UBI) ) != 0 );
		
		switch(op){
			case 1:  MOTOR_PORT |= (1<<MOTOR_UBI);   break;
			case 2:  MOTOR_PORT &= ~(1<<MOTOR_UBI);   break;
			case 3:  LED_PORT |= (1<<LED_UBI);   break;
			case 4:  LED_PORT &= ~(1<<LED_UBI);   break;
			case 5:  lcd_clear();
					 op=1;
					 current_op = mmppal;
					 break;
		}
		
	}
	_delay_ms(100);
	
}