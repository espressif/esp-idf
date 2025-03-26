/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 *
 * This test code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */

#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "esp_mac.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "test_utils.h"
#include "memory_checks.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#define CONNECT_TIMEOUT_MS   (10000)

#define GOT_IP_EVENT             (1)
#define WIFI_DISCONNECT_EVENT    (1<<1)
#define WIFI_STA_CONNECTED       (1<<2)
#define WIFI_AP_STA_CONNECTED    (1<<3)

#define EVENT_HANDLER_FLAG_DO_NOT_AUTO_RECONNECT 0x00000001

static uint32_t wifi_event_handler_flag;
static const char* TAG = "test_wifi";
static esp_netif_t* s_ap_netif = NULL;
static esp_netif_t* s_sta_netif = NULL;

static EventGroupHandle_t wifi_events;
static void stop_wifi(void);

static void wifi_ap_event_handler(void* arg, esp_event_base_t event_base,
                                  int32_t event_id, void* event_data)
{
    ESP_LOGI(TAG, "wifi event handler: %"PRIi32, event_id);
    switch (event_id) {
    case WIFI_EVENT_AP_START:
        ESP_LOGI(TAG, "WIFI_EVENT_AP_START");
        break;
    case WIFI_EVENT_AP_STACONNECTED:
        ESP_LOGI(TAG, "WIFI_EVENT_AP_STACONNECTED");
        if (wifi_events) {
            xEventGroupSetBits(wifi_events, WIFI_AP_STA_CONNECTED);
        }
        break;
    case WIFI_EVENT_AP_STADISCONNECTED:
        ESP_LOGI(TAG, "WIFI_EVENT_AP_STADISCONNECTED");
        ESP_LOGI(TAG, "sta disconnected");
        break;
    default:
        break;
    }
    return;
}

static void wifi_sta_event_handler(void* arg, esp_event_base_t event_base,
                                   int32_t event_id, void* event_data)
{
    ESP_LOGI(TAG, "wifi event handler: %"PRIi32, event_id);
    switch (event_id) {
    case WIFI_EVENT_STA_START:
        ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
        // make sure softap has started
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        TEST_ESP_OK(esp_wifi_connect());
        ESP_LOGI(TAG, "start esp_wifi_connect");
        break;
    case WIFI_EVENT_STA_CONNECTED:
        ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED");
        if (wifi_events) {
            xEventGroupSetBits(wifi_events, WIFI_STA_CONNECTED);
        }
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");
        wifi_event_sta_disconnected_t *event = (wifi_event_sta_disconnected_t *)event_data;
        ESP_LOGI(TAG, "disconnect reason: %u", event->reason);
        if (!(EVENT_HANDLER_FLAG_DO_NOT_AUTO_RECONNECT & wifi_event_handler_flag)) {
            TEST_ESP_OK(esp_wifi_connect());
        }
        if (wifi_events) {
            xEventGroupSetBits(wifi_events, WIFI_DISCONNECT_EVENT);
        }
        break;
    default:
        break;
    }
    return;
}

static void ip_event_handler(void* arg, esp_event_base_t event_base,
                             int32_t event_id, void* event_data)
{
    ip_event_got_ip_t *event;

    ESP_LOGI(TAG, "ip event handler");
    switch (event_id) {
    case IP_EVENT_STA_GOT_IP:
        event = (ip_event_got_ip_t*)event_data;
        ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        if (wifi_events) {
            xEventGroupSetBits(wifi_events, GOT_IP_EVENT);
        }
        break;
    default:
        break;
    }
    return;
}

static esp_err_t event_init(void)
{
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &ip_event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    return ESP_OK;
}

static esp_err_t event_deinit(void)
{
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, ESP_EVENT_ANY_ID, &ip_event_handler));
    ESP_ERROR_CHECK(esp_event_loop_delete_default());
    return ESP_OK;
}

#define EMPH_STR(s) "****** "s" ******"

static void start_wifi(void)
{

    event_init();

    if (wifi_events == NULL) {
        wifi_events = xEventGroupCreate();
    }
    xEventGroupClearBits(wifi_events, 0x00ffffff);

    TEST_ESP_OK(esp_wifi_start());

}

static void stop_wifi(void)
{
    ESP_LOGI(TAG, "Stopping wifi now");
    TEST_ESP_OK(esp_wifi_stop());
    event_deinit();
    if (wifi_events) {
        vEventGroupDelete(wifi_events);
        wifi_events = NULL;
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
}

static void test_wifi_connection_sta(void)
{
    EventBits_t bits;
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_sta_event_handler, NULL));
    s_sta_netif = esp_netif_create_default_wifi_sta();
    esp_wifi_set_mode(WIFI_MODE_STA);

    start_wifi();

    wifi_config_t cfg;
    esp_wifi_get_config(WIFI_IF_STA, &cfg);
    ESP_LOGI(TAG, "STA mode, %s %s", cfg.sta.ssid, cfg.sta.password);

    bits = xEventGroupWaitBits(wifi_events, GOT_IP_EVENT, 1, 0, CONNECT_TIMEOUT_MS / portTICK_PERIOD_MS);
    TEST_ASSERT(bits & GOT_IP_EVENT);

    xEventGroupClearBits(wifi_events, WIFI_DISCONNECT_EVENT);
    wifi_event_handler_flag |= EVENT_HANDLER_FLAG_DO_NOT_AUTO_RECONNECT;

    // Wait for 60s and the stop wifi
    vTaskDelay(60000 / portTICK_PERIOD_MS);
    esp_netif_destroy_default_wifi(s_sta_netif);

    stop_wifi();
}

static void test_wifi_connection_softap(void)
{
    EventBits_t bits;
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_ap_event_handler, NULL));
    s_ap_netif = esp_netif_create_default_wifi_ap();

    esp_wifi_set_mode(WIFI_MODE_AP);
    start_wifi();

    wifi_config_t cfg;
    esp_wifi_get_config(WIFI_IF_AP, &cfg);
    ESP_LOGI(TAG, "AP mode, %s %s", cfg.ap.ssid, cfg.ap.password);

    // wait station connected
    bits = xEventGroupWaitBits(wifi_events, WIFI_AP_STA_CONNECTED, 1, 0, CONNECT_TIMEOUT_MS / portTICK_PERIOD_MS);
    TEST_ASSERT(bits & WIFI_AP_STA_CONNECTED);

    // wait 70s (longer than station side)
    vTaskDelay((60000 + CONNECT_TIMEOUT_MS) / portTICK_PERIOD_MS);
    esp_netif_destroy_default_wifi(s_ap_netif);

    stop_wifi();
}

TEST_CASE_MULTIPLE_DEVICES("test wifi connection with station and AP", "[wifi][timeout=90]", test_wifi_connection_sta, test_wifi_connection_softap);
