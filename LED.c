#include "LED.h"

// Function to initialise On board LEDs
void LEDs_INIT (void)
{
	//ENABLE PORT
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;	//ONLY GPIO B clock enable
	

	// Clear the three GPIOB ports
	GPIOB->MODER &= ~(0x03<<(2*0));	//ONLY clear GPIOB0
	GPIOB->MODER &= ~(0x03<<(2*7)); //Clear GPIOB7
	GPIOB->MODER &= ~(0x03<<(2*14)); //Clear GPIOB14
	
	//CONFIGURE PORTS:GPIOB  PINS:0,7,14 TO OUTPUT for LED1,LED2 and LED3
	GPIOB->MODER |= 0x01<<(2*0);		//ONLY set  GPIOB0
	GPIOB->MODER |= 0x01<<(2*7);		//Only set GPIOB7 as Alternative Function
	GPIOB->MODER |= 0x01<<(2*14);		//Only set GPIOB14
		
	// Set output type to push-pull
	GPIOB->OTYPER &= ~(0x1 << (0*2));
	GPIOB->OTYPER &= ~(0x1 << (7 * 2));
	GPIOB->OTYPER &= ~(0x1 << (14 * 2));
	
	// No pull-up, pull-down
	GPIOB->PUPDR &= ~(0x3 << (0 * 2));
	GPIOB->PUPDR &= ~(0x3 << (7 * 2));
	GPIOB->PUPDR &= ~(0x3 << (14 * 2));
}	

void ONBOARD_LED_ON (char LED){
			GPIOB->BSRR = (1<<LED);							//ONLY TURN SPECIFIED LED ON
}

void ONBOARD_LED_OFF (char LED){
			GPIOB->BSRR = 1<<LED<<16;							//ONLY TURN SPECIFIED LED Off
}

void OFFBOARD_LED_ON (char LED){
			GPIOD->BSRR = (1<<LED);							//ONLY TURN SPECIFIED LED ON
}

void OFFBOARD_LED_OFF (char LED){
			GPIOD->BSRR = 1<<LED<<16;							//ONLY TURN SPECIFIED LED Off
}
