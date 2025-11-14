#include "stm32f4xx.h"
#include <stdint.h>
#include <stdio.h>

#define GPIOBEN               (1U<<1)
#define I2C1EN                (1U<<21)
#define I2C_100KHZ            80
#define SD_MODE_MAX_RISE_TIME 17

#define CR1_PE                (1U<<0)
#define SR2_BUSY              (1U<<1)
#define CR1_START             (1U<<8)
#define CR1_STOP              (1U<<9)
#define CR1_ACK               (1U<<10)
#define SR1_SB                (1U<<0)
#define SR1_ADDR              (1U<<1)
#define SR1_BTF               (1U<<2)
#define SR1_RXNE              (1U<<6)
#define SR1_TXE               (1U<<7)

#define TIMEOUT_MAX           100000

void I2C1_Init(void)
{
    RCC->AHB1ENR |= GPIOBEN;
    RCC->APB1ENR |= I2C1EN;

    // PB8 = SCL, PB9 = SDA
    GPIOB->MODER &= ~((3U << 16) | (3U << 18));
    GPIOB->MODER |= ((2U << 16) | (2U << 18));

    GPIOB->OTYPER |= (1U << 8) | (1U << 9);     // Open drain

    GPIOB->AFR[1] &= ~((0xF << 0) | (0xF << 4));
    GPIOB->AFR[1] |= (4U << 0) | (4U << 4);

    // Reset I2C
    I2C1->CR1 |= (1U << 15);
    I2C1->CR1 &= ~(1U << 15);

    I2C1->CR2 = 16;                 // 16 MHz APB1
    I2C1->CCR = I2C_100KHZ;         // Standard mode 100kHz
    I2C1->TRISE = SD_MODE_MAX_RISE_TIME;
    I2C1->CR1 |= CR1_PE | CR1_ACK;  // Enable I2C and ACK

    printf("I2C initialized\r\n");
}


int I2C1_Start(void)
{
    int timeout = TIMEOUT_MAX;
    while ((I2C1->SR2 & SR2_BUSY) && timeout--);
    if (timeout == 0) return -1;

    I2C1->CR1 |= CR1_START;
    timeout = TIMEOUT_MAX;
    while (!(I2C1->SR1 & SR1_SB) && timeout--);
    if (timeout == 0) return -1;

    return 0;
}

void I2C1_Stop(void)
{
    I2C1->CR1 |= CR1_STOP;
}

// ----------------------------------------------------
// Write Address and Byte
// ----------------------------------------------------
int I2C1_WriteAddress(uint8_t address)
{
    int timeout = TIMEOUT_MAX;
    I2C1->DR = address;
    while (!(I2C1->SR1 & SR1_ADDR) && timeout--);
    if (timeout == 0) return -1;
    volatile uint32_t temp = I2C1->SR2; (void)temp;  // Clear ADDR
    return 0;
}

int I2C1_WriteByte(uint8_t data)
{
    int timeout = TIMEOUT_MAX;
    while (!(I2C1->SR1 & SR1_TXE) && timeout--);
    if (timeout == 0) return -1;

    I2C1->DR = data;
    timeout = TIMEOUT_MAX;
    while (!(I2C1->SR1 & SR1_BTF) && timeout--);
    if (timeout == 0) return -1;

    return 0;
}

// ----------------------------------------------------
// Read ACK / NACK
// ----------------------------------------------------
uint8_t I2C1_ReadAck(void)
{
    int timeout = TIMEOUT_MAX;
    I2C1->CR1 |= CR1_ACK;
    while (!(I2C1->SR1 & SR1_RXNE) && timeout--);
    return I2C1->DR;
}

uint8_t I2C1_ReadNack(void)
{
    int timeout = TIMEOUT_MAX;
    I2C1->CR1 &= ~CR1_ACK;
    I2C1->CR1 |= CR1_STOP;
    while (!(I2C1->SR1 & SR1_RXNE) && timeout--);
    return I2C1->DR;
}

// ----------------------------------------------------
// Multi-byte Write
// ----------------------------------------------------
int I2C1_WriteMulti(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len)
{
    if (I2C1_Start() != 0) return -1;
    if (I2C1_WriteAddress(devAddr << 1) != 0) return -1;

    if (regAddr != 0xFF)
        if (I2C1_WriteByte(regAddr) != 0) return -1;

    for (uint8_t i = 0; i < len; i++)
        if (I2C1_WriteByte(data[i]) != 0) return -1;

    I2C1_Stop();
    return 0;
}

// ----------------------------------------------------
// Multi-byte Read (Universal + Fixed Repeated Start)
// ----------------------------------------------------
int I2C1_ReadMulti(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len)
{
    int timeout;

    // Register phase
    if (regAddr != 0xFF)
    {
        if (I2C1_Start() != 0) return -1;
        if (I2C1_WriteAddress(devAddr << 1) != 0) return -1;
        if (I2C1_WriteByte(regAddr) != 0) return -1;

        // Wait for TXE and BTF before repeated start
        timeout = TIMEOUT_MAX;
        while (!(I2C1->SR1 & SR1_TXE) && timeout--);
        timeout = TIMEOUT_MAX;
        while (!(I2C1->SR1 & SR1_BTF) && timeout--);
        for (volatile int i = 0; i < 1000; i++); // small delay
    }

    // Repeated start
    if (I2C1_Start() != 0) return -1;
    if (I2C1_WriteAddress((devAddr << 1) | 1) != 0) return -1;

    // Read bytes
    for (uint8_t i = 0; i < len; i++)
    {
        if (i == (len - 1))
            data[i] = I2C1_ReadNack();
        else
            data[i] = I2C1_ReadAck();
    }

    return 0;
}
