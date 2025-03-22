#ifndef __LED_H
#define __LED_H
#include <stm32f429xx.h>
#include "TIM.h"

void LED_INIT (void);
void ONBOARD_LED_ON (char LED);
void ONBOARD_LED_OFF (char LED);
void OFFBOARD_LED_ON (char LED);
void OFFBOARD_LED_OFF (char LED);
#define OFFBOARD_LED_PORT GPIOD
#define IR_LED 15
#define RED_LED 14

extern unsigned int adcVal;

#define LED_D0 2
#define LED_D1 3
#define LED_D2 4
#define LED_D3 5
#define LED_D4 6
#define LED_D5 7
#define LED_D6 8
#define LED_D7 9
#define LED_BAR_OE 0 // !OE for RGB Bar
#define PE12 12 // Red Latch
#define PE13 13 // Green Latch
#define PE14 14 // Blue Latch
#define LED_DIG_LE1 10
#define LED_DIG_LE2 11
#define LED_DIG_OE 15
#define LDIGIT 10
#define RDIGIT 11

void SEG_INIT(void);
void Write_LEDBus (char data1, char data2);
void setDigit (char data, char Latch);
	
//RGB BAR functions
void BAR_INIT(void);
void Write_RGB_LEDBus(uint8_t red, uint8_t green, uint8_t blue);
void update_RGB_bar_from_HR(void);
#endif
