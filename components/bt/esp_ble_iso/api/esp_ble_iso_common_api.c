/*
 * SPDX-FileCopyrightText: 2020 Intel Corporation
 * SPDX-FileCopyrightText: 2021-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_ble_iso_common_api.h"

esp_err_t esp_ble_iso_data_parse(const uint8_t ltv[], size_t size,
                                 bool (*func)(uint8_t type,
                                              const uint8_t *data,
                                              uint8_t data_len,
                                              void *user_data),
                                 void *user_data)
{
    if (ltv == NULL || func == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    for (size_t i = 0; i < size;) {
        const size_t remaining = size - i;
        const uint8_t len = ltv[i];
        uint8_t data_len;
        uint8_t type;

        /* LTV item format: [len][type][value...], total item size is len + 1.
         * len includes type and value, but excludes itself.
         */
        if (len < sizeof(uint8_t) || len > (remaining - 1)) {
            return ESP_ERR_INVALID_SIZE;
        }

        type = ltv[i + 1];
        data_len = len - sizeof(uint8_t);

        /* Skip empty value entries in strict parsing mode. */
        if (data_len == 0) {
            i += (size_t)len + 1;
            continue;
        }

        if (func(type, &ltv[i + 2], data_len, user_data) == false) {
            return ESP_OK;
        }

        i += (size_t)len + 1;
    }

    return ESP_OK;
}

