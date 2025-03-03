#include <stm32f429xx.h>
#include "LED.h"
#include "DAC.h"
#include "LCD.h"

void delay3(int x){								// delay a specified number of milliseconds
	volatile unsigned int i = 0;			// Create a local variable "i"
	while(i<SystemCoreClock/(36000/x)){			// 
		i++;														// increment "i"
	}
}

int main(void){
	LED_INIT();
	DAC_INIT();
	LCD_INIT();
	DAC2_DC(3);
	SHOWHR(50);
	while(1){
		OFFBOARD_LED_ON (RED_LED);
		ONBOARD_LED_ON (14);
		delay3(200);
		OFFBOARD_LED_OFF (RED_LED);
		ONBOARD_LED_OFF (14);
		delay3(800);
	}
}
