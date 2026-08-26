/*
 * SPDX-FileCopyrightText: 2020-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_ble_audio_aics_api.h"

#if CONFIG_BT_AICS
esp_ble_audio_aics_t *esp_ble_audio_aics_free_instance_get(void)
{
    return bt_aics_free_instance_get_safe();
}

void *esp_ble_audio_aics_svc_decl_get(esp_ble_audio_aics_t *aics)
{
    if (aics == NULL) {
        return NULL;
    }

    return bt_aics_svc_decl_get_safe(aics);
}

esp_err_t esp_ble_audio_aics_register(esp_ble_audio_aics_t *aics,
                                      esp_ble_audio_aics_register_param_t *param)
{
    int err;

    if (aics == NULL || param == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (param->mute > ESP_BLE_AUDIO_AICS_STATE_MUTE_DISABLED ||
            param->gain_mode > ESP_BLE_AUDIO_AICS_MODE_AUTO ||
            param->type > ESP_BLE_AUDIO_AICS_INPUT_TYPE_AMBIENT ||
            param->units == 0 ||
            param->min_gain > param->max_gain ||
            param->gain < param->min_gain ||
            param->gain > param->max_gain) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_aics_register_safe(aics, param);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_aics_activate(esp_ble_audio_aics_t *inst)
{
    int err;

    if (inst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_aics_activate_safe(inst);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_aics_deactivate(esp_ble_audio_aics_t *inst)
{
    int err;

    if (inst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_aics_deactivate_safe(inst);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_AICS */

#if CONFIG_BT_AICS || CONFIG_BT_AICS_CLIENT
esp_err_t esp_ble_audio_aics_state_get(esp_ble_audio_aics_t *inst)
{
    int err;

    if (inst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_aics_state_get_safe(inst);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_aics_gain_setting_get(esp_ble_audio_aics_t *inst)
{
    int err;

    if (inst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_aics_gain_setting_get_safe(inst);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_aics_type_get(esp_ble_audio_aics_t *inst)
{
    int err;

    if (inst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_aics_type_get_safe(inst);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_aics_status_get(esp_ble_audio_aics_t *inst)
{
    int err;

    if (inst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_aics_status_get_safe(inst);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_aics_disable_mute(esp_ble_audio_aics_t *inst)
{
    int err;

    if (inst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_aics_disable_mute_safe(inst);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_aics_unmute(esp_ble_audio_aics_t *inst)
{
    int err;

    if (inst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_aics_unmute_safe(inst);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_aics_mute(esp_ble_audio_aics_t *inst)
{
    int err;

    if (inst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_aics_mute_safe(inst);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_aics_gain_set_manual_only(esp_ble_audio_aics_t *inst)
{
    int err;

    if (inst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_aics_gain_set_manual_only_safe(inst);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_aics_gain_set_auto_only(esp_ble_audio_aics_t *inst)
{
    int err;

    if (inst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_aics_gain_set_auto_only_safe(inst);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_aics_manual_gain_set(esp_ble_audio_aics_t *inst)
{
    int err;

    if (inst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_aics_manual_gain_set_safe(inst);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_aics_automatic_gain_set(esp_ble_audio_aics_t *inst)
{
    int err;

    if (inst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_aics_automatic_gain_set_safe(inst);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_aics_gain_set(esp_ble_audio_aics_t *inst, int8_t gain)
{
    int err;

    if (inst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_aics_gain_set_safe(inst, gain);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_aics_description_get(esp_ble_audio_aics_t *inst)
{
    int err;

    if (inst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_aics_description_get_safe(inst);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_aics_description_set(esp_ble_audio_aics_t *inst,
                                             const char *description)
{
    int err;

    if (inst == NULL || description == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_aics_description_set_safe(inst, description);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_AICS || CONFIG_BT_AICS_CLIENT */

#if CONFIG_BT_AICS_CLIENT
esp_err_t esp_ble_audio_aics_discover(uint16_t conn_handle, esp_ble_audio_aics_t *inst,
                                      const esp_ble_audio_aics_discover_param_t *param)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    if (inst == NULL || param == NULL ||
            param->end_handle <= param->start_handle) {
        return ESP_ERR_INVALID_ARG;
    }

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_aics_discover(conn, inst, param);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_ble_audio_aics_t *esp_ble_audio_aics_client_free_instance_get(void)
{
    return bt_aics_client_free_instance_get_safe();
}

esp_err_t esp_ble_audio_aics_client_cb_register(esp_ble_audio_aics_t *inst,
                                                esp_ble_audio_aics_cb_t *cb)
{
    if (inst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    bt_aics_client_cb_register_safe(inst, cb);

    return ESP_OK;
}
#endif /* CONFIG_BT_AICS_CLIENT */
