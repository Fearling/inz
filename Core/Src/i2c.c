#include "i2c.h"
#include "main.h"

extern I2C_HandleTypeDef hi2c1;

const sensor_reg OV2640_manual_exposure[] = {
    {0xFF, 0x01},
    {0x13, 0xE0},
    {0x14, 0x48},
};
const sensor_reg OV2640_enable_sde[] = {
    {0xFF, 0x00},
    {0x86, 0x1D},
};
const sensor_reg OV2640_contrast_max[] = {
    {0xFF, 0x00},
    {0x7C, 0x00},
    {0x7D, 0x04},
    {0x7C, 0x07},
    {0x7D, 0x20},
    {0x7D, 0x28},
    {0x7D, 0x0c},
    {0x7D, 0x06},
};

void i2c_send_flash(uint8_t in, uint16_t reg)
{
	HAL_I2C_Mem_Write(&hi2c1, 0xA0, reg, I2C_MEMADD_SIZE_16BIT, &in, sizeof(in), HAL_MAX_DELAY);
	HAL_Delay(5);
}

uint8_t i2c_read_flash(uint16_t reg)
{
	uint8_t result = 0;
	HAL_I2C_Mem_Read(&hi2c1, 0xA0, reg, I2C_MEMADD_SIZE_16BIT, &result, sizeof(result), HAL_MAX_DELAY);
	HAL_Delay(5);
	return result;
}

HAL_StatusTypeDef i2c_send_cam(uint8_t in, uint8_t reg)
{
	HAL_Delay(5);
	return HAL_I2C_Mem_Write(&hi2c1, 0x60, reg, I2C_MEMADD_SIZE_8BIT, &in, sizeof(in), HAL_MAX_DELAY);
}

uint8_t i2c_read_cam(uint8_t reg)
{
	HAL_Delay(5);
	uint8_t result = 0;
	HAL_I2C_Mem_Read(&hi2c1, 0x60, reg, I2C_MEMADD_SIZE_8BIT, &result, 1, HAL_MAX_DELAY);
	return result;
}

void i2c_cam_init_test(void)
{
	i2c_send_cam(0x01, 0xFF);
	uint8_t pidh = i2c_read_cam(0x0A);
	uint8_t pidl = i2c_read_cam(0x0B);
	printf("PIDH=0x%02X PIDL=0x%02X (oczekiwane 0x26)\r\n", pidh, pidl);
}

void apply_regs_i2c(const sensor_reg *regs, uint8_t count) {
    for (int i = 0; i < count; i++) {
    	HAL_StatusTypeDef status = i2c_send_cam(regs[i].val, regs[i].reg);
    	if (status == HAL_OK) {
    		printf("OK reg=0x%02X val=0x%02X\r\n", regs[i].reg, regs[i].val);
    	} else {
    		printf("BLAD reg=0x%02X val=0x%02X status=%d\r\n", regs[i].reg, regs[i].val, status);
    	}
    }
}

void i2c_camera_init_test()
{
	i2c_cam_init_test();
	apply_regs_i2c(OV2640_manual_exposure, sizeof(OV2640_manual_exposure)/sizeof(sensor_reg));
	apply_regs_i2c(OV2640_enable_sde, sizeof(OV2640_enable_sde)/sizeof(sensor_reg));
	apply_regs_i2c(OV2640_contrast_max, sizeof(OV2640_contrast_max)/sizeof(sensor_reg));
}
