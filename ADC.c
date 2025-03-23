#include "ADC.h"

unsigned int adcVal = 0;

// Existing ADC initialization function
void Init_ADC(void)
{
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;	// Enable GPIOC clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;  // Enable GPIOA clock
    ADC_input_port->MODER |= (3u << (2 * ADC_input_pin));  // Set ADC pin to analog mode

    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;   // Enable ADC1 clock
    ADC1->SQR1 &= ~ADC_SQR1_L;             // Set sequence length to 1 conversion
    ADC1->SQR3 &= ~ADC_SQR3_SQ1;           // Clear channel selection bits
    ADC1->SQR3 |= ADC_Channel;             // Set the ADC channel
    ADC1->CR2 |= ADC_CR2_ADON;             // Enable the ADC
}

// A simple ADC conversion function.
// This is used by the heart rate code.
unsigned int read_adc(void)
{
    ADC1->CR2 |= ADC_CR2_SWSTART;             // Start conversion
    while ((ADC1->SR & ADC_SR_EOC) == 0) { __NOP(); }  // Wait for conversion to complete
    return ADC1->DR;                          // Return ADC result
}

//--------------------- Heart Rate Detection (Hysteresis) ---------------------//

// Parameters for the heartbeat detection algorithm.
#define HIGH_THRESHOLD   2200   // ADC average must exceed this value to count a beat
#define LOW_THRESHOLD    1000    // ADC average must drop below this value to allow a new beat
#define AVERAGE_COUNT    50      // Number of samples to average
#define REFRACTORY_MS    300    // Minimum time (in ms) between beats

// Static variables to hold heartbeat detection state.
static unsigned int hr_sampleBuffer[AVERAGE_COUNT];
static unsigned char hr_sampleIndex = 0;
static unsigned int hr_prevBeatTime = 0;  // Timestamp of the previous beat (ms)
static unsigned int hr_currentBPM = 0;    // Calculated BPM
static unsigned char hr_beatTriggered = 0; // Flag: 0 = no beat, 1 = beat detected

// Compute the average of the last AVERAGE_COUNT samples.
unsigned int hr_ComputeAverage(void)
{
    unsigned int sum = 0;
    unsigned char i;
    for(i = 0; i < AVERAGE_COUNT; i++){
        sum += hr_sampleBuffer[i];
    }
    return sum / AVERAGE_COUNT;
}

// Initialize heartbeat detection variables.
void ADC_HeartRate_Init(void)
{
    hr_sampleIndex = 0;
    hr_prevBeatTime = 0;
    hr_currentBPM = 0;
    hr_beatTriggered = 0;
}

// This function should be called every 1ms to process one ADC sample.
void ADC_HeartRate_Update(void)
{
    adcVal = read_adc();
    hr_sampleBuffer[hr_sampleIndex] = adcVal;
    hr_sampleIndex = (hr_sampleIndex + 1) % AVERAGE_COUNT;
    
    // When the sample buffer is filled (i.e. when hr_sampleIndex wraps to 0)
    if(hr_sampleIndex == 0)
    {
        unsigned int avgVal = hr_ComputeAverage();
				update_RGB_bar_from_HR();
        
        // If the averaged value exceeds HIGH_THRESHOLD, no beat is triggered,
        // and at least REFRACTORY_MS has elapsed since the previous beat, register a beat.
        if(avgVal < LOW_THRESHOLD && hr_beatTriggered == 0 &&
           ((timer_tick - hr_prevBeatTime) >= REFRACTORY_MS))
        {
            if(hr_prevBeatTime != 0)
            {
                unsigned int interval = timer_tick - hr_prevBeatTime;
                hr_currentBPM = 60000 / interval;
            }
            hr_prevBeatTime = timer_tick;
            hr_beatTriggered = 1;
						beep = 1;
        }
        
        // Reset the trigger when the average falls below LOW_THRESHOLD.
        if(avgVal > HIGH_THRESHOLD)
        {
            hr_beatTriggered = 0;
        }
    }
}

// Retrieve the latest calculated BPM.
unsigned int ADC_Get_HeartRateBPM(void)
{
    return hr_currentBPM;
}

static unsigned int buff_index = 0;
static unsigned int big_sampleBuffer[30]= {80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80};

void buffdate (void){
	big_sampleBuffer[buff_index] = ADC_Get_HeartRateBPM();
	if (buff_index <29)buff_index++;
	else buff_index = 0;
}

unsigned int getbpm (void){
	unsigned int sum = 0;
	unsigned char i;
	for(i = 0; i < 30; i++){
			sum += big_sampleBuffer[i];
	}
	return sum / 30;
}


// Function to get the minimum and maximum values stored in the ADC buffer.
// 'min' and 'max' are pointers where the results will be stored.
void ADC_Get_MinMax(unsigned int *min, unsigned int *max)
{
    unsigned int local_min = hr_sampleBuffer[0];
    unsigned int local_max = hr_sampleBuffer[0];
    unsigned char i;
    
    for (i = 1; i < AVERAGE_COUNT; i++) {
        if (hr_sampleBuffer[i] < local_min)
            local_min = hr_sampleBuffer[i];
        if (hr_sampleBuffer[i] > local_max)
            local_max = hr_sampleBuffer[i];
    }
    
    *min = local_min;
    *max = local_max;
}
