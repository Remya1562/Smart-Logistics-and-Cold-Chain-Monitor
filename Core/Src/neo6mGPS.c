#include "neo6mGPS.h"
#include <string.h>
#include <stdlib.h>

GPS_Data gps_data;
double GPSConvertCoord(const char *coord, char hemis){
	if(!coord || coord[0] == '\0')
		return 0;

	double raw = atof(coord);
	int degrees;
	double minutes;
	double result;

	if(raw<100)
		return 0;

	degrees = (int)(raw/100);
	minutes = raw - (degrees*100);

	result = degrees + (minutes / 60.0);

	if(hemis == 'S' || hemis == 'W')
		result = -result;

	return result;
}
void GPS_ParseGGA(char *sentence){
	char buffer[128];
	strncpy(buffer, sentence, sizeof(buffer));
	buffer[sizeof(buffer)-1] = '\0';

	char *tok = strtok(buffer, ",");
	int index = 0;

	char *utc_time = NULL;
	char *lat_str = NULL;
	char *lat_hemis = NULL;
	char *long_str = NULL;
	char *long_hemis = NULL;
	char *fix = NULL;
	char *satellite = NULL;
	char *alt_str = NULL;

	while(tok != NULL){
		switch(index){
		case 1: utc_time = tok;
			break;
		case 2: lat_str = tok;
			break;
		case 3: lat_hemis = tok;
			break;
		case 4: long_str = tok;
			break;
		case 5: long_hemis = tok;
			break;
		case 6: fix = tok;
			break;
		case 7: satellite = tok;
			break;
		case 9: alt_str = tok;
			break;
		}
		tok = strtok(NULL, ",");
		index++;
	}
	if(utc_time && utc_time[0] != '\0')
		strncpy(gps_data.time, utc_time, sizeof(gps_data.time));

	if(lat_str && lat_hemis && lat_str[0] != '\0')
		gps_data.latitude = GPSConvertCoord(lat_str, lat_hemis[0]);

	if(long_str && long_hemis && long_str[0] != '\0')
		gps_data.longitude = GPSConvertCoord(long_str, long_hemis[0]);

	if(fix)
		gps_data.fix = atoi(fix);

	if(satellite)
		gps_data.satellite = atoi(satellite);

	if(alt_str && alt_str[0] != '\0')
		gps_data.altitude = atof(alt_str);

}

void GPS_ParseRMC(char *sentence){
	char buffer[128];
	strncpy(buffer, sentence, sizeof(buffer));
	buffer[sizeof(buffer)-1] = '\0';

	char *tok = strtok(buffer, ",");
	int index = 0;

	char *utc_time = NULL;
	char *status = NULL;
	char *lat_str = NULL;
	char *lat_hemis = NULL;
	char *long_str = NULL;
	char *long_hemis = NULL;
	char *speed_knot = NULL;
	char *course_str = NULL;
	char *date_str = NULL;
	while(tok != NULL){
		switch(index){
		case 1: utc_time = tok;
			break;
		case 2: status = tok;
				break;
		case 3: lat_str = tok;
			break;
		case 4: lat_hemis = tok;
			break;
		case 5: long_str = tok;
			break;
		case 6: long_hemis = tok;
			break;
		case 7: speed_knot = tok;
			break;
		case 8: course_str = tok;
			break;
		case 9: date_str = tok;
		}
		tok = strtok(NULL, ",");
		index++;
	}
	if(!status || status[0] != 'A'){
		gps_data.fix = 0;
		return;
	}
	gps_data.fix = 1;

	if(utc_time && utc_time[0] != '\0')
			strncpy(gps_data.time, utc_time, sizeof(gps_data.time));

	if(lat_str && lat_hemis && lat_str[0] != '\0')
		gps_data.latitude = GPSConvertCoord(lat_str, lat_hemis[0]);

	if(long_str && long_hemis && long_str[0] != '\0')
		gps_data.longitude = GPSConvertCoord(long_str, long_hemis[0]);

	if(speed_knot && speed_knot[0] != '\0'){
		double knot = atof(speed_knot);
		gps_data.speed = knot*1.852;
	}

	if(course_str && course_str[0] != '\0')
		gps_data.course = atof(course_str);

	if(date_str && date_str[0] != '\0')
		strncpy(gps_data.date ,date_str, sizeof(gps_data.date));
}

void GPS_ProcessSentence(char *sentence){
	if(sentence == NULL)
		return;

	int len = strlen(sentence);

	while(len > 0 && (sentence[len-1] ==  '\r' || sentence[len-1] == '\n')){
		sentence[--len] = '\0';
	}

	if(len<6)
		return;

	if(strncmp(sentence, "$GPGGA", 6) == 0){
		GPS_ParseGGA(sentence);
	}
	else if(strncmp(sentence, "$GPRMC", 6) == 0){
		GPS_ParseRMC(sentence);
	}
	else{

			}
}
