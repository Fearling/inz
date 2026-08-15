/*
 * spi.h
 *
 *  Created on: Jun 14, 2026
 *      Author: user
 */

#ifndef INC_SPI_H_
#define INC_SPI_H_
#include "main.h"

typedef struct {
    uint8_t reg;
    uint8_t val;
} sensor_reg;

/* ---- GRUPA 1: test komunikacji SPI ---- */


void arducam_spi_test(void);



#endif /* INC_SPI_H_ */
