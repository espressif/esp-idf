/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* WPS Registrar example in softAP mode

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_log.h"
#include "esp_wps.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include <string.h>


/*set wps mode via project configuration */
#if CONFIG_EXAMPLE_WPS_TYPE_PBC
#define WPS_MODE WPS_TYPE_PBC
#elif CONFIG_EXAMPLE_WPS_TYPE_PIN
#if CONFIG_EXAMPLE_WPS_PIN_VALUE
#define EXAMPLE_WPS_PIN_VALUE CONFIG_EXAMPLE_WPS_PIN_VALUE
#endif
#define WPS_MODE WPS_TYPE_PIN
#else
#define WPS_MODE WPS_TYPE_DISABLE
#endif /*CONFIG_EXAMPLE_WPS_TYPE_PBC*/


#ifndef PIN2STR
#define PIN2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5], (a)[6], (a)[7]
#define PINSTR "%c%c%c%c%c%c%c%c"
#endif

#define EXAMPLE_ESP_WIFI_SSID      CONFIG_EXAMPLE_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_EXAMPLE_WIFI_PASSWORD
#define EXAMPLE_ESP_WIFI_CHANNEL   CONFIG_EXAMPLE_WIFI_CHANNEL
#define EXAMPLE_MAX_STA_CONN       CONFIG_EXAMPLE_MAX_STA_CONN

static const char *TAG = "example_wps_registrar";
static esp_wps_config_t config = WPS_CONFIG_INIT_DEFAULT(WPS_MODE);

static unsigned char pin[9];

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
		int32_t event_id, void* event_data)
{
	switch (event_id) {
	case WIFI_EVENT_AP_START:
		ESP_LOGI(TAG, "WIFI_EVENT_AP_START");
		break;
	case WIFI_EVENT_AP_STADISCONNECTED:
		{
			ESP_LOGI(TAG, "WIFI_EVENT_AP_STADISCONNECTED");
			wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
			ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
					MAC2STR(event->mac), event->aid);
		}
		break;
	case WIFI_EVENT_AP_STACONNECTED:
		{
			ESP_LOGI(TAG, "WIFI_EVENT_AP_STACONNECTED");
			wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
			ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
					MAC2STR(event->mac), event->aid);
		}
		break;
	case WIFI_EVENT_AP_WPS_RG_SUCCESS:
		{
			ESP_LOGI(TAG, "WIFI_EVENT_AP_WPS_RG_SUCCESS");
			wifi_event_ap_wps_rg_success_t *evt = (wifi_event_ap_wps_rg_success_t *)event_data;
			ESP_LOGI(TAG, "station "MACSTR" WPS successful",
					MAC2STR(evt->peer_macaddr));
		}
		break;
	case WIFI_EVENT_AP_WPS_RG_FAILED:
		{
			ESP_LOGI(TAG, "WIFI_EVENT_AP_WPS_RG_FAILED");
			wifi_event_ap_wps_rg_fail_reason_t *evt = (wifi_event_ap_wps_rg_fail_reason_t *)event_data;
			ESP_LOGI(TAG, "station "MACSTR" WPS failed, reason=%d",
						MAC2STR(evt->peer_macaddr), evt->reason);
			ESP_ERROR_CHECK(esp_wifi_ap_wps_disable());
			ESP_ERROR_CHECK(esp_wifi_ap_wps_enable(&config));
			ESP_ERROR_CHECK(esp_wifi_ap_wps_start(pin));
		}
		break;
	case WIFI_EVENT_AP_WPS_RG_TIMEOUT:
		{
			ESP_LOGI(TAG, "WIFI_EVENT_AP_WPS_RG_TIMEOUT");
			ESP_ERROR_CHECK(esp_wifi_ap_wps_disable());
			ESP_ERROR_CHECK(esp_wifi_ap_wps_enable(&config));
			ESP_ERROR_CHECK(esp_wifi_ap_wps_start(pin));
		}
		break;
	case WIFI_EVENT_AP_WPS_RG_PIN:
		{
			ESP_LOGI(TAG, "WIFI_EVENT_AP_WPS_RG_PIN");
			/* display the PIN code */
			wifi_event_ap_wps_rg_pin_t* event = (wifi_event_ap_wps_rg_pin_t *) event_data;
			ESP_LOGI(TAG, "WPS_PIN = " PINSTR, PIN2STR(event->pin_code));
		}
		break;
	default:
		break;
	}
}

/*init wifi as sta and start wps*/
static void start_wps_registrar(void)
{
	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_create_default_wifi_ap();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));

	wifi_config_t wifi_config = {
		.ap = {
			.ssid = EXAMPLE_ESP_WIFI_SSID,
			.ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
			.channel = EXAMPLE_ESP_WIFI_CHANNEL,
			.password = EXAMPLE_ESP_WIFI_PASS,
			.max_connection = EXAMPLE_MAX_STA_CONN,
			.authmode = WIFI_AUTH_WPA2_PSK,
			.pmf_cfg = {
				.required = false,
			},
		},
	};
	if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
		wifi_config.ap.authmode = WIFI_AUTH_OPEN;
	}

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));

	ESP_ERROR_CHECK(esp_wifi_start());

	ESP_LOGI(TAG, "start wps...");

	ESP_ERROR_CHECK(esp_wifi_ap_wps_enable(&config));
#if EXAMPLE_WPS_PIN_VALUE
	ESP_LOGI(TAG, "Staring WPS registrar with user specified pin %s", pin);
	snprintf((char *)pin, 9, "%08d", EXAMPLE_WPS_PIN_VALUE);
	ESP_ERROR_CHECK(esp_wifi_ap_wps_start(pin));
#else
	if (config.wps_type == WPS_TYPE_PBC) {
		ESP_LOGI(TAG, "Staring WPS registrar in PBC mode");
	} else {
		ESP_LOGI(TAG, "Staring WPS registrar with random generated pin");
	}
	ESP_ERROR_CHECK(esp_wifi_ap_wps_start(NULL));
#endif
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

	start_wps_registrar();
}
