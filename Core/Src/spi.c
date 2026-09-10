/*
 * arducam_capture_final.c
 *
 * Kompletna, poprawiona sekwencja przechwytywania zdjecia JPEG z modulu
 * ArduCAM Mini (OV2640) przez SPI, ze strumieniowaniem na UART.
 *
 * Zawiera wszystkie poprawki ustalone w toku debugowania:
 *  - ARDUCHIP_MODE (0x02) ustawiony na CAM2LCD_MODE przed capture
 *  - Kasowanie flagi CAP_DONE (0x41, bit0=1) PRZED i PO capture
 *  - HAL_SPI_TransmitReceive (nie sam Receive) dla burst-read - wymagane
 *    do poprawnego generowania zegara SCK w trybie master full-duplex
 *  - Rejestr testowy ARDUCHIP_TEST1 (0x00), nie ARDUCHIP_MODE (0x02)
 *
 * Zrodla: oficjalna dokumentacja ArduCAM (ArduCAM Mini 2MP Application Notes),
 * potwierdzone analizatorem logicznym w trakcie debugowania.
 */

#include "spi.h"
#include <stdio.h>

extern SPI_HandleTypeDef hspi1;

/* ---- Rejestry ArduChip uzywane w tej sekwencji ---- */
#define ARDUCHIP_TEST1      0x00
#define ARDUCHIP_MODE       0x02
#define CAM2LCD_MODE        0x01
#define ARDUCHIP_FIFO       0x04
#define FIFO_CLEAR_MASK     0x01
#define FIFO_RDPTR_RST_MASK 0x10
#define FIFO_WRPTR_RST_MASK 0x20
#define FIFO_START_MASK     0x02
#define ARDUCHIP_TRIG       0x41
#define CAP_DONE_MASK       0x08
#define FIFO_SIZE1          0x42
#define FIFO_SIZE2          0x43
#define FIFO_SIZE3          0x44
#define BURST_FIFO_READ     0x3C

/* ---- Niskopoziomowe funkcje SPI ---- */
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
    uint8_t tx[2] = {(uint8_t)(addr & 0x7F), 0x00};
    uint8_t rx[2] = {0x00, 0x00};
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi1, tx, rx, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
    return rx[1];
}

/* ---- Test komunikacji SPI - uzywa dedykowanego rejestru testowego ---- */
uint8_t arducam_spi_test(void)
{
    uint8_t before = spi_read_reg(ARDUCHIP_TEST1);
    spi_write_reg(ARDUCHIP_TEST1, 0x55);
    HAL_Delay(2);
    uint8_t after = spi_read_reg(ARDUCHIP_TEST1);

    printf("SPI TEST: before=0x%02X after=0x%02X (oczekiwane after=0x55)\r\n", before, after);

    if (after == 0x55) {
        printf("SPI TEST: OK\r\n");
        return 1;
    }
    printf("SPI TEST: BLAD\r\n");
    return 0;
}

/* ---- Sprawdzenie wersji ArduChip - powinno zwrocic 0x40 dla modulu 2MP ---- */
uint8_t arducam_check_version(void)
{
    uint8_t rev = spi_read_reg(0x40);
    printf("ArduChip REV = 0x%02X (oczekiwane 0x40 dla modulu 2MP)\r\n", rev);
    return rev;
}

/*
 * Pelna, poprawiona sekwencja przechwytywania zdjecia JPEG.
 * Strumieniuje dane bezposrednio na UART, bez buforowania calosci w RAM.
 * Zwraca 1 przy sukcesie, 0 przy bledzie.
 */
uint8_t arducam_capture_photo(UART_HandleTypeDef *huart)
{
    /* 1. Ustaw poprawny tryb pracy chipu (wymagane przed kazdym capture) */
    spi_write_reg(ARDUCHIP_MODE, CAM2LCD_MODE);

    /* 2. Skasuj ewentualna nieaktualna flage CAP_DONE z poprzedniej proby */
    spi_write_reg(ARDUCHIP_TRIG, 0x01);

    /* 3. Wyczysc i zresetuj wskazniki FIFO */
    spi_write_reg(ARDUCHIP_FIFO, FIFO_CLEAR_MASK);
    spi_write_reg(ARDUCHIP_FIFO, FIFO_RDPTR_RST_MASK);
    spi_write_reg(ARDUCHIP_FIFO, FIFO_WRPTR_RST_MASK);

    /* 4. Start capture */
    spi_write_reg(ARDUCHIP_FIFO, FIFO_START_MASK);

    /* 5. Czekaj na CAP_DONE_MASK */
    uint32_t timeout_ms = 3000;
    uint32_t waited = 0;
    uint8_t trig = 0;

    do {
        trig = spi_read_reg(ARDUCHIP_TRIG);
        if (trig & CAP_DONE_MASK) break;
        HAL_Delay(5);
        waited += 5;
    } while (waited < timeout_ms);

    if (!(trig & CAP_DONE_MASK)) {
        printf("CAPTURE: TIMEOUT\r\n");
        return 0;
    }

    /* 6. Odczytaj rozmiar danych */
    uint32_t len = spi_read_reg(FIFO_SIZE1)
                 | ((uint32_t)spi_read_reg(FIFO_SIZE2) << 8)
                 | ((uint32_t)spi_read_reg(FIFO_SIZE3) << 16);

    printf("CAPTURE: rozmiar obrazu = %lu bajtow, wysylam...\r\n", len);

    if (len == 0) {
        printf("CAPTURE: BLAD - rozmiar zerowy\r\n");
        return 0;
    }

    /* 7. Burst read - strumieniowanie SPI -> UART, bez pelnego bufora w RAM.
     *    KLUCZOWE: HAL_SPI_TransmitReceive, nie sam HAL_SPI_Receive -
     *    wymagane do poprawnego generowania zegara SCK. */
    uint8_t chunk[512];
    uint8_t dummy[512] = {0};
    uint32_t remaining = len;
    uint8_t cmd = BURST_FIFO_READ;

    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);

    while (remaining > 0) {
        uint32_t to_read = (remaining > sizeof(chunk)) ? sizeof(chunk) : remaining;
        HAL_SPI_TransmitReceive(&hspi1, dummy, chunk, to_read, HAL_MAX_DELAY);
        HAL_UART_Transmit(huart, chunk, to_read, HAL_MAX_DELAY);
        remaining -= to_read;
    }

    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);

    /* 8. Skasuj CAP_DONE po zakonczeniu - przygotowanie na nastepne capture */
    spi_write_reg(ARDUCHIP_TRIG, 0x01);

    printf("CAPTURE: wyslano %lu bajtow\r\n", len);
    return 1;
}
