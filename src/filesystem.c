#include <stdio.h>
#include <stdbool.h>
#include "ff.h"
#include "filesystem.h"

//Opens the file, writes the temperature inside and closes it.
//FA_OPEN_APPEND makes sure that it writes at the end of the file, and does not
//overwrite the previous writes. If the files does not exist it creates it.
void file_write_temperature(float temperature, const TCHAR* filename) {
    FIL fil;
    FRESULT fr;

    fr = f_open(&fil, filename, FA_WRITE | FA_OPEN_APPEND);
    if(fr != FR_OK) {
        printf("ERROR: Could not open file (%d)\r\n", fr);
        while(true);
    }

    int ret = f_printf(&fil, "T = %fC\r\n", temperature);
    if(ret < 0) {
        printf("ERROR: Could not write to file (%d)", ret);
        f_close(&fil);
        while(true);
    }

    fr = f_close(&fil);
    if(fr != FR_OK) {
        printf("ERROR: Could not close file (%d)\r\n", fr);
        while(true);
    }
}
