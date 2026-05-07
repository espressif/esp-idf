/*
 * SPDX-FileCopyrightText: 2020-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_ble_audio_vocs_api.h"

#if CONFIG_BT_VOCS
esp_ble_audio_vocs_t *esp_ble_audio_vocs_free_instance_get(void)
{
    return bt_vocs_free_instance_get_safe();
}

void *esp_ble_audio_vocs_svc_decl_get(esp_ble_audio_vocs_t *vocs)
{
    if (vocs == NULL) {
        return NULL;
    }

    return bt_vocs_svc_decl_get_safe(vocs);
}

esp_err_t esp_ble_audio_vocs_register(esp_ble_audio_vocs_t *vocs,
                                      const esp_ble_audio_vocs_register_param_t *param)
{
    int err;

    if (vocs == NULL || param == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_vocs_register_safe(vocs, param);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_VOCS */

#if CONFIG_BT_VOCS || CONFIG_BT_VOCS_CLIENT
esp_err_t esp_ble_audio_vocs_state_get(esp_ble_audio_vocs_t *inst)
{
    int err;

    if (inst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_vocs_state_get_safe(inst);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vocs_state_set(esp_ble_audio_vocs_t *inst, int16_t offset)
{
    int err;

    if (inst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_vocs_state_set_safe(inst, offset);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vocs_location_get(esp_ble_audio_vocs_t *inst)
{
    int err;

    if (inst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_vocs_location_get_safe(inst);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vocs_location_set(esp_ble_audio_vocs_t *inst, uint32_t location)
{
    int err;

    if (inst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_vocs_location_set_safe(inst, location);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vocs_description_get(esp_ble_audio_vocs_t *inst)
{
    int err;

    if (inst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_vocs_description_get_safe(inst);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vocs_description_set(esp_ble_audio_vocs_t *inst,
                                             const char *description)
{
    int err;

    if (inst == NULL || description == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_vocs_description_set_safe(inst, description);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_VOCS || CONFIG_BT_VOCS_CLIENT */

#if CONFIG_BT_VOCS_CLIENT
esp_err_t esp_ble_audio_vocs_client_cb_register(esp_ble_audio_vocs_t *inst,
                                                esp_ble_audio_vocs_cb_t *cb)
{
    if (inst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    bt_vocs_client_cb_register_safe(inst, cb);

    return ESP_OK;
}

esp_ble_audio_vocs_t *esp_ble_audio_vocs_client_free_instance_get(void)
{
    return bt_vocs_client_free_instance_get_safe();
}

esp_err_t esp_ble_audio_vocs_discover(uint16_t conn_handle,
                                      esp_ble_audio_vocs_t *inst,
                                      const esp_ble_audio_vocs_discover_param_t *param)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    if (inst == NULL || param == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (param->start_handle == 0 ||
        param->end_handle == 0 ||
        param->end_handle < param->start_handle) {
        /* Start handle and end handle shall be non-zero and
         * start handle shall be less than end handle.
         */
        return ESP_ERR_INVALID_ARG;
    }

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_vocs_discover(conn, inst, param);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_VOCS_CLIENT */
