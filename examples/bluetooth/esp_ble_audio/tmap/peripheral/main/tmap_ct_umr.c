/*
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <errno.h>

#include "tmap_peripheral.h"

static bool peer_is_cg;
static bool peer_is_ums;

bool tmap_peer_is_cg(void)
{
    return peer_is_cg;
}

bool tmap_peer_is_ums(void)
{
    return peer_is_ums;
}

static void tmap_discovery_complete(esp_ble_audio_tmap_role_t peer_role,
                                    esp_ble_conn_t *conn, int err)
{
    if (conn->handle != default_conn_handle_get()) {
        return;
    }

    if (err) {
        ESP_LOGE(TAG, "Failed to discovery TMAS, err %d", err);
        return;
    }

    peer_is_cg = ((peer_role & ESP_BLE_AUDIO_TMAP_ROLE_CG) != 0);
    peer_is_ums = ((peer_role & ESP_BLE_AUDIO_TMAP_ROLE_UMS) != 0);

    ESP_LOGI(TAG, "TMAP discovery done");

    /* TMAP related GATT procedures done, start MCP discovery */
    ccp_discover_tbs();
}

static esp_ble_audio_tmap_cb_t tmap_callbacks = {
    .discovery_complete = tmap_discovery_complete,
};

int tmap_ct_umr_init(void)
{
    int err;

    err = esp_ble_audio_tmap_register(ESP_BLE_AUDIO_TMAP_ROLE_CT | ESP_BLE_AUDIO_TMAP_ROLE_UMR);
    if (err) {
        ESP_LOGE(TAG, "Failed to register TMAP role, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "TMAP CT UMR initialized");

    return 0;
}

int tmap_discover_tmas(void)
{
    uint16_t conn_handle;
    int err;

    conn_handle = default_conn_handle_get();
    if (conn_handle == CONN_HANDLE_INIT) {
        ESP_LOGE(TAG, "%s, not connected", __func__);
        return -ENOTCONN;
    }

    err = esp_ble_audio_tmap_discover(conn_handle, &tmap_callbacks);
    if (err) {
        ESP_LOGE(TAG, "Failed to discover TMAS, err %d", err);
        return err;
    }

    return 0;
}
