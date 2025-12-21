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

	static  uint8_t cmd0_packet[6] = {0x48, 0x00, 0x00, 0x01, 0xAA, 0x87};

	CS_Enable();

	SPI1_TransmitBuffer(cmd0_packet,6);
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

uint8_t SD_SendCMD55(void){
	uint8_t r1;
	static uint8_t cmd0_packet[6] = {0x77, 0x00, 0x00, 0x00, 0x00, 0xFF};

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
//#include "sdcard.h"
//#include "spi1.h"
//#include "usart2.h"
//#include "stm32f4xx.h"
//#include <stdio.h>
//#include <stdint.h>
//
///* ============================================================
//   GLOBAL STATE
//   ============================================================ */
//static uint8_t sd_is_sdhc = 0;
//
///* ============================================================
//   DELAY
//   ============================================================ */
//void sd_delay_ms(uint32_t ms)
//{
//    for (uint32_t i = 0; i < ms * 4000; i++)
//        __NOP();
//}
//
///* ============================================================
//   SPI BYTE TRANSFER
//   ============================================================ */
//uint8_t SD_SPI_Send(uint8_t data)
//{
//    while (!(SPI1->SR & (1U << 1)));   // TXE
//    SPI1->DR = data;
//    while (!(SPI1->SR & (1U << 0)));   // RXNE
//    return SPI1->DR;
//}
//
///* ============================================================
//   SEND CMD (R1 ONLY) — CS TOGGLED INSIDE
//   DO NOT USE FOR CMD17 / CMD24
//   ============================================================ */
//uint8_t SD_Send_Command_R1(uint8_t cmd, uint32_t arg, uint8_t crc)
//{
//    uint8_t r1;
//
//    cs_enable();
//    SD_SPI_Send(0xFF);
//
//    SD_SPI_Send(0x40 | cmd);
//    SD_SPI_Send(arg >> 24);
//    SD_SPI_Send(arg >> 16);
//    SD_SPI_Send(arg >> 8);
//    SD_SPI_Send(arg);
//    SD_SPI_Send(crc);
//
//    for (int i = 0; i < 10; i++)
//    {
//        r1 = SD_SPI_Send(0xFF);
//        if (r1 != 0xFF)
//            break;
//    }
//
//    cs_disable();
//    SD_SPI_Send(0xFF);
//
//    printf("[CMD%u] R1=0x%02X\r\n", cmd, r1);
//    return r1;
//}
//
///* ============================================================
//   SEND CMD R7 (CMD8 / CMD58)
//   ============================================================ */
//uint8_t SD_Send_Command_R7(uint8_t cmd, uint32_t arg, uint8_t crc, uint8_t *r7)
//{
//    uint8_t r1 = SD_Send_Command_R1(cmd, arg, crc);
//    if (r1 > 0x01)
//        return r1;
//
//    cs_enable();
//    for (int i = 0; i < 4; i++)
//        r7[i] = SD_SPI_Send(0xFF);
//    cs_disable();
//    SD_SPI_Send(0xFF);
//
//    printf("[CMD%u] R7: %02X %02X %02X %02X\r\n",
//           cmd, r7[0], r7[1], r7[2], r7[3]);
//
//    return r1;
//}
//
///* ============================================================
//   ACMD41 — CS HELD LOW (CRITICAL)
//   ============================================================ */
//uint8_t ACMD41_CS_HOLD(uint32_t arg, uint32_t timeout_ms)
//{
//    uint8_t r1;
//    uint32_t elapsed = 0;
//
//    while (elapsed < timeout_ms)
//    {
//        cs_enable();
//
//        /* CMD55 */
//        SD_SPI_Send(0x77);
//        SD_SPI_Send(0); SD_SPI_Send(0); SD_SPI_Send(0); SD_SPI_Send(0);
//        SD_SPI_Send(0xFF);
//
//        for (int i = 0; i < 10; i++)
//        {
//            r1 = SD_SPI_Send(0xFF);
//            if (r1 != 0xFF) break;
//        }
//        printf("[CMD55] R1=0x%02X\r\n", r1);
//
//        /* CMD41 */
//        SD_SPI_Send(0x69);
//        SD_SPI_Send(arg >> 24);
//        SD_SPI_Send(arg >> 16);
//        SD_SPI_Send(arg >> 8);
//        SD_SPI_Send(arg);
//        SD_SPI_Send(0xFF);
//
//        for (int i = 0; i < 10; i++)
//        {
//            r1 = SD_SPI_Send(0xFF);
//            if (r1 != 0xFF) break;
//        }
//
//        cs_disable();
//        SD_SPI_Send(0xFF);
//
//        printf("[CMD41] R1=0x%02X\r\n", r1);
//
//        if (r1 == 0x00)
//            return 0x00;
//
//        sd_delay_ms(5);
//        elapsed += 5;
//    }
//
//    return r1;
//}
//
///* ============================================================
//   SD INIT
//   ============================================================ */
//void SD_Init(void)
//{
//    uint8_t r7[4];
//    uint8_t ocr[4];
//
//    printf("\r\n=== SD INIT START ===\r\n");
//
//    cs_disable();
//    for (int i = 0; i < 10; i++)
//        SD_SPI_Send(0xFF);
//
//    if (SD_Send_Command_R1(0, 0, 0x95) != 0x01)
//        return;
//
//    if (SD_Send_Command_R7(8, 0x1AA, 0x87, r7) != 0x01)
//        return;
//
//    if (ACMD41_CS_HOLD(0x40000000, 5000) != 0x00)
//        return;
//
//    if (SD_Send_Command_R7(58, 0, 0xFF, ocr) != 0x00)
//        return;
//
//    sd_is_sdhc = (ocr[0] & 0x40) ? 1 : 0;
//    printf("CARD TYPE: %s\r\n", sd_is_sdhc ? "SDHC/SDXC" : "SDSC");
//
//    printf("=== SD INIT DONE ===\r\n");
//}
//
///* ============================================================
//   WRITE BLOCK — CMD24 (FIXED)
//   ============================================================ */
//uint8_t SD_WriteBlock(uint32_t block, const uint8_t *buf)
//{
//    uint32_t addr = sd_is_sdhc ? block : block * 512;
//    uint8_t token;
//
//    printf("\r\n[WRITE] BLOCK=%lu ADDR=%lu\r\n", block, addr);
//
//    cs_enable();
//    SD_SPI_Send(0x58);
//    SD_SPI_Send(addr >> 24);
//    SD_SPI_Send(addr >> 16);
//    SD_SPI_Send(addr >> 8);
//    SD_SPI_Send(addr);
//    SD_SPI_Send(0xFF);
//
//    for (int i = 0; i < 10; i++)
//    {
//        token = SD_SPI_Send(0xFF);
//        if (token != 0xFF) break;
//    }
//
//    printf("[CMD24] R1=0x%02X\r\n", token);
//    if (token != 0x00)
//    {
//        cs_disable();
//        return token;
//    }
//
//    SD_SPI_Send(0xFE);
//
//    for (int i = 0; i < 512; i++)
//        SD_SPI_Send(buf[i]);
//
//    SD_SPI_Send(0xFF);
//    SD_SPI_Send(0xFF);
//
//    do {
//        token = SD_SPI_Send(0xFF);
//    } while (token == 0xFF);
//
//    printf("[WRITE] DATA RESP=0x%02X\r\n", token);
//
//    if ((token & 0x1F) != 0x05)
//    {
//        cs_disable();
//        return 0xFF;
//    }
//
//    while (SD_SPI_Send(0xFF) == 0x00);
//
//    cs_disable();
//    SD_SPI_Send(0xFF);
//
//    printf("[WRITE] SUCCESS\r\n");
//    return 0x00;
//}
//
///* ============================================================
//   READ BLOCK — CMD17 (FIXED)
//   ============================================================ */
//uint8_t SD_ReadBlock(uint32_t block, uint8_t *buf)
//{
//    uint32_t addr = sd_is_sdhc ? block : block * 512;
//    uint8_t token;
//    uint32_t timeout;
//
//    printf("\r\n[READ] BLOCK=%lu ADDR=%lu\r\n", block, addr);
//
//    cs_enable();
//    SD_SPI_Send(0x51);
//    SD_SPI_Send(addr >> 24);
//    SD_SPI_Send(addr >> 16);
//    SD_SPI_Send(addr >> 8);
//    SD_SPI_Send(addr);
//    SD_SPI_Send(0xFF);
//
//    for (timeout = 0; timeout < 10000; timeout++)
//    {
//        token = SD_SPI_Send(0xFF);
//        if (token != 0xFF) break;
//    }
//
//    printf("[CMD17] R1=0x%02X\r\n", token);
//    if (token != 0x00)
//    {
//        cs_disable();
//        return token;
//    }
//
//    for (timeout = 0; timeout < 50000; timeout++)
//    {
//        token = SD_SPI_Send(0xFF);
//        if (token == 0xFE) break;
//    }
//
//    if (token != 0xFE)
//    {
//        cs_disable();
//        printf("[READ] DATA TOKEN TIMEOUT\r\n");
//        return 0xFF;
//    }
//
//    for (int i = 0; i < 512; i++)
//        buf[i] = SD_SPI_Send(0xFF);
//
//    SD_SPI_Send(0xFF);
//    SD_SPI_Send(0xFF);
//
//    cs_disable();
//    SD_SPI_Send(0xFF);
//
//    printf("[READ] SUCCESS\r\n");
//    return 0x00;
//}
