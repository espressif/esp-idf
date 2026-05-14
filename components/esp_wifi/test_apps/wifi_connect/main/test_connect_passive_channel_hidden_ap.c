/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * Tests station association (L2 only; no DHCP / GOT_IP) to a hidden SoftAP when passive-hidden-AP support is enabled.
 *
 * Not in default wifi_connect builds: enable CONFIG_ESP_WIFI_PASSIVE_HIDDEN_AP_SUPPORT only via
 * sdkconfig.ci.passive_hidden_ap (pytest config "passive_hidden_ap"), or merge that file in
 * SDKCONFIG_DEFAULTS for local builds.
 *
 * Wi-Fi remote: CONFIG_WIFI_RMT_PASSIVE_HIDDEN_AP_SUPPORT maps to the same option.
 */

#include "sdkconfig.h"

#if CONFIG_ESP_WIFI_PASSIVE_HIDDEN_AP_SUPPORT

#include <string.h>
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_wifi_default.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "test_utils.h"
#include "freertos/event_groups.h"
#include "soc/soc_caps.h"

#ifndef TEST_SUFFIX_STR
#define TEST_SUFFIX_STR "_0000"
#endif

#define TEST_DEFAULT_SSID "SSID_" CONFIG_IDF_TARGET TEST_SUFFIX_STR
#define TEST_DEFAULT_PWD "PASS_" CONFIG_IDF_TARGET TEST_SUFFIX_STR

/* 2.4 GHz passive scan scenario: channel 12 with CN regulatory (see apply_country_auto). */
#define TEST_PASSIVE_HIDDEN_AP_CHANNEL (12)

#define CONNECT_TIMEOUT_MS   (30000)
#define MAXIMUM_RETRY            (5)

#define WIFI_DISCONNECT_EVENT    (1 << 1)
#define WIFI_STA_CONNECTED       (1 << 2)
#define WIFI_AP_STA_CONNECTED    (1 << 3)

static const char *TAG = "test_passive_hidden_ap";

static EventGroupHandle_t s_wifi_events;
static esp_netif_t *s_sta_netif;
static esp_netif_t *s_ap_netif;
static int s_retry_num;
static bool s_sta_conn_first;

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id) {
    case WIFI_EVENT_AP_STACONNECTED:
        ESP_LOGI(TAG, "WIFI_EVENT_AP_STACONNECTED");
        if (s_wifi_events) {
            xEventGroupSetBits(s_wifi_events, WIFI_AP_STA_CONNECTED);
        }
        break;
    case WIFI_EVENT_STA_CONNECTED:
        ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED");
        if (s_wifi_events) {
            xEventGroupSetBits(s_wifi_events, WIFI_STA_CONNECTED);
            s_sta_conn_first = true;
        }
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");
        if (s_wifi_events) {
            if ((s_retry_num < MAXIMUM_RETRY) && !s_sta_conn_first) {
                s_retry_num++;
                ESP_LOGI(TAG, "retry connect to hidden passive-channel AP (%d/%d)", s_retry_num, MAXIMUM_RETRY);
                esp_wifi_connect();
            } else {
                xEventGroupSetBits(s_wifi_events, WIFI_DISCONNECT_EVENT);
            }
        }
        break;
    default:
        break;
    }
}

static esp_err_t event_init_sta_side(void)
{
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    s_sta_netif = esp_netif_create_default_wifi_sta();
    return ESP_OK;
}

static esp_err_t event_deinit_sta_side(void)
{
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler));
    esp_netif_destroy_default_wifi(s_sta_netif);
    s_sta_netif = NULL;
    ESP_ERROR_CHECK(esp_event_loop_delete_default());
    return ESP_OK;
}

static esp_err_t event_init_ap_side(void)
{
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    s_ap_netif = esp_netif_create_default_wifi_ap();
    return ESP_OK;
}

static esp_err_t event_deinit_ap_side(void)
{
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler));
    esp_netif_destroy_default_wifi(s_ap_netif);
    s_ap_netif = NULL;
    ESP_ERROR_CHECK(esp_event_loop_delete_default());
    return ESP_OK;
}

static void apply_country_auto(void)
{
    wifi_country_t country = {0};
    memcpy(country.cc, "CN", 2);
    country.schan = 1;
    country.nchan = 13;
    country.policy = WIFI_COUNTRY_POLICY_AUTO;
#if CONFIG_SOC_WIFI_SUPPORT_5G
    country.wifi_5g_channel_mask = 0;
#endif
    TEST_ESP_OK(esp_wifi_set_country(&country));
}

