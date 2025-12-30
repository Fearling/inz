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

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */



/* USER CODE BEGIN Private defines */
void i2c_send_flash(uint8_t, uint16_t);
uint8_t i2c_read_flash(uint16_t);
void i2c_send_cam(uint8_t, uint8_t);
uint8_t i2c_read_cam(uint8_t);
/* USER CODE END Private defines */




/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* INC_I2C_H_ */
