/* BLE based Provisioning Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <nvs_flash.h>

#include <lwip/err.h>
#include <lwip/sys.h>

#include "app_prov.h"

#define EXAMPLE_AP_RECONN_ATTEMPTS  CONFIG_EXAMPLE_AP_RECONN_ATTEMPTS

static const char *TAG = "app";

static void start_ble_provisioning(void);

static void event_handler(void* arg, esp_event_base_t event_base,
                          int event_id, void* event_data)
{
    static int s_retry_num_ap_not_found = 0;
    static int s_retry_num_ap_auth_fail = 0;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_event_sta_disconnected_t* disconnected = (wifi_event_sta_disconnected_t*) event_data;
        switch (disconnected->reason) {
        case WIFI_REASON_AUTH_EXPIRE:
        case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
        case WIFI_REASON_BEACON_TIMEOUT:
        case WIFI_REASON_AUTH_FAIL:
        case WIFI_REASON_ASSOC_FAIL:
        case WIFI_REASON_HANDSHAKE_TIMEOUT:
            ESP_LOGW(TAG, "connect to the AP fail : auth Error");
            if (s_retry_num_ap_auth_fail < EXAMPLE_AP_RECONN_ATTEMPTS) {
                s_retry_num_ap_auth_fail++;
                esp_wifi_connect();
                ESP_LOGI(TAG, "retry connecting to the AP...");
            } else {
                /* Restart provisioning if authentication fails */
                start_ble_provisioning();
            }
            break;
        case WIFI_REASON_NO_AP_FOUND:
            ESP_LOGW(TAG, "connect to the AP fail : not found");
            if (s_retry_num_ap_not_found < EXAMPLE_AP_RECONN_ATTEMPTS) {
                s_retry_num_ap_not_found++;
                esp_wifi_connect();
                ESP_LOGI(TAG, "retry to connecting to the AP...");
            }
            break;
        default:
            /* None of the expected reasons */
            esp_wifi_connect();
            break;
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num_ap_not_found = 0;
        s_retry_num_ap_auth_fail = 0;
    }
}

static void wifi_init_sta(void)
{
    /* Set our event handling */
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, event_handler, NULL));

    /* Start Wi-Fi in station mode with credentials set during provisioning */
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static void start_ble_provisioning(void)
{
    /* Security version */
    int security = 0;
    /* Proof of possession */
    const protocomm_security_pop_t *pop = NULL;

#ifdef CONFIG_EXAMPLE_USE_SEC_1
    security = 1;
#endif

    /* Having proof of possession is optional */
#ifdef CONFIG_EXAMPLE_USE_POP
    const static protocomm_security_pop_t app_pop = {
        .data = (uint8_t *) CONFIG_EXAMPLE_POP,
        .len = (sizeof(CONFIG_EXAMPLE_POP)-1)
    };
    pop = &app_pop;
#endif

    ESP_ERROR_CHECK(app_prov_start_ble_provisioning(security, pop));
}

void app_main(void)
{
    /* Initialize networking stack */
    esp_netif_init();

    /* Create default event loop needed by the
     * main app and the provisioning service */
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* Initialize NVS needed by Wi-Fi */
    ESP_ERROR_CHECK(nvs_flash_init());

    /* Initialize Wi-Fi including netif with default config */
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /* Check if device is provisioned */
    bool provisioned;
    if (app_prov_is_provisioned(&provisioned) != ESP_OK) {
        ESP_LOGE(TAG, "Error getting device provisioning state");
        return;
    }

    if (provisioned == false) {
        /* If not provisioned, start provisioning via BLE */
        ESP_LOGI(TAG, "Starting BLE provisioning");
        start_ble_provisioning();
    } else {
        /* Else start as station with credentials set during provisioning */
        ESP_LOGI(TAG, "Starting WiFi station");
        wifi_init_sta();
    }
}
