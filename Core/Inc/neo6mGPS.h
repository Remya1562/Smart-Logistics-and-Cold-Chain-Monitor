
#ifndef INC_NEO6MGPS_H_
#define INC_NEO6MGPS_H_

#include "stm32f4xx.h"
#include <stdint.h>

typedef struct{
	double latitude;
	double longitude;
	double speed;
	double course;
	int fix;
	int satellite;
	double altitude;
	char time[16];
	char date[8];
}GPS_Data;

extern GPS_Data gps_data;

double GPSConvertCoord(const char *coord, char hemis);
void GPS_ParseGGA(char *sentence);
void GPS_ParseRMC(char *sentence);
void GPS_ProcessSentence(char *sentence);

#endif
