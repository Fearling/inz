/*
 * i2c.c
 *
 *  Created on: Dec 28, 2025
 *      Author: user
 */
#include "i2c.h"
#include "main.h"

extern I2C_HandleTypeDef hi2c1;

const sensor_reg OV2640_check_id[] = {
    {0xFF, 0x01},   // przełącz na bank SENSOR
    {0x0A, 0x00},   // odczyt PIDH (nie zapis - tu tylko adres do rdSensorReg)
    {0x0B, 0x00},   // odczyt PIDL (jw.)
};
const sensor_reg OV2640_manual_exposure[] = {
    {0xFF, 0x01},   // bank SENSOR
    {0x13, 0xE0},   // COM8: AGC=0, AEC=0, AWB pozostaw wg potrzeb
    {0x14, 0x48},   // COM9: pułap AGC (wartość domyślna z inicjalizacji QVGA)
};
const sensor_reg OV2640_enable_sde[] = {
    {0xFF, 0x00},   // przełącz na bank DSP
    {0x86, 0x1D},   // CTRL2: ustaw bit SDE_EN (0x10) + inne moduły domyślnie aktywne
};
const sensor_reg OV2640_contrast_template[] = {
    {0xFF, 0x00},   // bank DSP
    {0x7C, 0x00},
    {0x7D, 0x04},
    {0x7C, 0x07},
    {0x7D, 0x00},   // <- brightness_val (do ustalenia)
    {0x7D, 0x00},   // <- contrast_val1  (do ustalenia)
    {0x7D, 0x00},   // <- contrast_val2  (do ustalenia)
    {0x7D, 0x06},
};

void i2c_camera_init_test()
{
	apply_regs(OV2640_check_id, 3);
	apply_regs(OV2640_manual_exposure, 3);
	apply_regs(OV2640_enable_sde, 2);
	apply_regs(OV2640_contrast_template, 8);
}
void i2c_send_flash(uint8_t in, uint16_t reg)
{
	HAL_I2C_Mem_Write(&hi2c1, 0xA0, reg, 2, &in, sizeof(in), HAL_MAX_DELAY);

	HAL_Delay(5);
}

uint8_t i2c_read_flash(uint16_t reg)
{

	uint8_t result = 0;
	HAL_I2C_Mem_Read(&hi2c1, 0xA0, reg, 2, &result, sizeof(result), HAL_MAX_DELAY);

	HAL_Delay(5);

	return result;
}

HAL_StatusTypeDef i2c_send_cam(uint8_t in, uint8_t reg)
{
	HAL_Delay(5);
	HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&hi2c1, 0x60, reg, 1, &in, sizeof(in), HAL_MAX_DELAY);
	return status;
}

uint8_t i2c_read_cam(uint8_t reg)
{
	HAL_Delay(5);
    uint8_t result = 0;
    return HAL_I2C_Mem_Read(&hi2c1, 0x60, reg, 1, &result, 1, HAL_MAX_DELAY);;
}

void i2c_cam_init_test(void)
{
	// sprawdzenie ID sensora - musi byc pierwsze, przed konfiguracja
	i2c_send_cam(0x01, 0xFF);            // bank select -> sensor
	uint8_t pidh = i2c_read_cam(0x0A);
	uint8_t pidl = i2c_read_cam(0x0B);
	printf("PIDH=0x%02X PIDL=0x%02X (oczekiwane 0x26)\r\n", pidh, pidl);
}

void apply_regs(const sensor_reg *regs, int count) {
    for (int i = 0; i < count; i++) {
    	HAL_StatusTypeDef status = i2c_send_cam(regs[i].val, regs[i].reg);   // uwaga na kolejność argumentów w Twojej funkcji!
    	if(status == HAL_OK)
    	{
    		printf("OK reg=0x%02X val=0x%02X\r\n", regs[i].reg, regs[i].val);
    	}
    }
}

