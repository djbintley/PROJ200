#include "LED.h"

// Function to initialise On board LEDs
void LED_INIT (void)
{
	//ENABLE PORT
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;	//ONLY GPIO B clock enable
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;	//ONLY GPIO D clock enable
	

	// Clear the three GPIOB ports
	GPIOB->MODER &= ~(0x03<<(2*0));	//ONLY clear GPIOB0
	GPIOB->MODER &= ~(0x03<<(2*7)); //Clear GPIOB7
	GPIOB->MODER &= ~(0x03<<(2*14)); //Clear GPIOB14
	
	GPIOD->MODER &= ~(0x03<<(2*RED_LED)); //Clear GPIOB7
	GPIOD->MODER &= ~(0x03<<(2*IR_LED)); //Clear GPIOB14
	
	//CONFIGURE PORTS:GPIOB  PINS:0,7,14 TO OUTPUT for LED1,LED2 and LED3
	GPIOB->MODER |= 0x01<<(2*0);		//ONLY set  GPIOB0
	GPIOB->MODER |= 0x01<<(2*7);		//Only set GPIOB7 as Alternative Function
	GPIOB->MODER |= 0x01<<(2*14);		//Only set GPIOB14
	
	GPIOD->MODER |= 0x01<<(2*RED_LED);		//Only set GPIOB7 as Alternative Function
	GPIOD->MODER |= 0x01<<(2*IR_LED);	
		
	// Set output type to push-pull
	GPIOB->OTYPER &= ~(0x1 << (0*2));
	GPIOB->OTYPER &= ~(0x1 << (7 * 2));
	GPIOB->OTYPER &= ~(0x1 << (14 * 2));
	
	GPIOD->OTYPER &= ~(0x1 << (RED_LED * 2));
	GPIOD->OTYPER &= ~(0x1 << (IR_LED * 2));
	
	// No pull-up, pull-down
	GPIOB->PUPDR &= ~(0x3 << (0 * 2));
	GPIOB->PUPDR &= ~(0x3 << (7 * 2));
	GPIOB->PUPDR &= ~(0x3 << (14 * 2));
	
	GPIOD->PUPDR &= ~(0x3 << (RED_LED * 2));
	GPIOD->PUPDR &= ~(0x3 << (IR_LED * 2));
}	

void ONBOARD_LED_ON (char LED){
			GPIOB->BSRR = (1<<LED);							//ONLY TURN SPECIFIED LED ON
}

void ONBOARD_LED_OFF (char LED){
			GPIOB->BSRR = 1<<LED<<16;							//ONLY TURN SPECIFIED LED Off
}

void OFFBOARD_LED_ON (char LED){
			GPIOD->BSRR = (1<<LED);							//ONLY TURN SPECIFIED LED ON
}

void OFFBOARD_LED_OFF (char LED){
			GPIOD->BSRR = 1<<LED<<16;							//ONLY TURN SPECIFIED LED Off
}

#define MAX_VOLTAGE 3.0f  // Max voltage mapped to LED bar
#define MAX_LEDS 24   // Total number of LEDs (8 Red, 8 Green, 8 Blue)

