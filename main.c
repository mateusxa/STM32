
#include "stm32f030x6.h"



void MCUinit(void);
void CLOCKinit(void);
void GPIOinit(void);
void TIMER3init(void);


void USARTinit(void);
void USARTsendChar(uint8_t c);
void USARTsendString(char *string);
uint8_t USARTgetChar(void);


void Delay_us (uint16_t us);
void Delay_ms (uint16_t ms);

int main (void){

	MCUinit();
	while(1){
		GPIOA->BSRR |= (1 << 3);		// Set PA14
		Delay_ms(1000);
		GPIOA->BSRR |= ((1 << 3) << 16);		// Reset PA14
		Delay_ms(1000);
		
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


void USARTinit(void){
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;		// Enable USART CLOCK
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;			// Enable GPIOA CLOCK
	
	GPIOA->MODER |= (2 << 4);								// Bits (5:4) = 1:0 -> Alternate PA2 TX
	GPIOA->MODER |= (2 << 6);								// Bits (7:6) = 1:0 -> Alternate PA3 rX
	
	GPIOA->OSPEEDR |= (3 << 4) | (3 << 6);	// Bits (5:4) = 11 and Bits (7:6) = 11 -> High Speed PA2 and PA3
	
	GPIOA->AFR[0] |= (7 << 8); // Bytes 11:10:9:8 = 0111 -> af7 alternate function usart pa2
	GPIOA->AFR[0] |= (7 << 12); // Bytes 15:14:13:12 0111 -> af7 pa3
	
	USART1->CR1 |= 0x00; // clear all
	USART1->CR1 |= (1 << 13); // UE = 1 Enable USART
	
	USART1->CR1 &= ~(1 << 12);	// M=0; 8 bit word lenght
	
	USART1->BRR = (7 << 0) | (24 << 1); //  Baud Rate of 115200, PCLK at 45 MHz
	
	USART1->CR1 |= (1 << 2);	// RE = 1 Receive Enable
	USART1->CR1 |= (1 << 3);	// TE = 1 Transmitter Enable
	
}

void USARTsendChar(uint8_t c){
	USART1->TDR = c;
	while(!(USART1->SR & (1 << 6)); //wait dor TC too SET this indicates that the data has beeen transmitted
}

void USARTsendString(char *string){
	while(*string) USARTsendChar(*string++);
}

uint8_t USARTgetChar(void){

	uint8_t temp;
	
	while(!(USART1->SR & (1 << 5);		//wait for RXNE to set
	temp = USART1->RDR;
	
	return temp;
	
}










//-----------------------------------------------------------------------------------------------------------------------------------------
// Área de interrupçao

void USART1_IRQHandle(void){
	if(USART1->SR & USART1_SR_RXNE)								// Tem algum byte no registrador DR
	{															// Sim, existe
		USART1->SR &= ~USART_SR_RXNE;							// Zera o flag
		TempRx = USART1->DR;									// Faz a leitura do que chegou
		//GPIOC->ODR ^= GPIO_ODR_ODR13;							// Troca o estado do led no pino PC13
		PiscaLedOnBoard();
	}
}

//-----------------------------------------------------------------------------------------------------------------------------------------
// Configura a USART1

void Usart1Config(void)										
{
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->CRH |= GPIO_CRH_MODE9_1 | GPIO_CRH_MODE9_0 | GPIO_CRH_CNF9_1;				// Saída 50MHz e alternastiva push-pull
	GPIOA->CRH &= ~GPIO_CRH_CNF9_0;
	GPIOA->CRH &= ~(GPIO_CRH_MODE10_1 | GPIO_CRH_MODE10_0);								// Seta PA10 como entrada
	GPIOA->CRH |= GPIO_CRH_CNF10_0;														// Floating input
	
	USART1->BRR = (0x1D4C);																// Taxa de 9600 bps @ Fclk = 72M
	USART1->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;							// Habilita USART, TX e RX
	USART1->CR1 |= USART_CR1_RXNEIE;													// Habilita a interrupçao de recepçao serial
	NVIC_EnableIRQ(USART1_IRQn);
	//NVIC_SetPriority(USART1_IRQn, 0);
	// PA3 - USART RX
	// PA2 - USART TX
	
//	RCC->APB2ENR |= 0x00004000;							//
//	NVIC->ISER[1] |= 0x00000020;						//
//	USART1->BRR = 0x1D4C;								//
//	USART1->CR1 = 0x202C;								//
//	USART1->CR2 = 0x0000;								//
//	USART1->CR3 = 0x0001;								//
	
	
}

//-----------------------------------------------------------------------------------------------------------------------------------------
// Escreve um byte na serial

void Usart1EscByte(char Data)								
{
	USART1->DR = Data;									// Escreve no registrador DR o valor de Data
	while(!(USART1->SR & USART_SR_TXE));				// Esperar o dado ser transferido para o shift-register
}

//-----------------------------------------------------------------------------------------------------------------------------------------
// Escreve uma string na serial

void Usart1EscString(char* Str)								
{
	while(*Str != '\0')
	{
		Usart1EscByte(*Str);
		Str++;
	}
}

//-----------------------------------------------------------------------------------------------------------------------------------------
// Le uma string até chagar em <CR><LF>

uint8_t Usart1ReadString(void)									
{
	uint8_t NunCar;
	
	for(NunCar = 0; NunCar < 255; NunCar++)
	{
		while(!(USART1->SR & USART_SR_RXNE));						// Espera ter um dado recebido
		BufferRx[NunCar] = USART1->DR;								// Le o dado que chegou
		if(BufferRx[NunCar] == '\n')								// Se chegou no final
			break;
	}
	//PiscaLedOnBoard();
	return NunCar;
}

//-----------------------------------------------------------------------------------------------------------------------------------------
// Configura as portas de GPIO

void GpioConfig(void)											
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN;		// Habilita clock para o GPIOC
	GPIOC->CRH |= GPIO_CRH_MODE13_0 | GPIO_CRH_MODE13_1;			// Seta saída em 50Mhz
	GPIOC->CRH &= ~(GPIO_CRH_CNF13_0 | GPIO_CRH_CNF13_0);			// Modo push-pull
}

//-----------------------------------------------------------------------------------------------------------------------------------------
