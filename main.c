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
    Init_Timer2();
    Init_USART();
		BUZZ_INIT();
    
    // Use only the RED LED
    OFFBOARD_LED_ON(RED_LED);
    
    // Initialize heartbeat detection
    ADC_HeartRate_Init();
    
    unsigned int lastDisplayTime = 0;
    char msg[30];
    
    while(1)
    {
        // Process one ADC sample for heartbeat detection (call at 1ms rate)
        ADC_HeartRate_Update();
        
        // Every 500ms, update the display and USART output with the current BPM
        if((timer_tick - lastDisplayTime) >= 1000)
        {
            lastDisplayTime = timer_tick;
					
					// Clear the line by setting the cursor to the line and printing spaces.
						cmdLCD(LCD_LINE1);
						printLCD("                ");  // Adjust the number of spaces to cover the line
					
					// Now print the new BPM value.
            sprintf(msg, "BPM: %u", ADC_Get_HeartRateBPM());
            cmdLCD(LCD_LINE1);
            printLCD(msg);
					
            sprintf(msg, "BPM: %u\r\n", ADC_Get_HeartRateBPM());
            send_string(msg);
        }
        
        // Wait approximately 1ms before processing the next ADC sample.
        Delay(1);
    }
}
