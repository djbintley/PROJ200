#ifndef _LCD_H_
#define _LCD_H_
#define LCD_PORT	GPIOD
#define LCD_RS_pin	11
#define LCD_RW_pin	12
#define LCD_E_pin		13

#define LCD_D0_pin	0



#define LCD_LINE1		0x80
#define LCD_LINE2		0xc0

#define set_LCD_RS()	LCD_PORT->BSRR=(1u<<LCD_RS_pin)
#define clr_LCD_RS()	LCD_PORT->BSRR=(1u<<(LCD_RS_pin+16))
#define set_LCD_RW()	LCD_PORT->BSRR=(1u<<LCD_RW_pin)
#define clr_LCD_RW()	LCD_PORT->BSRR=(1u<<(LCD_RW_pin+16))
#define set_LCD_E()		LCD_PORT->BSRR=(1u<<LCD_E_pin)
#define clr_LCD_E()		LCD_PORT->BSRR=(1u<<(LCD_E_pin+16))

#define LCD_CLR()		cmdLCD(0x01)

#define set_LCD_bus_input()		LCD_PORT->MODER&=~(0xffff<<(2*LCD_D0_pin))
#define set_LCD_bus_output()	LCD_PORT->MODER|=(0x5555<<(2*LCD_D0_pin))

#define BUSY_FLAG_MASK (1U << 7)  // Bit 7 (Busy Flag).

#include <stm32f4xx.h>

void lcd_delayus(unsigned int us);
void WaitLcdBusy(void);
void set_LCD_data(unsigned char d);
void LCD_strobe(void);
void cmdLCD(unsigned char cmd);
void putLCD(unsigned char put);
void initLCD(void);
void setCursor(uint8_t line, uint8_t position);
void displayHeart(void);
void createHeartCharacter(void);

void writeMessage(char *message);
void send4BitData(uint8_t data, uint8_t isCommand);
void CrossCustomCharacter(void);
void createInverseHeartCharacter(void);

#endif
