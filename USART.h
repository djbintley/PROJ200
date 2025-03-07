#ifndef USART_H
#define USART_H
#include "stm32f4xx.h"
#include "TIM.h"
#include "ADC.h"

#define USART_PORT            GPIOD
#define USART_TX_pin    8
#define USART_RX_pin    9
#define BAUDRATE              230400

//SCREEN RESET
#define RESET_SCREEN "\033[2J\033[H" // Reset screen to display new data

//USART Function list
void Init_USART(void); // Initialisation sequence for USART
void send_usart(unsigned char d);
void send_string(char *data);
void display_volts();


#endif
