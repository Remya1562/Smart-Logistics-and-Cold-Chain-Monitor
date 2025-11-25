
#ifndef USART2_H_
#define USART2_H_

#include"stm32f4xx.h"
#include<stdio.h>
#include<stdint.h>

void uart2_rxtx_init(void);
void uart2_write(int ch);


#endif /* USART2_H_ */
