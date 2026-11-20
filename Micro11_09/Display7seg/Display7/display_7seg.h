#ifndef DISPLAY7SEG_H_
#define DISPLAY7SEG_H_

#include <avr/io.h>

#define  display_port  PORTA
#define  display_ddr   DDRA

#define  display_port  PORTB
#define  display_ddr   DDRB

#define  display_port  PORTC
#define  display_ddr   DDRC

#define  display_port  PORTD
#define  display_ddr   DDRD

#define  segA   6
#define  segB   5
#define  segC   2
#define  segD   3
#define  segE   4
#define  segF   1
#define  segG   0

void Display7Seg_Init(void);
void set_value_decoCC(char num);
void set_value_decoAC(char num);

#endif