#include "spi1.h"

#define GPIOAEN			(1U<<0)
#define SPI1EN			(1U<<12)
#define SPE_EN			(1U<<6)
#define SPI_MSTR		(1U<<2)
#define SPI_SSM			(1U<<9)
#define SPI_SSI			(1U<<8)
#define SPI_CPOL		(1U<<1)
#define SPI_CPHA		(1U<<0)
#define SR_TXE			(1U<<1)
#define SR_RXNE			(1U<<0)


void SPI1_Init(void){
	RCC->AHB1ENR |= GPIOAEN;
	RCC->APB2ENR |= SPI1EN;

	GPIOA->MODER &=~ ((3U<<10) | (3U<<12) | (3U<<14));
	GPIOA->MODER |=  ((2U<<10) | (2U<<12) | (2U<<14));

	GPIOA->AFR[0] &=~ ((0xF<<20) | (0xF<<24) | (0XF<<28));
	GPIOA->AFR[0] |=  ((5U<<20) | (5U<<24) | (5U<<28));

	GPIOA->MODER &=~ (3U<<8);
	GPIOA->MODER |= (1U<<8);
	GPIOA->ODR |= (1U<<4);

	SPI1->CR1 &=~ SPE_EN;
	SPI1->CR1 |=  SPI_MSTR;

	SPI1->CR1 |=  SPI_SSM;
	SPI1->CR1 |=  SPI_SSI;

	SPI1->CR1 &=~  SPI_CPOL;
	SPI1->CR1 &=~  SPI_CPHA;

	SPI1->CR1 &=~ (7U<<3);
	SPI1->CR1 |=  (7U<<3);

	SPI1->CR1 |= SPE_EN;

}

uint8_t SPI1_TransmitReceive(uint8_t data){
	while(!(SPI1->SR & SR_TXE));

	*((volatile uint8_t *)&SPI1->DR) = data;

	while (!(SPI1->SR & SR_RXNE));

	return *((volatile uint8_t *)&SPI1->DR);
}

void CS_Enable(void){
	GPIOA->ODR &=~ (1U<<4); //CS Low, start transaction
}

void CS_Disable(void){
	GPIOA->ODR |= (1U<<4); //CS High, stop transaction
}

void SPI1_TransmitBuffer(uint8_t *buf, uint32_t len){
	for(uint32_t i=0; i<len; i++){
		SPI1_TransmitReceive(buf[i]);
	}
}

void SPI1_ReceiveBuffer(uint8_t *buf, uint32_t len){
	for(uint32_t i=0; i<len; i++){
		buf[i] = SPI1_TransmitReceive(0xFF);
	}
}






