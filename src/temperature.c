#include "hardware/adc.h"
#include "temperature.h"

//Reads the on board temperature sensor
float read_temperature() {
    const float conv_factor = 3.3f / (1<<12);
    float adc = (float)adc_read() * conv_factor;
    float temp_c = 27.0f - (adc - 0.706f)/0.001721f;

    return temp_c;
}

//Sets up the adc to read the temperature sensor
void adc_temp_config() {
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);
}