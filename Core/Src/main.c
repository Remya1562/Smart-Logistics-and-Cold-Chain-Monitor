#include "stm32f4xx.h"
#include "usart2.h"
#include "i2c1.h"
#include "bme280.h"
#include "bh1750.h"
#include <stdio.h>

void delay_ms(uint32_t ms)
{
    for (uint32_t i = 0; i < ms * 16000; i++)
        __NOP();
}


int main(void)
{
    uart_rxtx_init();
    I2C1_Init();
    BME280_Init();
    BH1750_init();
    while (1){
           SensorData env = BME280_Read();
           float lux = BH1750_read();

           printf(" Temp: %.2f °C |  Hum: %.2f %% |  Light: %.2f lx\r\n",
                  env.temperature, env.humidity, lux);

           delay_ms(100);
       }
}
