#include "BUTTON.h"

void interrupt_init(void){
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;		//ONLY Enable the SYSCFG clock

  SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;  // Connect  EXTI13 to GPIOC

																				// Configure EXTI Line 13 for interrupt
  EXTI->IMR |= EXTI_IMR_MR13;  					// Unmask EXTI13
  EXTI->FTSR |= EXTI_FTSR_TR13;  				// Trigger on falling edge (button press)
  EXTI->RTSR |= EXTI_RTSR_TR13;  				// Trigger on rising edge (button release)

    
  NVIC_EnableIRQ(EXTI15_10_IRQn);			// Enable EXTI line 13 interrupt in NVIC
}

void Buttons_init (void){										//Function to initialise the MoSuB buttons
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;			// ONLY GPIO B clock enable
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;			// ONLY GPIO G clock enable
	
	GPIOC->MODER &= ~(3UL<<(2*Blue_Button));	// ONLY clear GPIOB bit so now is set as input
	GPIOC->OTYPER &= ~(1UL<<(Blue_Button));		// Put in default push pull mode
	GPIOC->OSPEEDR &= ~(3UL<<(Blue_Button));	// Default state SLOW	
	GPIOC->PUPDR &= ~(3UL<<(2*Blue_Button));	// ONLY clears BlueButton PUPDR bit
	interrupt_init();													// Initialises the Interrupt for the Blue Button
	
	GPIOG->MODER &= ~(												// ONLY clear GPIOG bits also sets mode to input
		3UL<<(2*0)|	
		3UL<<(2*1)|
		3UL<<(2*2)|
		3UL<<(2*3));	
	GPIOG->PUPDR &= ~(												// ONLY Clear PUPDR
		3UL<<(2*0)|	
		3UL<<(2*1)|
		3UL<<(2*2)|
		3UL<<(2*3));		
	GPIOG->PUPDR |= (
		2UL<<(2*0)|
		2UL<<(2*1)|
		2UL<<(2*2)|
		2UL<<(2*3));														// ONLY Sets the pins to have pull-down resistors
}

void Hold(void){
	GPIOB->BSRR = (1<<14);										// Set Red Onboard LED to high (on)
}
int pressed = 0;
int start = 0;
void EXTI15_10_IRQHandler(void){						// Interrupt handler for EXTI Line 13 (Blue Button)
	if (EXTI->PR & (1 << Blue_Button)) {  		// Check if EXTI13 caused the interrupt		
		if (pressed == 0) {											// Button Pressed down
			GPIOB->BSRR = (1u<<0);								// Blue LED on
			pressed = 1;
			start = TIM4->CNT;										// Capture time when button is pressed
		}else{																	// Button Released
			GPIOB->BSRR = (1u<<0<<16);						// Blue LED off
			pressed = 0;
			if(((TIM4->CNT)-start)> 2000)Hold();	// If the blue button has been held for longer than 1 second, call the Hold function
		}
	}
	EXTI->PR |= (1 << Blue_Button);  					// Clear the pending interrupt
}



void Toggle_LED (char LED)
{
	GPIOB->ODR^=(1u<<LED);								// XOR GPIOB output data register to invert the selected pin
}
