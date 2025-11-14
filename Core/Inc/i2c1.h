

#ifndef INC_I2C1_H_
#define INC_I2C1_H_


#include"stm32f4xx.h"
#include<stdint.h>
// Initialization
void I2C1_Init(void);

// High-level universal APIs
int I2C1_WriteMulti(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len);
int I2C1_ReadMulti(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len);

// Optional helper APIs


// ======== Low-level control (optional if you need custom control) ========
void I2C1_Start(void);
void I2C1_Stop(void);
void I2C1_ClearADDR(void);
void I2C1_WriteAddress(uint8_t address);
void I2C1_WriteByte(uint8_t data);
uint8_t I2C1_ReadAck(void);
uint8_t I2C1_ReadNack(void);


#endif /* INC_I2C1_H_ */
