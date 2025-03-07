#include "ADC.h"
#include "LCD.h"
#include "stdio.h"
#include "USART.h"

static char buffer[5];

void Init_ADC(void)
{
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOAEN;	//GPIOC clock enable
	ADC_input_port->MODER|=(3u<<(2*ADC_input_pin));	//ADC input pin is analogue mode
	
	RCC->APB2ENR|=RCC_APB2ENR_ADC1EN;		//ADC clock enable
	ADC1->SQR1&=~ADC_SQR1_L;						//set number of conversions per sequence to 1
	ADC1->SQR3&=~ADC_SQR3_SQ1;					//clear channel select bits
	ADC1->SQR3|=ADC_Channel;						//set channel
	ADC1->CR2|=ADC_CR2_ADON;						//enable ADC
}

unsigned int read_adc(void)
{
	ADC1->CR2|=ADC_CR2_SWSTART;				//start ADC conversion
	while((ADC1->SR&ADC_SR_EOC)==0){__NOP();}	//wait for ADC conversion complete
	return ADC1->DR;									//return converted value
	uint16_t pot_value = ADC1->DR;
  float pot_voltage = (pot_value * 3.3) / 4095; // Convert to voltage
	send_string("POT(v): ");
	sprintf(buffer, "%.3f", pot_voltage);
	send_string(buffer);
}