void BAR_INIT(void){
       RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN; // Enable GPIOE clock
    // Configure PE0 (RGB BAR Output enable) as output
    GPIOE->MODER &= ~(3UL << (2 * LED_BAR_OE));  // Clear PE0 mode bits
    GPIOE->MODER |= (1UL << (2 * LED_BAR_OE));   // Set PE0 as output (!OE for RGB Bar)

            // Configure PE13 (Green latch) as output
    GPIOE->MODER &= ~(3UL << (2 * PE13));  // Clear mode bits for PE12
    GPIOE->MODER |= (1UL << (2 * PE13));   // Set PE12 as output
      
        // Configure PE12 (Red latch) as output
    GPIOE->MODER &= ~(3UL << (2 * PE12));  // Clear mode bits for PE12
    GPIOE->MODER |= (1UL << (2 * PE12));   // Set PE12 as output
      
        // Configure PE14 (Blue latch) as output
    GPIOE->MODER &= ~(3UL << (2 * PE14));  // Clear mode bits for PE12
    GPIOE->MODER |= (1UL << (2 * PE14));   // Set PE12 as output
      
      
   
    // Set Output Type (Push-Pull)
    GPIOE->OTYPER &= ~((1UL << LED_BAR_OE) | (1UL << PE12) | (1UL << PE13) | (1UL << PE14));
   
    // Set Pull-down resistors to avoid floating states
    GPIOE->PUPDR &= ~((3UL << (2 * LED_BAR_OE)));
    GPIOE->PUPDR |= (2UL << (2 * LED_BAR_OE)); // Pull-down
                  
		GPIOE->BSRR = (1UL << PE12) << 16; // Set Red Latch LOW (Disabled)
    GPIOE->BSRR = (1UL << PE13) << 16; // Set Green Latch LOW (Disabled)
    GPIOE->BSRR = (1UL << PE14) << 16; // Set Blue Latch LOW (Disabled)
    GPIOE->BSRR = (1UL << LED_BAR_OE);  // Disable RGB Bar (OE HIGH)
  
}


// Function to write values to the LED bus for RGB BAR
void Write_RGB_LEDBus(uint8_t red, uint8_t green, uint8_t blue) {
    GPIOE->BSRR = 0xFF << LED_D0 << 16;  // Clear bus
    GPIOE->BSRR = red << LED_D0;         // Load red data
		Delay(1);
    GPIOE->BSRR = 1 << PE12;             // Latch Red
		Delay(1);
		GPIOE->BSRR = 1 << PE12 << 16;       // Disable Latch Red
   
    GPIOE->BSRR = 0xFF << LED_D0 << 16;  // Clear bus again
    GPIOE->BSRR = green << LED_D0;       // Load green data
    Delay(1);
		GPIOE->BSRR = 1 << PE13;             // Latch Green
    Delay(1);
		GPIOE->BSRR = 1 << PE13 << 16;       // Disable Latch Green
   
    GPIOE->BSRR = 0xFF << LED_D0 << 16;  // Clear bus again
    GPIOE->BSRR = blue << LED_D0;        // Load blue data
    Delay(1);
		GPIOE->BSRR = 1 << PE14;             // Latch Blue
		Delay(1);
		GPIOE->BSRR = 1 << PE14 << 16;       // Disable Latch Blue
      
    GPIOE->BSRR = 1UL<<LED_BAR_OE<<16;              //Enable output
}

void update_RGB_bar_from_HR(void) {

        // Convert to Voltage
        float voltage = ((adcVal)* 3.3f) / 4095.0f;

        // Scale voltage to LED steps (0V = 0 LEDs, 3V = 24 LEDs)
        uint8_t led_steps = (voltage / MAX_VOLTAGE) * MAX_LEDS;
        if (led_steps > MAX_LEDS) {
            led_steps = MAX_LEDS;  // Limit max LEDs
        }

        // Initialize LED counts
        uint8_t red_leds = 0;
        uint8_t green_leds = 0;
        uint8_t blue_leds = 0;

        // Assign LEDs to each color
        if (led_steps <= 8) {  
            red_leds = led_steps;  // Use only red LEDs
        }
        else if (led_steps <= 16) {  
            red_leds = 8;  // Full Red
            green_leds = led_steps - 8;  // Remaining LEDs go to Green
        }
        else {  
            red_leds = 8;   // Full Red
            green_leds = 8; // Full Green
            blue_leds = led_steps - 16; // Remaining LEDs go to Blue
        }

        // Convert to LED bus format (1 bit per LED)
        uint8_t red_mask = (1 << red_leds) - 1;
        uint8_t green_mask = (1 << green_leds) - 1;
        uint8_t blue_mask = (1 << blue_leds) - 1;

        // Update RGB LED Bar
        Write_RGB_LEDBus(red_mask, green_mask, blue_mask);

}

