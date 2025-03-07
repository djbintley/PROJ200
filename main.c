#include <stm32f429xx.h>
#include "LED.h"
#include "DAC.h"
#include "LCD.h"
#include "ADC.h"
#include "stdio.h"
#include "USART.h"
#include "TIM.h"
#include "PLL_Config.c"

void delay3(int x){								// delay a specified number of milliseconds
	volatile unsigned int i = 0;			// Create a local variable "i"
	while(i<SystemCoreClock/(36000/x)){			// 
		i++;														// increment "i"
	}
}

static char buffer[5];
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
int main(void){
	LED_INIT();
	Init_ADC();
	DAC_INIT();
	LCD_INIT();
	Init_Timer2();
	Init_USART();
	PLL_Config();									// Set system clock to 180MHz
	SystemCoreClockUpdate();			// Update SystemCoreClock

	DAC2_DC(3);
	SHOWHR(50);
	while(1){
		display_volts();
		Delay(2000);
		/*
		OFFBOARD_LED_ON (RED_LED);
		ONBOARD_LED_ON (14);
		OFFBOARD_LED_ON (IR_LED);
		SHOWHR(read_adc());
		Delay(20);
		OFFBOARD_LED_OFF (RED_LED);
		ONBOARD_LED_OFF (14);
		OFFBOARD_LED_OFF (IR_LED);
		Delay(80);
		*/
	}
}
