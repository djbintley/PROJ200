#include <stm32f429xx.h>
#include "LED.h"
#include "DAC.h"
#include "LCD.h"
#include "ADC.h"
#include "USART.h"
#include "TIM.h"
#include "BUZZ.h"
#include "Button.h"
#include "PLL_Config.c"
#include "stdio.h"

extern uint32_t timer_tick;
extern unsigned int adcVal;


int main(void)
{
    // Set up the system clock and update core frequency
    PLL_Config();
    SystemCoreClockUpdate();
    
    // Initialize peripherals
    LED_INIT();
    Init_ADC();
    DAC_INIT();
    LCD_INIT();
		BAR_INIT();
    Init_Timer2();
    Init_USART();
		BUZZ_INIT();
		interrupt_init();
		Buttons_init();
    
    // Use only the RED LED
    OFFBOARD_LED_ON(RED_LED);
		DAC2_DC(4000);
	
    // Initialize heartbeat detection
    ADC_HeartRate_Init();
    
    unsigned int lastDisplayTime = 0;
    
    hide_cursor();
		display_menu();
		
    while(1)
    {
        // Process one ADC sample for heartbeat detection (call at 1ms rate)
        ADC_HeartRate_Update();
        // Every 1000ms, update the display and USART output with the current BPM
        if((timer_tick - lastDisplayTime) >= 500)
        {
            lastDisplayTime = timer_tick;
				    buffdate();
						update_LCD();
					  update_menu();
						update_RGB_bar_from_HR();
        }
        
        // Wait approximately 1ms before processing the next ADC sample.
        Delay(1);
    }
}