static void start_hidden_softap_on_passive_channel(void)
{
    wifi_config_t w_config = {
        .ap.ssid = TEST_DEFAULT_SSID,
        .ap.password = TEST_DEFAULT_PWD,
        .ap.ssid_len = strlen(TEST_DEFAULT_SSID),
           .ap.channel = TEST_PASSIVE_HIDDEN_AP_CHANNEL,
           .ap.authmode = WIFI_AUTH_WPA2_PSK,
           .ap.ssid_hidden = true,
           .ap.max_connection = 4,
           .ap.beacon_interval = 100,
    };

    event_init_ap_side();
    if (s_wifi_events == NULL) {
        s_wifi_events = xEventGroupCreate();
    }
    xEventGroupClearBits(s_wifi_events, 0x00ffffff);

    apply_country_auto();
    TEST_ESP_OK(esp_wifi_set_mode(WIFI_MODE_AP));
    TEST_ESP_OK(esp_wifi_set_config(WIFI_IF_AP, &w_config));
    TEST_ESP_OK(esp_wifi_start());
    ESP_LOGI(TAG, "hidden softap on channel %u, ssid (hidden): %s", (unsigned)TEST_PASSIVE_HIDDEN_AP_CHANNEL, TEST_DEFAULT_SSID);
}

static void start_sta(void)
{
    event_init_sta_side();
    if (s_wifi_events == NULL) {
        s_wifi_events = xEventGroupCreate();
    }
    xEventGroupClearBits(s_wifi_events, 0x00ffffff);

    apply_country_auto();
    TEST_ESP_OK(esp_wifi_set_mode(WIFI_MODE_STA));
    TEST_ESP_OK(esp_wifi_start());
}

static void stop_wifi_sta_side(void)
{
    TEST_ESP_OK(esp_wifi_stop());
    event_deinit_sta_side();
    if (s_wifi_events) {
        vEventGroupDelete(s_wifi_events);
        s_wifi_events = NULL;
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
}

static void stop_wifi_ap_side(void)
{
    TEST_ESP_OK(esp_wifi_stop());
    event_deinit_ap_side();
    if (s_wifi_events) {
        vEventGroupDelete(s_wifi_events);
        s_wifi_events = NULL;
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
}

static void sta_connect_hidden_on_passive_channel(void)
{
    wifi_config_t w_config = { 0 };
    strncpy((char *)w_config.sta.ssid, TEST_DEFAULT_SSID, sizeof(w_config.sta.ssid));
    strncpy((char *)w_config.sta.password, TEST_DEFAULT_PWD, sizeof(w_config.sta.password));
    w_config.sta.channel = TEST_PASSIVE_HIDDEN_AP_CHANNEL;

    TEST_ESP_OK(esp_wifi_set_config(WIFI_IF_STA, &w_config));
    s_retry_num = 0;
    s_sta_conn_first = false;
    TEST_ESP_OK(esp_wifi_connect());
    ESP_LOGI(TAG, "esp_wifi_connect hidden AP, channel hint %u", (unsigned)TEST_PASSIVE_HIDDEN_AP_CHANNEL);

    EventBits_t bits = xEventGroupWaitBits(s_wifi_events,
                                           WIFI_STA_CONNECTED | WIFI_DISCONNECT_EVENT,
                                           pdTRUE,
                                           pdFALSE,
                                           pdMS_TO_TICKS(CONNECT_TIMEOUT_MS));
    TEST_ASSERT_MESSAGE(bits & WIFI_STA_CONNECTED, "STA did not connect (hidden passive-channel path, L2 only)");
}

static void test_passive_hidden_channel_sta(void)
{
    start_sta();
    vTaskDelay(pdMS_TO_TICKS(1000));
    sta_connect_hidden_on_passive_channel();
    stop_wifi_sta_side();
}

static void test_passive_hidden_channel_softap(void)
{
    start_hidden_softap_on_passive_channel();

    EventBits_t bits = xEventGroupWaitBits(s_wifi_events,
                                           WIFI_AP_STA_CONNECTED,
                                           pdFALSE,
                                           pdFALSE,
                                           pdMS_TO_TICKS(CONNECT_TIMEOUT_MS));
    TEST_ASSERT_MESSAGE(bits & WIFI_AP_STA_CONNECTED, "No station connected to hidden softap in time");

    stop_wifi_ap_side();
}

TEST_CASE_MULTIPLE_DEVICES("connect passive channel hidden softap",
                           "[wifi][group=passive_hidden_ap]",
                           test_passive_hidden_channel_sta,
                           test_passive_hidden_channel_softap);

#endif /* CONFIG_ESP_WIFI_PASSIVE_HIDDEN_AP_SUPPORT */
