/*
 * SPDX-FileCopyrightText: 2021-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_ble_audio_csip_api.h"

#if CONFIG_BT_CSIP_SET_MEMBER
void *esp_ble_audio_csip_set_member_svc_decl_get(const esp_ble_audio_csip_set_member_svc_inst_t *svc_inst)
{
    if (svc_inst == NULL) {
        return NULL;
    }

    return bt_csip_set_member_svc_decl_get_safe(svc_inst);
}

esp_err_t esp_ble_audio_csip_set_member_register(const esp_ble_audio_csip_set_member_register_param_t *param,
                                                 esp_ble_audio_csip_set_member_svc_inst_t **svc_inst)
{
    int err;

    if (param == NULL || svc_inst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (param->lockable && param->rank == 0) {
        /* Rank cannot be 0 if service is lockable */
        return ESP_ERR_INVALID_ARG;
    }

    if (param->rank > param->set_size) {
        /* Invalid rank (shall be less than or equal to set_size */
        return ESP_ERR_INVALID_ARG;
    }

#if CONFIG_BT_CSIP_SET_MEMBER_MAX_INSTANCE_COUNT > 1
    if (param->parent == NULL) {
        /* Parent service not provided */
        return ESP_ERR_INVALID_ARG;
    }
#endif /* CONFIG_BT_CSIP_SET_MEMBER_MAX_INSTANCE_COUNT > 1 */

    err = bt_csip_set_member_register_safe(param, svc_inst);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_csip_set_member_unregister(esp_ble_audio_csip_set_member_svc_inst_t *svc_inst)
{
    int err;

    if (svc_inst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_csip_set_member_unregister_safe(svc_inst);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_csip_set_member_sirk(esp_ble_audio_csip_set_member_svc_inst_t *svc_inst,
                                             const uint8_t sirk[ESP_BLE_AUDIO_CSIP_SIRK_SIZE])
{
    int err;

    if (svc_inst == NULL || sirk == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_csip_set_member_sirk_safe(svc_inst, sirk);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_csip_set_member_set_size_and_rank(esp_ble_audio_csip_set_member_svc_inst_t *svc_inst,
                                                          uint8_t size, uint8_t rank)
{
    int err;

    if (svc_inst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_csip_set_member_set_size_and_rank_safe(svc_inst, size, rank);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_csip_set_member_get_info(const esp_ble_audio_csip_set_member_svc_inst_t *svc_inst,
                                                 esp_ble_audio_csip_set_member_set_info_t *info)
{
    int err;

    if (svc_inst == NULL || info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_csip_set_member_get_info_safe(svc_inst, info);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_csip_set_member_generate_rsi(const esp_ble_audio_csip_set_member_svc_inst_t *svc_inst,
                                                     uint8_t rsi[ESP_BLE_AUDIO_CSIP_RSI_SIZE])
{
    int err;

    if (svc_inst == NULL || rsi == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_csip_set_member_generate_rsi_safe(svc_inst, rsi);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_csip_set_member_lock(esp_ble_audio_csip_set_member_svc_inst_t *svc_inst,
                                             bool lock, bool force)
{
    int err;

    if (svc_inst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_csip_set_member_lock_safe(svc_inst, lock, force);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_CSIP_SET_MEMBER */

#if CONFIG_BT_CSIP_SET_COORDINATOR
esp_err_t esp_ble_audio_csip_set_coordinator_discover(uint16_t conn_handle)
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

    err = bt_csip_set_coordinator_discover(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_ble_audio_csip_set_coordinator_set_member_t *
esp_ble_audio_csip_set_coordinator_set_member_by_conn(uint16_t conn_handle)
{
    esp_ble_audio_csip_set_coordinator_set_member_t *ret = NULL;
    void *conn;

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        goto unlock;
    }

    ret = bt_csip_set_coordinator_set_member_by_conn(conn);

unlock:
    bt_le_host_unlock();
    return ret;
}

bool esp_ble_audio_csip_set_coordinator_is_set_member(const uint8_t sirk[ESP_BLE_AUDIO_CSIP_SIRK_SIZE],
                                                      uint8_t data_type, const uint8_t *data, uint8_t data_len)
{
    struct bt_data ad = {
        .type = data_type,
        .data = data,
        .data_len = data_len,
    };

    if (sirk == NULL || data_type != BT_DATA_CSIS_RSI ||
            data == NULL || data_len != ESP_BLE_AUDIO_CSIP_RSI_SIZE) {
        return false;
    }

    return bt_csip_set_coordinator_is_set_member_safe(sirk, &ad);
}

esp_err_t esp_ble_audio_csip_set_coordinator_register_cb(esp_ble_audio_csip_set_coordinator_cb_t *cb)
{
    int err;

    if (cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_csip_set_coordinator_register_cb_safe(cb);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_csip_set_coordinator_ordered_access(const esp_ble_audio_csip_set_coordinator_set_member_t *members[],
                                                            uint8_t count,
                                                            const esp_ble_audio_csip_set_coordinator_set_info_t *set_info,
                                                            bt_csip_set_coordinator_ordered_access_t cb)
{
    int err;

    if (members == NULL ||
        count == 0 || count > CONFIG_BT_MAX_CONN ||
        set_info == NULL || cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_csip_set_coordinator_ordered_access_safe(members, count, set_info, cb);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_csip_set_coordinator_lock(const esp_ble_audio_csip_set_coordinator_set_member_t **members,
                                                  uint8_t count,
                                                  const esp_ble_audio_csip_set_coordinator_set_info_t *set_info)
{
    int err;

    if (members == NULL || count > CONFIG_BT_MAX_CONN || set_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_csip_set_coordinator_lock_safe(members, count, set_info);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_csip_set_coordinator_release(const esp_ble_audio_csip_set_coordinator_set_member_t **members,
                                                     uint8_t count,
                                                     const esp_ble_audio_csip_set_coordinator_set_info_t *set_info)
{
    int err;

    if (members == NULL || count > CONFIG_BT_MAX_CONN || set_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_csip_set_coordinator_release_safe(members, count, set_info);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_CSIP_SET_COORDINATOR */
