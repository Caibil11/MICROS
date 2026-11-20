#ifndef ADC_LIB_H_
#define ADC_LIB_H_

#define F_CPU 20000000UL
#include <avr/io.h>
#include <util/delay.h>



void ADC_Init();
int ADC_Read(char ch);
float mapeo(int conv, float x1, float y1, float x2, float y2);


#endif /* ADC_LIB_H_ */