/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include "esp_log.h"
#include "esp_event_base.h"
#include "esp_private/wifi.h"
#include "esp_smartconfig.h"
#include "smartconfig_ack.h"

/* Smartconfig events definitions */
ESP_EVENT_DEFINE_BASE(SC_EVENT);

static const char *TAG = "smartconfig";

static void handler_got_ssid_passwd(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)data;
    uint8_t ssid[33] = { 0 };
    uint8_t password[65] = { 0 };
    uint8_t cellphone_ip[4];
    esp_err_t err = ESP_OK;

    memcpy(ssid, evt->ssid, sizeof(evt->ssid));
    memcpy(password, evt->password, sizeof(evt->password));
    memcpy(cellphone_ip, evt->cellphone_ip, sizeof(evt->cellphone_ip));

    ESP_LOGD(TAG, "SSID:%s", ssid);
    ESP_LOGD(TAG, "PASSWORD:%s", password);
    ESP_LOGD(TAG, "Phone ip: %d.%d.%d.%d", cellphone_ip[0], cellphone_ip[1], cellphone_ip[2], cellphone_ip[3]);

    err = sc_send_ack_start(evt->type, evt->token, evt->cellphone_ip);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Send smartconfig ACK error: %d", err);
    }
}

esp_err_t esp_smartconfig_start(const smartconfig_start_config_t *config)
{
    esp_err_t err = ESP_OK;

    err = esp_event_handler_register(SC_EVENT, SC_EVENT_GOT_SSID_PSWD, handler_got_ssid_passwd, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Register smartconfig default event handler fail!");
        return err;
    }

    err = esp_smartconfig_internal_start(config);
    if (err != ESP_OK) {
        esp_event_handler_unregister(SC_EVENT, SC_EVENT_GOT_SSID_PSWD, handler_got_ssid_passwd);
    }
    return err;
}

esp_err_t esp_smartconfig_stop(void)
{
    esp_err_t err = ESP_OK;

    err = esp_smartconfig_internal_stop();
    if (err == ESP_OK) {
        sc_send_ack_stop();
        esp_event_handler_unregister(SC_EVENT, SC_EVENT_GOT_SSID_PSWD, handler_got_ssid_passwd);
    }
    return err;
}
