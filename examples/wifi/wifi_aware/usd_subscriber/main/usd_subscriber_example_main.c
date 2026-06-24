/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_nan.h"
#include "esp_mac.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_check.h"
#include "nvs_flash.h"

#define MAC_ADDR_LEN 6

#define EXAMPLE_USD_SVC_NAME CONFIG_ESP_WIFI_USD_SUB_SVC_NAME
#define EXAMPLE_USD_TTL CONFIG_ESP_WIFI_USD_SUB_TTL
#define EXAMPLE_USD_DEFAULT_SUB_CHANNEL CONFIG_ESP_WIFI_USD_DEFAULT_CHAN
#define EXAMPLE_USD_SVC_INFO CONFIG_ESP_WIFI_USD_SVC_SPECIFIC_INFO

static EventGroupHandle_t s_nan_event_group;
static int NAN_RECEIVE = BIT0;
static int NAN_SRV_MATCH = BIT1;
const char *TAG = "usd_subscriber";
uint8_t g_peer_inst_id;
uint8_t g_peer_mac[MAC_ADDR_LEN];
uint8_t g_subscribe_id;

static void nan_svc_match_event_handler(void *arg, esp_event_base_t event_base,
                                      int32_t event_id, void *event_data)
{
    wifi_event_nan_svc_match_t *evt = (wifi_event_nan_svc_match_t *)event_data;
    if (evt == NULL) {
        return;
    }
    g_peer_inst_id = evt->publish_id;
    memcpy(g_peer_mac, evt->pub_if_mac, MAC_ADDR_LEN);
    ESP_LOGI(TAG, "Service matched with peer_id %d peer mac "MACSTR"", evt->publish_id, MAC2STR(evt->pub_if_mac));
    if (evt->ssi_len) {
        ESP_LOGI(TAG, "SSI -");
        ESP_LOG_BUFFER_HEXDUMP(TAG, evt->ssi,
                evt->ssi_len, ESP_LOG_INFO);
    } else {
        ESP_LOGI(TAG, "SSI - [NULL]");
    }
    xEventGroupSetBits(s_nan_event_group, NAN_SRV_MATCH);
}

static void nan_receive_event_handler(void *arg, esp_event_base_t event_base,
                                      int32_t event_id, void *event_data)
{
    wifi_event_nan_receive_t *evt = (wifi_event_nan_receive_t *)event_data;
    if (evt == NULL) {
        return;
    }

    ESP_LOGI(TAG, "Received Follow-up message from peer with ID %d and MAC "MACSTR"",
                evt->peer_inst_id, MAC2STR(evt->peer_if_mac));
    if (evt->ssi_len) {
        ESP_LOG_BUFFER_HEXDUMP(TAG, evt->ssi, evt->ssi_len, ESP_LOG_INFO);
    } else {
        ESP_LOGI(TAG, "SSI - [NULL]");
    }
    xEventGroupSetBits(s_nan_event_group, NAN_RECEIVE);
    esp_wifi_nan_cancel_service(g_subscribe_id);
}

void wifi_usd_subscribe(void)
{
    s_nan_event_group = xEventGroupCreate();
    esp_event_handler_instance_t instance_any_id;

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                    WIFI_EVENT_NAN_SVC_MATCH,
                    &nan_svc_match_event_handler,
                    NULL,
                    &instance_any_id));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                    WIFI_EVENT_NAN_RECEIVE,
                    &nan_receive_event_handler,
                    NULL,
                    &instance_any_id));

    ESP_RETURN_VOID_ON_ERROR(esp_wifi_start(), TAG, "NAN-USD failed to start Wi-Fi");

    /* Start NAN-USD Discovery */
    ESP_RETURN_VOID_ON_ERROR(esp_wifi_nan_usd_start(), TAG, "NAN-USD initialization failed");

    wifi_nan_subscribe_cfg_t subscribe_cfg = {
        .service_name = EXAMPLE_USD_SVC_NAME,
        .ttl = EXAMPLE_USD_TTL,
        .ssi = NULL,
        .ssi_len = 0,
        .usd_discovery_flag = 1,
        .usd_subscribe_config = esp_wifi_usd_get_default_subscribe_cfg(),
    };
    subscribe_cfg.usd_subscribe_config.usd_default_channel = EXAMPLE_USD_DEFAULT_SUB_CHANNEL;

    g_subscribe_id = esp_wifi_nan_subscribe_service(&subscribe_cfg);
    if (g_subscribe_id == 0) {
        ESP_LOGE(TAG, "Subscribing to %s failed", subscribe_cfg.service_name);
        return;
    }

    EventBits_t bits = xEventGroupWaitBits(s_nan_event_group, NAN_SRV_MATCH, pdTRUE, pdFALSE, pdMS_TO_TICKS(EXAMPLE_USD_TTL*1000));
    if (bits & NAN_SRV_MATCH) {
        wifi_nan_followup_params_t fup_params = {
            .inst_id = g_subscribe_id,
            .peer_inst_id = g_peer_inst_id,
            .ssi = (uint8_t *)EXAMPLE_USD_SVC_INFO,
            .ssi_len = strlen(EXAMPLE_USD_SVC_INFO),
        };
        memcpy(fup_params.peer_mac, g_peer_mac, MAC_ADDR_LEN);

        ESP_LOGI(TAG, "Sending message to the peer with ID %d", g_peer_inst_id);
        if (fup_params.ssi_len) {
            ESP_LOG_BUFFER_HEXDUMP(TAG, fup_params.ssi, fup_params.ssi_len, ESP_LOG_INFO);
        } else {
            ESP_LOGI(TAG, "SSI - [NULL]");
        }
        ESP_RETURN_VOID_ON_ERROR(esp_wifi_nan_send_message(&fup_params), TAG, "Sending message to the peer with ID %d failed!", g_peer_inst_id);
    }
    return;
}

void initialise_wifi(void)
{
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    initialise_wifi();
    wifi_usd_subscribe();

    return;
}
