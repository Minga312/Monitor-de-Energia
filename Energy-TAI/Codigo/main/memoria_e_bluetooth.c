/*
This example code is in the Public Domain (or CC0 licensed, at your option.)

Unless required by applicable law or agreed to in writing, this
software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_system.h"
#include "time.h"
#include "sys/time.h"

#define SPP_TAG "SPP_ACCEPTOR_DEMO"
#define SPP_SERVER_NAME "SPP_SERVER"
#define EXAMPLE_DEVICE_NAME "TESTES"
#define SPP_SHOW_DATA 0
#define SPP_SHOW_SPEED 1
#define SPP_SHOW_MODE SPP_SHOW_DATA /*Choose show mode: show data or speed*/

#define WIFI_DEFAULT "Engenharia"
#define SENHA_DEFAULT "12345678"
#define NUMERO_DEFAULT "5531916239"
#define KEY_DEFAULT "442757"

unsigned char naoapagapfvr[255] = "FALOU O MEU AMOR";
unsigned char dados[255] = "Os dados enviados foram: ";
char wifi[20], senha[20], numero[20], key[20], stralarme[20], strtarifa[20], strimpostos[20];
float tarifa, impostos, alarme;
int mes_salvo = 0;
float valor_salvo = 0;
int32_t inttarifa, intimpostos, intalarme, int_valor_salvo;
int idx = 0;

unsigned char tamanho_dados = 25;
static const esp_spp_mode_t esp_spp_mode = ESP_SPP_MODE_CB;

static struct timeval time_new, time_old;
static long data_num = 0;

static const esp_spp_sec_t sec_mask = ESP_SPP_SEC_AUTHENTICATE;
static const esp_spp_role_t role_slave = ESP_SPP_ROLE_SLAVE;

