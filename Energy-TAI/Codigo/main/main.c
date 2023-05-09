#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <stdio.h>
#include <stdlib.h>
#include "leitura_sensor.h"
#include "envio_internet.h"
#include "memoria_e_bluetooth.h"

extern SemaphoreHandle_t kwh_mutex;
extern int mes_salvo;
extern float valor_salvo;
extern struct tm tempo_atual;
extern float alarme;
float precoatual;

void app_main()
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    read_memory();
    init_bluetooth();
    init_wifi();
    init_readings();
    // monta_e_envia_mensagem();

    while (1)
    {
        printLocalTime();

        xSemaphoreTake(kwh_mutex, portMAX_DELAY);
        // CONTA
        xSemaphoreGive(kwh_mutex);

        if (tempo_atual.tm_mon != mes_salvo || alarme >= precoatual - valor_salvo)
            // monta_e_envia_mensagem();

            vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}