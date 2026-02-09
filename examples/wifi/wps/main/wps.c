/* WiFi Connection Example using WPS

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/**
 * @brief Demonstrates how to use WPS.
 *
 * This example supports two modes, which can be selected in menuconfig:
 *
 * - WPS_TYPE_PBC:
 *   Start the ESP device and it will enter WPS PBC mode. Then push the WPS button on the router.
 *   The ESP device will receive the SSID and password and connect to the router.
 *
 * - WPS_TYPE_PIN:
 *   Start the ESP device, and you'll see an eight-digit PIN number in the log output.
 *   Enter this PIN code on the router, and the ESP device will connect.
 *
 */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_wps.h"
#include "nvs_flash.h"


/*set wps mode via project configuration */
#if CONFIG_EXAMPLE_WPS_TYPE_PBC
#define WPS_MODE WPS_TYPE_PBC
#elif CONFIG_EXAMPLE_WPS_TYPE_PIN
#define WPS_MODE WPS_TYPE_PIN
#else
#define WPS_MODE WPS_TYPE_DISABLE
#endif /*CONFIG_EXAMPLE_WPS_TYPE_PBC*/

#define MAX_RETRY_ATTEMPTS     2

#ifndef PIN2STR
#define PIN2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5], (a)[6], (a)[7]
#define PINSTR "%c%c%c%c%c%c%c%c"
#endif

static const char *TAG = "example_wps";
static esp_wps_config_t config = WPS_CONFIG_INIT_DEFAULT(WPS_MODE);
static wifi_config_t s_wps_ap_creds[MAX_WPS_AP_CRED];
static int s_ap_creds_num = 0;
static int s_retry_num = 0;
static bool s_wps_done = false;

static void wps_restart(void)
{
    ESP_ERROR_CHECK(esp_wifi_wps_disable());
    ESP_ERROR_CHECK(esp_wifi_wps_enable(&config));
    ESP_ERROR_CHECK(esp_wifi_wps_start());
}

static void handle_wps_success(void *event_data)
{
    wifi_event_sta_wps_er_success_t *evt = (wifi_event_sta_wps_er_success_t *)event_data;
    if (evt) {
        s_ap_creds_num = evt->ap_cred_cnt;
        for (int idx = 0; idx < s_ap_creds_num; idx++) {
            memcpy(s_wps_ap_creds[idx].sta.ssid, evt->ap_cred[idx].ssid,
                   sizeof(evt->ap_cred[idx].ssid));
            memcpy(s_wps_ap_creds[idx].sta.password, evt->ap_cred[idx].passphrase,
                   sizeof(evt->ap_cred[idx].passphrase));
        }
        /* If multiple AP credentials are received from WPS, connect with first one */
        ESP_LOGI(TAG, "Connecting to SSID: %s, Passphrase: %s",
                 s_wps_ap_creds[0].sta.ssid, s_wps_ap_creds[0].sta.password);
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &s_wps_ap_creds[0]));
    }
    /*
     * If only one AP credential is received from WPS, there will be no event data and
     * esp_wifi_set_config() is already called by WPS modules for backward compatibility
     * with legacy apps. So directly attempt connection here.
     */
    ESP_ERROR_CHECK(esp_wifi_wps_disable());
    s_wps_done = true;
    esp_wifi_connect();
}

static void handle_wps_pin(void *event_data)
{
    /* display the PIN code */
    wifi_event_sta_wps_er_pin_t* event = (wifi_event_sta_wps_er_pin_t*) event_data;
    ESP_LOGI(TAG, "WPS_PIN = " PINSTR, PIN2STR(event->pin_code));
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    static int ap_idx = 1;

    switch (event_id) {
    case WIFI_EVENT_STA_START:
        ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");
        if (s_wps_done) {
            if (s_retry_num < MAX_RETRY_ATTEMPTS) {
                esp_wifi_connect();
                s_retry_num++;
                ESP_LOGI(TAG, "retrying to connect to the AP");
            } else if (ap_idx < s_ap_creds_num) {
                /* Try the next AP credential if first one fails */
                ESP_LOGI(TAG, "Connecting to SSID: %s, Passphrase: %s",
                         s_wps_ap_creds[ap_idx].sta.ssid, s_wps_ap_creds[ap_idx].sta.password);
                ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &s_wps_ap_creds[ap_idx++]));
                esp_wifi_connect();
                s_retry_num = 0;
            } else {
                ESP_LOGI(TAG, "Failed to connect!");
            }
        }
        break;
    case WIFI_EVENT_STA_WPS_ER_SUCCESS:
        ESP_LOGI(TAG, "WIFI_EVENT_STA_WPS_ER_SUCCESS");
        handle_wps_success(event_data);
        break;
    case WIFI_EVENT_STA_WPS_ER_FAILED:
        ESP_LOGI(TAG, "WIFI_EVENT_STA_WPS_ER_FAILED");
        wps_restart();
        break;
    case WIFI_EVENT_STA_WPS_ER_TIMEOUT:
        ESP_LOGI(TAG, "WIFI_EVENT_STA_WPS_ER_TIMEOUT");
        wps_restart();
        break;
    case WIFI_EVENT_STA_WPS_ER_PIN:
        ESP_LOGI(TAG, "WIFI_EVENT_STA_WPS_ER_PIN");
        handle_wps_pin(event_data);
        break;
    default:
        break;
    }
}

static void got_ip_event_handler(void* arg, esp_event_base_t event_base,
                             int32_t event_id, void* event_data)
{
    ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
    ESP_LOGI(TAG, "got ip: " IPSTR, IP2STR(&event->ip_info.ip));
}

/**
 * @brief Start WPS registration
 */
static void start_wps(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &got_ip_event_handler, NULL));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "start wps...");

    ESP_ERROR_CHECK(esp_wifi_wps_enable(&config));
    ESP_ERROR_CHECK(esp_wifi_wps_start());
}

void app_main(void)
{
    /* Initialize NVS — it is used to store PHY calibration data */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    start_wps();
}
