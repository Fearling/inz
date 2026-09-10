/*
 * spi.h
 *
 *  Created on: Jun 14, 2026
 *      Author: user
 */

#ifndef INC_SPI_H_
#define INC_SPI_H_
#include "main.h"
#include "i2c.h"
#include "uart.h"

typedef struct {
    uint8_t reg;
    uint8_t val;
} sensor_reg_spi;

/* ---- GRUPA 1: test komunikacji SPI ---- */



void spi_write_reg(uint8_t addr, uint8_t val);
uint8_t spi_read_reg(uint8_t addr);
void apply_regs_spi(const sensor_reg_spi *regs, uint8_t count);

uint8_t arducam_spi_test(void);
uint8_t arducam_capture_photo(UART_HandleTypeDef *huart2);



#endif /* INC_SPI_H_ */