void read_memory()
{

    printf("\n");
    printf("Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    printf("Reading restart counter from NVS ... \n");
    size_t required_size = 0;
    int32_t restart_counter = 0; // value will default to 0, if not set yet in NVS
    err = nvs_get_str(my_handle, "wifi", NULL, &required_size);
    if (err == ESP_OK)
    {
        if (required_size > sizeof(wifi))
        {
            printf("Error: value too large for buffer\n");
            // handle the error accordingly
        }
        else
        {
            err = nvs_get_str(my_handle, "wifi", wifi, &required_size);
            printf("Done\n");
            printf("wifi value = %s\n", wifi);
        }
    }
    else
    {
        printf("Error (%s) reading!\n", esp_err_to_name(err));
        strcpy(wifi, WIFI_DEFAULT);
    }

    required_size = 0;
    err = nvs_get_str(my_handle, "senha", NULL, &required_size);
    if (err == ESP_OK)
    {
        if (required_size > sizeof(senha))
        {
            printf("Error: value too large for buffer\n");
            // handle the error accordingly
        }
        else
        {
            err = nvs_get_str(my_handle, "senha", senha, &required_size);
            printf("Done\n");
            printf("senha value = %s\n", senha);
        }
    }
    else
    {
        printf("Error (%s) reading!\n", esp_err_to_name(err));
        strcpy(senha, SENHA_DEFAULT);
    }

    required_size = 0;
    err = nvs_get_str(my_handle, "numero", NULL, &required_size);
    if (err == ESP_OK)
    {
        if (required_size > sizeof(numero))
        {
            printf("Error: value too large for buffer\n");
            // handle the error accordingly
        }
        else
        {
            err = nvs_get_str(my_handle, "numero", numero, &required_size);
            printf("Done\n");
            printf("numero value = %s\n", numero);
        }
    }
    else
    {
        printf("Error (%s) reading!\n", esp_err_to_name(err));
        strcpy(numero, NUMERO_DEFAULT);
    }

    required_size = 0;
    err = nvs_get_str(my_handle, "key", NULL, &required_size);
    if (err == ESP_OK)
    {
        if (required_size > sizeof(key))
        {
            printf("Error: value too large for buffer\n");
            // handle the error accordingly
        }
        else
        {
            err = nvs_get_str(my_handle, "key", key, &required_size);
            printf("Done\n");
            printf("key value = %s\n", key);
        }
    }
    else
    {
        printf("Error (%s) reading!\n", esp_err_to_name(err));
        strcpy(key, KEY_DEFAULT);
    }

    err = nvs_get_i32(my_handle, "inttarifa", &inttarifa);
    tarifa = (float)inttarifa / 1000;
    switch (err)
    {
    case ESP_OK:
        printf("Done\n");
        printf("inttarifa value = %d\n", inttarifa);
        break;
    default:
        printf("Error (%s) reading inttarifa!\n", esp_err_to_name(err));
        tarifa = 1;
    }

    err = nvs_get_i32(my_handle, "intimpostos", &intimpostos);
    impostos = (float)intimpostos / 1000;
    switch (err)
    {
    case ESP_OK:
        printf("Done\n");
        printf("intimpostos value = %d\n", intimpostos);
        break;
    default:
        printf("Error (%s) reading intimpostos!\n", esp_err_to_name(err));
        impostos = 0;
    }

    err = nvs_get_i32(my_handle, "mes_salvo", &mes_salvo);
    switch (err)
    {
    case ESP_OK:
        printf("Done\n");
        printf("mes_salvo value = %d\n", mes_salvo);
        break;
    default:
        printf("Error (%s) reading mes_salvo!\n", esp_err_to_name(err));
        mes_salvo = 0;
    }

    err = nvs_get_i32(my_handle, "valor_salvo", &int_valor_salvo);
    valor_salvo = (float)int_valor_salvo / 1000;
    switch (err)
    {
    case ESP_OK:
        printf("Done\n");
        printf("int_valor_salvo value = %d\n", int_valor_salvo);
        break;
    default:
        printf("Error (%s) reading int_valor_salvo!\n", esp_err_to_name(err));
        valor_salvo = 0;
    }

    err = nvs_get_i32(my_handle, "intalarme", &intalarme);
    alarme = (float)intalarme / 1000;
    switch (err)
    {
    case ESP_OK:
        printf("Done\n");
        printf("intalarme value = %d\n", intalarme);
        break;
    default:
        printf("Error (%s) reading intalarme!\n", esp_err_to_name(err));
        intalarme = 20;
    }

    nvs_close(my_handle);
}

void salva_ultimo_mes()
{
    printf("\n");
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    err = nvs_set_i32(my_handle, "mes_salvo", mes_salvo);
    err = nvs_commit(my_handle);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
    nvs_close(my_handle);
}

void salva_ultimo_valor()
{
    printf("\n");
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    err = nvs_set_i32(my_handle, "valor_salvo", (int32_t)valor_salvo * 1000);
    err = nvs_commit(my_handle);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
    nvs_close(my_handle);
}

static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    switch (event)
    {
    case ESP_SPP_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_INIT_EVT");
        esp_spp_start_srv(sec_mask, role_slave, 0, SPP_SERVER_NAME);
        break;
    case ESP_SPP_DISCOVERY_COMP_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_DISCOVERY_COMP_EVT");
        break;
    case ESP_SPP_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_OPEN_EVT");
        break;
    case ESP_SPP_CLOSE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CLOSE_EVT");
        break;
    case ESP_SPP_START_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_START_EVT");
        esp_bt_dev_set_device_name(EXAMPLE_DEVICE_NAME);
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
        break;
    case ESP_SPP_CL_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CL_INIT_EVT");
        break;
    case ESP_SPP_DATA_IND_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_DATA_IND_EVT len=%d handle=%d",
                 param->data_ind.len, param->data_ind.handle);
        printf("%s\n", param->data_ind.data);

        if (strstr((const char *)param->data_ind.data, "ler#"))
        {

            sprintf(strtarifa, "%g", tarifa);
            sprintf(strimpostos, "%g", impostos);
            sprintf(stralarme, "%g", alarme);
            strcpy((char *)dados, wifi);
            strcat((char *)dados, "+");
            strcat((char *)dados, senha);
            strcat((char *)dados, "+");
            strcat((char *)dados, numero);
            strcat((char *)dados, "+");
            strcat((char *)dados, key);
            strcat((char *)dados, "+");
            strcat((char *)dados, strtarifa);
            strcat((char *)dados, "+");
            strcat((char *)dados, strimpostos);
            strcat((char *)dados, "+");
            strcat((char *)dados, stralarme);
            strcat((char *)dados, "\r\n");

            printf("\nPacote montado: %s \n ", dados);
            esp_spp_write(param->data_ind.handle, strlen((const char *)dados), dados);
        }
        else if (strstr((const char *)param->data_ind.data, "bauticabaubau#"))
        {
            printf("FALOU O MEU AMOR");
            esp_spp_write(param->data_ind.handle, strlen((const char *)naoapagapfvr), naoapagapfvr);
        }

        else // parte que coloca valores

        {
            char palavra[20][20];

            int i = 0, j = 0, k = 0;
            while (param->data_ind.data[i] != '#')
            {

                if (param->data_ind.data[i] == '+')
                {
                    palavra[j][k] = '\0';
                    j++;
                    k = 0;
                }
                else
                {
                    palavra[j][k] = param->data_ind.data[i];
                    k++;
                }
                i++;
            }
            palavra[j][k] = '\0'; // a última palavra não é seguida por vírgula

            // Open
            printf("\n");
            printf("Opening Non-Volatile Storage (NVS) handle... ");
            nvs_handle_t my_handle;
            esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);

            strcpy(wifi, palavra[0]);
            err = nvs_set_str(my_handle, "wifi", wifi);
            printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

            strcpy(senha, palavra[1]);
            printf("Updating test value in NVS ... ");
            err = nvs_set_str(my_handle, "senha", senha);
            printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

            strcpy(numero, palavra[2]);
            printf("Updating test value in NVS ... ");
            err = nvs_set_str(my_handle, "numero", numero);
            printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

            strcpy(key, palavra[3]);
            printf("Updating test value in NVS ... ");
            err = nvs_set_str(my_handle, "key", key);
            printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

            tarifa = atof(palavra[4]);
            inttarifa = (int)tarifa * 1000;
            err = nvs_set_i32(my_handle, "inttarifa", inttarifa);
            printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

            impostos = atof(palavra[5]);
            intimpostos = (int)impostos * 1000;
            err = nvs_set_i32(my_handle, "intimpostos", intimpostos);
            printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

            alarme = atof(palavra[6]);
            intalarme = (int)alarme * 1000;
            err = nvs_set_i32(my_handle, "intalarme", intalarme);
            printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

            printf("O wifi e': %s\n", wifi);
            printf("A senha e': %s\n", senha);
            printf("O numero e': %s\n", numero);
            printf("A key e': %s\n", key);
            printf("A tarifa e': %f\n", tarifa);
            printf("Os impostos sao: %f\n", impostos);
            printf("O alarme e': %f\n", alarme);
            nvs_close(my_handle);
        }
        break;
        break;
    case ESP_SPP_CONG_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CONG_EVT");
        break;
    case ESP_SPP_WRITE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_WRITE_EVT");
        break;
    case ESP_SPP_SRV_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_OPEN_EVT");
        gettimeofday(&time_old, NULL);
        break;
    case ESP_SPP_SRV_STOP_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_STOP_EVT");
        break;
    case ESP_SPP_UNINIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_UNINIT_EVT");
        break;
    default:
        break;
    }
}

