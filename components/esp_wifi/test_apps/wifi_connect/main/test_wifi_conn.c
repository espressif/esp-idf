/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
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

#ifndef TEST_SUFFIX_STR
#define TEST_SUFFIX_STR "_0000"
#endif

#define TEST_DEFAULT_SSID "SSID_" CONFIG_IDF_TARGET TEST_SUFFIX_STR
#define TEST_DEFAULT_PWD "PASS_" CONFIG_IDF_TARGET TEST_SUFFIX_STR
#define TEST_DEFAULT_CHANNEL (6)
#define CONNECT_TIMEOUT_MS   (8000)

#define GOT_IP_EVENT             (1)
#define WIFI_DISCONNECT_EVENT    (1<<1)
#define WIFI_STA_CONNECTED       (1<<2)
#define WIFI_AP_STA_CONNECTED    (1<<3)

#define EVENT_HANDLER_FLAG_DO_NOT_AUTO_RECONNECT 0x00000001

static const char* TAG = "test_wifi";
static uint32_t wifi_event_handler_flag;
static esp_netif_t* s_ap_netif = NULL;
static esp_netif_t* s_sta_netif = NULL;

static EventGroupHandle_t wifi_events;

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    ESP_LOGI(TAG, "wifi event handler: %"PRIi32, event_id);
    switch (event_id) {
    case WIFI_EVENT_STA_START:
        ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
        break;
    case WIFI_EVENT_AP_STACONNECTED:
        ESP_LOGI(TAG, "WIFI_EVENT_AP_STACONNECTED");
        if (wifi_events) {
            xEventGroupSetBits(wifi_events, WIFI_AP_STA_CONNECTED);
        }
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
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &ip_event_handler, NULL));
    s_sta_netif = esp_netif_create_default_wifi_sta();
    s_ap_netif = esp_netif_create_default_wifi_ap();
    return ESP_OK;
}

static esp_err_t event_deinit(void)
{
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler));
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, ESP_EVENT_ANY_ID, &ip_event_handler));
    esp_netif_destroy_default_wifi(s_sta_netif);
    esp_netif_destroy_default_wifi(s_ap_netif);
    ESP_ERROR_CHECK(esp_event_loop_delete_default());
    return ESP_OK;
}

#define EMPH_STR(s) "****** "s" ******"

static void start_wifi_as_softap(void)
{
    wifi_config_t w_config = {
        .ap.ssid = TEST_DEFAULT_SSID,
        .ap.password = TEST_DEFAULT_PWD,
        .ap.ssid_len = strlen(TEST_DEFAULT_SSID),
           .ap.channel = TEST_DEFAULT_CHANNEL,
           .ap.authmode = WIFI_AUTH_WPA2_PSK,
           .ap.ssid_hidden = false,
           .ap.max_connection = 4,
           .ap.beacon_interval = 100,
    };

    event_init();

    if (wifi_events == NULL) {
        wifi_events = xEventGroupCreate();
    }
    xEventGroupClearBits(wifi_events, 0x00ffffff);

    TEST_ESP_OK(esp_wifi_set_mode(WIFI_MODE_AP));
    TEST_ESP_OK(esp_wifi_set_config(WIFI_IF_AP, &w_config));
    TEST_ESP_OK(esp_wifi_start());
    ESP_LOGI(TAG, "start wifi softap: %s", TEST_DEFAULT_SSID);
}

static void start_wifi_as_sta(void)
{

    event_init();

    if (wifi_events == NULL) {
        wifi_events = xEventGroupCreate();
    }
    xEventGroupClearBits(wifi_events, 0x00ffffff);

    TEST_ESP_OK(esp_wifi_set_mode(WIFI_MODE_STA));
    TEST_ESP_OK(esp_wifi_start());

}

