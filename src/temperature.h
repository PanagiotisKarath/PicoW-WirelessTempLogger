#ifndef TEMPERATURE_H
#define TEMPERATURE_H

/*
 * Configures the i2c to read temperature from the mcp9808 sensor
*/
void i2c_temperature_config();

/*
 * Converts the raw temperature data given by the sensor into human readable 
 * form. Raw temperature data is split into 2 bytes. 
 * 
 * @param upper_byte First byte of temperature data.
 * @param lower_byte Second byte of temperature data.
*/
float mcp9808_convert_temperature(uint8_t upper_byte, uint8_t lower_byte);

/*
 * Returns the temperature in human readable form.
*/
float read_temperature();

#endif