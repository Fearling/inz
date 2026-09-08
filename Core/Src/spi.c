/*
 * spi.c
 *
 *  Created on: Jun 14, 2026
 *      Author: user
 */
#include "spi.h"
#include <stdio.h>

extern SPI_HandleTypeDef hspi1;

/* ---- GRUPA 3: przygotowanie FIFO przed zdjeciem ---- */
const sensor_reg SPI_fifo_prepare[] = {
    {0x04, 0x01},   // FIFO_CLEAR_MASK
    {0x04, 0x10},   // FIFO_RDPTR_RST_MASK
    {0x04, 0x20},   // FIFO_WRPTR_RST_MASK
};

/* ---- GRUPA 4: start przechwytywania ---- */
const sensor_reg SPI_start_capture[] = {
    {0x04, 0x02},   // FIFO_START_MASK
};

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
    uint8_t tx[2] = {(uint8_t)(addr & 0x7F), 0x00};
    uint8_t rx[2] = {0x00, 0x00};
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi1, tx, rx, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
    return rx[1];
}

void apply_regs_spi(const sensor_reg_spi *regs, uint8_t count)
{
    for (int i = 0; i < count; i++) {
        spi_write_reg(regs[i].reg, regs[i].val);
        printf("SPI OK reg=0x%02X val=0x%02X\r\n", regs[i].reg, regs[i].val);
    }
}

void arducam_spi_test(void)
{
    uint8_t before = spi_read_reg(0x02);
    HAL_Delay(1);

    spi_write_reg(0x02, 0x55);
    HAL_Delay(10);

    uint8_t after = spi_read_reg(0x02);
    HAL_Delay(10);

    printf("SPI TEST: before=0x%02X after=0x%02X (oczekiwane after=0x55)\r\n", before, after);

    if (after == 0x55) {
        printf("SPI TEST: OK - komunikacja SPI dziala\r\n");
    } else {
        printf("SPI TEST: BLAD - sprawdz polaczenie/CS/tryb SPI\r\n");
    }
}

/*
 * Pelna sekwencja: wyczysc FIFO -> start capture -> czekaj na CAP_DONE
 * -> odczytaj rozmiar -> burst read do bufora.
 * Zwraca 1 przy sukcesie, 0 przy bledzie (np. za maly bufor, timeout).
 */
uint8_t arducam_capture_photo(UART_HandleTypeDef *huart)
{
    apply_regs_spi(SPI_fifo_prepare, sizeof(SPI_fifo_prepare) / sizeof(sensor_reg));
    apply_regs_spi(SPI_start_capture, sizeof(SPI_start_capture) / sizeof(sensor_reg));

    uint32_t timeout_ms = 3000;
    uint32_t waited = 0;
    uint8_t trig = 0;

    do {
        trig = spi_read_reg(0x41);
        if (trig & 0x08) break;
        HAL_Delay(5);
        waited += 5;
    } while (waited < timeout_ms);

    if (!(trig & 0x08)) {
        printf("CAPTURE: TIMEOUT\r\n");
        return 0;
    }

    uint32_t len = spi_read_reg(0x42)
                 | ((uint32_t)spi_read_reg(0x43) << 8)
                 | ((uint32_t)spi_read_reg(0x44) << 16);

    printf("CAPTURE: rozmiar obrazu = %lu bajtow, wysylam...\r\n", len);

    /* --- STRUMIENIOWANIE: SPI -> UART, bez pelnego bufora w RAM --- */
    uint8_t chunk[512];
    uint32_t remaining = len;
    uint8_t cmd = 0x3C;   /* BURST_FIFO_READ */

    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);

    while (remaining > 0) {
        uint32_t to_read = (remaining > sizeof(chunk)) ? sizeof(chunk) : remaining;
        HAL_SPI_Receive(&hspi1, chunk, to_read, HAL_MAX_DELAY);
        HAL_UART_Transmit(huart, chunk, to_read, HAL_MAX_DELAY);
        remaining -= to_read;
        /* UWAGA: ZERO printf() w tej pętli - patrz wyjaśnienie niżej */
    }

    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);

    printf("CAPTURE: wyslano %lu bajtow\r\n", len);
    return 1;
}
