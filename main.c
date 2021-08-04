
#include "stm32f030x6.h"



void MCUinit(void);
void CLOCKinit(void);
void GPIOinit(void);
void TIMER3init(void);
void Delay_us (uint16_t us);
void Delay_ms (uint16_t ms);

int main (void){

	MCUinit();
	
	while(1){
		GPIOA->BSRR |= (1 << 3);		// Set PA14
		Delay_ms(100);
		GPIOA->BSRR |= ((1 << 3) << 16);		// Reset PA14
		Delay_ms(100);
	}
	
}

void MCUinit(void){

	CLOCKinit();
	GPIOinit();
	TIMER3init();
}

void CLOCKinit(void){
	
	// ENABLE HSI and wait for the HSE to become Ready
	RCC->CR |= RCC_CR_HSION;
	while(!(RCC->CR & RCC_CR_HSIRDY));
	
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1; // NUCLEO RODANDO 8MHZ
	
	RCC->CFGR |= RCC_CFGR_PPRE_DIV1;	// PERIFERICOS RODANDO A 8MHZ
	
}

void GPIOinit(void){

	// Enable GPIO Port A Clock
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;	
	
	// Setting PA14 as output
	GPIOA->MODER |= (1 << 6); // BITS 29:28 -> 01
	
	
}
void TIMER3init(void){

	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
 
	TIM3->PSC = 8-1; // 8MHz / 8 = 1 MHz uS delay
	TIM3->ARR = 0xFFFF; // MAX ARR value
	
	TIM3->CR1 |= (1 << 0); // Enable counter CEN
	
	while(!(TIM3->SR & (1<<0)));

}

void Delay_us (uint16_t us){
	TIM3->CNT = 0;
	while (TIM3->CNT < us);
}
void Delay_ms(uint16_t ms){
	for (uint16_t i=0; i<ms; i++){
		Delay_us(1000);
	}	
}
