// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "bt.h"
#include "bta_api.h"

#include "esp_blufi_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "blufi.h"

#define WIFI_LIST_NUM   10

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int CONNECTED_BIT = BIT0;



static wifi_config_t sta_config;

static char tmp_ssid[33];
static char tmp_passwd[33];
static bool confirm = false;

void wifi_set_blue_config(char *ssid, char *passwd)
{
    memset(tmp_ssid, 0, 33);
    memset(tmp_passwd, 0, 33);
    strcpy(tmp_ssid, ssid);
    strcpy(tmp_passwd, passwd);
    confirm = true;
    LOG_DEBUG("confirm true\n");
}

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        esp_blufi_send_config_state(ESP_BLUFI_CONFIG_OK);
        esp_disable_bluetooth(); //close bluetooth function
        //esp_deinit_bluetooth();  //free bluetooth resource
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP32 WiFi libs don't currently
           auto-reassociate. */
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

static void initialise_wifi(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}


void wifiTestTask(void *pvParameters)
{
    esp_err_t ret;

    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        if (confirm) {
            confirm = false;

            strcpy(sta_config.sta.ssid, tmp_ssid);
            strcpy(sta_config.sta.password, tmp_passwd);
            sta_config.sta.bssid_set = 0;

            ret = esp_wifi_disconnect();
            LOG_INFO("esp_wifi config\n");
            esp_wifi_set_config(WIFI_IF_STA, &sta_config);
            LOG_INFO("esp_wifi connect\n");
            ret = esp_wifi_connect();
            if (ret != ESP_OK) {
                LOG_ERROR("esp_wifi connect failed\n");
                esp_blufi_send_config_state(ESP_BLUFI_CONFIG_FAILED);
            }
        }
    }
}

void app_main()
{
    esp_err_t ret;

    nvs_flash_init();
    initialise_wifi();

    //vTaskDelay(3000 / portTICK_PERIOD_MS);

    bt_controller_init();
    xTaskCreatePinnedToCore(&wifiTestTask, "wifiTestTask", 2048, NULL, 20, NULL, 0);

    LOG_ERROR("%s init bluetooth\n", __func__);
    ret = esp_init_bluetooth();
    if (ret) {
        LOG_ERROR("%s init bluetooth failed\n", __func__);
        return;
    }
    blufi_init();
}
