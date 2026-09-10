/*
 * i2c.h
 *
 *  Created on: Dec 28, 2025
 *      Author: user
 */

#ifndef __I2C_H__
#define __I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"


typedef struct {
    uint8_t reg;
    uint8_t val;
} sensor_reg;

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */



/* USER CODE BEGIN Private defines */


void i2c_send_flash(uint8_t, uint16_t);
uint8_t i2c_read_flash(uint16_t);
HAL_StatusTypeDef i2c_send_cam(uint8_t, uint8_t);
uint8_t i2c_read_cam(uint8_t);
void i2c_cam_init_test(void);
void apply_regs_i2c(const sensor_reg *regs, uint8_t);
void i2c_flash_test(void);
void i2c_camera_init_test(void);
void i2c_bus_recovery(void);


/* USER CODE END Private defines */




/* USER CODE BEGIN Prototypes */

/* =========================================================
 *  OV2640 - konfiguracja I2C pogrupowana wg funkcji
 *  Format zgodny z ArduCAM: struct sensor_reg { reg, val }
 * ========================================================= */


/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* INC_I2C_H_ */
