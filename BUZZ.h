#ifndef	__BUZZ_H
#define __BUZZ_H
#include <stm32f429xx.h>

#define Buzzer 		13
#define Buzzon 		GPIOB->BSRR=1<<13
#define Buzzoff 	GPIOB->BSRR=1<<13<<16

void BUZZ_INIT(void); 					//Initialise Buzzer and Timer 7
void TIM7_IRQHandler(void);			//Interrupt handler for Timer7

extern unsigned int beep;
#endif