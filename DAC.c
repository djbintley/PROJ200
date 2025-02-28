#include "DAC.h"

void init_DAC(void)
{
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOAEN;												//DAC port clock enable
	DAC_port->MODER|=((3u<<(2*DAC1_pin)|(3u<<(2*DAC2_pin))));	//DAC output pins set as analogue
	RCC->APB1ENR|=RCC_APB1ENR_DACEN;													//DAC clock enable
	DAC->CR|=DAC_CR_EN2;																			//DAC Channel 2 enabled
	DAC->CR|=DAC_CR_EN1;																			//DAC Channel 1 enabled

}

void DAC1DC(unsigned short d)	// Write a DC value to DAC channel 1
{
	DAC->DHR12R1=d;							//Write data byte to DAC 1 output register
}



void interrupt_square(int freq) {				// Write a Square wave to DAC Channel 1 at specified frequency using interrupts
    int period_ms = (2000 / freq);  		// Period for the square wave
    Init_Timer2(period_ms, 90000);      // Initialize and start timer for the desired frequency and prescale for ms
}
	

int DAC_Mode = 0;

void sine_wave(int freq) {			//Rough sine wave generator
		DAC_Mode = 0;	
    int period_us = (1000000 / (freq * SINE_SAMPLES));  // Timer interval in microseconds
    Init_Timer5(period_us, 90);													// Configure and start the timer for DAC updates and prescale for microseconds
}


void square(int freq){					// Write a Square wave to DAC Channel 1 at specified frequency
	DAC_Mode = 0;
	int delayms = (1000/freq)/2;	// Convert frequency to period in ms
	while (1){										// Loop foreve
	DAC->DHR8R1= 0xFE;						// Set DAC channel 1 high
	GPIOB->BSRR = (1u<<14);				// Set Red LED high to confirm
	WaiT4ms(delayms);							// Wait according to the period
	GPIOB->BSRR = (1u<<14<<16);		// Set Red LED low
	DAC->DHR8R1= 0;								// Set DAC channel 1 low
	WaiT4ms(delayms);							// Wait according to the period
	}
}

void triangle(int frequency){				// Function to Produce a triangular wave
	DAC_Mode = 1;
	frequency = (50000/frequency);
	Init_Timer5(frequency, 90000);
}


