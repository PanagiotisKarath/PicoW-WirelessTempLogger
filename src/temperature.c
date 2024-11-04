#include "hardware/adc.h"
#include "temperature.h"

float read_temperature() {
    const float conv_factor = 3.3f / (1<<12);
    float adc = (float)adc_read() * conv_factor;
    float temp_c = 27.0f - (adc - 0.706f)/0.001721f;

    return temp_c;
}