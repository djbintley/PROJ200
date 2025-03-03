#ifndef __DAC_H
#define __DAC_H
#include <stm32f429xx.h>


#define DAC_PORT	GPIOA	 // DAC Port
#define DAC_PIN		5		 // DAC Channel 2 pin

void DAC_INIT(void);							// DAC pins initialiser
void DAC2_DC(unsigned short d);		// Variable DC generator

#endif
	