#ifndef TIM_H
#define TIM_H
#include "stm32f4xx.h"

void Init_Timer2(void); // Initialize Timer 2 - currently at 100ms
void Delay(uint32_t ms);
void TIM2_IRQHandler(void);

#endif
