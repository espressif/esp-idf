/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifdef CONFIG_ESP_WIFI_BSS_MAX_IDLE_SUPPORT

#include <string.h>
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "test_utils.h"
#include "freertos/event_groups.h"
#include "unity_test_utils.h"
#include "esp_private/wifi.h"

#ifdef __CHECKER__
#define __force __attribute__((force))
#define __bitwise __attribute__((bitwise))
#else
#define __force
#define __bitwise
#endif

#if defined(__linux__) || defined(__GLIBC__)
#include <endian.h>
#include <byteswap.h>
#else
#include <machine/endian.h>
#define __BYTE_ORDER     BYTE_ORDER
#define __LITTLE_ENDIAN  LITTLE_ENDIAN
#endif /* __linux__ */
#ifndef __BYTE_ORDER
#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN 1234
#endif
#endif

typedef uint16_t u16;
typedef uint8_t u8;
typedef u16 __bitwise be16;

#define host_to_be16(n) ((__force be16) __builtin_bswap16((n)))

#ifndef TEST_SUFFIX_STR
#define TEST_SUFFIX_STR "_0000"
#endif

#define TEST_DEFAULT_SSID "SSID_" CONFIG_IDF_TARGET TEST_SUFFIX_STR
#define TEST_DEFAULT_PWD "PASS_" CONFIG_IDF_TARGET TEST_SUFFIX_STR
#define TEST_DEFAULT_CHANNEL (6)
#define CONNECT_TIMEOUT_MS   (8000)
#define MAXIMUM_RETRY (5)

#define WIFI_DISCONNECT_EVENT    (1)
#define WIFI_STA_CONNECTED       (1<<1)
#define WIFI_AP_STA_CONNECTED    (1<<2)
#define WIFI_FAIL                (1<<3)
#define EMPH_STR(s) "****** "s" ******"

#define MAX_IDLE_PERIOD      (5)
#define ETHTYPE_IP            0x0800

static const char* TAG = "test_bss_max_idle";

static bool s_sta_conn_first = false;
static bool s_keep_alive_received = false;
static int s_retry_num = 0;
static EventGroupHandle_t wifi_events;

struct eth_header {
    u8 dest_mac[6];
    u8 src_mac[6];
    be16 ethertype;
};

static esp_err_t wifi_ap_receive_test(void *buffer, uint16_t len, void *eb)
{
    struct eth_header *pac = (struct eth_header *)buffer;
    if ((host_to_be16(pac->ethertype) == ETHTYPE_IP) && (len < 48)) {
        ESP_LOGI(TAG, "KEEP ALIVE RECEIVED");
        s_keep_alive_received = true;
        esp_wifi_deauth_sta(0);
    }
    esp_wifi_internal_free_rx_buffer(eb);
    return ESP_OK;
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    ESP_LOGI(TAG, "wifi event handler: %"PRIi32, event_id);
    switch (event_id) {
    case WIFI_EVENT_AP_START:
        ESP_LOGI(TAG, "WIFI_EVENT_AP_START");
        esp_wifi_internal_reg_rxcb(WIFI_IF_AP, wifi_ap_receive_test);
        break;
    case WIFI_EVENT_STA_START:
        ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
        break;
    case WIFI_EVENT_AP_STACONNECTED:
        ESP_LOGI(TAG, "Wi-Fi AP got a station connected");
        break;
    case WIFI_EVENT_STA_CONNECTED:
        ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED");
        if (wifi_events) {
            xEventGroupSetBits(wifi_events, WIFI_STA_CONNECTED);
            s_sta_conn_first = true;
        }
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        if ((s_retry_num < MAXIMUM_RETRY) && !s_sta_conn_first) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to AP");
        } else {
            xEventGroupSetBits(wifi_events, WIFI_DISCONNECT_EVENT);
            ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");
            wifi_event_sta_disconnected_t *event = (wifi_event_sta_disconnected_t *)event_data;
            ESP_LOGI(TAG, "disconnect reason: %u", event->reason);
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
    return ESP_OK;
}

static esp_err_t event_deinit(void)
{
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler));
    ESP_ERROR_CHECK(esp_event_loop_delete_default());
    return ESP_OK;
}

static void start_wifi_as_softap(void)
{
    wifi_config_t w_config = {
        .ap.ssid = TEST_DEFAULT_SSID,
        .ap.password = TEST_DEFAULT_PWD,
        .ap.ssid_len = strlen(TEST_DEFAULT_SSID),
           .ap.channel = TEST_DEFAULT_CHANNEL,
           .ap.authmode = WIFI_AUTH_WPA3_PSK,
           .ap.ssid_hidden = false,
           .ap.max_connection = 4,
           .ap.beacon_interval = 100,
        .ap.bss_max_idle_cfg = {
            .period = MAX_IDLE_PERIOD,
            .protected_keep_alive = true,
        },
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

static void wifi_connect(void)
{
    wifi_config_t w_config = {
        .sta.ssid = TEST_DEFAULT_SSID,
        .sta.password = TEST_DEFAULT_PWD,
        .sta.channel = TEST_DEFAULT_CHANNEL,
    };

    TEST_ESP_OK(esp_wifi_set_config(WIFI_IF_STA, &w_config));
    TEST_ESP_OK(esp_wifi_connect());
    ESP_LOGI(TAG, "start esp_wifi_connect: %s", TEST_DEFAULT_SSID);
}

static void test_bss_max_idle_sta(void)
{
    start_wifi_as_sta();
    wifi_connect();
    // Waiting untill connection est or failed
    EventBits_t bits = xEventGroupWaitBits(wifi_events,
                                           WIFI_STA_CONNECTED | WIFI_FAIL,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);
    TEST_ASSERT(bits & WIFI_STA_CONNECTED);
    if (bits != WIFI_FAIL) {
        bits = xEventGroupWaitBits(wifi_events,
                                   WIFI_DISCONNECT_EVENT,
                                   pdFALSE,
                                   pdFALSE,
                                   portMAX_DELAY);
    }
    stop_wifi();
}

static void test_bss_max_idle_softap(void)
{
    start_wifi_as_softap();

    vTaskDelay((CONNECT_TIMEOUT_MS + MAX_IDLE_PERIOD * 1000) / portTICK_PERIOD_MS);

    TEST_ASSERT(s_keep_alive_received);
    stop_wifi();
}

TEST_CASE_MULTIPLE_DEVICES("Connection with bss max idle enabled", "[wifi][bss max idle]",  test_bss_max_idle_sta, test_bss_max_idle_softap);

#endif /* CONFIG_ESP_WIFI_BSS_MAX_IDLE_SUPPORT */
