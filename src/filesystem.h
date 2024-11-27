#ifndef FILESYSTEM_H
#define FILESYSTEM_H

/*
 * Is given the temperature from the sensor and writes it in a text file.
 *
 * @param temperature Value given by sensor.
 * @param filename Declared in the main file, name of txt file to be written.
*/
void file_write_temperature(float temperature, const TCHAR* filename);

#endif