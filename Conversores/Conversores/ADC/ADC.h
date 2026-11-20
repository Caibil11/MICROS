/*
 * ADC.h
 *
 * Created: 16/10/2026 9:40:18 a. m.
 *  Author: dylan
 */ 
#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
#include <stdint.h>

#define ADCA0 0
#define ADCA1 1
#define ADCA2 2
#define ADCA3 3
#define ADCA4 4
#define ADCA5 5
#define ADCA6 6
#define ADCA7 7

#define _DIV2A   0
#define _DIV2B   1
#define _DIV4    2
#define _DIV8    3
#define _DIV16   4
#define _DIV32   5
#define _DIV64   6
#define _DIV128  7

#define MAX_CONV  1023.0

void ADC_init();
uint16_t ADC_read(uint8_t channel);
float ADC_Map(int conv1, float conv_min, float conv_max, float sal_min, float sal_max);




#endif /* ADC_H_ */