/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <inttypes.h>
#include "esp_system.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "unity.h"
#include "utils/common.h"
#include "esp_wifi_types.h"
#include "esp_wifi_driver.h"
#include "memory_checks.h"
#include "common/ieee802_11_defs.h"
#include "test_utils.h"
#include "test_wpa_supplicant_common.h"
#include "sdkconfig.h"

#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
#define WIFI_CONNECTED_BIT       BIT0
#define WIFI_FAIL_BIT            BIT1
#define WIFI_AP_STA_CONNECTED    BIT2
#define CONNECT_TIMEOUT_MS   (10000)

#ifndef TEST_SUFFIX_STR
#define TEST_SUFFIX_STR "_0000"
#endif
#define TEST_DEFAULT_SSID "SSID_" CONFIG_IDF_TARGET TEST_SUFFIX_STR
#define TEST_DEFAULT_PWD "PASS_" CONFIG_IDF_TARGET TEST_SUFFIX_STR
#define TEST_DEFAULT_CHANNEL (6)

static const char *TAG = "test_ext_bss";
static esp_netif_t* s_ap_netif = NULL;
static esp_netif_t* s_sta_netif = NULL;
static EventGroupHandle_t wifi_event;
static int retry_count = 0;

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    switch (event_id) {
    case WIFI_EVENT_STA_START:
        ESP_LOGI(TAG, "WIFI Started");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        if (retry_count < 5) {
            ESP_LOGI(TAG, "retry connection");
            esp_wifi_connect();
            retry_count++;
        } else {
            xEventGroupSetBits(wifi_event, WIFI_FAIL_BIT);
        }
        break;
    case WIFI_EVENT_STA_CONNECTED:
        retry_count = 0;
        xEventGroupSetBits(wifi_event, WIFI_CONNECTED_BIT);
        break;
    case WIFI_EVENT_AP_STACONNECTED:
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
        xEventGroupSetBits(wifi_event, WIFI_AP_STA_CONNECTED);
        break;
    case WIFI_EVENT_AP_STADISCONNECTED:
        ESP_LOGI(TAG, "station leave");
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
    s_sta_netif = esp_netif_create_default_wifi_sta();
    s_ap_netif = esp_netif_create_default_wifi_ap();

    return ESP_OK;
}

static esp_err_t event_deinit(void)
{
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler));
    esp_netif_destroy_default_wifi(s_sta_netif);
    esp_netif_destroy_default_wifi(s_ap_netif);
    ESP_ERROR_CHECK(esp_event_loop_delete_default());
    return ESP_OK;
}

static void stop_wifi(void)
{
    TEST_ESP_OK(esp_wifi_stop());
    event_deinit();
    if (wifi_event) {
        vEventGroupDelete(wifi_event);
        wifi_event = NULL;
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
    TEST_ESP_OK(esp_wifi_deinit());
    vTaskDelay(1);
}

static void start_wifi_as_sta(void)
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    cfg.nvs_enable = false;
    set_leak_threshold(2000);

    event_init();

    if (wifi_event == NULL) {
        wifi_event = xEventGroupCreate();
    } else {
        xEventGroupClearBits(wifi_event, 0x00ffffff);
    }
    // Test wifi init
    TEST_ESP_OK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = TEST_DEFAULT_SSID,
            .password = TEST_DEFAULT_PWD,
        },
    };

    // Test wifi set mode and wifi start
    TEST_ESP_OK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    TEST_ESP_OK(esp_wifi_start());
}

static void test_station_connection(void)
{
    EventBits_t bits;
    start_wifi_as_sta();

    // wait for softap to start
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    TEST_ESP_OK(esp_wifi_connect());
    ESP_LOGI(TAG, "start esp_wifi_connect: %s", TEST_DEFAULT_SSID);

    bits = xEventGroupWaitBits(wifi_event, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                               pdTRUE, pdFALSE, portMAX_DELAY);
    TEST_ASSERT(bits & WIFI_CONNECTED_BIT);
    stop_wifi();
}

static void start_wifi_as_softap(void)
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    cfg.nvs_enable = false;
    set_leak_threshold(2000);

    event_init();

    if (wifi_event == NULL) {
        wifi_event = xEventGroupCreate();
    } else {
        xEventGroupClearBits(wifi_event, 0x00ffffff);
    }
    // Test wifi init
    TEST_ESP_OK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = TEST_DEFAULT_SSID,
            .password = TEST_DEFAULT_PWD,
            .channel = TEST_DEFAULT_CHANNEL,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .max_connection = 5,
        },
    };

    TEST_ESP_OK(esp_wifi_set_mode(WIFI_MODE_AP));
    TEST_ESP_OK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    TEST_ESP_OK(esp_wifi_start());
    ESP_LOGI(TAG, "start wifi softap: %s", TEST_DEFAULT_SSID);
}

static void test_softap_connection(void)
{
    EventBits_t bits;
    start_wifi_as_softap();

    // wait for station to be connected
    bits = xEventGroupWaitBits(wifi_event, WIFI_AP_STA_CONNECTED, 1, 0, CONNECT_TIMEOUT_MS / portTICK_PERIOD_MS);
    TEST_ASSERT(bits & WIFI_AP_STA_CONNECTED);

    stop_wifi();
}

/* Test that wifi starts, scans and stops normally when .bss segment is allowed to move to external memory  */
TEST_CASE_MULTIPLE_DEVICES("test wifi connection for sta and softap when ESP_ALLOW_BSS_SEG_EXTERNAL_MEMORY is enabled", "[esp_wifi][test_env=two_duts]", test_station_connection, test_softap_connection);

#endif  // CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
