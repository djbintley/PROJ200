#include "USART.h"
#include "stdio.h"
#include "SPI.h"

//USART is set so Baud Rate for 230400 bps, 8 data bits, 1 stop bit, even parity, flow control off

void Init_USART(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;  // Enable GPIO clock
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // Enable USART3 clock

    // Configure GPIO TX/RX Pins
    USART_PORT->MODER &= ~((3u << (2 * USART_TX_pin)) | (3u << (2 * USART_RX_pin)));
    USART_PORT->MODER |= ((2u << (2 * USART_TX_pin)) | (2u << (2 * USART_RX_pin)));

    USART_PORT->AFR[1] &= ~((0xF << ((USART_TX_pin - 8) * 4)) | (0xF << ((USART_RX_pin - 8) * 4)));
    USART_PORT->AFR[1] |= ((7 << ((USART_TX_pin - 8) * 4)) | (7 << ((USART_RX_pin - 8) * 4)));

    // Get the actual APB1 Clock (NOT SystemCoreClock)
    uint32_t apb1_clock;
    uint32_t ppre1 = (RCC->CFGR & RCC_CFGR_PPRE1) >> 10;  // Extract APB1 prescaler

    if (ppre1 < 4) {
        apb1_clock = SystemCoreClock / 2;  // APB1 runs at 90 MHz
    } else {
        apb1_clock = SystemCoreClock / 4;  // APB1 runs at 45 MHz
    }

    // Set USART3 Baud Rate for 230400 bps
    USART3->BRR = apb1_clock / 230400;

     // Configure USART3: 8 data bits, 1 stop bit, even parity
            USART3->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE; // Enable TX, RX, USART
    USART3->CR1 |= USART_CR1_M | USART_CR1_PCE; // 9-bit mode (8 data + parity), even parity
    USART3->CR2 &= ~USART_CR2_STOP; // 1 stop bit

    USART3->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE; // Enable TX, RX, and USART
}

void send_usart(unsigned char d) {
    while (!(USART3->SR & USART_SR_TXE)); // Wait until TX buffer is empty
    USART3->DR = d; // Transmit character
    while (!(USART3->SR & USART_SR_TC)); // Wait until transmission completes
}
void send_string(char *data) {
    while (*data != '\0') {
        send_usart(*data);
        Delay(1); // Small delay (1 ms)
        data++;
    }
}

char buffer[7];

void display_volts(){
      // --- Read POT ---
        ADC1->CR2 &= ~ADC_CR2_ADON;  // Disable ADC before switching
        Delay(10);
        ADC1->SQR3 = (ADC_Channel << 0);
        ADC1->CR2 |= ADC_CR2_ADON;  // Re-enable ADC
        Delay(2);
        ADC1->CR2 |= ADC_CR2_SWSTART;
        while (!(ADC1->SR & ADC_SR_EOC));
        uint16_t pot_value = ADC1->DR;
        float pot_voltage = (pot_value * 3.3) / 4095; // Convert to voltage

       
      send_string(RESET_SCREEN); // Clear screen and reset cursor
      
      send_string("POT(v): ");
      sprintf(buffer, "%.3f", pot_voltage);
      send_string(buffer);
      send_string("v""\r\n");

      
};

char HR[30]; //buffer used for heartrate

void display_menu(void) {
    send_string(RESET_SCREEN); // Clear screen and reset cursor

    // Title
    send_string(COLOUR_WHITE "=== SPO2 MONITOR ===" COLOUR_RESET "\r\n\r\n");

    // Menu items with colors
    send_string(COLOUR_RED "H - Heartrate:     " COLOUR_RESET "\r\n");
    send_string(COLOUR_WHITE "O - Oxygen Level:  " COLOUR_RESET "\r\n");
    send_string(COLOUR_YELLOW "T - Temperature:   " COLOUR_RESET "\r\n");
    send_string(COLOUR_GREEN "P - Pressure:     " COLOUR_RESET "\r\n");
    send_string(COLOUR_BLUE "M - Movement:     " COLOUR_RESET "\r\n");
    send_string(COLOUR_CYAN "V - Voltage:     " COLOUR_RESET "\r\n");
    send_string(COLOUR_WHITE "Samples:" COLOUR_RESET "\r\n");
    send_string(COLOUR_WHITE "Max = " COLOUR_RESET "\r\n");
		send_string(COLOUR_WHITE "Min =      " COLOUR_RESET "\r\n");
		send_string(COLOUR_WHITE "Average =      " COLOUR_RESET "\r\n");
	
	
}

