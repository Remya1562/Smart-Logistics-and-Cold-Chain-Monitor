
#ifndef INC_MPU6050_H_
#define INC_MPU6050_H_

#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>

#define MPU6050_ADDR   0x68
#define WHO_AM_I_REG   0x75
#define PWR_MGMT_1     0x6B
#define ACCEL_XOUT_H   0x3B
#define GYRO_XOUT_H    0x43

int mpu6050_init(void);
int mpu_whoami(uint8_t *who);
int mpu_read_accel_raw(int16_t *ax, int16_t *ay, int16_t *az);
int mpu_read_gyro_raw(int16_t *gx, int16_t *gy, int16_t *gz);

#endif


#endif /* INC_MPU6050_H_ */
