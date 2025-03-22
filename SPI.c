#include "stm32f4xx.h"
#include "SPI.h"

// Define SPI pins and CS
#define SPL06_CS_LOW()    (GPIOB->BSRR = (1 << (2 + 16)))  // PB2 Low
#define SPL06_CS_HIGH()   (GPIOB->BSRR = (1 << 2))         // PB2 High

// Compensation Coefficients
static int32_t c0, c1, c00, c10, c01, c11, c20, c21, c30;

// SPI state machine
volatile enum {IDLE, READING_PRESSURE, READING_TEMP} spi_state = IDLE;
static uint8_t spi_rx_buf[7];
static uint8_t spi_rx_index = 0;

// Latest sensor values
volatile float latest_pressure = 0;
volatile float latest_temperature = 0;

// Function Prototypes
void SPL06_GPIO_SPI_Init(void);
void SPL06_SPI1_Init(void);
void SPL06_TIM3_Init(void);
void SPL06_ReadCalibrationCoefficients(void);
uint8_t SPL06_ReadRegister(uint8_t reg);
void SPL06_WriteRegister(uint8_t reg, uint8_t value);
void SPL06_ProcessRaw(int32_t raw_pressure, int32_t raw_temp, float* pressure_pa, float* temp_c);
void SPL06_Init(void);

// Initialization function
void SPL06_Init(void) {
    SPL06_GPIO_SPI_Init();
    SPL06_SPI1_Init();
    SPL06_TIM3_Init();

    // Sensor configuration (oversampling x8)
    SPL06_WriteRegister(0x06, 0x07);  // PRS_CFG
    SPL06_WriteRegister(0x07, 0x07);  // TMP_CFG
    SPL06_WriteRegister(0x08, 0x0F);  // MEAS_CFG
    SPL06_WriteRegister(0x09, 0x00);  // CFG_REG

    SPL06_ReadCalibrationCoefficients();
}

// GPIO and SPI1 initialization
void SPL06_GPIO_SPI_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    // PB3(SCK), PB4(MISO), PB5(MOSI) as AF5
    GPIOB->MODER |= (2 << 6) | (2 << 8) | (2 << 10);
    GPIOB->AFR[0] |= (5 << 12) | (5 << 16) | (5 << 20);

    // PB2 as output for CS
    GPIOB->MODER |= (1 << 4);
    GPIOB->BSRR = (1 << 2);  // CS High
}

void SPL06_SPI1_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_BR_1;
    NVIC_EnableIRQ(SPI1_IRQn);
    SPI1->CR1 |= SPI_CR1_SPE;
}

void SPL06_TIM3_Init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    TIM3->PSC = 8399;    // 84 MHz / 8400 = 10 kHz
    TIM3->ARR = 4999;    // 10kHz / 5000 = 2Hz
    TIM3->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM3_IRQn);
    TIM3->CR1 |= TIM_CR1_CEN;
}

// SPI register read/write
uint8_t SPL06_ReadRegister(uint8_t reg) {
    SPL06_CS_LOW();
    SPI1->DR = reg | 0x80;
    while (!(SPI1->SR & SPI_SR_RXNE));
    (void)SPI1->DR;
    SPI1->DR = 0x00;
    while (!(SPI1->SR & SPI_SR_RXNE));
    uint8_t value = SPI1->DR;
    SPL06_CS_HIGH();
    return value;
}

void SPL06_WriteRegister(uint8_t reg, uint8_t value) {
    SPL06_CS_LOW();
    SPI1->DR = reg & 0x7F;
    while (!(SPI1->SR & SPI_SR_RXNE));
    (void)SPI1->DR;
    SPI1->DR = value;
    while (!(SPI1->SR & SPI_SR_RXNE));
    (void)SPI1->DR;
    SPL06_CS_HIGH();
}

// Read calibration coefficients
void SPL06_ReadCalibrationCoefficients(void) {
    uint8_t buffer[18];
    for (uint8_t i = 0; i < 18; i++) {
        buffer[i] = SPL06_ReadRegister(0x10 + i);
    }

    c0 = ((buffer[0] << 4) | (buffer[1] >> 4));
    if (c0 & (1 << 11)) c0 |= 0xFFFFF000;

    c1 = (((buffer[1] & 0x0F) << 8) | buffer[2]);
    if (c1 & (1 << 11)) c1 |= 0xFFFFF000;

    c00 = ((buffer[3] << 12) | (buffer[4] << 4) | (buffer[5] >> 4));
    if (c00 & (1 << 19)) c00 |= 0xFFF00000;

    c10 = (((buffer[5] & 0x0F) << 16) | (buffer[6] << 8) | buffer[7]);
    if (c10 & (1 << 19)) c10 |= 0xFFF00000;

    c01 = (buffer[8] << 8) | buffer[9];
    c11 = (buffer[10] << 8) | buffer[11];
    c20 = (buffer[12] << 8) | buffer[13];
    c21 = (buffer[14] << 8) | buffer[15];
    c30 = (buffer[16] << 8) | buffer[17];
}

// Compensation function
void SPL06_ProcessRaw(int32_t raw_pressure, int32_t raw_temp, float* pressure_pa, float* temp_c) {
    float temp_sc = raw_temp / 524288.0f;
    float prs_sc  = raw_pressure / 524288.0f;

    *temp_c = c0 / 2.0f + c1 * temp_sc;
    *pressure_pa = c00 + prs_sc * (c10 + prs_sc * (c20 + prs_sc * c30)) +
                   temp_sc * (c01 + prs_sc * (c11 + prs_sc * c21));
}

// Timer interrupt for 2Hz sampling
void TIM3_IRQHandler(void) {
    if (TIM3->SR & TIM_SR_UIF) {
        TIM3->SR &= ~TIM_SR_UIF;

        spi_state = READING_PRESSURE;
        spi_rx_index = 0;
        SPL06_CS_LOW();
        SPI1->DR = 0x00 | 0x80;  // Read pressure MSB
        SPI1->CR2 |= SPI_CR2_RXNEIE;
    }
}

// SPI1 interrupt handler
void SPI1_IRQHandler(void) {
    if (SPI1->SR & SPI_SR_RXNE) {
        uint8_t data = SPI1->DR;

        if (spi_state != IDLE) {
            spi_rx_buf[spi_rx_index++] = data;

            if ((spi_state == READING_PRESSURE && spi_rx_index == 4) ||
                (spi_state == READING_TEMP && spi_rx_index == 7)) {

                SPL06_CS_HIGH();
                SPI1->CR2 &= ~SPI_CR2_RXNEIE;

                if (spi_state == READING_PRESSURE) {
                    spi_state = READING_TEMP;
                    SPL06_CS_LOW();
                    SPI1->DR = 0x03 | 0x80;  // Read temp MSB
                    SPI1->CR2 |= SPI_CR2_RXNEIE;
                } else {
                    spi_state = IDLE;
                    int32_t pr = (spi_rx_buf[1] << 16) | (spi_rx_buf[2] << 8) | spi_rx_buf[3];
                    int32_t tr = (spi_rx_buf[4] << 16) | (spi_rx_buf[5] << 8) | spi_rx_buf[6];
                    if (pr & 0x800000) pr |= 0xFF000000;
                    if (tr & 0x800000) tr |= 0xFF000000;

                    SPL06_ProcessRaw(pr, tr, (float*)&latest_pressure, (float*)&latest_temperature);
                    // Now latest_pressure and latest_temperature are updated
                }
            } else {
                SPI1->DR = 0x00;  // Continue dummy read
            }
        }
    }
}