//\033[<row>;<col>H for movement  3:15, 4:18, 5:17, 6:14, 7:14


char selected_menu = 'H'; // default selection
// Function uses cursor control to update the values after the menu and if else statement to highlight selected value

void update_menu(void) {
	//Update Heartrate
	send_string("\033[3;15H");
	send_string(RESET_LINE);
	send_string(selected_menu == 'H' ? HIGHLIGHT : NORMAL);
	sprintf(HR, "%u", getbpm());
  send_string(HR);
	
	//Update Oxygen
	send_string("\033[4;18H");
	send_string(RESET_LINE);
	send_string(selected_menu == 'O' ? HIGHLIGHT : NORMAL);
  send_string("98%");
	
	//Update Temperature
	send_string("\033[5;17H");
	send_string(RESET_LINE);
	float temp = latest_temperature;
	send_string(selected_menu == 'T' ? HIGHLIGHT : NORMAL);
	sprintf(HR, "%.1f C", temp+426.3);
  send_string(HR);
	
	//Update Pressure
	send_string("\033[6;14H");
	send_string(RESET_LINE);
	temp = latest_pressure;
	send_string(selected_menu == 'P' ? HIGHLIGHT : NORMAL);
	sprintf(HR, "%.1f Pa" , temp-139000);
  send_string(HR);
	
	//Update Movement
	send_string("\033[7;15H");
  send_string(selected_menu == 'M' ? HIGHLIGHT : NORMAL);
	if(MovementDetected()) {
						send_string(RESET_LINE);
            send_string("Movement Detected!");
																 }
					else
						send_string(RESET_LINE);
            send_string("Still");
					
	//Update Voltage
		float voltage = get_ADC_voltage();
    char buffer[30];

    send_string("\033[8;13H");    // Position cursor (line 8, col 1)
		send_string(selected_menu == 'V' ? HIGHLIGHT : NORMAL);
    send_string(RESET_LINE);     // Clear line

    sprintf(buffer, "%.2f V", voltage);
    send_string(buffer);	

		//Update Averages
			unsigned int avg = 0;
			avg = hr_ComputeAverage();
			//Update Samples
			send_string("\033[10;7H");
			send_string(selected_menu == 'A' ? HIGHLIGHT : NORMAL);
			send_string(RESET_LINE);
			unsigned int minVal, maxVal;
			ADC_Get_MinMax(&minVal, &maxVal);
			sprintf(HR, "%u", maxVal);
			send_string(HR);
			send_string("\033[11;7H");
			send_string(RESET_LINE);
			sprintf(HR, "%u", minVal);
			send_string(HR);
			send_string("\033[12;11H");
			send_string(RESET_LINE);
			sprintf(HR, "%u", avg);
			send_string(HR);
}
unsigned char receive_usart(void) {
    if (USART3->SR & USART_SR_RXNE) {
        return USART3->DR;
    }
    return 0;
}

void process_input(void) {
    char input = receive_usart();
    if (input == 0) return;  // No input received, skip

    switch (input) {
        case 'H':
        case 'h':
            selected_menu = 'H';
            Menu = 0;
            break;
        case 'O':
        case 'o':
            selected_menu = 'O';
            Menu = 2;
            break;
        case 'T':
        case 't':
            selected_menu = 'T';
            Menu = 1;
            break;
        case 'P':
        case 'p':
            selected_menu = 'P';
            Menu = 3;
            break;
        case 'M':
        case 'm':
            selected_menu = 'M';
            Menu = 4;
            break;
				case 'V':
        case 'v':
            selected_menu = 'V';
            Menu = 5;
            break;
				case 'A':
        case 'a':
            selected_menu = 'A';
            Menu = 6;
            break;
        default:
            return;
    }
    update_menu();  // Refresh immediately upon input
    update_LCD();   // Also update LCD to reflect selection


}


void hide_cursor(void){
	send_string("\033[?25l");
}