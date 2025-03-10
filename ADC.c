#include "ADC.h"

// Existing ADC initialization function
void Init_ADC(void)
{
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

/* ------------------------- Heart Rate Conversion Code ------------------------- */
/* The following code uses a circular buffer to store recent ADC samples.
   Every 1 ms (at 1 kHz), a new sample is read and placed into the buffer.
   The last 5 samples are averaged to reduce noise. When the averaged value exceeds
   a set threshold and enough time has passed since the last detected peak, a heartbeat
   is counted, and the BPM (beats per minute) is updated. */

/* Configuration constants (tweak these as needed) */
#define PEAK_THRESHOLD          2000    // Averaged ADC value threshold for a heartbeat peak
#define MIN_PEAK_INTERVAL_MS    300     // Minimum interval between peaks (in ms)
#define SAMPLE_RATE_HZ          1000    // Sampling rate: 1 kHz
#define AVERAGE_COUNT           5       // Number of samples to average
#define CIRC_BUFFER_SIZE        100     // Circular buffer size (holds the last 100 samples)

/* Circular buffer and related variables */
static unsigned int adcCircularBuffer[CIRC_BUFFER_SIZE];
static unsigned int writeIndex = 0;    // Next write location in the buffer
static unsigned int sampleCount = 0;     // Total samples stored (max = CIRC_BUFFER_SIZE)

/* Variables for filtering and heart rate calculation */
static unsigned int filteredSample = 0;
static unsigned int lastPeakTime = 0;    // Time (in ms) of last detected peak
static unsigned int heartRateBPM = 0;      // Calculated heart rate in BPM
static unsigned int peakDetected = 0;      // Flag: 0 = no peak detected, 1 = peak detected
static unsigned int prevFilteredSample = 0; // For outlier rejection

// Helper: Average the last AVERAGE_COUNT samples from the circular buffer
static unsigned int AverageLastSamples(void)
{
    unsigned int sum = 0;
    unsigned int i;
    unsigned int start;
    
    // Determine starting index for the last AVERAGE_COUNT samples
    if (writeIndex >= AVERAGE_COUNT)
        start = writeIndex - AVERAGE_COUNT;
    else
        start = CIRC_BUFFER_SIZE + writeIndex - AVERAGE_COUNT;
    
    // Sum the samples, wrapping around if needed
    for (i = 0; i < AVERAGE_COUNT; i++) {
        unsigned int idx = (start + i) % CIRC_BUFFER_SIZE;
        sum += adcCircularBuffer[idx];
    }
    
    return sum / AVERAGE_COUNT;
}

// This function should be called every 1 ms (e.g., from a timer interrupt)
void ADC_HeartRate_Update(void)
{
    unsigned int newSample = read_adc();
    
    // Simple outlier rejection:
    // If a new sample is more than 50% above the previous average, assume it's an artifact.
    if (sampleCount > 0 && newSample > (prevFilteredSample * 3 / 2))
    {
        newSample = prevFilteredSample;
    }
    
    // Store the new sample into the circular buffer at the current write index.
    adcCircularBuffer[writeIndex] = newSample;
    writeIndex = (writeIndex + 1) % CIRC_BUFFER_SIZE;
    if (sampleCount < CIRC_BUFFER_SIZE)
        sampleCount++;
    
    // Only process if we have at least AVERAGE_COUNT samples.
    if (sampleCount >= AVERAGE_COUNT)
    {
        filteredSample = AverageLastSamples();
        
        // Peak detection:
        // When the averaged value exceeds the threshold and no peak is already marked,
        // and sufficient time has passed since the last peak, then register a heartbeat.
        if (filteredSample > PEAK_THRESHOLD && peakDetected == 0)
        {
            unsigned int currentTime = timer_tick;  // timer_tick must be updated every 1 ms.
            if ((currentTime - lastPeakTime) >= MIN_PEAK_INTERVAL_MS)
            {
                if (lastPeakTime != 0)
                {
                    // BPM = 60000 ms divided by the interval between peaks
                    heartRateBPM = 60000 / (currentTime - lastPeakTime);
                }
                lastPeakTime = currentTime;
                peakDetected = 1;
            }
        }
        // Clear the peak flag when the signal drops below the threshold.
        if (filteredSample < PEAK_THRESHOLD)
        {
            peakDetected = 0;
        }
        
        // Save the current filtered value for outlier rejection in future samples.
        prevFilteredSample = filteredSample;
    }
}

// Initialization for heart rate conversion.
void ADC_HeartRate_Init(void)
{
    writeIndex = 0;
    sampleCount = 0;
    filteredSample = 0;
    lastPeakTime = 0;
    heartRateBPM = 0;
    peakDetected = 0;
    prevFilteredSample = 0;
}

// Returns the latest calculated heart rate in BPM.
unsigned int ADC_Get_HeartRateBPM(void)
{
    return heartRateBPM;
}
