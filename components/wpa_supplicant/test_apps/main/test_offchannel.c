/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 *
 * This test code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
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

#define WIFI_START_EVENT        BIT(0)
#define WIFI_ROC_DONE_EVENT     BIT(1)
#define WIFI_ACTION_RX_EVENT    BIT(2)
#define WIFI_SCAN_DONE_EVENT    BIT(3)

#define TEST_LISTEN_CHANNEL     6

static const char *TAG = "test_offchan";
esp_netif_t *wifi_netif;
static EventGroupHandle_t wifi_event;

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    switch (event_id) {
    case WIFI_EVENT_STA_START:
        ESP_LOGI(TAG, "WIFI Started");
        xEventGroupSetBits(wifi_event, WIFI_START_EVENT);
        break;
    case WIFI_EVENT_ACTION_TX_STATUS: {
        wifi_event_action_tx_status_t *evt =
            (wifi_event_action_tx_status_t *)event_data;

        if (evt->status == WIFI_ACTION_TX_DONE) {
            ESP_LOGI(TAG, "Action Tx Successful");
        }
    }
    break;
    case WIFI_EVENT_ROC_DONE:
        ESP_LOGI(TAG, "ROC Done");
        xEventGroupSetBits(wifi_event, WIFI_ROC_DONE_EVENT);
        break;
    case WIFI_EVENT_SCAN_DONE:
        ESP_LOGI(TAG, "Scan Done");
        xEventGroupSetBits(wifi_event, WIFI_SCAN_DONE_EVENT);
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
    wifi_netif = esp_netif_create_default_wifi_sta();

    return ESP_OK;
}

static void start_wifi_as_sta(void)
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    cfg.nvs_enable = false;

    event_init();

    // can't deinit event loop, need to reset leak check
    test_utils_record_free_mem();

    if (wifi_event == NULL) {
        wifi_event = xEventGroupCreate();
    } else {
        xEventGroupClearBits(wifi_event, 0x00ffffff);
    }

    TEST_ESP_OK(esp_wifi_init(&cfg));
    TEST_ESP_OK(esp_wifi_set_mode(WIFI_MODE_STA));
    TEST_ESP_OK(esp_wifi_start());

}

static void stop_wifi(void)
{
    esp_event_loop_delete_default();
    ESP_LOGI(TAG, "Stop wifi");
    TEST_ESP_OK(esp_wifi_stop());
    TEST_ESP_OK(esp_wifi_deinit());
    esp_wifi_clear_default_wifi_driver_and_handlers(wifi_netif);
    esp_netif_destroy(wifi_netif);
    if (wifi_event) {
        vEventGroupDelete(wifi_event);
        wifi_event = NULL;
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

int dummy_rx_action(uint8_t *hdr, uint8_t *payload, size_t len, uint8_t channel)
{
    return ESP_OK;
}

static const char *frame_data = "This is a test data";

void esp_send_action_frame(uint8_t *dest_mac, const uint8_t *buf, uint32_t len,
                           uint8_t channel, uint32_t wait_time_ms)
{
    wifi_action_tx_req_t *req = os_zalloc(sizeof(*req) + len);;
    TEST_ASSERT(req != NULL);

    req->ifx = WIFI_IF_STA;
    memcpy(req->dest_mac, dest_mac, ETH_ALEN);
    req->no_ack = false;
    req->data_len = len;
    req->rx_cb = dummy_rx_action;
    req->channel = channel;
    req->sec_channel = WIFI_SECOND_CHAN_NONE;
    req->wait_time_ms = wait_time_ms;
    req->type = WIFI_OFFCHAN_TX_REQ;

    memcpy(req->data, buf, req->data_len);

    ESP_LOGI(TAG, "Action Tx - MAC:" MACSTR ", Channel-%d, WaitT-%" PRId32 "",
             MAC2STR(dest_mac), channel, wait_time_ms);

    TEST_ESP_OK(esp_wifi_action_tx_req(req));

    os_free(req);
}

/* Test that foreground Scan doesn't preempt ROC & vice versa */
TEST_CASE("Test scan and ROC simultaneously", "[Offchan]")
{
    wifi_action_rx_cb_t rx_cb = dummy_rx_action;
    EventBits_t bits;

    set_leak_threshold(6000);
    start_wifi_as_sta();

    xEventGroupWaitBits(wifi_event, WIFI_START_EVENT, 1, 0, 5000 / portTICK_PERIOD_MS);

    wifi_roc_req_t req = {0};
    req.ifx = WIFI_IF_STA;
    req.type = WIFI_ROC_REQ;
    req.channel = TEST_LISTEN_CHANNEL;
    req.wait_time_ms = 100;
    req.rx_cb = rx_cb;
    req.done_cb = NULL;

    TEST_ESP_OK(esp_wifi_remain_on_channel(&req));

    ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, false));
    bits = xEventGroupWaitBits(wifi_event, WIFI_ROC_DONE_EVENT | WIFI_SCAN_DONE_EVENT,
                               pdTRUE, pdFALSE, 5000 / portTICK_PERIOD_MS);
    TEST_ASSERT_TRUE(bits == WIFI_ROC_DONE_EVENT);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, false));

    TEST_ESP_OK(esp_wifi_remain_on_channel(&req));

    bits = xEventGroupWaitBits(wifi_event, WIFI_ROC_DONE_EVENT | WIFI_SCAN_DONE_EVENT,
                               pdTRUE, pdFALSE, 5000 / portTICK_PERIOD_MS);
    TEST_ASSERT_TRUE(bits == WIFI_SCAN_DONE_EVENT);

    stop_wifi();
}

