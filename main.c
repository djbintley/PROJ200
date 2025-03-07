#include <stm32f429xx.h>
#include "LED.h"
#include "DAC.h"
#include "LCD.h"
#include "ADC.h"
#include "stdio.h"
#include "USART.h"
#include "TIM.h"
#include "PLL_Config.c"

int main(void){
	PLL_Config();									// Set system clock to 180MHz
	SystemCoreClockUpdate();			// Update SystemCoreClock
	LED_INIT();
	Init_ADC();
	DAC_INIT();
	LCD_INIT();
	Init_Timer2();
	Init_USART();

	OFFBOARD_LED_ON (IR_LED);
	DAC2_DC(3);
	SHOWHR(50);
	while(1){
		display_volts();
		/*
		OFFBOARD_LED_ON (RED_LED);
		ONBOARD_LED_ON (14);
		OFFBOARD_LED_ON (IR_LED);
		Delay(8);
		OFFBOARD_LED_OFF (RED_LED);
		ONBOARD_LED_OFF (14);
		OFFBOARD_LED_OFF (IR_LED);
		*/
		Delay(8);
		
	}
}
