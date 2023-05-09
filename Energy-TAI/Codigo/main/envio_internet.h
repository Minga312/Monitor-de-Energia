
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include <stdlib.h>

#include "esp_tls.h"

#include "esp_http_client.h"

#include <stdio.h>
#include <time.h>
#include "esp_sntp.h"

void init_wifi();
void monta_e_envia_mensagem();
void printLocalTime();
void obtain_time();
