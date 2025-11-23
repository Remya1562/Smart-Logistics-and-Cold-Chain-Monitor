

#ifndef INC_I2C1_H_
#define INC_I2C1_H_


#include"stm32f4xx.h"
#include<stdint.h>

void I2C1_Init(void);

int I2C1_WriteMulti(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len);
int I2C1_ReadMulti(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len);

int I2C1_Start(void);
void I2C1_Stop(void);
void I2C1_ClearADDR(void);
int I2C1_WriteAddress(uint8_t address);
int I2C1_CheckDevice(uint8_t devAddr);
int I2C1_WriteByte(uint8_t data);
uint8_t I2C1_ReadAck(void);
uint8_t I2C1_ReadNack(void);


#endif /* INC_I2C1_H_ */
