/* WiFi Connection Example using WPS

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/*
   Showing how to use WPS.

   WPS_TYPE_PBC: Start esp32 and it will enter wps PBC mode. Then push the button of wps on router down. The esp32 will connected to the router.
   WPS_TYPE_PIN: Start esp32, You'll see PIN code which is a eight-digit number showing on COM. Enter the PIN code in router and then the esp32 will connected to router.
*/

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_wps.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include <string.h>


/*set wps mode via "make menuconfig"*/
#if CONFIG_EXAMPLE_WPS_TYPE_PBC
#define WPS_TEST_MODE WPS_TYPE_PBC
#elif CONFIG_EXAMPLE_WPS_TYPE_PIN
#define WPS_TEST_MODE WPS_TYPE_PIN
#else
#define WPS_TEST_MODE WPS_TYPE_DISABLE
#endif /*CONFIG_EXAMPLE_WPS_TYPE_PBC*/

#define MAX_RETRY_ATTEMPTS     2

#ifndef PIN2STR
#define PIN2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5], (a)[6], (a)[7]
#define PINSTR "%c%c%c%c%c%c%c%c"
#endif


static const char *TAG = "example_wps";
static esp_wps_config_t config = WPS_CONFIG_INIT_DEFAULT(WPS_TEST_MODE);
static wifi_config_t wps_ap_creds[MAX_WPS_AP_CRED];
static int s_ap_creds_num = 0;
static int s_retry_num = 0;

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    static int ap_idx = 1;

    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
	ESP_LOGI(TAG, "got ip:%s\n",
		ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
    ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
        if (s_retry_num < MAX_RETRY_ATTEMPTS) {
            ESP_ERROR_CHECK(esp_wifi_connect());
            s_retry_num++;
        } else if (ap_idx < s_ap_creds_num) {
            /* Try the next AP credential if first one fails */
	
            if (ap_idx < s_ap_creds_num) {
                ESP_LOGI(TAG, "Connecting to SSID: %s, Passphrase: %s",
                wps_ap_creds[ap_idx].sta.ssid, wps_ap_creds[ap_idx].sta.password);
                    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wps_ap_creds[ap_idx++]) );
                    ESP_ERROR_CHECK(esp_wifi_connect());
                }
            s_retry_num = 0;
        } else {
            ESP_LOGI(TAG, "Failed to connect!");
        }

        break;
    case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
	/*point: the function esp_wifi_wps_start() only get ssid & password
	 * so call the function esp_wifi_connect() here
	 * */
	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_SUCCESS");
    {
        wifi_event_sta_wps_er_success_t *evt = &event->event_info.sta_er_success;
        int i;
        if (evt && evt->ap_cred_cnt > 1) {
            s_ap_creds_num = evt->ap_cred_cnt;
            for (i = 0; i < s_ap_creds_num; i++) {
                memcpy(wps_ap_creds[i].sta.ssid, evt->ap_cred[i].ssid, sizeof(evt->ap_cred[i].ssid));
                memcpy(wps_ap_creds[i].sta.password, evt->ap_cred[i].passphrase, sizeof(evt->ap_cred[i].passphrase));
            }
            /* If multiple AP credentials are received from WPS, connect with first one */
            ESP_LOGI(TAG, "Connecting to SSID: %s, Passphrase: %s", wps_ap_creds[0].sta.ssid, wps_ap_creds[0].sta.password);
            ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wps_ap_creds[0]) );
        }
        /*
         * If only one AP credential is received from WPS, there will be no event data and
         * esp_wifi_set_config() is already called by WPS modules for backward compatibility
         * with legacy apps. So directly attempt connection here.
         */
         ESP_ERROR_CHECK(esp_wifi_wps_disable());
         ESP_ERROR_CHECK(esp_wifi_connect());
    }
	break;
    case SYSTEM_EVENT_STA_WPS_ER_FAILED:
	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_FAILED");
	ESP_ERROR_CHECK(esp_wifi_wps_disable());
	ESP_ERROR_CHECK(esp_wifi_wps_enable(&config));
	ESP_ERROR_CHECK(esp_wifi_wps_start(0));
	break;
    case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_TIMEOUT");
	ESP_ERROR_CHECK(esp_wifi_wps_disable());
	ESP_ERROR_CHECK(esp_wifi_wps_enable(&config));
	ESP_ERROR_CHECK(esp_wifi_wps_start(0));
 	break;
    case SYSTEM_EVENT_STA_WPS_ER_PIN:
	ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_PIN");
	/*show the PIN code here*/
	ESP_LOGI(TAG, "WPS_PIN = "PINSTR, PIN2STR(event->event_info.sta_er_pin.pin_code));
	break;
    default:
        break;
    }
    return ESP_OK;
}

/*init wifi as sta and start wps*/
static void start_wps(void)
{
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    ESP_LOGI(TAG, "start wps...");
    
       
    ESP_ERROR_CHECK(esp_wifi_wps_enable(&config));
    ESP_ERROR_CHECK(esp_wifi_wps_start(0));
}

void app_main()
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
