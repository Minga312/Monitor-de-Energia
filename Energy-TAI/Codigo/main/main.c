#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <stdio.h>
#include <stdlib.h>
#include "leitura_sensor.h"
#include "envio_internet.h"
#include "memoria_e_bluetooth.h"

extern SemaphoreHandle_t kwh_mutex;
extern float kwatt_hr;
extern struct tm tempo_atual;
extern float tarifa;
extern float alarme;
extern float valor_salvo;
extern int mes_salvo;
extern int wifi_conectado;

extern float kwatt_hr;
extern float precoatual;

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
    if (wifi_conectado)
        monta_e_envia_mensagem();

    while (1)
    {

        xSemaphoreTake(kwh_mutex, portMAX_DELAY);
        precoatual = (kwatt_hr - (valor_salvo / tarifa)) * tarifa;
        xSemaphoreGive(kwh_mutex);

        if (wifi_conectado)
        {
            printLocalTime();
            if (tempo_atual.tm_min != mes_salvo)
            {
                printf("\n Envio Por Data \n");

                monta_e_envia_mensagem();
                mes_salvo = tempo_atual.tm_min;
                salva_ultimo_mes();
            }
            else if (precoatual - valor_salvo >= alarme)
            {
                printf("\n Envio Por Alarme \n");

                monta_e_envia_mensagem();
                valor_salvo = precoatual;
                salva_ultimo_valor();
            }
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}