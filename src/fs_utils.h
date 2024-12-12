#ifndef FSUTILS_H
#define FSUTILS_H

/*
 * Is given the temperature from the temperature sensor and writes it in a 
 * text file in the SD card.
 * 
 * @param temperature Value given by sensor
*/
void sd_file_write_temperature(float temperature);

/*
 * Is given the temperature from the temperature sensor and writes it in a 
 * text file in the on-board flash memory.
 * 
 * @param temperature Value given by sensor
*/
void flash_file_write_temperature(float temperature);
#endif