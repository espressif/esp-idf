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
    esp_err_t ret = ESP_OK;
    int err;

    bt_le_host_lock();

    err = bt_tmap_register(role);
    if (err) {
        ret = ESP_FAIL;
        goto end;
    }

#if BLE_AUDIO_SVC_DEFERRED_ADD
    err = bt_le_tmas_init();
    if (err) {
        /* TODO: rollback register once lib exposes an unregister API;
         * retry will hit -EALREADY. Only reachable on GATT alloc failure.
         */
        ret = ESP_FAIL;
        goto end;
    }
#endif /* BLE_AUDIO_SVC_DEFERRED_ADD */

end:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_tmap_discover(uint16_t conn_handle,
                                      const esp_ble_audio_tmap_cb_t *tmap_cb)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    if (tmap_cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

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
