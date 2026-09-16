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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vcp_vol_rend_included_get(included);

    bt_le_host_unlock();

    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_rend_register(esp_ble_audio_vcp_vol_rend_register_param_t *param)
{
    esp_err_t ret = ESP_OK;
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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vcp_vol_rend_register(param);
    if (err) {
        ret = ESP_FAIL;
        goto end;
    }

#if BLE_AUDIO_SVC_DEFERRED_ADD
    err = bt_le_vcp_vol_rend_init();
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

esp_err_t esp_ble_audio_vcp_vol_rend_set_step(uint8_t volume_step)
{
    int err;

    if (volume_step == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vcp_vol_rend_set_step(volume_step);

    bt_le_host_unlock();

    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_rend_get_state(void)
{
    int err;

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vcp_vol_rend_get_state();

    bt_le_host_unlock();

    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_rend_get_flags(void)
{
    int err;

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vcp_vol_rend_get_flags();

    bt_le_host_unlock();

    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_rend_vol_down(void)
{
    int err;

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vcp_vol_rend_vol_down();

    bt_le_host_unlock();

    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_rend_vol_up(void)
{
    int err;

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vcp_vol_rend_vol_up();

    bt_le_host_unlock();

    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_rend_unmute_vol_down(void)
{
    int err;

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vcp_vol_rend_unmute_vol_down();

    bt_le_host_unlock();

    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_rend_unmute_vol_up(void)
{
    int err;

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vcp_vol_rend_unmute_vol_up();

    bt_le_host_unlock();

    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_rend_set_vol(uint8_t volume)
{
    int err;

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vcp_vol_rend_set_vol(volume);

    bt_le_host_unlock();

    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_rend_unmute(void)
{
    int err;

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vcp_vol_rend_unmute();

    bt_le_host_unlock();

    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_rend_mute(void)
{
    int err;

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vcp_vol_rend_mute();

    bt_le_host_unlock();

    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_vcp_vol_rend_reset_state(const esp_ble_audio_vcp_vol_rend_reset_state_param_t *param)
{
    int err;

    if (param == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vcp_vol_rend_reset_state(param);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vcp_vol_ctlr_cb_register(cb);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vcp_vol_ctlr_cb_unregister(cb);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

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

    BT_LE_HOST_LOCK_OR_RETURN(NULL);

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = lib_vcp_vol_ctlr_included_get(vol_ctlr, included);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vcp_vol_ctlr_read_state(vol_ctlr);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vcp_vol_ctlr_read_flags(vol_ctlr);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vcp_vol_ctlr_vol_down(vol_ctlr);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vcp_vol_ctlr_vol_up(vol_ctlr);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vcp_vol_ctlr_unmute_vol_down(vol_ctlr);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vcp_vol_ctlr_unmute_vol_up(vol_ctlr);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vcp_vol_ctlr_set_vol(vol_ctlr, volume);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vcp_vol_ctlr_unmute(vol_ctlr);

    bt_le_host_unlock();

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

    BT_LE_HOST_LOCK_OR_RETURN(ESP_ERR_TIMEOUT);

    err = bt_vcp_vol_ctlr_mute(vol_ctlr);

    bt_le_host_unlock();

    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_VCP_VOL_CTLR */