#if CONFIG_BT_ISO_UNICAST
#if CONFIG_BT_ISO_PERIPHERAL
esp_err_t esp_ble_iso_server_register(esp_ble_iso_server_t *server)
{
    int err;

    err = bt_iso_server_register_safe(server);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_iso_server_unregister(esp_ble_iso_server_t *server)
{
    int err;

    err = bt_iso_server_unregister_safe(server);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_ISO_PERIPHERAL */

#if CONFIG_BT_ISO_CENTRAL
esp_err_t esp_ble_iso_cig_create(esp_ble_iso_cig_param_t *param,
                                 esp_ble_iso_cig_t **out_cig)
{
    int err;

    err = bt_iso_cig_create_safe(param, out_cig);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_iso_cig_reconfigure(esp_ble_iso_cig_t *cig,
                                      esp_ble_iso_cig_param_t *param)
{
    int err;

    err = bt_iso_cig_reconfigure_safe(cig, param);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_iso_cig_terminate(esp_ble_iso_cig_t *cig)
{
    int err;

    err = bt_iso_cig_terminate_safe(cig);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_iso_chan_connect(esp_ble_iso_connect_param_t *param,
                                   uint16_t conn_handle, size_t count)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    if (param == NULL || count == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    for (size_t i = 0; i < count; i++) {
        param[i].acl = conn;
    }

    err = bt_iso_chan_connect(param, count);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_ISO_CENTRAL */

esp_err_t esp_ble_iso_chan_disconnect(esp_ble_iso_chan_t *chan)
{
    int err;

    err = bt_iso_chan_disconnect_safe(chan);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_ISO_UNICAST */

static inline bool valid_dir(uint8_t dir)
{
    if (dir != ESP_BLE_ISO_DATA_PATH_DIR_INPUT &&
            dir != ESP_BLE_ISO_DATA_PATH_DIR_OUTPUT) {
        return false;
    }

    return true;
}

esp_err_t esp_ble_iso_setup_data_path(const esp_ble_iso_chan_t *chan, uint8_t dir,
                                      const esp_ble_iso_chan_path_t *path)
{
    int err;

    if (chan == NULL || path == NULL || valid_dir(dir) == false) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_iso_setup_data_path_safe(chan, dir, path);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_iso_remove_data_path(const esp_ble_iso_chan_t *chan, uint8_t dir)
{
    int err;

    if (chan == NULL || valid_dir(dir) == false) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_iso_remove_data_path_safe(chan, dir);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

#if CONFIG_BT_ISO_BROADCAST
esp_err_t esp_ble_iso_big_register_cb(esp_ble_iso_big_cb_t *cb)
{
    int err;

    if (cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_iso_big_register_cb_safe(cb);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

#if CONFIG_BT_ISO_BROADCASTER
esp_err_t esp_ble_iso_big_ext_adv_add(esp_ble_iso_ext_adv_info_t *info)
{
    int err;

    if (info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_le_ext_adv_new_safe(info->adv_handle);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_iso_big_ext_adv_delete(esp_ble_iso_ext_adv_info_t *info)
{
    int err;

    if (info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_le_ext_adv_delete_safe(info->adv_handle);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_iso_big_create(uint8_t adv_handle,
                                 esp_ble_iso_big_create_param_t *param,
                                 esp_ble_iso_big_t **out_big)
{
    esp_err_t ret = ESP_OK;
    void *adv;
    int err;

    bt_le_host_lock();

    adv = bt_le_ext_adv_find(adv_handle);
    if (adv == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_iso_big_create(adv, param, out_big);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_ISO_BROADCASTER */

#if CONFIG_BT_ISO_SYNC_RECEIVER
esp_err_t esp_ble_iso_big_sync(uint16_t sync_handle,
                               esp_ble_iso_big_sync_param_t *param,
                               esp_ble_iso_big_t **out_big)
{
    void *per_adv_sync;
    int err;

    per_adv_sync = bt_le_per_adv_sync_find_safe(sync_handle);
    if (per_adv_sync == NULL) {
        return ESP_ERR_NOT_FOUND;
    }

    err = bt_iso_big_sync_safe(per_adv_sync, param, out_big);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_ISO_SYNC_RECEIVER */

esp_err_t esp_ble_iso_big_terminate(esp_ble_iso_big_t *big)
{
    int err;

    err = bt_iso_big_terminate_safe(big);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_ISO_BROADCAST */

esp_err_t esp_ble_iso_chan_get_info(esp_ble_iso_chan_t *chan,
                                    esp_ble_iso_info_t *info)
{
    int err;

    err = bt_iso_chan_get_info_safe(chan, info);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

#if CONFIG_BT_ISO_TX
esp_err_t esp_ble_iso_chan_get_tx_sync(esp_ble_iso_chan_t *chan,
                                       esp_ble_iso_tx_info_t *info)
{
    int err;

    err = bt_iso_chan_get_tx_sync_safe(chan, info);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_iso_chan_send(esp_ble_iso_chan_t *chan,
                                const uint8_t *sdu,
                                uint16_t sdu_len,
                                uint16_t seq_num)
{
    struct net_buf buf = {
        /* Note: only data and len are used */
        .data = (void *)sdu,
        .len  = sdu_len,
    };
    int err;

    if (chan == NULL || (!sdu ^ !sdu_len)) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_iso_chan_send_safe(chan, &buf, seq_num);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_iso_chan_send_ts(esp_ble_iso_chan_t *chan,
                                   const uint8_t *sdu,
                                   uint16_t sdu_len,
                                   uint16_t seq_num,
                                   uint32_t ts)
{
    struct net_buf buf = {
        /* Note: only data and len are used */
        .data = (void *)sdu,
        .len  = sdu_len,
    };
    int err;

    if (chan == NULL || (!sdu ^ !sdu_len)) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_iso_chan_send_ts_safe(chan, &buf, seq_num, ts);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_ISO_TX */

void esp_ble_iso_gap_app_post_event(uint8_t type, void *param)
{
    bt_le_gap_app_post_event(type, param);
}

esp_err_t esp_ble_iso_common_init(esp_ble_iso_init_info_t *info)
{
    bool gap_cb_registered = false;
    int err;

    if (info) {
        if (info->gap_cb) {
            err = bt_le_gap_app_cb_register(info->gap_cb);
            if (err) {
                return ESP_FAIL;
            }

            gap_cb_registered = true;
        }
    }

    err = bt_le_host_init();
    if (err) {
        goto unregister_gap;
    }

    return ESP_OK;

unregister_gap:
    if (gap_cb_registered) {
        bt_le_gap_app_cb_unregister();
    }
    return ESP_FAIL;
}
