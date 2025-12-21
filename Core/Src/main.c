//#include "stm32f4xx.h"
//#include "usart2.h"
//#include "usart1.h"
//#include "neo6mGPS.h"
//#include <stdio.h>
//
//extern GPS_Data gps_data;
//
//void delay_ms(uint32_t ms)
//{
//    for (uint32_t i = 0; i < ms * 16000; i++)
//        __NOP();
//}
//
//int main(void)
//{
//    uart2_rxtx_init();
//    printf("USART2 ready\r\n");
//
//    uart1_rxtx_init();
//    printf("USART1 ready (GPS)\r\n");
//
//    char line[128];
//    int pos = 0;
//
//    while (1)
//    {
//        char c = uart1_read();       // read from GPS (USART1)
//
//        if (c == '\r')
//            continue;                // ignore CR
//
//        if (c == '\n')
//        {
//            line[pos] = '\0';
//
//            // print full NMEA sentence to USART2
//            printf("%s\r\n", line);
//
//            // process sentence
//            GPS_ProcessSentence(line);
//
//            pos = 0; // reset buffer
//
//            // if GPS fix = 1 → print decoded data
//            if (gps_data.fix == 1)
//            {
//                printf("----- GPS FIX -----\r\n");
//                printf("Time: %s  Date: %s\r\n", gps_data.time, gps_data.date);
//                printf("Lat: %.6f  Lon: %.6f\r\n", gps_data.latitude, gps_data.longitude);
//                printf("Alt: %.2f m\r\n", gps_data.altitude);
//                printf("Speed: %.2f km/h  Course: %.2f deg\r\n", gps_data.speed, gps_data.course);
//                printf("Sats: %d\r\n", gps_data.satellite);
//                printf("--------------------\r\n\r\n");
//            }
//        }
//        else
//        {
//            // accumulate NMEA characters
//            if (pos < sizeof(line) - 1)
//                line[pos++] = c;
//        }
//    }
//}

//#include "stm32f4xx.h"
//#include "sdcard.h"
//#include <stdio.h>
//#include <stdint.h>
//#include "spi1.h"
//#include "usart2.h"
//
//uint8_t tx[512];
//uint8_t rx[512];
//
//int main(void)
//{
//    uart2_rxtx_init();
//    spi_gpio_init();
//    spi1_config();
//
//    SD_Init();
//
//    for (int i = 0; i < 512; i++)
//        tx[i] = i;
//
//    if (SD_WriteBlock(200, tx) != 0)
//        while (1);
//
//    if (SD_ReadBlock(200, rx) != 0)
//        while (1);
//
//    for (int i = 0; i < 512; i++)
//        if (tx[i] != rx[i])
//            while (1);
//
//    while (1);   // SUCCESS if execution reaches here
//}


#include "stm32f4xx.h"
#include <stdio.h>
#include <stdint.h>
#include "spi1.h"
#include "sdcard.h"
#include "usart2.h"

int main(){

    uint8_t r1;
    uint8_t r7[4];

    uart2_rxtx_init();   // for printf
    SPI1_Init();         // SPI hardware init

    SD_SendIdleClocks(); // mandatory after power-up

    r1 = SD_SendCMD0();
    printf("CMD0 R1 = 0x%02X\r\n", r1);

    r1 = SD_SendCMD8(r7);
    printf("CMD8 R1 = 0x%02X\r\n", r1);
    printf("CMD8 R7 = %02X %02X %02X %02X\r\n",
           r7[0], r7[1], r7[2], r7[3]);

    r1 = SD_SendCMD55();
    printf("CMD55 R1 = 0x%02X\r\n", r1);


    while (1);

}

//#include "stm32f4xx.h"
//#include <stdio.h>
//
//#include "usart2.h"
//#include "i2c1.h"
//#include "bme280.h"
//#include "bh1750.h"
//#include "mpu6050.h"
//
//#include "FreeRTOS.h"
//#include "task.h"
//#include "semphr.h"
//
//SemaphoreHandle_t xI2C1Mutex;
//SemaphoreHandle_t xUSART2Mutex;
//
//void vTaskBME280(void *pvParameters){
//	while(1){
//		xSemaphoreTake(xI2C1Mutex, portMAX_DELAY);
//		SensorData env = BME280_Read();
//		xSemaphoreGive(xI2C1Mutex);
//
//		xSemaphoreTake(xUSART2Mutex, portMAX_DELAY);
//		printf("%.2f C | %.2f%% |  %.2f hPa\r\n",
//		                      env.temperature, env.humidity, env.pressure);
//		xSemaphoreGive(xUSART2Mutex);
//
//		vTaskDelay(pdMS_TO_TICKS(500));
//	}
//}
//
//void vTaskBH1750(void *pvParameters){
//	while(1){
//		xSemaphoreTake(xI2C1Mutex, portMAX_DELAY);
//		float lux = BH1750_Read();
//		xSemaphoreGive(xI2C1Mutex);
//
//		xSemaphoreTake(xUSART2Mutex, portMAX_DELAY);
//		printf("light intensity = %.2f lx\r\n",lux);
//		xSemaphoreGive(xUSART2Mutex);
//
//		vTaskDelay(pdMS_TO_TICKS(200));
//	}
//}
//
//void vTaskMPU6050(void *pvParameters){
//	while(1){
//		xSemaphoreTake(xI2C1Mutex, portMAX_DELAY);
//		MPUData result = MPU6050_Read();
//		xSemaphoreGive(xI2C1Mutex);
//
//		xSemaphoreTake(xUSART2Mutex, portMAX_DELAY);
//		printf("Ax=%.2f Ay=%.2f Az=%.2f Gx=%.2f Gy=%.2f Gz=%.2f \r\n",
//  			  result.Ax,result.Ay,result.Az,result.Gx,result.Gy,result.Gz);
//		xSemaphoreGive(xUSART2Mutex);
//
//		vTaskDelay(pdMS_TO_TICKS(400));
//	}
//}
//
//int main(){
//	 uart2_rxtx_init();
//	 I2C1_Init();
//
//	 xI2C1Mutex = xSemaphoreCreateMutex();
//	 xUSART2Mutex = xSemaphoreCreateMutex();
//
//	 xSemaphoreTake(xI2C1Mutex, portMAX_DELAY);
//	 BME280_Init();
//	 BH1750_Init();
//	 MPU6050_Init();
//	 xSemaphoreGive(xI2C1Mutex);
//
//	 xTaskCreate(vTaskBME280,"Environment measurements",768,NULL,2,NULL);
//	 xTaskCreate(vTaskBH1750,"Light Intensity",256,NULL,1,NULL);
//	 xTaskCreate(vTaskMPU6050,"Shock & Orientation",768,NULL,3,NULL);
//
//	 vTaskStartScheduler();
//	 while(1);
//}
