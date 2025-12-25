
#ifndef INC_SPI1_H_
#define INC_SPI1_H_
#include "stm32f4xx.h"
void SPI_SetHighSpeed(void){

void SPI1_Init(void);
uint8_t SPI1_TransmitReceive(uint8_t data);
void CS_Enable(void);
void CS_Disable(void);
void SPI1_TransmitBuffer(uint8_t *buf, uint32_t len);
void SPI1_ReceiveBuffer(uint8_t *buf, uint32_t len);

#endif /* INC_SPI1_H_ */
