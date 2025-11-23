#include "stm32f4xx.h"
#include "usart2.h"
#include "i2c1.h"
#include "bme280.h"
#include "bh1750.h"
#include "mpu6050.h"
#include <stdio.h>


void delay_ms(uint32_t ms){
    for (uint32_t i = 0; i < ms * 16000; i++)
        __NOP();
}


int main(void){
    uart_rxtx_init();
    I2C1_Init();
    BME280_Init();
    BH1750_Init();
    MPU6050_Init();
    while (1){
    	MPUData result = MPU6050_Read();
    	printf("Ax=%.2f Ay=%.2f Az=%.2f Gx=%.2f Gy=%.2f Gz=%.2f \r\n",
    			result.Ax,result.Ay,result.Az,result.Gx,result.Gy,result.Gz);
    	SensorData env = BME280_Read();
    	float lux = BH1750_Read();
    	printf("%.2f C | %.2f%% |  %.2f hPa |  %.2f lx\r\n",
                         env.temperature, env.humidity, env.pressure, lux);
    	delay_ms(100);
    }
}