static void stop_wifi(void)
{
    TEST_ESP_OK(esp_wifi_stop());
    event_deinit();
    if (wifi_events) {
        vEventGroupDelete(wifi_events);
        wifi_events = NULL;
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
}

static void receive_ds2ds_packet(void)
{
    start_wifi_as_softap();

    // wait for sender to send packets
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    stop_wifi();
}

static const char ds2ds_pdu[] = {
    0x48, 0x03, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xE8, 0x65, 0xD4, 0xCB, 0x74, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x60, 0x94, 0xE8, 0x65, 0xD4, 0xCB, 0x74, 0x1C, 0x26, 0xB9,
    0x0D, 0x02, 0x7D, 0x13, 0x00, 0x00, 0x01, 0xE8, 0x65, 0xD4, 0xCB, 0x74,
    0x1C, 0x00, 0x00, 0x26, 0xB9, 0x00, 0x00, 0x00, 0x00
};

static void send_ds2ds_packet(void)
{
    start_wifi_as_softap();

    // send packet 20 times to make sure receiver will get this packet
    for (uint16_t i = 0; i < 20; i++) {
        esp_wifi_80211_tx(WIFI_IF_AP, ds2ds_pdu, sizeof(ds2ds_pdu), true);
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
    stop_wifi();
}

TEST_CASE_MULTIPLE_DEVICES("send receive ds2ds packet without exception", "[wifi]", receive_ds2ds_packet, send_ds2ds_packet);

static void wifi_connect(void)
{
    EventBits_t bits;

    wifi_config_t w_config = {
        .sta.ssid = TEST_DEFAULT_SSID,
        .sta.password = TEST_DEFAULT_PWD,
    };

    TEST_ESP_OK(esp_wifi_set_config(WIFI_IF_STA, &w_config));
    TEST_ESP_OK(esp_wifi_connect());
    ESP_LOGI(TAG, "start esp_wifi_connect: %s", TEST_DEFAULT_SSID);
    bits = xEventGroupWaitBits(wifi_events, GOT_IP_EVENT, 1, 0, CONNECT_TIMEOUT_MS / portTICK_PERIOD_MS);
    TEST_ASSERT(bits & GOT_IP_EVENT);
}

static void test_wifi_connection_sta(void)
{
    EventBits_t bits;
    start_wifi_as_sta();

    // make sure softap has started
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    wifi_connect();

    // do not auto reconnect after connected
    xEventGroupClearBits(wifi_events, WIFI_DISCONNECT_EVENT);
    wifi_event_handler_flag |= EVENT_HANDLER_FLAG_DO_NOT_AUTO_RECONNECT;

    // disconnect event not triggered after 60s
    bits = xEventGroupWaitBits(wifi_events, WIFI_DISCONNECT_EVENT, 1, 0, 60000 / portTICK_PERIOD_MS);
    TEST_ASSERT((bits & WIFI_DISCONNECT_EVENT) == 0);

    stop_wifi();
}

static void test_wifi_connection_softap(void)
{
    EventBits_t bits;
    start_wifi_as_softap();

    // wait station connected
    bits = xEventGroupWaitBits(wifi_events, WIFI_AP_STA_CONNECTED, 1, 0, CONNECT_TIMEOUT_MS / portTICK_PERIOD_MS);
    TEST_ASSERT(bits & WIFI_AP_STA_CONNECTED);

    // wait 70s (longer than station side)
    vTaskDelay((60000 + CONNECT_TIMEOUT_MS) / portTICK_PERIOD_MS);

    stop_wifi();
}

TEST_CASE_MULTIPLE_DEVICES("test wifi retain connection for 60s", "[wifi][timeout=90]", test_wifi_connection_sta, test_wifi_connection_softap);

// single core have issue as WIFIBUG-92
#if !CONFIG_FREERTOS_UNICORE
static void esp_wifi_connect_first_time(void)
{
    start_wifi_as_sta();
    // make sure softap has started
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    wifi_config_t w_config;
    memset(&w_config, 0, sizeof(w_config));
    memcpy(w_config.sta.ssid, TEST_DEFAULT_SSID, strlen(TEST_DEFAULT_SSID));
    memcpy(w_config.sta.password, TEST_DEFAULT_PWD, strlen(TEST_DEFAULT_PWD));
    w_config.sta.channel = 1;

    wifi_event_handler_flag |= EVENT_HANDLER_FLAG_DO_NOT_AUTO_RECONNECT;

    TEST_ESP_OK(esp_wifi_set_config(WIFI_IF_STA, &w_config));
    ESP_LOGI(TAG, "start esp_wifi_connect first time: %s", TEST_DEFAULT_SSID);
    TEST_ESP_OK(esp_wifi_connect());
}

static void test_wifi_connect_at_scan_phase(void)
{

    esp_wifi_connect_first_time();

    // connect when first connect in scan
    vTaskDelay(300 / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "connect when first connect in scan");
    TEST_ESP_ERR(ESP_ERR_WIFI_CONN, esp_wifi_connect());
    wifi_event_handler_flag |= EVENT_HANDLER_FLAG_DO_NOT_AUTO_RECONNECT;

    stop_wifi();
}

static void test_wifi_connect_before_connected_phase(void)
{

    esp_wifi_connect_first_time();

    // connect before connected
    vTaskDelay(730 / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "connect when first connect after scan before connected");
    TEST_ESP_ERR(ESP_ERR_WIFI_CONN, esp_wifi_connect());
    wifi_event_handler_flag |= EVENT_HANDLER_FLAG_DO_NOT_AUTO_RECONNECT;

    stop_wifi();
}

static void test_wifi_connect_after_connected_phase(void)
{
    EventBits_t bits;

    start_wifi_as_sta();
    wifi_event_handler_flag = 0;
    wifi_connect();
    xEventGroupClearBits(wifi_events, WIFI_STA_CONNECTED | WIFI_DISCONNECT_EVENT);
    ESP_LOGI(TAG, "connect after connected");
    TEST_ESP_OK(esp_wifi_connect());
    bits = xEventGroupWaitBits(wifi_events, WIFI_STA_CONNECTED | WIFI_DISCONNECT_EVENT, pdTRUE, pdFALSE, CONNECT_TIMEOUT_MS / portTICK_PERIOD_MS);
    // shouldn't reconnect
    TEST_ASSERT((bits & WIFI_AP_STA_CONNECTED) == 0);
    // shouldn't disconnect
    TEST_ASSERT((bits & WIFI_DISCONNECT_EVENT) == 0);

    wifi_event_handler_flag |= EVENT_HANDLER_FLAG_DO_NOT_AUTO_RECONNECT;

    stop_wifi();
}

static void set_wifi_softap(void)
{
    start_wifi_as_softap();

    // wait for sta connect
    vTaskDelay(20000 / portTICK_PERIOD_MS);
    stop_wifi();
}

TEST_CASE_MULTIPLE_DEVICES("test wifi connect at scan", "[wifi]", test_wifi_connect_at_scan_phase, set_wifi_softap);
TEST_CASE_MULTIPLE_DEVICES("test wifi connect before connected", "[wifi]", test_wifi_connect_before_connected_phase, set_wifi_softap);
TEST_CASE_MULTIPLE_DEVICES("test wifi connect after connected", "[wifi]", test_wifi_connect_after_connected_phase, set_wifi_softap);
#endif
