
#ifndef INC_SDCARD_H_
#define INC_SDCARD_H_


#include "stm32f4xx.h"
#include <stdio.h>
#include "spi1.h"
void SD_SendIdleClocks(void);
uint8_t SD_SendCMD0(void);
uint8_t SD_SendCMD8(uint8_t *r7);
uint8_t SD_SendCMD55(void);
uint8_t SD_SendCMD41(void);
uint8_t SD_SendACMD41(void);
uint8_t SD_Init(void);
uint8_t SD_SendCMD58(uint8_t *ocr);
uint8_t SD_SendCMD58(uint8_t *r7);
uint8_t SD_ReadBlock(uint32_t block, uint8_t *buf);
uint8_t SD_WriteBlock(uint8_t block, uint8_t *buf);


#endif /* INC_SDCARD_H_ */
