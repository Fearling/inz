/*
 * capture_uart.c
 *
 * Odbiera strumień JPEG z STM32 przez UART, wyłuskuje obraz
 * między znacznikami 0xFFD8 (SOI) i 0xFFD9 (EOI), zapisuje do pliku.
 *
 * Kompilacja: gcc -o capture_uart capture_uart.c
 * Użycie:     ./capture_uart /dev/ttyACM1 zdjecie.jpg
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>

#define MAX_IMAGE_SIZE (500 * 1024)   /* 500KB - z zapasem, dostosuj w razie potrzeby */

static int open_serial(const char *path, int baud)
{
    int fd = open(path, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        close(fd);
        return -1;
    }

    cfsetospeed(&tty, baud);
    cfsetispeed(&tty, baud);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_iflag = 0;                 /* PEŁNE wyzerowanie - eliminuje ISTRIP i inne */
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_cc[VMIN]  = 1;
    tty.c_cc[VTIME] = 5;

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        close(fd);
        return -1;
    }

    return fd;
}


int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Uzycie: %s <port_szeregowy> <plik_wyjsciowy.jpg>\n", argv[0]);
        fprintf(stderr, "Przyklad: %s /dev/ttyACM1 zdjecie.jpg\n", argv[0]);
        return 1;
    }

    const char *port_path = argv[1];
    const char *out_path = argv[2];

    int fd = open_serial(port_path, B115200);
    if (fd < 0) {
        return 1;
    }

    unsigned char *image = malloc(MAX_IMAGE_SIZE);
    if (!image) {
        fprintf(stderr, "Brak pamieci na bufor obrazu\n");
        close(fd);
        return 1;
    }

    unsigned char byte;
    unsigned char prev = 0;
    long image_len = 0;
    int found_start = 0;

    printf("Czekam na SOI (0xFFD8)...\n");

    /* Krok 1: szukaj poczatku obrazu (0xFF 0xD8) */
    while (!found_start) {
        ssize_t n = read(fd, &byte, 1);
        if (n <= 0) {
            continue;   /* timeout - sprobuj ponownie */
        }
        if (prev == 0xFF && byte == 0xD8) {
            found_start = 1;
            image[0] = 0xFF;
            image[1] = 0xD8;
            image_len = 2;
        }
        prev = byte;
    }

    printf("Znaleziono SOI, odbieram dane...\n");

    /* Krok 2: zbieraj dane az do konca obrazu (0xFF 0xD9) */
    prev = 0;
    while (image_len < MAX_IMAGE_SIZE) {
        ssize_t n = read(fd, &byte, 1);
        if (n <= 0) {
            continue;
        }

        image[image_len++] = byte;

        if (prev == 0xFF && byte == 0xD9) {
            printf("Znaleziono EOI, koniec obrazu\n");
            break;
        }
        prev = byte;
    }

    close(fd);

    if (image_len >= MAX_IMAGE_SIZE) {
        fprintf(stderr, "UWAGA: osiagnieto MAX_IMAGE_SIZE bez znalezienia EOI - plik moze byc niekompletny\n");
    }

    FILE *f = fopen(out_path, "wb");
    if (!f) {
        perror("fopen");
        free(image);
        return 1;
    }

    fwrite(image, 1, image_len, f);
    fclose(f);
    free(image);

    printf("Zapisano %ld bajtow do %s\n", image_len, out_path);
    return 0;
}
