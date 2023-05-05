/* ADC1 Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sensors_readings.h"
#include "driver/dac.h"

static esp_adc_cal_characteristics_t *adc_chars;
static const adc_channel_t channel_current = ADC_CHANNEL_4; // Pino 32   CORRENTE
static const adc_channel_t channel_voltage = ADC_CHANNEL_7; // Pino 35   TENSAO
static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_1;

void app_main()
{
    /*
        // Configure DAC
        dac_output_enable(DAC_CHANNEL_1); // Enable DAC channel 1

        // Calculate DAC value for 1.65V output
        uint8_t dac_val = (uint8_t)(1.65 / 3.3 * 255); // Calculate DAC value for 1.65V output
                                                       // Set DAC output to 1.65V
        dac_output_voltage(DAC_CHANNEL_1, dac_val);    // Set DAC output to the calculated value
    */
    // Configure ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
    printf("Characterized using %s\n", val_type == ESP_ADC_CAL_VAL_EFUSE_TP ? "eFuse Two Point" : (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF ? "eFuse Vref" : "Default Vref"));
    adc1_config_channel_atten(channel_voltage, atten);
    adc1_config_channel_atten(channel_current, atten);

    while (1)
    {
        float current = get_current();
        printf("CORRENTE: %.2f A\n", current);
        float voltage = get_voltage();
        printf("TENSAO: %.2f V\n", voltage);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

float get_current()
{
    uint32_t adc_raw = 0;
    uint32_t higher_value = 0;
    uint32_t clean_higher_value = 0;

    for (int z = 0; z < 50; z++)
    {
        for (int i = 0; i < NO_OF_SAMPLES; i++)
        {
            adc_raw = adc1_get_raw((adc1_channel_t)channel_current);
            if (adc_raw > higher_value)
                higher_value = adc_raw;
        }
        clean_higher_value += higher_value;
    }
    clean_higher_value /= 50;

    float voltage = esp_adc_cal_raw_to_voltage(clean_higher_value, adc_chars);
    float current = (-5.00 + (voltage * 0.003));

    if (current > 0.3) // Abaixo disso ele esta lendo é ruido
        return (-5.00 + (voltage * 0.003));
    else
        return 0;
}
// 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
float get_voltage()
{
    uint32_t adc_raw = 0;
    uint32_t higher_value = 0;
    uint32_t clean_higher_value = 0;

    for (int z = 0; z < 50; z++)
    {
        for (int i = 0; i < NO_OF_SAMPLES; i++)
        {
            adc_raw = adc1_get_raw((adc1_channel_t)channel_voltage);
            if (adc_raw > higher_value)
                higher_value = adc_raw;
        }
        clean_higher_value += higher_value;
    }
    clean_higher_value /= 50;

    float voltage = esp_adc_cal_raw_to_voltage(clean_higher_value, adc_chars);
    float aux = (voltage * 0.208) - 349.392;
    if (aux > 5)// Apenas por seguraça  para nao retornar valores errados       
        return (voltage * 0.208) - 349.392;
    else
        return 0;
}