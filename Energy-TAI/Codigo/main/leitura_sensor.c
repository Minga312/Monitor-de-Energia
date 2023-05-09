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
#include "freertos/event_groups.h"
#include "esp_expression_with_stack.h"
#include "leitura_sensor.h"
#include "driver/dac.h"
#include "memoria_e_bluetooth.h"

static esp_adc_cal_characteristics_t *adc_chars;
static const adc_channel_t channel_current = ADC_CHANNEL_4; // Pino 32   CORRENTE
static const adc_channel_t channel_voltage = ADC_CHANNEL_7; // Pino 35   TENSAO
static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_1;

float current = 0;
float voltage = 0;
float kwatt_hr = 0;

SemaphoreHandle_t kwh_mutex = NULL; // Mutex para proteger o acesso à variável result

uint64_t start_time;
uint64_t end_time;
uint64_t exec_time;

void make_readings()
{
    // Configure ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
    printf("Characterized using %s\n", val_type == ESP_ADC_CAL_VAL_EFUSE_TP ? "eFuse Two Point" : (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF ? "eFuse Vref" : "Default Vref"));
    adc1_config_channel_atten(channel_voltage, atten);
    adc1_config_channel_atten(channel_current, atten);

    while (1)
    {
        // Início da medição do tempo
        start_time = esp_timer_get_time();

        current = get_current();
        printf("\nCORRENTE: %.2f A\n", current);
        voltage = get_voltage();
        printf("TENSAO: %.2f V\n", voltage);

        // Fim da medição do tempo
        end_time = esp_timer_get_time();

        while (!xSemaphoreTake(kwh_mutex, portMAX_DELAY))
            ;

        // Tempo de execução da função em microssegundos
        exec_time = end_time - start_time;

        kwatt_hr += ((voltage * current * exec_time) / (3600.0 * 1000000000.0));
        printf("KWh: %.8f W\n", kwatt_hr);
        salva_kwhr_atual();
        xSemaphoreGive(kwh_mutex);
        vTaskDelay(55 / portTICK_PERIOD_MS);
        // 19.22
    }
}

void init_readings()
{
    // Crie o mutex para proteger o acesso à variável result
    kwh_mutex = xSemaphoreCreateMutex();
    xTaskCreate(make_readings, "make_readings", 12400, NULL, 5, NULL);
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

    float pin_volt = esp_adc_cal_raw_to_voltage(clean_higher_value, adc_chars);
    float aux = (-5.00 + (pin_volt * 0.003));

    if (aux > 0.3) // Abaixo disso ele esta lendo é ruido
        return (-5.00 + (pin_volt * 0.003));
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

    float pin_volt = esp_adc_cal_raw_to_voltage(clean_higher_value, adc_chars);
    float aux = (pin_volt * 0.208) - 349.392;
    if (aux > 5) // Apenas por seguraça  para nao retornar valores errados
        return (pin_volt * 0.208) - 349.392;
    else
        return 0;
}