
#ifndef INC_MPU6050_H_
#define INC_MPU6050_H_

#ifndef MPU6050_H
#define MPU6050_H

#include <i2c1.h>
#include <stdint.h>
typedef struct{
	float Ax,Ay,Az;
	float Gx,Gy,Gz;
}MPUData;

void MPU6050_Init(void);
MPUData MPU6050_Read(void);

#endif


#endif /* INC_MPU6050_H_ */
