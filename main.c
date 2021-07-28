
#include "stm32f030x6.h"


#define PLL	2

void MCUinit(void);
void CLOCKinit(void);
void GPIOinit(void);

int main (void){

	MCUinit();
	
	while(1){
		GPIOA->BSRR |= (1 << 14);		// Set PA14
		delay(1000000);
		GPIOA->BSRR |= ((1 << 14) << 16);		// Reset PA14
		delay(1000000);
	}
	
}

void MCUinit(void){

	CLOCKinit();
	GPIOinit();
	
}

void CLOCKinit(void){
	
	// ENABLE HSI and wait for the HSE to become Ready
	RCC->CR |= RCC_CR_HSION;
	while(!(RCC->CR & RCC_CR_HSIRDY));
	
	// Set the POWER ENABLE CLOCK and VOLTAGE REGULATOR
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;

	// Configure the PRESCALARS HCLK, PLCK1, PLCK2
	// AHB PR
	RCC->CFGR &= RCC_CFGR_HPRE_0;
	
	// APB PR
	RCC->CFGR |= RCC_CFGR_PPRE_0;
	
	// Set Internal Clock as clock source
	RCC->CFGR |= RCC_CFGR_SW_HSI;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);
	
}

void GPIOinit(void){

	// Enable GPIO Port A Clock
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
	// Setting PA14 as output
	GPIOA->MODER |= (1 << 28); // BITS 29:28 -> 01
	
	
}