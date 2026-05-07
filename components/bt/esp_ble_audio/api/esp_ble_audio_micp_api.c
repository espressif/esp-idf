/*
 * SPDX-FileCopyrightText: 2020-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_ble_audio_micp_api.h"

#if CONFIG_BT_MICP_MIC_DEV
esp_err_t esp_ble_audio_micp_mic_dev_register(esp_ble_audio_micp_mic_dev_register_param_t *param)
{
    int err;

    if (param == NULL ||
            (CONFIG_BT_MICP_MIC_DEV_AICS_INSTANCE_COUNT && param->aics_param == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_micp_mic_dev_register_safe(param);
    if (err) {
        return ESP_FAIL;
    }

#if BLE_AUDIO_SVC_SEP_ADD
    err = bt_le_micp_mic_dev_init();
    if (err) {
        return ESP_FAIL;
    }
#endif /* BLE_AUDIO_SVC_SEP_ADD */

    return ESP_OK;
}

esp_err_t esp_ble_audio_micp_mic_dev_included_get(esp_ble_audio_micp_included_t *included)
{
    int err;

    if (included == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_micp_mic_dev_included_get_safe(included);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_micp_mic_dev_unmute(void)
{
    int err;

    err = bt_micp_mic_dev_unmute_safe();
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_micp_mic_dev_mute(void)
{
    int err;

    err = bt_micp_mic_dev_mute_safe();
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_micp_mic_dev_mute_disable(void)
{
    int err;

    err = bt_micp_mic_dev_mute_disable_safe();
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_micp_mic_dev_mute_get(void)
{
    int err;

    err = bt_micp_mic_dev_mute_get_safe();
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_MICP_MIC_DEV */

#if CONFIG_BT_MICP_MIC_CTLR
#if CONFIG_BT_MICP_MIC_CTLR_AICS
esp_err_t esp_ble_audio_micp_mic_ctlr_included_get(esp_ble_audio_micp_mic_ctlr_t *mic_ctlr,
                                                   esp_ble_audio_micp_included_t *included)
{
    int err;

    if (mic_ctlr == NULL || included == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_micp_mic_ctlr_included_get_safe(mic_ctlr, included);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_MICP_MIC_CTLR_AICS */

esp_ble_audio_micp_mic_ctlr_t *esp_ble_audio_micp_mic_ctlr_get_by_conn(uint16_t conn_handle)
{
    esp_ble_audio_micp_mic_ctlr_t *ret = NULL;
    void *conn;

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        goto unlock;
    }

    ret = bt_micp_mic_ctlr_get_by_conn(conn);

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_micp_mic_ctlr_discover(uint16_t conn_handle,
                                               esp_ble_audio_micp_mic_ctlr_t **mic_ctlr)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    if (mic_ctlr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_micp_mic_ctlr_discover(conn, mic_ctlr);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_micp_mic_ctlr_unmute(esp_ble_audio_micp_mic_ctlr_t *mic_ctlr)
{
    int err;

    if (mic_ctlr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_micp_mic_ctlr_unmute_safe(mic_ctlr);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_micp_mic_ctlr_mute(esp_ble_audio_micp_mic_ctlr_t *mic_ctlr)
{
    int err;

    if (mic_ctlr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_micp_mic_ctlr_mute_safe(mic_ctlr);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_micp_mic_ctlr_mute_get(esp_ble_audio_micp_mic_ctlr_t *mic_ctlr)
{
    int err;

    if (mic_ctlr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_micp_mic_ctlr_mute_get_safe(mic_ctlr);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_micp_mic_ctlr_cb_register(esp_ble_audio_micp_mic_ctlr_cb_t *cb)
{
    int err;

    if (cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_micp_mic_ctlr_cb_register_safe(cb);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_MICP_MIC_CTLR */
