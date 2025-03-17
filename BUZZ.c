#include "BUZZ.h"

void Init_BuzzBuzz(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;		// ONLY GPIO B clock enable	
	GPIOB->MODER &= ~(0x03UL<<(2*Buzzer));	// ONLY clear GPIOB bit	
	GPIOB->MODER |= 0x01UL<<(2*Buzzer);			// ONLY set  GPIOB bit
	GPIOB->OTYPER &= ~(1UL<<(Buzzer));			// Put in default push pull mode
	GPIOB->OSPEEDR &= ~(3UL<<(Buzzer));			// Default state SLOW
	GPIOB->PUPDR &= ~(3UL<<(Buzzer));	
}

void Init_Timer7(void){ 
	RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;		// Enable Timer Clock
	TIM7->PSC = 90 - 1;         					// Set prescaler
	TIM7->ARR = 478;       									// Set auto-reload value
	TIM7->DIER |= TIM_DIER_UIE;    				// Enable update interrupt
	TIM7->CNT=0;									 				// Zero timer counter
	TIM7->CR1 |= TIM_CR1_CEN;      				// Enable timer counter
	NVIC_EnableIRQ(TIM7_IRQn);						// Enable TIM7 interrupt in NVIC
}

void BUZZ_INIT(void){
	Init_BuzzBuzz();
	Init_Timer7();
}

unsigned int onoff = 0;	
unsigned int counter = 0;
unsigned int beep = 0;

void TIM7_IRQHandler(void) {													// TIM7 Update handler
	TIM7->SR &= ~TIM_SR_UIF;
	if (beep ==1) {
		if (counter < 383){
			counter++;
			if (onoff == 0){
					onoff = 1;
					GPIOB->BSRR = 1<<13;
					GPIOB->BSRR = 1<<7;
			}else{
					GPIOB->BSRR = 1<<13<<16;
					GPIOB->BSRR = 1<<7<<16;
					onoff = 0;
			}
		}else{
			counter = 0;
			beep = 0;
		}
	}
}
