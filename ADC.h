#ifndef ADC_H
#define ADC_H
#include "stm32f4xx.h"
#define ADC_input_port		GPIOC
#define ADC_input_pin			3
#define ADC_BUFFER_SIZE 	3
#define ADC_Channel				13

void Init_ADC(void); // initalise ADC
unsigned int read_adc(void);	//Read ADC

#endif
