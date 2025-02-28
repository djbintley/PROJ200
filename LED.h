#ifndef __LED_H
#define __LED_H
#include <stm32f429xx.h>

void LEDs_INIT (void);
void ONBOARD_LED_ON (char LED);
void ONBOARD_LED_OFF (char LED);
void OFFBOARD_LED_ON (char LED);
void OFFBOARD_LED_OFF (char LED);
#define OFFBOARD_LED_PORT GPIOD
#define IR_LED 15
#define RED_LED 14
#endif
