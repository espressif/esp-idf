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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_has_client_cb_register(cb);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_has_client_presets_read(has, index, max_count);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_has_client_preset_set(has, index, sync);

    bt_le_host_unlock();

    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_has_client_preset_name_write(esp_ble_audio_has_t *has,
                                                     uint8_t index, const char *name)
{
    int err;

    if (has == NULL || index == ESP_BLE_AUDIO_HAS_PRESET_INDEX_NONE || name == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (strlen(name) < ESP_BLE_AUDIO_HAS_PRESET_NAME_MIN ||
            strlen(name) > ESP_BLE_AUDIO_HAS_PRESET_NAME_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_has_client_preset_name_write(has, index, name);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_has_client_preset_next(has, sync);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_has_client_preset_prev(has, sync);

    bt_le_host_unlock();

    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_HAS_CLIENT */

#if CONFIG_BT_HAS
esp_err_t esp_ble_audio_has_register(const esp_ble_audio_has_features_param_t *features)
{
    esp_err_t ret = ESP_OK;
    int err;

    if (features == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_has_register(features);
    if (err) {
        ret = ESP_FAIL;
        goto end;
    }

#if BLE_AUDIO_SVC_DEFERRED_ADD
    err = bt_le_has_init();
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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_has_preset_register(param);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_has_preset_unregister(index);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_has_preset_available(index);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_has_preset_unavailable(index);

    bt_le_host_unlock();

    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_has_preset_active_set(uint8_t index)
{
    int err;

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_has_preset_active_set(index);

    bt_le_host_unlock();

    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

uint8_t esp_ble_audio_has_preset_active_get(void)
{
    uint8_t index;

    BT_LE_HOST_LOCK_OR_RETURN(ESP_BLE_AUDIO_HAS_PRESET_INDEX_NONE);

    index = bt_has_preset_active_get();

    bt_le_host_unlock();

    return index;
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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_has_preset_name_change(index, name);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_has_features_set(features);

    bt_le_host_unlock();

    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_HAS */