void SEG_INIT(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;	//ONLY GPIO E clock enable
	
	//for(char i=2; i<=9; i++){
		//GPIOE->MODER &= ~(3UL<<(2*i));     //Set bits to default INPUT (not used because it is unclear and assumes contiguity)
	//}
	
	GPIOE->MODER &= ~(						//Sets to default INPUT to clear
			3UL<<(2*LED_D0)|
			3UL<<(2*LED_D1)|
			3UL<<(2*LED_D2)|
			3UL<<(2*LED_D3)|
			3UL<<(2*LED_D4)|
			3UL<<(2*LED_D5)|
			3UL<<(2*LED_D6)|
			3UL<<(2*LED_D7)|
			3UL<<(2*LED_DIG_LE1)|
			3UL<<(2*LED_DIG_LE2)|
			3UL<<(2*LED_DIG_OE));
	
	GPIOE->MODER |= (						//Sets to OUTPUT
			1UL<<(2*LED_D0)|
			1UL<<(2*LED_D1)|
			1UL<<(2*LED_D2)|
			1UL<<(2*LED_D3)|
			1UL<<(2*LED_D4)|
			1UL<<(2*LED_D5)|
			1UL<<(2*LED_D6)|
			1UL<<(2*LED_D7)|
			1UL<<(2*LED_DIG_LE1)|
			1UL<<(2*LED_DIG_LE2)|
			1UL<<(2*LED_DIG_OE));
			
	GPIOE->BSRR = 0xFF<<2<<16;			//set bus to 0
	GPIOE->BSRR = 1UL<<LED_DIG_LE1<<16;		//SET latch enable 1 to 0 
	GPIOE->BSRR = 1UL<<LED_DIG_LE2<<16;		//SET latch enable 2 to 0 
	GPIOE->BSRR = 1UL<<LED_DIG_OE;				//Set Output Enable to 0 (off as it is active high)
}

const int LED_Values[] = {0x7B,0x60,0x3E,0x7C,0x65,0x5D,0x5F,0x70,0x7F,0x7D};

void Write_LEDBus (char data1, char data2){
	GPIOE->BSRR = 0xFF<<2<<16;							//set bus to 0
	GPIOE->BSRR = data1<<2;									//set bus to data for left digit 
	Delay(1);															//Setup and hold delay
	GPIOE->BSRR = 1<<LED_DIG_LE1; 					//Writes data to Latch 1 (Left Digit)
	Delay(1);
	GPIOE->BSRR = 1UL<<LED_DIG_LE1<<16; 		//Disables Latch 1
	GPIOE->BSRR = 0xFF<<2<<16;							//set bus to 0
	GPIOE->BSRR = data2<<2;									//set bus to data for right digit 
	Delay(1);
	GPIOE->BSRR = 1<<LED_DIG_LE2; 					//Writes data to Latch 2	(Right Digit)
	Delay(1);
	GPIOE->BSRR = 1UL<<LED_DIG_LE2<<16; 		//Disables Latch 2
	GPIOE->BSRR = 1UL<<LED_DIG_OE<<16;			//Enable output	
}

void setDigit (char data, char Latch){
	GPIOE->BSRR = 0xFF<<2<<16;							//set bus to 0
	GPIOE->BSRR = LED_Values[data]<<2;									//set bus to data for left digit 
	Delay(1);															//Setup and hold delay
	GPIOE->BSRR = 1<<Latch; 					//Writes data to Latch 1 (Left Digit)
	Delay(1);
	GPIOE->BSRR = 1UL<<Latch<<16; 		//Disables Latch 1
	GPIOE->BSRR = 1UL<<LED_DIG_OE<<16;			//Enable output	
}
