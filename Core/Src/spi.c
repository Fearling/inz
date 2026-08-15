/*
 * spi.c
 *
 *  Created on: Jun 14, 2026
 *      Author: user
 */
#include "spi.h"

const sensor_reg SPI_test_comm[] = {
    {0x00, 0x55},   // ARDUCHIP_TEST1 - zapisz dowolną wartość testową
                     // odczytaj z powrotem 0x00 - jeśli 0x55, SPI działa
};

/* ---- GRUPA 2: reset/zasilanie sensora (opcjonalnie, przed capture) ---- */
const sensor_reg SPI_gpio_sensor[] = {
    {0x06, 0x00},   // ARDUCHIP_GPIO - GPIO_RESET_MASK=0 (reset),
                     // potem ustaw 0x06,0x01 by wybudzić sensor
};

/* ---- GRUPA 3: przygotowanie FIFO przed zdjęciem ---- */
const sensor_reg SPI_fifo_prepare[] = {
    {0x04, 0x01},   // ARDUCHIP_FIFO - FIFO_CLEAR_MASK: wyczyść flagę FIFO
    {0x04, 0x10},   // ARDUCHIP_FIFO - FIFO_RDPTR_RST_MASK: reset wskaźnika odczytu
    {0x04, 0x20},   // ARDUCHIP_FIFO - FIFO_WRPTR_RST_MASK: reset wskaźnika zapisu
};

/* ---- GRUPA 4: start przechwytywania ---- */
const sensor_reg SPI_start_capture[] = {
    {0x04, 0x02},   // ARDUCHIP_FIFO - FIFO_START_MASK: start capture
};

/* ---- GRUPA 5: sprawdzanie statusu (odczyt, nie zapis) ---- */
const sensor_reg SPI_check_status[] = {
    {0x41, 0x00},   // ARDUCHIP_TRIG - odczytaj, sprawdź bit CAP_DONE_MASK (0x08)
};

/* ---- GRUPA 6: odczyt rozmiaru gotowych danych (odczyt, nie zapis) ---- */
const sensor_reg SPI_read_size[] = {
    {0x42, 0x00},   // FIFO_SIZE1 - bity [7:0]
    {0x43, 0x00},   // FIFO_SIZE2 - bity [15:8]
    {0x44, 0x00},   // FIFO_SIZE3 - bity [18:16]
};

/* ---- GRUPA 7: odczyt danych obrazu z FIFO ---- */
const sensor_reg SPI_read_fifo[] = {
    {0x3C, 0x00},   // BURST_FIFO_READ - ciągły odczyt strumienia obrazu
    // albo pojedynczo:
    // {0x3D, 0x00},   // SINGLE_FIFO_READ - odczyt bajt po bajcie
};

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
