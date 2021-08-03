
#include "stm32f030x6.h"



void MCUinit(void);
void CLOCKinit(void);
void GPIOinit(void);
void delay (uint32_t time);

int main (void){

	MCUinit();
	
	while(1){
		GPIOA->BSRR |= (1 << 3);		// Set PA14
		delay(1000000);
		GPIOA->BSRR |= ((1 << 3) << 16);		// Reset PA14
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
	
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1; // NUCLEO RODANDO 8MHZ
	
	RCC->CFGR |= RCC_CFGR_PPRE_DIV8;	// PERIFERICOS RODANDO A 1MHZ
	
}

void GPIOinit(void){

	// Enable GPIO Port A Clock
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;	
	
	// Setting PA14 as output
	GPIOA->MODER |= (1 << 6); // BITS 29:28 -> 01
	
	
}
void delay (uint32_t time){
	do{
		__NOP();
		__NOP();
		__NOP();
		__NOP();
	}while(time--);
}
