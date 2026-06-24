/*
 * SPDX-FileCopyrightText: 2022 Codecoup
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>

#include "tmap_peripheral.h"

#define CSIP_SIRK_DEBUG     { 0xcd, 0xcc, 0x72, 0xdd, 0x86, 0x8c, 0xcd, 0xce, \
                              0x22, 0xfd, 0xa1, 0x21, 0x09, 0x7d, 0x7d, 0x45 }

static esp_ble_audio_csip_set_member_svc_inst_t *svc_inst;

static void csip_lock_changed_cb(esp_ble_conn_t *conn,
                                 esp_ble_audio_csip_set_member_svc_inst_t *inst,
                                 bool locked)
{
    ESP_LOGI(TAG, "Client %p %s the lock", conn, locked ? "locked" : "released");
}

static uint8_t sirk_read_req_cb(esp_ble_conn_t *conn,
                                esp_ble_audio_csip_set_member_svc_inst_t *inst)
{
    return ESP_BLE_AUDIO_CSIP_READ_SIRK_REQ_RSP_ACCEPT;
}

static esp_ble_audio_csip_set_member_cb_t csip_cb = {
    .lock_changed  = csip_lock_changed_cb,
    .sirk_read_req = sirk_read_req_cb,
};

int csip_set_member_init(void)
{
    esp_ble_audio_csip_set_member_register_param_t param = {
        .set_size = 2,
        .rank     = CONFIG_EXAMPLE_TMAP_PER_SET_RANK,
        .lockable = false,
        .sirk     = CSIP_SIRK_DEBUG,
        .cb       = &csip_cb,
    };
    int err;

    err = esp_ble_audio_cap_acceptor_register(&param, &svc_inst);
    if (err) {
        ESP_LOGE(TAG, "Failed to register cap acceptor, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "CSIP set member initialized");

    return 0;
}

int csip_generate_rsi(uint8_t rsi[ESP_BLE_AUDIO_CSIP_RSI_SIZE])
{
    char rsi_str[13];
    int err;

    if (svc_inst == NULL) {
        ESP_LOGE(TAG, "CSIS service instant not exists");
        return -1;
    }

    err = esp_ble_audio_csip_set_member_generate_rsi(svc_inst, rsi);
    if (err) {
        ESP_LOGE(TAG, "Failed to generate RSI, err %d", err);
        return err;
    }

    snprintf(rsi_str, ARRAY_SIZE(rsi_str), "%02x%02x%02x%02x%02x%02x",
             rsi[0], rsi[1], rsi[2], rsi[3], rsi[4], rsi[5]);

    ESP_LOGI(TAG, "PRSI: 0x%s", rsi_str);

    return 0;
}

void *csip_svc_inst_get(void)
{
    return svc_inst;
}
