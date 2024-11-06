#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "temperature.h"

//Bus address. Default is 0x18, if a different one is needed, it's defined by
//the A0, A1 and A2 pins on the mcp9808 board.
static uint8_t ADDRESS = 0x18;

//Hardware registers. Not all will be used in this project.
static uint8_t REG_POINTER = 0x00;
static uint8_t REG_CONFIG = 0x01;
static uint8_t REG_TEMP_UPPER = 0x02;
static uint8_t REG_TEMP_LOWER = 0x03;
static uint8_t REG_TEMP_CRIT = 0x04;
static uint8_t REG_TEMP_AMB = 0x05;
static uint8_t REG_RESOLUTION = 0x08;

//Initialise i2c to read the temperature from mcp9808 sensor
void i2c_temperature_config() {
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
}
//Temperature is converted from binary form to human readable form
float mcp9808_convert_temperature(uint8_t upper_byte, uint8_t lower_byte) {
    float temperature;

    //Conversion is different depending on the temeprature sign. Temperature
    //sign is bit 4 of upper byte.
    if ((upper_byte & 0x10) == 0x10) {
        upper_byte = upper_byte & 0x0F;
        temperature = 256 - (((float)upper_byte * 16) + ((float)lower_byte / 16));
    } else {
        temperature = (((float)upper_byte * 16) + ((float)lower_byte / 16));
    }

    return temperature;
}

float read_temperature() {
    //Buffer that will hold the 2 bytes that contain the ambient temperature
    //information
    uint8_t buf[2];
    uint16_t upper_byte;
    uint16_t lower_byte;

    //Read ambient temperature register for 2 bytes
    i2c_write_blocking(i2c_default, ADDRESS, &REG_TEMP_AMB, 1, true);
    i2c_read_blocking(i2c_default, ADDRESS, buf, 2, false);
    
    upper_byte = buf[0];
    lower_byte = buf[1];
    
    float temp_celsius = mcp9808_convert_temperature(upper_byte & 0x1F, lower_byte);
    return temp_celsius;
}