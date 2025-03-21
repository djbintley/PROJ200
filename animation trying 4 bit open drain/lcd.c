#include "lcd.h"

void lcd_delayus(unsigned int us) {
    unsigned char i;
    while (us--) {
        for (i = 0; i < SystemCoreClock / 4000000; i++) {
            __NOP();
        }
    }
}

void WaitLcdBusy(void) {
    set_LCD_bus_input();  // Set the data bus as input
    set_LCD_RW();         // Set R/W to 1 (read mode)
    clr_LCD_RS();         // Clear RS for command mode
    
    while (1) {
        set_LCD_E();  // Enable high
        lcd_delayus(1);
        if (!(LCD_PORT->IDR & BUSY_FLAG_MASK)) {
            clr_LCD_E();  // Reset Enable bit
            break;  // Exit loop if not busy
        }
        clr_LCD_E();  // Reset Enable bit
    }

    set_LCD_bus_output();  // Set the data bus back to output
}

void send4BitData(uint8_t data, uint8_t isCommand) {
    if (isCommand) {
        clr_LCD_RS();
    } else {
        set_LCD_RS();
    }
    clr_LCD_RW(); // Set write mode
    
    set_LCD_data((data & 0xF0));  // Send upper 4 bits
    LCD_strobe();
    set_LCD_data((data << 4));  // Send lower 4 bits
    LCD_strobe();
}

void cmdLCD(unsigned char cmd) {
    WaitLcdBusy();
    send4BitData(cmd, 1);
}

void putLCD(unsigned char put) {
    WaitLcdBusy();
    send4BitData(put, 0);
}


void writeMessage(char *message) {
    while (*message) {
        putLCD((unsigned char)*message);
        message++;
    }
}

void setCursor(uint8_t line, uint8_t position) {
    unsigned char address = (line == 0) ? (LCD_LINE1 + position) : (LCD_LINE2 + position);
    cmdLCD(address);
}

void set_LCD_data(unsigned char d) {
    LCD_PORT->BSRR = (0xF0 << (LCD_D0_pin + 16)); // Clear only upper 4 data lines
    LCD_PORT->BSRR = (d & 0xF0) << LCD_D0_pin;  // Set upper 4 bits
}

void LCD_strobe(void) {
    lcd_delayus(1);
    set_LCD_E();
    lcd_delayus(1);
    clr_LCD_E();
}


void initLCD(void) {
    // Update System Clock
    SystemCoreClockUpdate();
    // Enable the GPIOD clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

    // Delay for power stabilization (~40ms min as per the datasheet)
    lcd_delayus(45000);

    // Configure the GPIO pins for output
    LCD_PORT->MODER &= ~((3u << (2 * LCD_RS_pin)) | (3u << (2 * LCD_RW_pin)) | (3u << (2 * LCD_E_pin)) | (0xFFFF << (2 * LCD_D0_pin)));
    LCD_PORT->MODER |= ((1u << (2 * LCD_RS_pin)) | (1u << (2 * LCD_RW_pin)) | (1u << (2 * LCD_E_pin)) | (0x5555 << (2 * LCD_D0_pin)));

	
	lcd_delayus(45000);
	
    // Initialization sequence to enter 4-bit mode
    clr_LCD_RS();
    clr_LCD_RW();
    clr_LCD_E();

    // Step 1: Send 0x30 (three times as per datasheet, to reset the LCD)
    set_LCD_data(0x30); LCD_strobe(); lcd_delayus(41000); // Wait 5ms
    set_LCD_data(0x30); LCD_strobe(); lcd_delayus(41000);  // Wait 160us
    set_LCD_data(0x30); LCD_strobe(); lcd_delayus(41000);

    // Step 2: Set to 4-bit mode by sending 0x20
    set_LCD_data(0x20); LCD_strobe(); lcd_delayus(41000);

    // Step 3: Function Set (4-bit, 2-line, 5x8 dots)
    cmdLCD(0x28);

    // Step 4: Display ON/OFF control (display ON, cursor OFF, blink OFF)
    cmdLCD(0x0C);

    // Step 5: Display Clear
    cmdLCD(0x01);
    lcd_delayus(41000);  // Wait for display to clear

    // Step 6: Entry Mode Set (increment cursor, no display shift)
    cmdLCD(0x06);
		lcd_delayus(45000);
}





