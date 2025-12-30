/*
 * i2c.c
 *
 *  Created on: Dec 28, 2025
 *      Author: user
 */
#include "i2c.h"
#include "main.h"

extern I2C_HandleTypeDef hi2c1;


void i2c_send_flash(uint8_t in, uint16_t reg)
{
	  HAL_I2C_Mem_Write(&hi2c1, 0xA0, reg, 2, &in, sizeof(in), HAL_MAX_DELAY);

	  HAL_Delay(5);

	  return 0;
}

uint8_t i2c_read_flash(uint16_t reg)
{

	  uint8_t result = 0;
	  HAL_I2C_Mem_Read(&hi2c1, 0xA0, reg, 2, &result, sizeof(result), HAL_MAX_DELAY);

	  HAL_Delay(5);

	  return result;
}

void i2c_send_cam(uint8_t in, uint8_t reg)
{
	  HAL_I2C_Mem_Write(&hi2c1, 0x60, reg, 1, &in, sizeof(in), HAL_MAX_DELAY);

	  HAL_Delay(5);

	  return 0;
}

uint8_t i2c_read_cam(uint8_t reg)
{

	  uint8_t result = 0;
	  HAL_I2C_Mem_Read(&hi2c1, 0x30, reg, 1, &result, sizeof(result), HAL_MAX_DELAY);

	  HAL_Delay(5);

	  return result;
}

