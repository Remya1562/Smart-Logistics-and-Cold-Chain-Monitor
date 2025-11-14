#include "bme280.h"
#include "i2c1.h"
#include <stdio.h>

#define BME280_RESET_REG     0xE0
#define BME280_CTRL_HUM      0xF2
#define BME280_CTRL_MEAS     0xF4
#define BME280_CONFIG        0xF5
#define BME280_PRESS_MSB     0xF7
#define BME280_TEMP_MSB      0xFA
#define BME280_HUM_MSB       0xFD
#define BME280_ID_REG        0xD0

#define BME280_ADDR      0x76

typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint8_t  dig_H1;
    int16_t  dig_H2;
    uint8_t  dig_H3;
    int16_t  dig_H4;
    int16_t  dig_H5;
    int8_t   dig_H6;
} BME280_CalibData;

static BME280_CalibData calib;
static int32_t t_fine;

static void BME280_ReadCalib(void){
    uint8_t buf[26], hum[7];
    I2C1_ReadMulti(BME280_ADDR, 0x88, buf, 26);
    I2C1_ReadMulti(BME280_ADDR, 0xE1, hum, 7);

    calib.dig_T1 = (buf[1] << 8) | buf[0];
    calib.dig_T2 = (int16_t)((buf[3] << 8) | buf[2]);
    calib.dig_T3 = (int16_t)((buf[5] << 8) | buf[4]);
    calib.dig_H1 = buf[25];

    calib.dig_H2 = (int16_t)((hum[1] << 8) | hum[0]);
    calib.dig_H3 = hum[2];
    calib.dig_H4 = (int16_t)((hum[3] << 4) | (hum[4] & 0x0F));
    calib.dig_H5 = (int16_t)((hum[5] << 4) | (hum[4] >> 4));
    calib.dig_H6 = (int8_t)hum[6];
}

static int32_t compensate_temp(int32_t adc_T){
    int32_t var1, var2;
    var1 = ((((adc_T >> 3) - ((int32_t)calib.dig_T1 << 1))) * ((int32_t)calib.dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)calib.dig_T1)) *
             ((adc_T >> 4) - ((int32_t)calib.dig_T1))) >> 12) *
             ((int32_t)calib.dig_T3)) >> 14;
    t_fine = var1 + var2;
    return (t_fine * 5 + 128) >> 8;
}

static uint32_t compensate_humidity(int32_t adc_H){
    int32_t v_x1 = t_fine - 76800;
    v_x1 = (((((adc_H << 14) - (((int32_t)calib.dig_H4) << 20)
              - (((int32_t)calib.dig_H5) * v_x1)) + 16384) >> 15) *
              (((((((v_x1 * ((int32_t)calib.dig_H6)) >> 10) *
              (((v_x1 * ((int32_t)calib.dig_H3)) >> 11) + 32768)) >> 10) + 2097152)
              * ((int32_t)calib.dig_H2) + 8192) >> 14));
    v_x1 = v_x1 - (((((v_x1 >> 15) * (v_x1 >> 15)) >> 7) * ((int32_t)calib.dig_H1)) >> 4);
    if (v_x1 < 0)
        v_x1 = 0;
    if (v_x1 > 419430400)
        v_x1 = 419430400;
    return (v_x1 >> 12);
}

void BME280_Init(void){
    uint8_t data;

    data = 0xB6;
    I2C1_WriteMulti(BME280_ADDR, BME280_RESET_REG, &data, 1);

    for (volatile int i = 0; i < 50000; i++);
    data = 0x01;
    I2C1_WriteMulti(BME280_ADDR, BME280_CTRL_HUM, &data, 1);

    data = 0x27;
    I2C1_WriteMulti(BME280_ADDR, BME280_CTRL_MEAS, &data, 1);

    data = 0x00;
    I2C1_WriteMulti(BME280_ADDR, BME280_CONFIG, &data, 1);
}

SensorData BME280_Read(void){
    uint8_t raw[8];
    SensorData data;

    I2C1_ReadMulti(BME280_ADDR, BME280_PRESS_MSB, raw, 8);

    int32_t adc_T = ((int32_t)raw[3] << 12) | ((int32_t)raw[4] << 4) | (raw[5] >> 4);
    int32_t adc_H = ((int32_t)raw[6] << 8) | raw[7];

    data.temperature = (float)compensate_temp(adc_T) / 100.0f;
    data.humidity = (float)compensate_humidity(adc_H) / 1024.0f;

    return data;
}
