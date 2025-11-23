#include "mpu6050.h"

#define MPU6050_ADDR			0x68
#define PWR_MGMT_1				0x6B
#define GYRO_CONFIG				0x1B
#define ACCEL_CONFIG			0x1C
#define SMPLRT_DIV 				0x19
#define DLPF_CFG				0x1A

#define ACCEL_XOUT_H			0x3B



void MPU6050_Init(void){
	uint8_t data;

	data = 0x00;
	I2C1_WriteMulti(MPU6050_ADDR, PWR_MGMT_1, &data,1);

	data = 0x00;
	I2C1_WriteMulti(MPU6050_ADDR, GYRO_CONFIG, &data, 1);

	data = 0x00;
	I2C1_WriteMulti(MPU6050_ADDR, ACCEL_CONFIG, &data, 1);

	data = 0x06;
	I2C1_WriteMulti(MPU6050_ADDR, DLPF_CFG, &data, 1);

	data = 0x07;
	I2C1_WriteMulti(MPU6050_ADDR, SMPLRT_DIV, &data, 1);

}

MPUData MPU6050_Read(void){
	uint8_t buffer[14];
	I2C1_ReadMulti(MPU6050_ADDR,ACCEL_XOUT_H, buffer, 14);
	MPUData mpu;
	int16_t raw_Ax = (buffer[0] << 8) | buffer[1];
	int16_t raw_Ay = (buffer[2] << 8) | buffer[3];
	int16_t raw_Az = (buffer[4] << 8) | buffer[5];

	int16_t raw_Gx = (buffer[8] << 8) | buffer[9];
	int16_t raw_Gy = (buffer[10] << 8) | buffer[11];
	int16_t raw_Gz = (buffer[12] << 8) | buffer[13];

	mpu.Ax = raw_Ax / 16384.0f;
	mpu.Ay = raw_Ay / 16384.0f;
	mpu.Az = raw_Az / 16384.0f;

	mpu.Gx = raw_Gx / 131.0f;
	mpu.Gy = raw_Gy / 131.0f;
	mpu.Gz = raw_Gz / 131.0f;

	return mpu;

}
