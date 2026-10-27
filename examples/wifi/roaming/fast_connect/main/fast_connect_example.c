/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 *
 * SPDX-FileContributor: 2026 Philip Kearney, Nightwhistle Labs
 */
/* Wi-Fi Fast Connect Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/*
 * The roaming app's fast connect remembers the AP each connection ends on and connects to it directly
 * at the next boot, and its check after a connection moves to a much stronger AP once the application
 * is quiet. This example keeps the saved station configuration across boots (where the record lives),
 * logs the time from boot to the first IP address, and can play a busy application.
 */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "esp_wifi_roaming.h"
#include "nvs_flash.h"

#define EXAMPLE_WIFI_SSID CONFIG_EXAMPLE_WIFI_SSID
#define EXAMPLE_WIFI_PASS CONFIG_EXAMPLE_WIFI_PASSWORD

static const char *TAG = "fast connect example";
static bool s_first_ip_seen;

#if CONFIG_ESP_WIFI_ROAMING_JOIN_CHECK && CONFIG_EXAMPLE_BUSY_AFTER_IP_MS > 0
static esp_timer_handle_t s_busy_timer;

static void example_report_hold(bool awaiting_reply)
{
    wifi_roaming_app_hold_t hold = {
        .awaiting_reply = awaiting_reply,
        .settling = false,
    };
    esp_err_t err = esp_wifi_roaming_set_app_hold(&hold);

    if (err != ESP_OK) {
        ESP_LOGW(TAG, "esp_wifi_roaming_set_app_hold: %s", esp_err_to_name(err));
    }
}

static void example_busy_done(void *arg)
{
    ESP_LOGI(TAG, "application quiet");
    example_report_hold(false);
}
#endif

#if CONFIG_EXAMPLE_RESTART_INTERVAL > 0
static void example_restart(void *arg)
{
    ESP_LOGI(TAG, "restarting to measure boot to IP again");
    esp_restart();
}
#endif

static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
        wifi_event_sta_connected_t *conn = event_data;
        ESP_LOGI(TAG, "connected on channel %d, %lld ms after boot", conn->channel, esp_timer_get_time() / 1000);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        /* The roaming app reconnects. */
        wifi_event_sta_disconnected_t *disconn = event_data;
        ESP_LOGI(TAG, "disconnected, reason %d", disconn->reason);
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        if (!s_first_ip_seen) {
            s_first_ip_seen = true;
            /* What fast connect shortens: compare this line with and without a remembered AP. */
            ESP_LOGI(TAG, "boot to IP: %lld ms", esp_timer_get_time() / 1000);
#if CONFIG_EXAMPLE_RESTART_INTERVAL > 0
            static esp_timer_handle_t s_restart_timer;
            const esp_timer_create_args_t restart_args = {
                .callback = example_restart,
                .name = "example_restart",
            };
            ESP_ERROR_CHECK(esp_timer_create(&restart_args, &s_restart_timer));
            ESP_ERROR_CHECK(esp_timer_start_once(s_restart_timer, CONFIG_EXAMPLE_RESTART_INTERVAL * 1000000LL));
#endif
        }
#if CONFIG_ESP_WIFI_ROAMING_JOIN_CHECK && CONFIG_EXAMPLE_BUSY_AFTER_IP_MS > 0
        ESP_LOGI(TAG, "application busy for %d ms", CONFIG_EXAMPLE_BUSY_AFTER_IP_MS);
        example_report_hold(true);
        esp_timer_stop(s_busy_timer); /* ESP_ERR_INVALID_STATE when it is not running */
        ESP_ERROR_CHECK(esp_timer_start_once(s_busy_timer, CONFIG_EXAMPLE_BUSY_AFTER_IP_MS * 1000LL));
#endif
    }
}

/* Set the station configuration only when the network changes. Setting it at every boot would replace
 * fast connect's record of the AP, which lives in the saved configuration, and every boot would scan
 * every channel again. A new network starts with every channel, strongest first. */
static void example_set_network_if_new(void)
{
    wifi_config_t saved = {0};

    ESP_ERROR_CHECK(esp_wifi_get_config(WIFI_IF_STA, &saved));
    if (strncmp((const char *) saved.sta.ssid, EXAMPLE_WIFI_SSID, sizeof(saved.sta.ssid)) == 0 &&
            strncmp((const char *) saved.sta.password, EXAMPLE_WIFI_PASS, sizeof(saved.sta.password)) == 0) {
        ESP_LOGI(TAG, "keeping the saved station configuration (AP remembered: %s)", saved.sta.bssid_set ? "yes" : "no");
        return;
    }

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
            .scan_method = WIFI_ALL_CHANNEL_SCAN,
            .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
        },
    };
    ESP_LOGI(TAG, "saving a new network");
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
}

static void wifi_init_sta(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL));

#if CONFIG_ESP_WIFI_ROAMING_JOIN_CHECK && CONFIG_EXAMPLE_BUSY_AFTER_IP_MS > 0
    const esp_timer_create_args_t busy_args = {
        .callback = example_busy_done,
        .name = "example_busy",
    };
    ESP_ERROR_CHECK(esp_timer_create(&busy_args, &s_busy_timer));
#endif

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    example_set_network_if_new();
    ESP_ERROR_CHECK(esp_wifi_start());
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifi_init_sta();
}
