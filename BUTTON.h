#ifndef __BUTTON_H
#define __BUTTON_H
#include <stm32f429xx.h>
#include "LED.h"

#define Blue_Button 13 
void Buttons_init (void);   			//Initialise the blue button
void interrupt_init(void);				//Initialise the interrupts for the blue button
void EXTI15_10_IRQHandler(void);	//Interrupt Handler for blue button
void Init_LED(void);							//Initialises the onboard LEDs
void Hold(void);									//Function that is called when the blue button is held for more than 1 second
#endif