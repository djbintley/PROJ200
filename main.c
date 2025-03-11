#include <stm32f429xx.h>
#include "LED.h"
#include "DAC.h"
#include "LCD.h"
#include "ADC.h"
#include "USART.h"
#include "TIM.h"
#include "PLL_Config.c"
#include "stdio.h"  // For sprintf

uint32_t timer_tick;

int main(void)
{
    // Configure the PLL and update the system clock before initializing peripherals.
    PLL_Config();              
    SystemCoreClockUpdate();

    // Initialize the peripherals.
    LED_INIT();
    Init_ADC();
    DAC_INIT();
    LCD_INIT();
    Init_Timer2();
    Init_USART();

    // Initialize the ADC heart rate processing.
    ADC_HeartRate_Init();

    // Turn on the IR LED.
    OFFBOARD_LED_ON(RED_LED);

    // Variables to help update the display periodically.
    unsigned int lastDisplayTime = 0;
    char msg[20];

    while(1)
    {
        // Call the heart rate update function every 1ms.
        // (Here, we use Delay(1) to approximate a 1ms period.)
        ADC_HeartRate_Update();

        // Update the display and USART output every 500ms.
        if ((timer_tick - lastDisplayTime) >= 500)
        {
            lastDisplayTime = timer_tick;
           
            // Retrieve the current heart rate in BPM.
            // (Assumes that the value is within an appropriate range.)
            unsigned int bpm = ADC_Get_HeartRateBPM();
           
            // Display heart rate on the LCD.
            // Note: SHOWHR() is defined in LCD.c. If needed, modify it to accept a wider range.
            SHOWHR((char)bpm);  
           
            // Format the heart rate into a message and send it via USART (to Putty).
            sprintf(msg, "HR: %u BPM\r\n", bpm);
            send_string(msg);
        }
       
        // Wait 1ms before taking the next sample.
        Delay(1);
    }
}

