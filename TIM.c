#include "TIM.h"


uint32_t timer_tick = 0; // Global variable to track timer ticks - used with timer 2

void Init_Timer2(void)
{
      RCC->APB1ENR|=RCC_APB1ENR_TIM2EN;         //timer 2 clock enabled
      TIM2->DIER|=TIM_DIER_UIE;                                   //timer update interrupt enabled
                                                                                                                  //APB clock is Fcy/2 = 180MHz/2 = 90MHz
      TIM2->PSC=9000-1;                                                       //Prescaler of 9000 => 90MHz/9000 = 10kHz clock
      TIM2->ARR=10-1;                                                           //Auto reload register counter reload value, gives a timer period of 1ms
      TIM2->CNT=0;                                                                        //zero timer counter
      NVIC->ISER[0]|=(1u<<28);                                    //timer 2 global interrupt enabled
      TIM2->CR1|=TIM_CR1_CEN;                                     //start timer counter
}

void Timer4_init(void)
{
    // Enable clock for Timer4
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

    // Set prescaler (assuming 84 MHz system clock, prescaler of 8399 gives 10 kHz)
    TIM4->PSC = 8999;

    // Set auto-reload value for 1-second overflow (at 10 kHz, this would be 9999)
    TIM4->ARR = 9999;

    // Enable update interrupt
    TIM4->DIER |= TIM_DIER_UIE;

    // Enable counter
    TIM4->CR1 |= TIM_CR1_CEN;

    // Enable TIM4 interrupt in NVIC
    NVIC_SetPriority(TIM4_IRQn, 1); // Adjust priority if necessary
    NVIC_EnableIRQ(TIM4_IRQn);
}

// External functions you will define elsewhere
extern void update_menu(void);
extern void update_LCD(void);

void TIM4_IRQHandler(void)
{
    if (TIM4->SR & TIM_SR_UIF) // Check update interrupt flag
    {
        TIM4->SR &= ~TIM_SR_UIF; // Clear interrupt flag
        
        // Call your update functions
        update_menu();
        update_LCD();
    }
}

void Delay(uint32_t ms) {
    uint32_t start_tick = timer_tick;       //Uses TIM2_IRQHandler
    while ((timer_tick - start_tick) < ms);
}

void TIM2_IRQHandler(void)                //TIMER 2 INTERRUPT SERVICE ROUTINE
{
      TIM2->SR&=~TIM_SR_UIF;                    //clear interrupt flag in status register
      //ADC_HeartRate_Update();
      timer_tick++;                                               // Increment tick counter
}


//BAD TIMER
void delay3(int x){								// delay a specified number of milliseconds
	volatile unsigned int i = 0;			// Create a local variable "i"
	while(i<SystemCoreClock/(36000/x)){			// 
		i++;														// increment "i"
	}
}