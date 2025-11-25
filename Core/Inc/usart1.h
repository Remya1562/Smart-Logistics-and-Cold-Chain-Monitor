
#ifndef INC_USART1_H_
#define INC_USART1_H_

#include "stm32f4xx.h"
#include <stdint.h>
void uart1_rxtx_init(void);
char uart1_read(void);
void uart1_write(int ch);

#endif /* INC_USART1_H_ */
