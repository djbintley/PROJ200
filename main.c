#include <stm32f429xx.h>
#include "LED.h"
#include "DAC.h"

int main(void){
	LEDs_INIT();
	DAC_INIT();
	DAC2_DC(3);
}