void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    switch (event)
    {
    case ESP_BT_GAP_AUTH_CMPL_EVT:
    {
        if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS)
        {
            ESP_LOGI(SPP_TAG, "authentication success: %s", param->auth_cmpl.device_name);
            esp_log_buffer_hex(SPP_TAG, param->auth_cmpl.bda, ESP_BD_ADDR_LEN);
        }
        else
        {
            ESP_LOGE(SPP_TAG, "authentication failed, status:%d", param->auth_cmpl.stat);
        }
        break;
    }
    case ESP_BT_GAP_PIN_REQ_EVT:
    {
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_PIN_REQ_EVT min_16_digit:%d", param->pin_req.min_16_digit);
        if (param->pin_req.min_16_digit)
        {
            ESP_LOGI(SPP_TAG, "Input pin code: 0000 0000 0000 0000");
            esp_bt_pin_code_t pin_code = {0};
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
        }
        else
        {
            ESP_LOGI(SPP_TAG, "Input pin code: 1234");
            esp_bt_pin_code_t pin_code;
            pin_code[0] = '1';
            pin_code[1] = '2';
            pin_code[2] = '3';
            pin_code[3] = '4';
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
        }

        break;
    }

#if (CONFIG_BT_SSP_ENABLED == true)
    case ESP_BT_GAP_CFM_REQ_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %d", param->cfm_req.num_val);
        esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;
    case ESP_BT_GAP_KEY_NOTIF_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_NOTIF_EVT passkey:%d", param->key_notif.passkey);
        break;
    case ESP_BT_GAP_KEY_REQ_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_REQ_EVT Please enter passkey!");
        break;
#endif

    default:
    {
        ESP_LOGI(SPP_TAG, "event: %d", event);
        break;
    }
    }
    return;
}

void init_bluetooth()
{
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));
    esp_err_t ret;
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if ((ret = esp_bt_controller_init(&bt_cfg)) != ESP_OK)
    {
        ESP_LOGE(SPP_TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK)
    {
        ESP_LOGE(SPP_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_init()) != ESP_OK)
    {
        ESP_LOGE(SPP_TAG, "%s initialize bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_enable()) != ESP_OK)
    {
        ESP_LOGE(SPP_TAG, "%s enable bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bt_gap_register_callback(esp_bt_gap_cb)) != ESP_OK)
    {
        ESP_LOGE(SPP_TAG, "%s gap register failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_spp_register_callback(esp_spp_cb)) != ESP_OK)
    {
        ESP_LOGE(SPP_TAG, "%s spp register failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_spp_init(esp_spp_mode)) != ESP_OK)
    {
        ESP_LOGE(SPP_TAG, "%s spp init failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

#if (CONFIG_BT_SSP_ENABLED == true)
    /* Set default parameters for Secure Simple Pairing */
    esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
    esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
#endif

    /*
     * Set default parameters for Legacy Pairing
     * Use variable pin, input pin code when pairing
     */
    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_VARIABLE;
    esp_bt_pin_code_t pin_code;
    esp_bt_gap_set_pin(pin_type, 0, pin_code);
}
