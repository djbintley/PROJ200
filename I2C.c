#include "I2C.h"

// Function to initialise I2C communication and wake up the IMU (MPU-6050 sensor)
void IMU_INIT(void) {
	// Turn on clocks for GPIOB and I2C2 peripheral
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;

	// Set pins PB10 (SCL) and PB11 (SDA) to Alternate Function mode
	// This allows these pins to work specifically as I2C pins
    GPIOB->MODER |= (2UL << 20) | (2UL << 22);

	// Set these pins to open-drain output (required for I2C)
    GPIOB->OTYPER |= (1UL << 10) | (1UL << 11);
	
	// Set pins to high-speed mode for reliable data transfer
    GPIOB->OSPEEDR |= (3UL << 20) | (3UL << 22);
	
	// Assign I2C alternate function (AF4) to pins PB10 and PB11
    GPIOB->AFR[1] |= (4UL << 8) | (4UL << 12);

	// Configure I2C communication settings
    I2C2->CR2 = 45;  // Tell I2C peripheral that APB1 bus frequency is 45MHz
    I2C2->CCR = 225; // Clock Control Register (sets I2C speed to 100kHz)
    I2C2->TRISE = 46; // Max rise time for signals based on bus frequency (45MHz + 1)
	
	// Enable I2C2 peripheral
    I2C2->CR1 |= I2C_CR1_PE;

    // Send a "wake-up" command to the IMU
    I2C2->CR1 |= I2C_CR1_START;  // Generate a "Start condition" (start signal)
    while (!(I2C2->SR1 & I2C_SR1_SB)); // Wait until start bit is sent successfully
	
	// Send device address (writing mode)
    I2C2->DR = IMU_Addr;
    while (!(I2C2->SR1 & I2C_SR1_ADDR)); // Wait until device responds
    (void)I2C2->SR2; // Read and ignore SR2 to clear address matched condition
	
	// Select IMU power management register
    I2C2->DR = 0x6B;
    while (!(I2C2->SR1 & I2C_SR1_TXE)); // Wait until data register is empty again
	
	// Write 0x00 to wake the sensor
    I2C2->DR = 0x00;
    while (!(I2C2->SR1 & I2C_SR1_BTF)); // Wait until transmission is complete
	
	// Generate "Stop condition" to end transmission
    I2C2->CR1 |= I2C_CR1_STOP;
}

// Helper function to read raw sensor data (two bytes)
static int16_t read_sensor(uint8_t reg) {
    int16_t val = 0;

	// Send register address we want to read from
    I2C2->CR1 |= I2C_CR1_START;              
    while (!(I2C2->SR1 & I2C_SR1_SB));
    (void)I2C2->SR1; // Ignore SR1 (side-effect: clears flags)

    I2C2->DR = IMU_Addr;                
    while (!(I2C2->SR1 & I2C_SR1_ADDR));
    (void)I2C2->SR1; (void)I2C2->SR2; // Clear ADDR flags

    I2C2->DR = reg;                         
    while (!(I2C2->SR1 & I2C_SR1_BTF));     // Wait until byte transmitted fully

	// Restart condition to change to reading mode
    I2C2->CR1 |= I2C_CR1_START;             
    while (!(I2C2->SR1 & I2C_SR1_SB));      
    (void)I2C2->SR1;

    I2C2->DR = IMU_Addr | 0x01;         // Address for reading (last bit = 1)
    while (!(I2C2->SR1 & I2C_SR1_ADDR));
    (void)I2C2->SR1; (void)I2C2->SR2;       

    I2C2->CR1 |= I2C_CR1_ACK;               // Enable ACK for first byte

    while (!(I2C2->SR1 & I2C_SR1_RXNE));
    val = (I2C2->DR << 8);                  // Read high byte first

    I2C2->CR1 &= ~I2C_CR1_ACK;              // Disable ACK for last byte
    I2C2->CR1 |= I2C_CR1_STOP;              // Send STOP after last byte

    while (!(I2C2->SR1 & I2C_SR1_RXNE));
    val |= I2C2->DR;                        // Read low byte

    return val;                             // Return combined result
}

// Our own "Absolute" function since we can't use external libraries
int Absolute(int x) {
    return (x < 0) ? -x : x;
}

// Detect significant movements by checking if the sensor reading changes
int MovementDetected(void) {
    static int16_t last_Ax = 0, last_Ay = 0, last_Az = 0;

	// Get new readings
    int16_t Ax = read_sensor(0x3B);
    int16_t Ay = read_sensor(0x3D);
    int16_t Az = read_sensor(0x3F);

	// Find out how much readings have changed since last check
    int diffAx = Absolute(Ax - last_Ax);
    int diffAy = Absolute(Ay - last_Ay);
    int diffAz = Absolute(Az - last_Az);

	// Save current readings for next comparison
    last_Ax = Ax;
    last_Ay = Ay;
    last_Az = Az;

	// Set a sensible threshold to detect real movement
    int threshold = 1500;

	// If any axis moved significantly, return 1 (true)
    if (diffAx > threshold || diffAy > threshold || diffAz > threshold) {
        return 1;
    }

	// If movement is small, return 0 (false)
    return 0;
}
