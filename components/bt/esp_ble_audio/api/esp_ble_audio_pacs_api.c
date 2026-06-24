/*
 * SPDX-FileCopyrightText: 2021 Intel Corporation
 * SPDX-FileCopyrightText: 2021-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_ble_audio_pacs_api.h"

#if CONFIG_BT_PACS
esp_err_t esp_ble_audio_pacs_register(const esp_ble_audio_pacs_register_param_t *param)
{
    int err;

    if (param == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_pacs_register_safe(param);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_pacs_unregister(void)
{
    int err;

    err = bt_pacs_unregister_safe();
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

static bool dir_is_valid(esp_ble_audio_dir_t dir)
{
    if (dir != ESP_BLE_AUDIO_DIR_SINK &&
            dir != ESP_BLE_AUDIO_DIR_SOURCE) {
        return false;
    }

    return true;
}

esp_err_t esp_ble_audio_pacs_cap_register(esp_ble_audio_dir_t dir,
                                          esp_ble_audio_pacs_cap_t *cap)
{
    int err;

    if (dir_is_valid(dir) == false ||
            cap == NULL || cap->codec_cap == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_pacs_cap_register_safe(dir, cap);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_pacs_cap_unregister(esp_ble_audio_dir_t dir,
                                            esp_ble_audio_pacs_cap_t *cap)
{
    int err;

    if (dir_is_valid(dir) == false || cap == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_pacs_cap_unregister_safe(dir, cap);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_pacs_set_location(esp_ble_audio_dir_t dir,
                                          esp_ble_audio_location_t location)
{
    int err;

    if (dir_is_valid(dir) == false) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_pacs_set_location_safe(dir, location);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_pacs_set_available_contexts(esp_ble_audio_dir_t dir,
                                                    esp_ble_audio_context_t contexts)
{
    int err;

    if (dir_is_valid(dir) == false) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_pacs_set_available_contexts_safe(dir, contexts);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_ble_audio_context_t esp_ble_audio_pacs_get_available_contexts(esp_ble_audio_dir_t dir)
{
    if (dir_is_valid(dir) == false) {
        return ESP_BLE_AUDIO_CONTEXT_TYPE_NONE;
    }

    return bt_pacs_get_available_contexts_safe(dir);
}

esp_err_t esp_ble_audio_pacs_conn_set_available_contexts_for_conn(uint16_t conn_handle,
                                                                  esp_ble_audio_dir_t dir,
                                                                  esp_ble_audio_context_t *contexts)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    /* Note: contexts could be NULL */

    if (dir_is_valid(dir) == false) {
        return ESP_ERR_INVALID_ARG;
    }

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_pacs_conn_set_available_contexts_for_conn(conn, dir, contexts);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_ble_audio_context_t esp_ble_audio_pacs_get_available_contexts_for_conn(uint16_t conn_handle,
                                                                           esp_ble_audio_dir_t dir)
{
    esp_ble_audio_context_t ret = ESP_BLE_AUDIO_CONTEXT_TYPE_NONE;
    void *conn;

    if (dir_is_valid(dir) == false) {
        return ESP_BLE_AUDIO_CONTEXT_TYPE_NONE;
    }

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        goto unlock;
    }

    ret = bt_pacs_get_available_contexts_for_conn(conn, dir);

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_pacs_set_supported_contexts(esp_ble_audio_dir_t dir,
                                                    esp_ble_audio_context_t contexts)
{
    int err;

    if (dir_is_valid(dir) == false) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_pacs_set_supported_contexts_safe(dir, contexts);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_PACS */