static void test_wifi_offchan_tx(void)
{
    int i;
    char mac_str[19];
    uint8_t mac[6];

    set_leak_threshold(6000);
    start_wifi_as_sta();
    xEventGroupWaitBits(wifi_event, WIFI_START_EVENT, 1, 0, 5000 / portTICK_PERIOD_MS);

    unity_wait_for_signal_param("Listener mac", mac_str, 19);

    TEST_ASSERT_TRUE(unity_util_convert_mac_from_string(mac_str, mac));

    for (i = 0; i < 3; i++) {
        esp_send_action_frame(mac, (const uint8_t *)frame_data, strlen(frame_data),
                              TEST_LISTEN_CHANNEL, 500);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    stop_wifi();
}

static int test_rx_action(uint8_t *hdr, uint8_t *payload, size_t len, uint8_t channel)
{
    struct ieee80211_hdr *rx_hdr = (struct ieee80211_hdr *)hdr;

    ESP_LOGI(TAG, "Rxd Action Frame from " MACSTR " (Seq-%lu)", MAC2STR(rx_hdr->addr2),
             WLAN_GET_SEQ_SEQ(rx_hdr->seq_ctrl));

    if (!os_memcmp(payload, frame_data, strlen(frame_data))) {
        xEventGroupSetBits(wifi_event, WIFI_ACTION_RX_EVENT);
    }

    return ESP_OK;
}

static void test_wifi_roc(void)
{
    wifi_action_rx_cb_t rx_cb = test_rx_action;
    char mac_str[19] = {0};
    EventBits_t bits;
    uint8_t mac[6];

    set_leak_threshold(6000);
    start_wifi_as_sta();

    xEventGroupWaitBits(wifi_event, WIFI_START_EVENT, 1, 0, 5000 / portTICK_PERIOD_MS);
    TEST_ESP_OK(esp_wifi_get_mac(WIFI_IF_STA, mac));
    sprintf(mac_str, MACSTR, MAC2STR(mac));
    unity_send_signal_param("Listener mac", mac_str);

    wifi_roc_req_t req = {0};
    req.ifx = WIFI_IF_STA;
    req.type = WIFI_ROC_REQ;
    req.channel = TEST_LISTEN_CHANNEL;
    req.wait_time_ms = 10000;
    req.rx_cb = rx_cb;
    req.done_cb = NULL;
    TEST_ESP_OK(esp_wifi_remain_on_channel(&req));

    bits = xEventGroupWaitBits(wifi_event, WIFI_ROC_DONE_EVENT | WIFI_ACTION_RX_EVENT,
                               pdTRUE, pdFALSE, portMAX_DELAY);
    /* Confirm that Frame has been received successfully */
    if (bits == WIFI_ACTION_RX_EVENT) {
        /* op_id is retained from previous ROC request */
        req.type = WIFI_ROC_CANCEL;
        req.rx_cb = NULL;
        TEST_ESP_OK(esp_wifi_remain_on_channel(&req));

        vTaskDelay(1000 / portTICK_PERIOD_MS);
        stop_wifi();
    } else {
        stop_wifi();
        TEST_FAIL();
    }
}

TEST_CASE_MULTIPLE_DEVICES("test ROC and Offchannel Action Frame Tx", "[Offchan][test_env=two_duts][timeout=90]", test_wifi_roc, test_wifi_offchan_tx);