/*
void initLCD(void) {
	
    SystemCoreClockUpdate();
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Enable GPIOD clock
    
	lcd_delayus(46000);
	
    // Configure LCD GPIO Pins
    LCD_PORT->MODER &= ~((3u << (2 * LCD_RS_pin)) | (3u << (2 * LCD_RW_pin)) | (3u << (2 * LCD_E_pin)) | (0xFFFF << (2 * LCD_D0_pin)));
    LCD_PORT->MODER |= ((1u << (2 * LCD_RS_pin)) | (1u << (2 * LCD_RW_pin)) | (1u << (2 * LCD_E_pin)) | (0x5555 << (2 * LCD_D0_pin)));
    

	
    // LCD Initialization Sequence (4-bit Mode)
    clr_LCD_RS();
    clr_LCD_RW();
    clr_LCD_E();
    lcd_delayus(45000);
    
    set_LCD_data(0x30); LCD_strobe(); lcd_delayus(5000);
    set_LCD_data(0x30); LCD_strobe(); lcd_delayus(1000);
    set_LCD_data(0x30); LCD_strobe(); lcd_delayus(1000);
    set_LCD_data(0x20); LCD_strobe(); lcd_delayus(1000); // Set to 4-bit mode
    
    // Function Set: 4-bit, 2 Line, 5x7 Dots
    cmdLCD(0x28);
    // Display ON/OFF Control: Display ON, Cursor OFF, Blink OFF
    cmdLCD(0x0C);
    // Clear Display
    cmdLCD(0x01);
    lcd_delayus(2000);
    // Entry Mode Set: Increment Cursor, No Shift
    cmdLCD(0x06);
}
*/
/*
void CrossCustomCharacter(void)
{
	unsigned char customchar[8] = {
		0b00001,
		0b00011,
		0b00101,
		0b01001,
		0b01001,
		0b01011,
		0b11011,
		0b11000,
	};
	
	cmdLCD(0x40) ;
	
	for (int i = 0; i < 8; i++){
		putLCD(customchar[i]);
	}
	
	cmdLCD(0x80);
	
	putLCD(0x00);
}
*/

void createHeartCharacter(void) {
    unsigned char heart[8] = {
        0b00000,
        0b01010,
        0b11111,
        0b11111,
        0b01110,
        0b00100,
        0b00000,
        0b00000
    };

    cmdLCD(0x40); // Set CGRAM Address
    for (int i = 0; i < 8; i++) {
			putLCD(heart[i]);
        //putLCD(heart[i]);
    }
		
		//LCD_CLR();
		cmdLCD(0x80);
		putLCD(0x00);
		//while(1);
}

void createInverseHeartCharacter(void) {
    unsigned char inverse_heart[8] = {
        0b11111,
        0b10101,
        0b00000,
        0b00000,
        0b10001,
        0b11011,
        0b11111,
        0b11111
    };

    cmdLCD(0x48); // Set CGRAM Address for the second custom character
    for (int i = 0; i < 8; i++) {
        putLCD(inverse_heart[i]);
    }
		
    cmdLCD(0x80); // Move cursor to the first position
    putLCD(0x01); // Display the inverse heart character (CGRAM address 0x01)
}





void displayHeart(void) {
    putLCD(0x00); // Display custom character at CGRAM address 0x00
}

#include <stdio.h>
void displayNumber(int number) {
    char buffer[16];
    cmdLCD(LCD_LINE1);
    sprintf(buffer, "Dec: %d", number);
    writeMessage(buffer);
    cmdLCD(LCD_LINE2);
    sprintf(buffer, "Hex: %X", number);
    writeMessage(buffer);
}
