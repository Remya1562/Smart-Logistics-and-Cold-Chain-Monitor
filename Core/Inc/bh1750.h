

#ifndef INC_BH1750_H_
#define INC_BH1750_H_

#include"i2c1.h"


#include <stdint.h>


#include<stdio.h>
int BH1750_send_Command(uint8_t cmd);
void BH1750_init(void);
float BH1750_read(void);
void delay_ms(uint32_t ms);


#endif /* INC_BH1750_H_ */
