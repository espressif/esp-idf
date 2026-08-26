/*
 * SPDX-FileCopyrightText: 2020 Bose Corporation
 * SPDX-FileCopyrightText: 2020-2024 Nordic Semiconductor ASA
 * SPDX-FileCopyrightText: 2022 Codecoup
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include "tmap_bmr.h"

static esp_ble_audio_vcp_included_t vcp_included;

static void vcs_state_cb(esp_ble_conn_t *conn, int err, uint8_t volume, uint8_t mute)
{
    if (err) {
        ESP_LOGE(TAG, "Failed to get VCS state, err %d", err);
    } else {
        ESP_LOGI(TAG, "VCS volume %u, mute %u", volume, mute);
    }
}

static void vcs_flags_cb(esp_ble_conn_t *conn, int err, uint8_t flags)
{
    if (err) {
        ESP_LOGE(TAG, "Failed to get VCS flags, err %d", err);
    } else {
        ESP_LOGI(TAG, "VCS flags 0x%02x", flags);
    }
}

static esp_ble_audio_vcp_vol_rend_cb_t vcp_cbs = {
    .state = vcs_state_cb,
    .flags = vcs_flags_cb,
};

int vcp_vol_renderer_init(void)
{
    esp_ble_audio_vcp_vol_rend_register_param_t vcp_register_param = {0};
    int err;

    vcp_register_param.step = 1;
    vcp_register_param.mute = ESP_BLE_AUDIO_VCP_STATE_UNMUTED;
    vcp_register_param.volume = 10;
    vcp_register_param.cb = &vcp_cbs;

    err = esp_ble_audio_vcp_vol_rend_register(&vcp_register_param);
    if (err) {
        ESP_LOGE(TAG, "Failed to register VCP renderer, err %d", err);
        return err;
    }

    err = esp_ble_audio_vcp_vol_rend_included_get(&vcp_included);
    if (err) {
        ESP_LOGE(TAG, "Failed to get VCP renderer included service, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "VCP volume renderer initialized");

    return 0;
}
