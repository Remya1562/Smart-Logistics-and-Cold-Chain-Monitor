#include "usart1.h"

#define GPIOAEN				(1U<<0)
#define UART1EN				(1U<<4)
#define CR1_TE				(1U<<3)
#define CR1_RE				(1U<<2)
#define CR1_UE				(1U<<13)
#define SR_TXE				(1U<<7)
#define SR_RXNE				(1U<<5)

#define APB2_CLK			16000000
#define UART_BAUDRATE		9600

char uart1_read(void){
	while(!(USART1->SR & SR_RXNE)){}
	return USART1->DR;
}

void uart1_write(int ch){
	while(!(USART1->SR & SR_TXE)){}
	USART1->DR= ch & 0xFF;
}

static uint16_t compute_uart_bd(uint32_t periphclk, uint32_t baudrate){
	return ((periphclk+(baudrate/2U))/baudrate);
}

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t periphclk, uint32_t baudrate){
	USARTx->BRR = compute_uart_bd(periphclk,baudrate);
}

void uart1_rxtx_init(void){
	RCC->AHB1ENR |= GPIOAEN;
	RCC->APB2ENR |= UART1EN;

	GPIOA->MODER &=~ (3U<<18);
	GPIOA->MODER |=  (2U<<18);

	GPIOA->AFR[1] &=~ (0xF<<4);
	GPIOA->AFR[1] |=  (0x7<<4);

	GPIOA->MODER &=~ (3U<<20);
	GPIOA->MODER |=  (2U<<20);

	GPIOA->AFR[1] &=~ (0xF<<8);
	GPIOA->AFR[1] |=  (0x7<<8);

	uart_set_baudrate(USART1,APB2_CLK,UART_BAUDRATE);

	USART1->CR1 = (CR1_TE | CR1_RE);
	USART1->CR1 |= CR1_UE;

}
