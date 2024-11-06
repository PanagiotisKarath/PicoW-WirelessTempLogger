#ifndef TEMPERATURE_H
#define TEMPERATURE_H

void i2c_temperature_config();
float mcp9808_convert_temperature(uint8_t upper_byte, uint8_t lower_byte);
float read_temperature();

#endif