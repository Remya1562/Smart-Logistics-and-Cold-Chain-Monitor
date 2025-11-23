
#ifndef BME280_H_
#define BME280_H_

#include <stdint.h>
#include <stdio.h>

typedef struct {
    float temperature;
    float humidity;
    float pressure;   // ✅ New field
} SensorData;


void BME280_Init(void);
SensorData BME280_Read(void);

#endif /* BME280_H_ */
