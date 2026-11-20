#include "display_7seg.h"


uint8_t decoCC[10]={191, 134, 219, 207, 230, 237, 253, 135, 255, 231 };
uint8_t decoAC[10]={64, 121, 36, 48, 25, 18, 2, 120, 0, 24 };

void Display7Seg_Init(void){
	display_ddr = 0xFF;
}
void  set_value_decoCC(char num){
	char val_deco = decoCC[num];
	
	if( (val_deco & 1)  == 0 ) display_port &= ~(1<<segA); else display_port |= (1<<segA);
	if( (val_deco & 2)  == 0 ) display_port &= ~(1<<segB); else display_port |= (1<<segB);
	if( (val_deco & 4)  == 0 ) display_port &= ~(1<<segC); else display_port |= (1<<segC);
	if( (val_deco & 8)  == 0 ) display_port &= ~(1<<segD); else display_port |= (1<<segD);
	if( (val_deco & 16) == 0 ) display_port &= ~(1<<segE); else display_port |= (1<<segE);
	if( (val_deco & 32) == 0 ) display_port &= ~(1<<segF); else display_port |= (1<<segF);
	if( (val_deco & 64) == 0 ) display_port &= ~(1<<segG); else display_port |= (1<<segG);
}
void set_value_decoAC(char num){
	char val_deco = decoAC[num];
	
	if( (val_deco & 1)  == 0 ) display_port &= ~(1<<segA); else display_port |= (1<<segA);
	if( (val_deco & 2)  == 0 ) display_port &= ~(1<<segB); else display_port |= (1<<segB);
	if( (val_deco & 4)  == 0 ) display_port &= ~(1<<segC); else display_port |= (1<<segC);
	if( (val_deco & 8)  == 0 ) display_port &= ~(1<<segD); else display_port |= (1<<segD);
	if( (val_deco & 16) == 0 ) display_port &= ~(1<<segE); else display_port |= (1<<segE);
	if( (val_deco & 32) == 0 ) display_port &= ~(1<<segF); else display_port |= (1<<segF);
	if( (val_deco & 64) == 0 ) display_port &= ~(1<<segG); else display_port |= (1<<segG);
}