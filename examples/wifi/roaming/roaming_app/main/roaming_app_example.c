/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* WiFi Roaming App Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

/* The examples use WiFi configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
#define MAXIMUM_RETRY 5
const char *TAG = "wifi roaming app";
static bool s_first_ip_seen;

#if CONFIG_ESP_EXAMPLE_RESTART_INTERVAL > 0
static void example_restart(void *arg)
{
    ESP_LOGI(TAG, "restarting to measure boot to IP again");
    esp_restart();
}
#endif

static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
        wifi_event_sta_connected_t *conn = event_data;
        ESP_LOGI(TAG, "connected on channel %d, %lld ms after boot", conn->channel, esp_timer_get_time() / 1000);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_event_sta_disconnected_t *disconn = event_data;
        if (disconn->reason == WIFI_REASON_ROAMING) {
            ESP_LOGI(TAG, "station disconnected during roaming");
        } else {
            ESP_LOGI(TAG, "station disconnected with reason %d", disconn->reason);
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        if (!s_first_ip_seen) {
            s_first_ip_seen = true;
            /* What fast connect shortens: compare this line with and without a remembered AP. */
            ESP_LOGI(TAG, "boot to IP: %lld ms", esp_timer_get_time() / 1000);
#if CONFIG_ESP_EXAMPLE_RESTART_INTERVAL > 0
            static esp_timer_handle_t s_restart_timer;
            const esp_timer_create_args_t restart_args = {
                .callback = example_restart,
                .name = "example_restart",
            };
            ESP_ERROR_CHECK(esp_timer_create(&restart_args, &s_restart_timer));
            ESP_ERROR_CHECK(esp_timer_start_once(s_restart_timer, CONFIG_ESP_EXAMPLE_RESTART_INTERVAL * 1000000LL));
#endif
        }
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

/* With fast connect (CONFIG_ESP_WIFI_ROAMING_FAST_CONNECT), set the station configuration only when the SSID or
 * password changes: the roaming app keeps its record of the AP in the saved station configuration, so setting the
 * configuration at every boot would replace the record, and every boot would scan every channel again.
 * Without fast connect, set it at every boot, as before. That also replaces a record left in the saved
 * configuration by an earlier firmware with fast connect, which nothing would clear after a failed connection. */
static void example_set_station_config(void)
{
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .btm_enabled = 1,
            .rm_enabled = 1,
            .mbo_enabled = 1,
            .ft_enabled = 1,
#if CONFIG_ESP_WIFI_ROAMING_FAST_CONNECT
            /* A new network starts with every channel, strongest AP first. */
            .scan_method = WIFI_ALL_CHANNEL_SCAN,
            .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
#endif
        }
    };

#if CONFIG_ESP_WIFI_ROAMING_FAST_CONNECT
    wifi_config_t saved = {0};
    ESP_ERROR_CHECK(esp_wifi_get_config(WIFI_IF_STA, &saved));
    if (strncmp((const char *) saved.sta.ssid, EXAMPLE_ESP_WIFI_SSID, sizeof(saved.sta.ssid)) == 0 &&
            strncmp((const char *) saved.sta.password, EXAMPLE_ESP_WIFI_PASS, sizeof(saved.sta.password)) == 0) {
        ESP_LOGI(TAG, "keeping the saved station configuration (AP remembered: %s)", saved.sta.bssid_set ? "yes" : "no");
        return;
    }
    ESP_LOGI(TAG, "saving a new network");
#endif
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
}

void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    example_set_station_config();
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
}

void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();
}
