#include "DAC.h"

void DAC_INIT(void)
{
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOAEN;												//DAC port clock enable
	DAC_PORT->MODER|=(3u<<(2*DAC_PIN));												//DAC output pin set as analogue
	RCC->APB1ENR|=RCC_APB1ENR_DACEN;													//DAC clock enable
	DAC->CR|=DAC_CR_EN2;																			//DAC Channel 2 enabled
}

void DAC2_DC(unsigned short volts)	// Write a DC value to DAC channel 2
{
	DAC->DHR12R2=volts*1241;							//Write data byte to DAC 2 12 bit right aligned output register
}

