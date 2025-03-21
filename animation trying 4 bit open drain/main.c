#include "lcd.h"

int v1 = 0;
//unsigned int t2_HundredsOfMicroseconds = 10000; // = 1s
/*void Init_Timer2(unsigned int t2_HundredsOfMicroseconds);
void Init_Timer2(unsigned int t2_HundredsOfMicroseconds)
{
	RCC->APB1ENR|=RCC_APB1ENR_TIM2EN;		//timer 2 clock enabled
	TIM2->DIER|=TIM_DIER_UIE;						//timer uptdate interrupt enabled
																			//APB clock is Fcy/2 = 180MHz/9000 = 10MHz
	TIM2->PSC = 9000-1;						
	TIM2->ARR = (t2_HundredsOfMicroseconds / 1) * 1;										//counter reload value, gives a timer period of 100us when F_APB = 90MHz and PSC = 9000
	TIM2->CNT=0;												//zero timer counter
	NVIC->ISER[0]|=(1u<<28);						//timer 2 global interrupt enabled
	TIM2->CR1|=TIM_CR1_CEN;							//start timer counter
}
*/

int main(void)
{
	//Init_Timer2(t2_HundredsOfMicroseconds);

	initLCD();
	
	cmdLCD(LCD_LINE1);
	
	// Clear the display
	LCD_CLR();
   
    lcd_delayus(46000);
    //setCursor(1, 5);                   // Move to the bottom line, position 5
    //writeMessage("Open Drain!");       // Write "Open Drain!" at the new position
	 
	lcd_delayus(46000);
   
    setCursor(0, 1); 
	// Set cursor to the top-left corner
    writeMessage("hello world");       // Write "hello world" on the display
	lcd_delayus(46000);
    setCursor(1, 5);                   // Move to the bottom line, position 5
    writeMessage("LCD Test!");         // Write "LCD Test!" at the new position
	 //displayHeart(); 
	setCursor(1, 15);
	createHeartCharacter();

//CrossCustomCharacter();
	while(1)
	{
		lcd_delayus(50000);
		//LCD_CLR();
		setCursor (0, 1);
		createHeartCharacter();
		lcd_delayus(50000);
		//cmdLCD(' ');
		setCursor(1 , 1);
		createInverseHeartCharacter();
		lcd_delayus(50000);
		createHeartCharacter();
		
		//LCD_CLR();
		//createHeartCharacter();
		//lcd_delayus(500000);
		//createInverseHeartCharacter();
	}
}

/*
void TIM2_IRQHandler(void)			//TIMER 2 INTERRUPT SERVICE ROUTINE
{
	
	
	LCD_CLR();
	if (v1 ==0 )
		{
			
			createHeartCharacter();
			
			v1=1;
			//lcd_delayus(500000);
		}
	if (v1 ==1)
	{
		createInverseHeartCharacter();
		v1 = 0;
		//lcd_delayus(500000);
	}
		//lcd_delayus(500);
} */
