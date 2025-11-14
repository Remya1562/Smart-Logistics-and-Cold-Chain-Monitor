#include "bh1750.h"
#include "i2c1.h"

#define BH1750_ADDR 0x5C
#define BH1750_POWER_ON 0x01
#define BH1750_RESET 0x07
#define BH1750_CONT_H_RES 0x10

void BH1750_init(void)
{
    uint8_t cmd;
    cmd = BH1750_POWER_ON;
    I2C1_WriteMulti(BH1750_ADDR, 0xFF, &cmd, 1);
    cmd = BH1750_RESET;
    I2C1_WriteMulti(BH1750_ADDR, 0xFF, &cmd, 1);
    cmd = BH1750_CONT_H_RES;
    I2C1_WriteMulti(BH1750_ADDR, 0xFF, &cmd, 1);
}

float BH1750_read(void)
{
    uint8_t data[2];
    I2C1_ReadMulti(BH1750_ADDR, 0xFF, data, 2);
    uint16_t raw = (data[0] << 8) | data[1];
    float lux = raw / 1.2f;
    return lux;
}
