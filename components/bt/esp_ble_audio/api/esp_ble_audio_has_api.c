/*
 * SPDX-FileCopyrightText: 2022 Codecoup
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_ble_audio_has_api.h"

#if CONFIG_BT_HAS_CLIENT
esp_err_t esp_ble_audio_has_client_cb_register(const esp_ble_audio_has_client_cb_t *cb)
{
    int err;

    if (cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_has_client_cb_register_safe(cb);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_has_client_discover(uint16_t conn_handle)
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

    err = bt_has_client_discover(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_has_client_presets_read(esp_ble_audio_has_t *has,
                                                uint8_t index,
                                                uint8_t max_count)
{
    int err;

    if (has == NULL ||
            index == ESP_BLE_AUDIO_HAS_PRESET_INDEX_NONE ||
            max_count == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_has_client_presets_read_safe(has, index, max_count);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_has_client_preset_set(esp_ble_audio_has_t *has,
                                              uint8_t index, bool sync)
{
    int err;

    if (has == NULL || index == ESP_BLE_AUDIO_HAS_PRESET_INDEX_NONE) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_has_client_preset_set_safe(has, index, sync);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_has_client_preset_next(esp_ble_audio_has_t *has, bool sync)
{
    int err;

    if (has == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_has_client_preset_next_safe(has, sync);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_has_client_preset_prev(esp_ble_audio_has_t *has, bool sync)
{
    int err;

    if (has == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_has_client_preset_prev_safe(has, sync);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_HAS_CLIENT */

#if CONFIG_BT_HAS
esp_err_t esp_ble_audio_has_register(const esp_ble_audio_has_features_param_t *features)
{
    int err;

    if (features == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_has_register_safe(features);
    if (err) {
        return ESP_FAIL;
    }

#if BLE_AUDIO_SVC_SEP_ADD
    err = bt_le_has_init();
    if (err) {
        return ESP_FAIL;
    }
#endif /* BLE_AUDIO_SVC_SEP_ADD */

    return ESP_OK;
}

esp_err_t esp_ble_audio_has_preset_register(const esp_ble_audio_has_preset_register_param_t *param)
{
    int err;

    if (param == NULL ||
            param->index == ESP_BLE_AUDIO_HAS_PRESET_INDEX_NONE ||
            param->name == NULL ||
            param->ops == NULL || param->ops->select == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (strlen(param->name) < ESP_BLE_AUDIO_HAS_PRESET_NAME_MIN ||
            strlen(param->name) > ESP_BLE_AUDIO_HAS_PRESET_NAME_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!IS_ENABLED(CONFIG_BT_HAS_PRESET_NAME_DYNAMIC) &&
            (param->properties & ESP_BLE_AUDIO_HAS_PROP_WRITABLE) > 0) {
        /* Writable presets is not supported */
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_has_preset_register_safe(param);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_has_preset_unregister(uint8_t index)
{
    int err;

    if (index == ESP_BLE_AUDIO_HAS_PRESET_INDEX_NONE) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_has_preset_unregister_safe(index);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_has_preset_available(uint8_t index)
{
    int err;

    if (index == ESP_BLE_AUDIO_HAS_PRESET_INDEX_NONE) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_has_preset_available_safe(index);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_has_preset_unavailable(uint8_t index)
{
    int err;

    if (index == ESP_BLE_AUDIO_HAS_PRESET_INDEX_NONE) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_has_preset_unavailable_safe(index);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_has_preset_active_set(uint8_t index)
{
    int err;

    if (index == ESP_BLE_AUDIO_HAS_PRESET_INDEX_NONE) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_has_preset_active_set_safe(index);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

uint8_t esp_ble_audio_has_preset_active_get(void)
{
    return bt_has_preset_active_get_safe();
}

esp_err_t esp_ble_audio_has_preset_name_change(uint8_t index, const char *name)
{
    int err;

    if (index == ESP_BLE_AUDIO_HAS_PRESET_INDEX_NONE || name == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (strlen(name) < ESP_BLE_AUDIO_HAS_PRESET_NAME_MIN ||
            strlen(name) > ESP_BLE_AUDIO_HAS_PRESET_NAME_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_has_preset_name_change_safe(index, name);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_has_features_set(const esp_ble_audio_has_features_param_t *features)
{
    int err;

    if (features == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_has_features_set_safe(features);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_HAS */
