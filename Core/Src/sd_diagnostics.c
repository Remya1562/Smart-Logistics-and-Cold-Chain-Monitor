#include "sd_diagnostics.h"
#include "stm32f4xx.h"
#include <stdio.h>

/* You MUST provide these from your existing code */
extern uint8_t SD_SPI_Send(uint8_t data);
extern void cs_enable(void);
extern void cs_disable(void);
extern void sd_delay_ms(uint32_t ms);

/* -----------------------------------------
   TEST 1: MISO idle window (200 samples)
------------------------------------------*/
static void test_miso_idle_window(void)
{
    printf("\r\n[MISO IDLE WINDOW TEST]\r\n");
    cs_disable();

    int high = 0, low = 0;

    for(int i=0; i<200; i++)
    {
        uint8_t v = SD_SPI_Send(0xFF);
        if (v == 0xFF) high++;
        if (v == 0x00) low++;
    }

    printf("Samples: HIGH=%d, LOW=%d\r\n", high, low);

    if (high == 0)
        printf("RESULT: MISO stuck LOW (module or card clamps DAT0)\r\n");
    else if (low == 0)
        printf("RESULT: MISO pulled HIGH (rare)\r\n");
    else
        printf("RESULT: MISO floating/weak — missing pull-up likely\r\n");
}

/* -----------------------------------------
   TEST 2: Force-release test
   Check whether card ever releases MISO
------------------------------------------*/
static void test_force_release(void)
{
    printf("\r\n[FORCE RELEASE TEST]\r\n");

    cs_enable();
    SD_SPI_Send(0xFF);
    sd_delay_ms(5);

    cs_disable();
    SD_SPI_Send(0xFF);
    sd_delay_ms(5);

    uint8_t v1 = SD_SPI_Send(0xFF);
    uint8_t v2 = SD_SPI_Send(0xFF);
    uint8_t v3 = SD_SPI_Send(0xFF);

    printf("Values after release: %02X %02X %02X\r\n", v1, v2, v3);

    if (v1 == 0x00 && v2 == 0x00)
        printf("RESULT: DAT0 did NOT release → hardware holds it LOW\r\n");
}

/* -----------------------------------------
   TEST 3: CMD0 edge-case
------------------------------------------*/
static void test_cmd0_edge_case(void)
{
    printf("\r\n[CMD0 EDGE TEST]\r\n");

    cs_disable();
    for(int i=0;i<6;i++) SD_SPI_Send(0xFF);

    cs_enable();

    SD_SPI_Send(0x40);
    SD_SPI_Send(0x00);
    SD_SPI_Send(0x00);
    SD_SPI_Send(0x00);
    SD_SPI_Send(0x00);
    SD_SPI_Send(0x95);

    uint8_t r = 0xFF;

    for (int i=0; i<20; i++)
    {
        r = SD_SPI_Send(0xFF);
        if (r != 0xFF) break;
    }

    cs_disable();
    SD_SPI_Send(0xFF);

    printf("CMD0(R1) = %02X\r\n", r);

    if (r == 0x01)
        printf("RESULT: Card responds normally → SPI logic OK\r\n");
    else
        printf("RESULT: Card fails edge-case → CS/MISO issue\r\n");
}

/* -----------------------------------------
   TEST 4: Raw clock toggling
------------------------------------------*/
static void test_raw_clocking(void)
{
    printf("\r\n[RAW CLOCK TEST]\r\n");

    int count0 = 0, countFF = 0;

    for(int i=0;i<200;i++)
    {
        uint8_t b = SD_SPI_Send(0xFF);
        if      (b == 0x00) count0++;
        else if (b == 0xFF) countFF++;
    }

    printf("Clock-only: 0x00=%d, 0xFF=%d\r\n", count0, countFF);

    if (count0 == 200)
        printf("RESULT: Module forces MISO LOW (level shifter fault)\r\n");
}

/* -----------------------------------------
   TEST 5: Busy-state test
------------------------------------------*/
static void test_busy_behavior(void)
{
    printf("\r\n[BUSY BEHAVIOR TEST]\r\n");

    /* Send CMD0 to enter idle; R1 is ignored here */
    cs_enable();
    SD_SPI_Send(0x40);
    SD_SPI_Send(0x00);
    SD_SPI_Send(0x00);
    SD_SPI_Send(0x00);
    SD_SPI_Send(0x00);
    SD_SPI_Send(0x95);

    for(int i=0;i<20;i++)
    {
        uint8_t v = SD_SPI_Send(0xFF);
        printf("%02X ", v);
    }
    printf("\r\n");

    printf("If all bytes are 00 → Card never releases DAT0 → hardware fault\r\n");

    cs_disable();
}

/* -----------------------------------------
   Run all diagnostics
------------------------------------------*/
void run_sd_diagnostics(void)
{
    printf("\r\n===================================\r\n");
    printf("     SD DIAGNOSTIC SUITE START     \r\n");
    printf("===================================\r\n");

    test_miso_idle_window();
    test_force_release();
    test_cmd0_edge_case();
    test_raw_clocking();
    test_busy_behavior();

    printf("\r\n=== END OF DIAGNOSTICS ===\r\n");
}
