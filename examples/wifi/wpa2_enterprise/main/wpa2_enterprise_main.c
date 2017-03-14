/* WiFi Connection Example using WPA2 Enterprise
 *
 * Original Copyright (C) 2006-2016, ARM Limited, All Rights Reserved, Apache 2.0 License.
 * Additions Copyright (C) Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD, Apache 2.0 License.
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"

/* The examples use simple WiFi configuration that you can set via
   'make menuconfig'.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"

   You can choose EAP method via 'make menuconfig' according to the
   configuration of AP.
*/
#define EXAMPLE_WIFI_SSID CONFIG_WIFI_SSID
#define EXAMPLE_EAP_METHOD CONFIG_EAP_METHOD

#define EXAMPLE_EAP_ID CONFIG_EAP_ID
#define EXAMPLE_EAP_USERNAME CONFIG_EAP_USERNAME
#define EXAMPLE_EAP_PASSWORD CONFIG_EAP_PASSWORD

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int CONNECTED_BIT = BIT0;

/* Constants that aren't configurable in menuconfig */
#define EAP_PEAP 1
#define EAP_TTLS 2

static const char *TAG = "example";

/* CA cert, taken from wpa2_ca.pem
   Client cert, taken from wpa2_client.crt
   Client key, taken from wpa2_client.key

   The PEM, CRT and KEY file were provided by the person or organization
   who configured the AP with wpa2 enterprise.

   To embed it in the app binary, the PEM, CRT and KEY file is named
   in the component.mk COMPONENT_EMBED_TXTFILES variable.
*/
extern uint8_t ca_pem_start[] asm("_binary_wpa2_ca_pem_start");
extern uint8_t ca_pem_end[]   asm("_binary_wpa2_ca_pem_end");
extern uint8_t client_crt_start[] asm("_binary_wpa2_client_crt_start");
extern uint8_t client_crt_end[]   asm("_binary_wpa2_client_crt_end");
extern uint8_t client_key_start[] asm("_binary_wpa2_client_key_start");
extern uint8_t client_key_end[]   asm("_binary_wpa2_client_key_end");

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
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
    unsigned int ca_pem_bytes = ca_pem_end - ca_pem_start;
    unsigned int client_crt_bytes = client_crt_end - client_crt_start;
    unsigned int client_key_bytes = client_key_end - client_key_start;

    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_ca_cert(ca_pem_start, ca_pem_bytes) );
    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_cert_key(client_crt_start, client_crt_bytes,\
    		client_key_start, client_key_bytes, NULL, 0) );
    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EXAMPLE_EAP_ID, strlen(EXAMPLE_EAP_ID)) );
    if (EXAMPLE_EAP_METHOD == EAP_PEAP || EXAMPLE_EAP_METHOD == EAP_TTLS) {
        ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EXAMPLE_EAP_USERNAME, strlen(EXAMPLE_EAP_USERNAME)) );
        ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EXAMPLE_EAP_PASSWORD, strlen(EXAMPLE_EAP_PASSWORD)) );
    }
    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_enable() );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

static void wpa2_enterprise_task(void *pvParameters)
{
    tcpip_adapter_ip_info_t ip;
    memset(&ip, 0, sizeof(tcpip_adapter_ip_info_t));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    while (1) {
        vTaskDelay(2000 / portTICK_PERIOD_MS);

        if (tcpip_adapter_get_ip_info(ESP_IF_WIFI_STA, &ip) == 0) {
            ESP_LOGI(TAG, "~~~~~~~~~~~");
            ESP_LOGI(TAG, "IP:"IPSTR, IP2STR(&ip.ip));
            ESP_LOGI(TAG, "MASK:"IPSTR, IP2STR(&ip.netmask));
            ESP_LOGI(TAG, "GW:"IPSTR, IP2STR(&ip.gw));
            ESP_LOGI(TAG, "~~~~~~~~~~~");
        }
    }
}

void app_main()
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    initialise_wifi();
    xTaskCreate(&wpa2_enterprise_task, "wpa2_enterprise_task", 4096, NULL, 5, NULL);
}
