
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_timer.h"

#define DEFAULT_VREF 1100 // Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES 16  // Multisampling
#define RESISTOR 22.0
#define CURRENT_CONSTANT 1
#define VOLTAGE_CONSTANT 2.8 / 180.0// Calculado empiricamente para ter maior precisao 

struct Sensor
{
    uint32_t voltage;
    uint32_t buffer;
    uint32_t raw;
    float value;
};

void init_sensors();
float get_current();
float get_voltage();