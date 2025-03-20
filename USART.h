#ifndef USART_H
#define USART_H
#include "stm32f4xx.h"
#include "TIM.h"
#include "ADC.h"

extern char HR[30]; // buffer used to displate heartrate

#define USART_PORT            GPIOD
#define USART_TX_pin    8
#define USART_RX_pin    9
#define BAUDRATE              230400

// ANSI Colour Codes
#define COLOUR_RED   "\033[38;5;196m"   // RED
#define COLOUR_BLUE   "\033[38;5;21m"   // BLUE
#define COLOUR_YELLOW   "\033[38;5;226m"   // YELLOW
#define COLOUR_GREEN   "\033[38;5;46m"   // GREEN
#define COLOUR_WHITE  "\033[38;5;255m"  // White
#define COLOUR_RESET    "\033[0m" //Turn off text colour

//ANSI COLOUR FOR BG


//SCREEEN RESET
#define RESET_SCREEN "\033[2J\033[H" // Reset screen to display new data
#define RESET_LINE "\033[0K" // reset line after cursor
#define RESET_CURSOR "\033[H" //reset cursor location to home

//USART Function list
void Init_USART(void); // Initialisation sequence for USART
void send_usart(unsigned char d);
void send_string(char *data);
void display_volts();
void display_menu(void);
void update_menu(void);
void hide_cursor(void);


#endif
