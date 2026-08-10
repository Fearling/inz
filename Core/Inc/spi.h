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

void arducam_spi_test(void);



#endif /* INC_SPI_H_ */
