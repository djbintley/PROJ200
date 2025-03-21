#include "LCD.h"
#include "stdio.h"

void lcd_delayus(unsigned int us)		//blocking delay for LCD, argument is approximate number of micro-seconds to delay
{
	unsigned char i;
	while(us--)
	{
		for(i=0; i<SystemCoreClock/4000000; i++){__NOP();}
	}
}

void WaitLcdBusy(void)
{
	unsigned int port = 0x80;
	set_LCD_bus_input();
	set_LCD_RW();
	clr_LCD_RS();
	lcd_delayus(10); 								//Address Set-up Time
	while (port & 0x80){
		set_LCD_E();
		lcd_delayus(10);
		port= LCD_PORT->IDR;
		clr_LCD_E();
		lcd_delayus(10);
	}
	lcd_delayus(10);
	set_LCD_bus_output();

}

void printLCD(char *data){
	while(*data != 0){
		putLCD(*data);
		data++;
	}
}

void SHOWHR(char heartrate){
	char message[50];
  sprintf(message, "Heartrate = %d", heartrate);
	cmdLCD(LCD_LINE1);
	printLCD(message);

}

void set_LCD_data(unsigned int d)
{
	LCD_PORT->BSRR=(0xff<<(LCD_D0_pin+16));	//clear data lines
	LCD_PORT->BSRR=(d<<LCD_D0_pin);					//put data on lines
}

void LCD_strobe(void)		//10us high pulse on LCD enable line
{
	lcd_delayus(10);
	set_LCD_E();
	lcd_delayus(10);
	clr_LCD_E();
}


void cmdLCD(unsigned char cmd)		//sends a byte to the LCD control register
{
	WaitLcdBusy();				//wait for LCD to be not busy
	clr_LCD_RS();					//control command
	clr_LCD_RW();					//write command
	set_LCD_data(cmd);		//set data on bus
	LCD_strobe();					//apply command
}

void putLCD(unsigned char put)	//sends a char to the LCD display
{
	WaitLcdBusy();				//wait for LCD to be not busy
	set_LCD_RS();					//text command
	clr_LCD_RW();					//write command
	set_LCD_data(put);		//set data on bus
	LCD_strobe();					//apply command
}

void LCD_INIT(void)
{
		SystemCoreClockUpdate();
		RCC->AHB1ENR|=RCC_AHB1ENR_GPIODEN;	//enable LCD port clock
	
	
			//CONFIG LCD GPIO PINS
		LCD_PORT->MODER&=~(					//clear pin direction settings
			(3u<<(2*LCD_RS_pin))
			|(3u<<(2*LCD_RW_pin))
			|(3u<<(2*LCD_E_pin))
			|(0xffff<<(2*LCD_D0_pin))
			);
	
	
	LCD_PORT->MODER|=(				//reset pin direction settings to digital outputs
			(1u<<(2*LCD_RS_pin))
			|(1u<<(2*LCD_RW_pin))
			|(1u<<(2*LCD_E_pin))
			|(0x5555<<(2*LCD_D0_pin))
		);

	
			//LCD INIT COMMANDS
	clr_LCD_RS();					//all lines default low
	clr_LCD_RW();
	clr_LCD_E();
	
	lcd_delayus(25000);		//25ms startup delay
	cmdLCD(0x38);	//Function set: 2 Line, 8-bit, 5x7 dots
	cmdLCD(0x0c);	//Display on, Cursor blinking command
	cmdLCD(0x01);	//Clear LCD
	cmdLCD(0x06);	//Entry mode, auto increment with no shift
	cmdLCD(LCD_LINE1);
	createHeartCharacter();
	createInverseHeartCharacter();
	printLCD("Hello");
}

char TEMP[30]; //buffer used for Temperature

void update_LCD(void){
	if (Menu == 0){
		// Clear LCD
			cmdLCD(LCD_LINE1);
			printLCD("                ");  // Adjust the number of spaces to cover the line
		
		// Now print the new BPM value.
			sprintf(HR, "BPM: %u", getbpm());
			cmdLCD(LCD_LINE1);
			printLCD(HR);
	}
	if (Menu == 1) {
		// Clear LCD
			cmdLCD(LCD_LINE1);
			printLCD("                ");  // Adjust the number of spaces to cover the line			
		
		//print test
			cmdLCD(LCD_LINE1);
			int temp = 21;
			sprintf(TEMP, "Temperature = %d", temp);
			cmdLCD(LCD_LINE1);
			printLCD(TEMP);
	}
	if (Menu == 2) {
		// Clear LCD
			cmdLCD(LCD_LINE1);
			printLCD("                ");  // Adjust the number of spaces to cover the line			
		
		//print test2
			cmdLCD(LCD_LINE1);
			int ox = 98;
			sprintf(TEMP, "SpO2 = %d%%", ox);
			cmdLCD(LCD_LINE1);
			printLCD(TEMP);
		
	}
	
	if (Menu == 3) {
		// Clear LCD
			cmdLCD(LCD_LINE1);
			printLCD("                ");  // Adjust the number of spaces to cover the line			
		
		//print test3
			cmdLCD(LCD_LINE1);
			printLCD("test3");
	}
}

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
    }
		cmdLCD(0x80);
		putLCD(0x00);
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
