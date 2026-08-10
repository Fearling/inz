/*
 * spi.c
 *
 *  Created on: Jun 14, 2026
 *      Author: user
 */
#include "spi.h"

extern SPI_HandleTypeDef hspi1;

void spi_write_reg(uint8_t addr, uint8_t val)
{
    uint8_t tx[2] = {addr | 0x80, val};
    uint8_t rx[2];
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi1, tx, rx, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
}

uint8_t spi_read_reg(uint8_t addr)
{
    uint8_t tx[2] = {addr & 0x7F, 0x00};
    uint8_t rx[2] = {0x00, 0x00};
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi1, tx, rx, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
    return rx[1];
}

void arducam_spi_test(void)
{
    uint8_t before = spi_read_reg(0x02 & 0x7F);  // odczytaj przed zapisem
    HAL_Delay(1);

    spi_write_reg(0x02 | 0x80, 0x56);             // zapisz 0x55
    HAL_Delay(10);

    uint8_t after = spi_read_reg(0x02 & 0x7F);   // odczytaj po zapisie
    HAL_Delay(10);

    // before = wartość domyślna rejestru 0x02
    // after  = powinno być 0x55 jeśli zapis działa
    // ustaw breakpoint tutaj
}
