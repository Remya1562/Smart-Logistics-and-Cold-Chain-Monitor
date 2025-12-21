

#include "stm32f4xx.h"
#include <stdio.h>

#include "usart2.h"
#include "i2c1.h"
#include "bme280.h"
#include "bh1750.h"
#include "mpu6050.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

SemaphoreHandle_t xI2C1Mutex;
SemaphoreHandle_t xUSART2Mutex;

void vTaskBME280(void *pvParameters){
	while(1){
		xSemaphoreTake(xI2C1Mutex, portMAX_DELAY);
		SensorData env = BME280_Read();
		xSemaphoreGive(xI2C1Mutex);

		xSemaphoreTake(xUSART2Mutex, portMAX_DELAY);
		printf("%.2f C | %.2f%% |  %.2f hPa\r\n",
		                      env.temperature, env.humidity, env.pressure);
		xSemaphoreGive(xUSART2Mutex);

		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

void vTaskBH1750(void *pvParameters){
	while(1){
		xSemaphoreTake(xI2C1Mutex, portMAX_DELAY);
		float lux = BH1750_Read();
		xSemaphoreGive(xI2C1Mutex);

		xSemaphoreTake(xUSART2Mutex, portMAX_DELAY);
		printf("light intensity = %.2f lx\r\n",lux);
		xSemaphoreGive(xUSART2Mutex);

		vTaskDelay(pdMS_TO_TICKS(200));
	}
}

void vTaskMPU6050(void *pvParameters){
	while(1){
		xSemaphoreTake(xI2C1Mutex, portMAX_DELAY);
		MPUData result = MPU6050_Read();
		xSemaphoreGive(xI2C1Mutex);

		xSemaphoreTake(xUSART2Mutex, portMAX_DELAY);
		printf("Ax=%.2f Ay=%.2f Az=%.2f Gx=%.2f Gy=%.2f Gz=%.2f \r\n",
  			  result.Ax,result.Ay,result.Az,result.Gx,result.Gy,result.Gz);
		xSemaphoreGive(xUSART2Mutex);

		vTaskDelay(pdMS_TO_TICKS(400));
	}
}

int main(){
	 uart2_rxtx_init();
	 I2C1_Init();

	 xI2C1Mutex = xSemaphoreCreateMutex();
	 xUSART2Mutex = xSemaphoreCreateMutex();

	 xSemaphoreTake(xI2C1Mutex, portMAX_DELAY);
	 BME280_Init();
	 BH1750_Init();
	 MPU6050_Init();
	 xSemaphoreGive(xI2C1Mutex);

	 xTaskCreate(vTaskBME280,"Environment measurements",768,NULL,2,NULL);
	 xTaskCreate(vTaskBH1750,"Light Intensity",256,NULL,1,NULL);
	 xTaskCreate(vTaskMPU6050,"Shock & Orientation",768,NULL,3,NULL);

	 vTaskStartScheduler();
	 while(1);
}
