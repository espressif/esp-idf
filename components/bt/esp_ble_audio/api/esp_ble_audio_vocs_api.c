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
    esp_ble_audio_vocs_t *inst;

    BT_LE_HOST_LOCK_OR_RETURN(NULL);

    inst = bt_vocs_free_instance_get();

    bt_le_host_unlock();

    return inst;
}

void *esp_ble_audio_vocs_svc_decl_get(esp_ble_audio_vocs_t *vocs)
{
    void *decl;

    if (vocs == NULL) {
        return NULL;
    }

    BT_LE_HOST_LOCK_OR_RETURN(NULL);

    decl = bt_vocs_svc_decl_get(vocs);

    bt_le_host_unlock();

    return decl;
}

esp_err_t esp_ble_audio_vocs_register(esp_ble_audio_vocs_t *vocs,
                                      const esp_ble_audio_vocs_register_param_t *param)
{
    int err;

    if (vocs == NULL || param == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vocs_register(vocs, param);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vocs_state_get(inst);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vocs_state_set(inst, offset);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vocs_location_get(inst);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vocs_location_set(inst, location);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vocs_description_get(inst);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vocs_description_set(inst, description);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    bt_vocs_client_cb_register(inst, cb);

    bt_le_host_unlock();

    return ESP_OK;
}

esp_ble_audio_vocs_t *esp_ble_audio_vocs_client_free_instance_get(void)
{
    esp_ble_audio_vocs_t *inst;

    BT_LE_HOST_LOCK_OR_RETURN(NULL);

    inst = bt_vocs_client_free_instance_get();

    bt_le_host_unlock();

    return inst;
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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

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
