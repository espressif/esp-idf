/*
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include "tmap_central.h"

static esp_ble_audio_vcp_vol_ctlr_t *vcp_vol_ctlr;
/* VCS control-point writes carry a change counter that must match the server's.
 * After discovery the local counter is 0 while the server may have incremented it
 * during a previous session (e.g. before the central rebooted). Read the state
 * first to sync the counter, then issue the demo mute from the state callback.
 */
static bool initial_state_read_pending;

static void vcs_discover_cb(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr, int err,
                            uint8_t vocs_count, uint8_t aics_count)
{
    if (err) {
        ESP_LOGE(TAG, "VCP discovery cb failed, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "VCP discovery done");

    initial_state_read_pending = true;
    err = esp_ble_audio_vcp_vol_ctlr_read_state(vol_ctlr);
    if (err) {
        ESP_LOGE(TAG, "Failed to read VCS state, err %d", err);
        initial_state_read_pending = false;
    }
}

static void vcs_write_cb(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr, int err)
{
    if (err) {
        ESP_LOGE(TAG, "VCP write cb failed, err %d", err);
    } else {
        ESP_LOGI(TAG, "VCP write cb done");
    }
}

static void vcs_state_cb(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr,
                         int err, uint8_t volume, uint8_t mute)
{
    if (err) {
        ESP_LOGE(TAG, "VCP state cb failed, err %d", err);
        initial_state_read_pending = false;
        return;
    }

    ESP_LOGI(TAG, "VCP state cb done, volume %u mute %u", volume, mute);

    if (initial_state_read_pending) {
        initial_state_read_pending = false;

        err = esp_ble_audio_vcp_vol_ctlr_mute(vol_ctlr);
        if (err) {
            ESP_LOGE(TAG, "Failed to send mute command, err %d", err);
        }
    }
}

static void vcs_flags_cb(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr,
                         int err, uint8_t flags)
{
    if (err) {
        ESP_LOGE(TAG, "VCP flags cb failed, err %d", err);
    } else {
        ESP_LOGI(TAG, "VCP flags cb done, flags 0x%02x", flags);
    }
}

static esp_ble_audio_vcp_vol_ctlr_cb_t vcp_cbs = {
    .discover        = vcs_discover_cb,
    .vol_down        = vcs_write_cb,
    .vol_up          = vcs_write_cb,
    .mute            = vcs_write_cb,
    .unmute          = vcs_write_cb,
    .vol_down_unmute = vcs_write_cb,
    .vol_up_unmute   = vcs_write_cb,
    .vol_set         = vcs_write_cb,
    .state           = vcs_state_cb,
    .flags           = vcs_flags_cb,
};

int vcp_vol_ctlr_discover(void)
{
    uint16_t conn_handle;
    int err;

    conn_handle = default_conn_handle_get();
    if (conn_handle == CONN_HANDLE_INIT) {
        ESP_LOGE(TAG, "%s, not connected", __func__);
        return -ENOTCONN;
    }

    err = esp_ble_audio_vcp_vol_ctlr_discover(conn_handle, &vcp_vol_ctlr);
    if (err) {
        ESP_LOGE(TAG, "Failed to discover vcp vol ctlr, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "VCP volume controller discovering");

    return 0;
}

int vcp_vol_ctlr_init(void)
{
    int err;

    err = esp_ble_audio_vcp_vol_ctlr_cb_register(&vcp_cbs);
    if (err) {
        ESP_LOGE(TAG, "Failed to register vcp vol ctlr cb, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "VCP volume controller initialized");

    return 0;
}
