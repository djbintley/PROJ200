#ifndef ADC_H
#define ADC_H
#include "stm32f4xx.h"
#include "TIM.h"
#include "Buzz.h"
#include "LED.h"

#define ADC_input_port      GPIOC
#define ADC_input_pin       3
#define ADC_BUFFER_SIZE     3
#define ADC_Channel         13

extern unsigned int beep;
extern unsigned int adcVal;

// ADC initialization and conversion functions
void Init_ADC(void);            // Initialize ADC hardware
unsigned int read_adc(void);    // Read a single ADC conversion result

/* ----------------- Heart Rate Conversion Functions ----------------- */
// These functions implement heart rate calculation using a circular buffer.
// They are designed to be called at a 1kHz rate (every 1 ms).

void ADC_HeartRate_Init(void);       // Initialize heart rate processing variables
void ADC_HeartRate_Update(void);     // Update heart rate calculation (call every 1 ms)
unsigned int ADC_Get_HeartRateBPM(void); // Retrieve the latest calculated heart rate in BPM

void buffdate (void);	//Update the 15 second buffer
unsigned int getbpm (void);	//Read the 15 second buffer

void ADC_Get_MinMax(unsigned int *min, unsigned int *max); // reads the minmax from buffer



 #endif
