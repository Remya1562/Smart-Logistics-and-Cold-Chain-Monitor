#include "sdcard.h"
#include "spi1.h"

void SD_SendIdleClocks(void){
	CS_Disable();
	for(int i=0; i<10; i++){
		SPI1_TransmitReceive(0xFF);
	}
}

uint8_t SD_SendCMD0(void){
	uint8_t r1;
	static uint8_t cmd0_packet[6] = {0x40, 0x00, 0x00, 0x00, 0x00, 0x95};

	CS_Enable();

	SPI1_TransmitBuffer(cmd0_packet,6);
	for(int i=0; i<10; i++){
		r1 = SPI1_TransmitReceive(0xFF);
		if(r1 != 0xFF)
		break;
	}
	CS_Disable();
	return r1;
}

uint8_t SD_SendCMD8(uint8_t *r7){
	uint8_t r1;

	static  uint8_t cmd8_packet[6] = {0x48, 0x00, 0x00, 0x01, 0xAA, 0x87};

	CS_Enable();

	SPI1_TransmitBuffer(cmd8_packet,6);
	for(int i=0; i<10; i++){
		r1 = SPI1_TransmitReceive(0xFF);
		if(r1 != 0xFF)
			break;
	}

	for(int i=0; i<4; i++){
		r7[i] = SPI1_TransmitReceive(0xFF);
	}
	CS_Disable();
	return r1;
}


uint8_t SD_SendACMD41(void){
	uint8_t r1;

	CS_Enable();

	static uint8_t cmd55_packet[6] = {0x77, 0x00, 0x00, 0x00, 0x00, 0xFF};

	SPI1_TransmitBuffer(cmd55_packet,6);

	for(int i=0; i<10; i++){
		r1 = SPI1_TransmitReceive(0xFF);
		if(r1 != 0xFF)
		break;
	}

	SPI1_TransmitReceive(0xFF);

	static uint8_t cmd41_packet[6] = {0x69, 0x40, 0x00, 0x00, 0x00, 0xFF};
	SPI1_TransmitBuffer(cmd41_packet,6);
	for(int i=0; i<10; i++){
		r1 = SPI1_TransmitReceive(0xFF);
		if(r1 != 0xFF)
		break;
	}
	CS_Disable();
	SPI1_TransmitReceive(0xFF);
	return r1;
}


uint8_t SD_SendCMD58(uint8_t *r7){
	uint8_t r1;

	static  uint8_t cmd58_packet[6] = {0x7A, 0x00, 0x00, 0x00, 0x00, 0xFF};

	CS_Enable();

	SPI1_TransmitBuffer(cmd58_packet,6);
	for(int i=0; i<10; i++){
		r1 = SPI1_TransmitReceive(0xFF);
		if(r1 != 0xFF)
			break;
	}

	for(int i=0; i<4; i++){
		r7[i] = SPI1_TransmitReceive(0xFF);
	}

	CS_Disable();
	SPI1_TransmitReceive(0xFF);
	return r1;
}

uint8_t SD_ReadBlock(uint32_t block, uint8_t *buf){
	uint8_t r1;
	uint8_t token;

	CS_Enable();
	SPI1_TransmitReceive(0x51);
	SPI1_TransmitReceive(block >> 24);
	SPI1_TransmitReceive(block >> 16);
	SPI1_TransmitReceive(block >> 8);
	SPI1_TransmitReceive(block >> 0);
	SPI1_TransmitReceive(0xFF);

	for(int i=0; i<10; i++){
		r1 = SPI1_TransmitReceive(0xFF);
		if(r1 != 0xFF)
		break;
	}
	if(r1 != 0x00){
		CS_Disable();
		return r1;
	}

	for(int i=0 ; i<100000 ; i++){
		token = SPI1_TransmitReceive(0xFF);
		if(token == 0xFE)
			break;
	}

	for(int i=0 ; i<512 ; i++){
		buf[i] = SPI1_TransmitReceive(0xFF);
	}

	SPI1_TransmitReceive(0xFF);
	SPI1_TransmitReceive(0xFF);

	CS_Disable();
	SPI1_TransmitReceive(0xFF);
	return 0x00;

}

uint8_t SD_WriteBlock(uint8_t block, uint8_t *buf){
	uint8_t r1;
	uint8_t resp;

	CS_Enable();
	SPI1_TransmitReceive(0x58);
	SPI1_TransmitReceive(block >> 24);
	SPI1_TransmitReceive(block >> 16);
	SPI1_TransmitReceive(block >> 8);
	SPI1_TransmitReceive(block >> 0);
	SPI1_TransmitReceive(0xFF);

	for(int i=0; i<10; i++){
		r1 = SPI1_TransmitReceive(0xFF);
		if(r1 != 0xFF)
			break;
	}
	if(r1 != 0x00){
		CS_Disable();
		SPI1_TransmitReceive(0xFE);
		return r1;
	}

    SPI1_TransmitReceive(0xFE);

	for(int i=0 ; i<512 ; i++){
		SPI1_TransmitReceive(buf[i]);
	}

	SPI1_TransmitReceive(0xFF);
	SPI1_TransmitReceive(0xFF);

	resp = SPI1_TransmitReceive(0xFF);

	if((resp & 0x1F) != 0x05 ){
		CS_Disable();
		return resp;
	}

	while (SPI1_TransmitReceive(0xFF) == 0x00);

	CS_Disable();
	SPI1_TransmitReceive(0xFF);
	return 0x00;
}
