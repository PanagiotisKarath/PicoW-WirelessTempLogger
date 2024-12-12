#include <stdio.h>
#include <stdbool.h>
#include "filesystem/vfs.h"
#include <errno.h>
#include <string.h>

void sd_file_write_temperature(float temperature) {
    printf("WRITING TO SD CARD"); //DEBUG
    FILE *fp = fopen("/sd/s_temp.txt", "a");
    if (fp == NULL) {
        printf("fopen error: %s", strerror(errno));
    }
    fprintf(fp, "%f\n", temperature);
    int err = fclose(fp);
    if (err == -1) { 
        printf("fclose error: %s", strerror(errno));
    }
}

void flash_file_write_temperature(float temperature) {
    printf("WRITING TO FLASH"); //DEBUG
    FILE *fp = fopen("/ap_temp.txt", "w");
    if (fp == NULL) {
        printf("fopen error: %s", strerror(errno));
    }
    fprintf(fp, "%f\n", temperature);
    int err = fclose(fp);
    if (err == -1) { 
        printf("fclose error: %s", strerror(errno));
    }
}

