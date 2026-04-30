/*
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_ble_audio_tmap_api.h"

#if CONFIG_BT_TMAP
esp_err_t esp_ble_audio_tmap_register(esp_ble_audio_tmap_role_t role)
{
    int err;

    err = bt_tmap_register_safe(role);
    if (err) {
        return ESP_FAIL;
    }

#if BLE_AUDIO_SVC_SEP_ADD
    err = bt_le_tmas_init();
    if (err) {
        return ESP_FAIL;
    }
#endif /* BLE_AUDIO_SVC_SEP_ADD */

    return ESP_OK;
}

esp_err_t esp_ble_audio_tmap_discover(uint16_t conn_handle,
                                      const esp_ble_audio_tmap_cb_t *tmap_cb)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_tmap_discover(conn, tmap_cb);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

void esp_ble_audio_tmap_set_role(esp_ble_audio_tmap_role_t role)
{
    bt_tmap_set_role_safe(role);
}
#endif /* CONFIG_BT_TMAP */
