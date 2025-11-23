#include "stm32f4xx.h"
#include "i2c1.h"
#include "usart2.h"
#include "bme280.h"
#include <stdio.h>

#define BME280_ADDR      0x76
#define BME280_ID_REG    0xD0
#define BME280_RESET     0xE0
#define BME280_CTRL_HUM  0xF2
#define BME280_STATUS    0xF3
#define BME280_CTRL_MEAS 0xF4
#define BME280_CONFIG    0xF5
#define BME280_PRESS_MSB 0xF7
#define BME280_TEMP_MSB  0xFA
#define BME280_HUM_MSB   0xFD

typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
    uint8_t  dig_H1;
    int16_t  dig_H2;
    uint8_t  dig_H3;
    int16_t  dig_H4;
    int16_t  dig_H5;
    int8_t   dig_H6;
} BME280_CalibData;

static BME280_CalibData calib;
static int32_t t_fine;

void BME280_ReadCalib(void)
{
    uint8_t buf1[26];
    uint8_t buf2[7];

    I2C1_ReadMulti(BME280_ADDR, 0x88, buf1, 26);
    I2C1_ReadMulti(BME280_ADDR, 0xE1, buf2, 7);

    // Temperature
    calib.dig_T1 = (buf1[1] << 8) | buf1[0];
    calib.dig_T2 = (int16_t)((buf1[3] << 8) | buf1[2]);
    calib.dig_T3 = (int16_t)((buf1[5] << 8) | buf1[4]);

    // Pressure
    calib.dig_P1 = (buf1[7] << 8) | buf1[6];
    calib.dig_P2 = (int16_t)((buf1[9] << 8) | buf1[8]);
    calib.dig_P3 = (int16_t)((buf1[11] << 8) | buf1[10]);
    calib.dig_P4 = (int16_t)((buf1[13] << 8) | buf1[12]);
    calib.dig_P5 = (int16_t)((buf1[15] << 8) | buf1[14]);
    calib.dig_P6 = (int16_t)((buf1[17] << 8) | buf1[16]);
    calib.dig_P7 = (int16_t)((buf1[19] << 8) | buf1[18]);
    calib.dig_P8 = (int16_t)((buf1[21] << 8) | buf1[20]);
    calib.dig_P9 = (int16_t)((buf1[23] << 8) | buf1[22]);

    calib.dig_H1 = buf1[25];
    calib.dig_H2 = (int16_t)((buf2[1] << 8) | buf2[0]);
    calib.dig_H3 = buf2[2];
    calib.dig_H4 = (int16_t)((buf2[3] << 4) | (buf2[4] & 0x0F));
    calib.dig_H5 = (int16_t)((buf2[5] << 4) | (buf2[4] >> 4));
    calib.dig_H6 = (int8_t)buf2[6];
}


void BME280_Init(void)
{
    uint8_t data;

    // Reset
    data = 0xB6;
    I2C1_WriteMulti(BME280_ADDR, BME280_RESET, &data, 1);
    for (volatile int i = 0; i < 50000; i++);

    // Humidity oversampling x1
    data = 0x01;
    I2C1_WriteMulti(BME280_ADDR, BME280_CTRL_HUM, &data, 1);

    // Pressure/Temp oversampling x1, mode = normal
    data = 0x27;
    I2C1_WriteMulti(BME280_ADDR, BME280_CTRL_MEAS, &data, 1);

    // Config (standby 0.5ms, filter off)
    data = 0x00;
    I2C1_WriteMulti(BME280_ADDR, BME280_CONFIG, &data, 1);

    // Read calibration
    BME280_ReadCalib();
}


uint8_t BME280_CheckConnection(void)
{
    uint8_t id = 0;
    I2C1_ReadMulti(BME280_ADDR, BME280_ID_REG, &id, 1);
    return (id == 0x60);
}

int32_t compensate_T(int32_t adc_T)
{
    int32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((int32_t)calib.dig_T1 << 1))) * ((int32_t)calib.dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)calib.dig_T1)) * ((adc_T >> 4) - ((int32_t)calib.dig_T1))) >> 12) *
            ((int32_t)calib.dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

uint32_t compensate_P(int32_t adc_P)
{
    int64_t var1, var2, p;
    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)calib.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)calib.dig_P3) >> 8) + ((var1 * (int64_t)calib.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)calib.dig_P1) >> 33;

    if (var1 == 0) return 0;

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)calib.dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)calib.dig_P7) << 4);
    return (uint32_t)p; // in Pa
}

uint32_t compensate_H(int32_t adc_H)
{
    int32_t v_x1_u32r;
    v_x1_u32r = t_fine - 76800;
    v_x1_u32r = (((((adc_H << 14) - (((int32_t)calib.dig_H4) << 20) -
                    (((int32_t)calib.dig_H5) * v_x1_u32r)) + 16384) >> 15) *
                  (((((((v_x1_u32r * ((int32_t)calib.dig_H6)) >> 10) *
                       (((v_x1_u32r * ((int32_t)calib.dig_H3)) >> 11) + 32768)) >> 10) +
                     2097152) * ((int32_t)calib.dig_H2) + 8192) >> 14));
    v_x1_u32r = v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                               ((int32_t)calib.dig_H1)) >> 4);
    if (v_x1_u32r < 0)
        v_x1_u32r = 0;
    if (v_x1_u32r > 419430400)
        v_x1_u32r = 419430400;
    return (uint32_t)(v_x1_u32r >> 12);
}


SensorData BME280_Read(void)
{
    uint8_t raw[8];
    I2C1_ReadMulti(BME280_ADDR, BME280_PRESS_MSB, raw, 8);

    int32_t adc_P = ((int32_t)raw[0] << 12) | ((int32_t)raw[1] << 4) | (raw[2] >> 4);
    int32_t adc_T = ((int32_t)raw[3] << 12) | ((int32_t)raw[4] << 4) | (raw[5] >> 4);
    int32_t adc_H = ((int32_t)raw[6] << 8) | raw[7];

    SensorData data;
    data.temperature = compensate_T(adc_T) / 100.0f;
    data.pressure = compensate_P(adc_P) / 25600.0f; // hPa
    data.humidity = compensate_H(adc_H) / 1024.0f;

    return data;
}
