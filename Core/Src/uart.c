/*
 * uart.c
 *
 *  Created on: Aug 10, 2026
 *      Author: user
 */
#include "uart.h"

int _write(int file, char *ptr, int len)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)ptr, len, HAL_MAX_DELAY);
    return len;
}
