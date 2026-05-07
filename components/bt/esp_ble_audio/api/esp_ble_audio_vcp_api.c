/*
 * SPDX-FileCopyrightText: 2020-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_ble_audio_vcp_api.h"

#if CONFIG_BT_VCP_VOL_REND
esp_err_t esp_ble_audio_vcp_vol_rend_included_get(esp_ble_audio_vcp_included_t *included)
{
    int err;

    if (included == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_vcp_vol_rend_included_get_safe(included);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_rend_register(esp_ble_audio_vcp_vol_rend_register_param_t *param)
{
    int err;

    if (param == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (CONFIG_BT_VCP_VOL_REND_VOCS_INSTANCE_COUNT && param->vocs_param == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (CONFIG_BT_VCP_VOL_REND_AICS_INSTANCE_COUNT && param->aics_param == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (param->mute > ESP_BLE_AUDIO_VCP_STATE_MUTED || param->step == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_vcp_vol_rend_register_safe(param);
    if (err) {
        return ESP_FAIL;
    }

#if BLE_AUDIO_SVC_SEP_ADD
    err = bt_le_vcp_vol_rend_init();
    if (err) {
        return ESP_FAIL;
    }
#endif /* BLE_AUDIO_SVC_SEP_ADD */

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_rend_set_step(uint8_t volume_step)
{
    int err;

    if (volume_step == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_vcp_vol_rend_set_step_safe(volume_step);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_rend_get_state(void)
{
    int err;

    err = bt_vcp_vol_rend_get_state_safe();
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_rend_get_flags(void)
{
    int err;

    err = bt_vcp_vol_rend_get_flags_safe();
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_rend_vol_down(void)
{
    int err;

    err = bt_vcp_vol_rend_vol_down_safe();
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_rend_vol_up(void)
{
    int err;

    err = bt_vcp_vol_rend_vol_up_safe();
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_rend_unmute_vol_down(void)
{
    int err;

    err = bt_vcp_vol_rend_unmute_vol_down_safe();
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_rend_unmute_vol_up(void)
{
    int err;

    err = bt_vcp_vol_rend_unmute_vol_up_safe();
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_rend_set_vol(uint8_t volume)
{
    int err;

    err = bt_vcp_vol_rend_set_vol_safe(volume);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_rend_unmute(void)
{
    int err;

    err = bt_vcp_vol_rend_unmute_safe();
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_rend_mute(void)
{
    int err;

    err = bt_vcp_vol_rend_mute_safe();
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_VCP_VOL_REND */

#if CONFIG_BT_VCP_VOL_CTLR
esp_err_t esp_ble_audio_vcp_vol_ctlr_cb_register(esp_ble_audio_vcp_vol_ctlr_cb_t *cb)
{
    int err;

    if (cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_vcp_vol_ctlr_cb_register_safe(cb);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_ctlr_cb_unregister(esp_ble_audio_vcp_vol_ctlr_cb_t *cb)
{
    int err;

    if (cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_vcp_vol_ctlr_cb_unregister_safe(cb);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_ctlr_discover(uint16_t conn_handle,
                                              esp_ble_audio_vcp_vol_ctlr_t **vol_ctlr)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    if (vol_ctlr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_vcp_vol_ctlr_discover(conn, vol_ctlr);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_ble_audio_vcp_vol_ctlr_t *esp_ble_audio_vcp_vol_ctlr_get_by_conn(uint16_t conn_handle)
{
    esp_ble_audio_vcp_vol_ctlr_t *ret = NULL;
    void *conn;

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        goto unlock;
    }

    ret = bt_vcp_vol_ctlr_get_by_conn(conn);

unlock:
    bt_le_host_unlock();
    return ret;
}

#if CONFIG_BT_VCP_VOL_CTLR_VOCS || CONFIG_BT_VCP_VOL_CTLR_AICS
esp_err_t esp_ble_audio_vcp_vol_ctlr_included_get(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr,
                                                  esp_ble_audio_vcp_included_t *included)
{
    int err;

    if (vol_ctlr == NULL || included == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_vcp_vol_ctlr_included_get_safe(vol_ctlr, included);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_VCP_VOL_CTLR_VOCS || CONFIG_BT_VCP_VOL_CTLR_AICS */

esp_err_t esp_ble_audio_vcp_vol_ctlr_read_state(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr)
{
    int err;

    if (vol_ctlr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_vcp_vol_ctlr_read_state_safe(vol_ctlr);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_ctlr_read_flags(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr)
{
    int err;

    if (vol_ctlr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_vcp_vol_ctlr_read_flags_safe(vol_ctlr);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_ctlr_vol_down(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr)
{
    int err;

    if (vol_ctlr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_vcp_vol_ctlr_vol_down_safe(vol_ctlr);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_ctlr_vol_up(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr)
{
    int err;

    if (vol_ctlr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_vcp_vol_ctlr_vol_up_safe(vol_ctlr);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_ctlr_unmute_vol_down(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr)
{
    int err;

    if (vol_ctlr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_vcp_vol_ctlr_unmute_vol_down_safe(vol_ctlr);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_ctlr_unmute_vol_up(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr)
{
    int err;

    if (vol_ctlr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_vcp_vol_ctlr_unmute_vol_up_safe(vol_ctlr);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_ctlr_set_vol(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr, uint8_t volume)
{
    int err;

    if (vol_ctlr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_vcp_vol_ctlr_set_vol_safe(vol_ctlr, volume);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_ctlr_unmute(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr)
{
    int err;

    if (vol_ctlr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_vcp_vol_ctlr_unmute_safe(vol_ctlr);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_ctlr_mute(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr)
{
    int err;

    if (vol_ctlr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_vcp_vol_ctlr_mute_safe(vol_ctlr);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_VCP_VOL_CTLR */
