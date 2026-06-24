/*
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include "nvs_flash.h"

#include "tmap_bmr.h"

static void ext_scan_recv(esp_ble_audio_gap_app_event_t *event)
{
    bap_broadcast_scan_recv(event);
}

static void pa_sync(esp_ble_audio_gap_app_event_t *event)
{
    ESP_LOGI(TAG, "PA synced: handle %u sid %u phy %u peer %02x:%02x:%02x:%02x:%02x:%02x",
             event->pa_sync.sync_handle, event->pa_sync.sid,
             event->pa_sync.adv_phy,
             EXAMPLE_BT_ADDR_PRINT_ARGS(event->pa_sync.addr.val));

    bap_broadcast_pa_sync(event);
}

static void pa_sync_lost(esp_ble_audio_gap_app_event_t *event)
{
    ESP_LOGI(TAG, "PA sync lost: sync_handle %u reason 0x%02x",
             event->pa_sync_lost.sync_handle, event->pa_sync_lost.reason);

    bap_broadcast_pa_lost(event);
}

static void iso_gap_app_cb(esp_ble_audio_gap_app_event_t *event)
{
    switch (event->type) {
    case ESP_BLE_AUDIO_GAP_EVENT_EXT_SCAN_RECV:
        ext_scan_recv(event);
        break;
    case ESP_BLE_AUDIO_GAP_EVENT_PA_SYNC:
        pa_sync(event);
        break;
    case ESP_BLE_AUDIO_GAP_EVENT_PA_SYNC_LOST:
        pa_sync_lost(event);
        break;
    default:
        break;
    }
}

void app_main(void)
{
    esp_ble_audio_init_info_t init_info = {
        .gap_cb  = iso_gap_app_cb,
        .gatt_cb = NULL,
    };
    esp_err_t err;

    /* Initialize NVS — it is used to store PHY calibration data */
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    err = bluetooth_init();
    if (err) {
        ESP_LOGE(TAG, "Failed to initialize BLE, err %d", err);
        return;
    }

    err = app_host_init();
    if (err) {
        ESP_LOGE(TAG, "Failed to init host, err %d", err);
        return;
    }

    err = esp_ble_audio_common_init(&init_info);
    if (err) {
        ESP_LOGE(TAG, "Failed to initialize audio, err %d", err);
        return;
    }

    err = esp_ble_audio_tmap_register(ESP_BLE_AUDIO_TMAP_ROLE_BMR);
    if (err) {
        ESP_LOGE(TAG, "Failed to register TMAP, err %d", err);
        return;
    }

    err = vcp_vol_renderer_init();
    if (err) {
        return;
    }

    err = bap_broadcast_sink_init();
    if (err) {
        return;
    }

    err = esp_ble_audio_common_start(NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to start audio, err %d", err);
        return;
    }

    bap_broadcast_sink_scan();
}
