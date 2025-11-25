#include "usart2.h"

#define GPIOAEN				(1U<<0)
#define UART2EN				(1U<<17)
#define CR1_TE				(1U<<3)
#define CR1_RE				(1U<<2)
#define CR1_UE				(1U<<13)
#define SR_TXE				(1U<<7)
#define SR_RXNE				(1U<<5)

#define APB1_CLK			16000000
#define UART_BAUDRATE		9600

char uart2_read(void){
	while(!(USART2->SR & SR_RXNE)){}
	return USART2->DR;
}


int __io_putchar(int ch){
	uart2_write(ch);
	return ch;
}

void uart2_write(int ch){
	while(!(USART2->SR & SR_TXE)){}
	USART2->DR= ch & 0xFF;
}

static uint16_t compute_uart_bd(uint32_t periphclk, uint32_t baudrate){
	return ((periphclk+(baudrate/2U))/baudrate);
}

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t periphclk, uint32_t baudrate){
	USARTx->BRR = compute_uart_bd(periphclk,baudrate);
}

void uart2_rxtx_init(void){
	RCC->AHB1ENR |= GPIOAEN;
	RCC->APB1ENR |= UART2EN;

	GPIOA->MODER &=~ (3U<<4);
	GPIOA->MODER |=  (2U<<4);

	GPIOA->AFR[0] &=~ (0xF<<8);
	GPIOA->AFR[0] |=  (0x7<<8);

	GPIOA->MODER &=~ (3U<<6);
	GPIOA->MODER |=  (2U<<6);

	GPIOA->AFR[0] &=~ (0xF<<12);
	GPIOA->AFR[0] |=  (0x7<<12);

	uart_set_baudrate(USART2,APB1_CLK,UART_BAUDRATE);

	USART2->CR1 = (CR1_TE | CR1_RE);
	USART2->CR1 |= CR1_UE;
}
