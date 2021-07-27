
#include "stm32f030x6.h"


#define PLL	2


void CLOCKinit(void);

int main (void){

	
	while(1){
		
	}
	
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