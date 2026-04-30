/*
 * SPDX-FileCopyrightText: 2020 Bose Corporation
 * SPDX-FileCopyrightText: 2021-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_ble_audio_bap_api.h"

#if CONFIG_BT_BAP_UNICAST_SERVER
esp_err_t esp_ble_audio_bap_unicast_server_register(const esp_ble_audio_bap_unicast_server_register_param_t *param)
{
    int err;

    if (param == NULL ||
            param->snk_cnt > CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT ||
            param->src_cnt > CONFIG_BT_ASCS_MAX_ASE_SRC_COUNT ||
            (param->snk_cnt == 0 && param->src_cnt == 0)) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_unicast_server_register_safe(param);
    if (err) {
        return ESP_FAIL;
    }

#if BLE_AUDIO_SVC_SEP_ADD
    err = bt_le_ascs_init();
    if (err) {
        bt_bap_unicast_server_unregister_safe();
        return ESP_FAIL;
    }
#endif /* BLE_AUDIO_SVC_SEP_ADD */

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_unicast_server_unregister(void)
{
    int err;

    err = bt_bap_unicast_server_unregister_safe();
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_unicast_server_register_cb(const esp_ble_audio_bap_unicast_server_cb_t *cb)
{
    int err;

    if (cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_unicast_server_register_cb_safe(cb);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_unicast_server_unregister_cb(const esp_ble_audio_bap_unicast_server_cb_t *cb)
{
    int err;

    if (cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_unicast_server_unregister_cb_safe(cb);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_unicast_server_config_ase(uint16_t conn_handle,
                                                      esp_ble_audio_bap_stream_t *stream,
                                                      esp_ble_audio_codec_cfg_t *codec_cfg,
                                                      const esp_ble_audio_bap_qos_cfg_pref_t *qos_pref)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    if (stream == NULL || codec_cfg == NULL || qos_pref == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_bap_unicast_server_config_ase(conn, stream, codec_cfg, qos_pref);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_BAP_UNICAST_SERVER */

#if CONFIG_BT_BAP_UNICAST_CLIENT
esp_err_t esp_ble_audio_bap_unicast_group_create(esp_ble_audio_bap_unicast_group_param_t *param,
                                                 esp_ble_audio_bap_unicast_group_t **unicast_group)
{
    int err;

    if (param == NULL || unicast_group == NULL ||
            param->params == NULL ||
            param->params_count == 0 ||
            param->params_count > CONFIG_BT_BAP_UNICAST_CLIENT_GROUP_STREAM_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_unicast_group_create_safe(param, unicast_group);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_unicast_group_reconfig(esp_ble_audio_bap_unicast_group_t *unicast_group,
                                                   const esp_ble_audio_bap_unicast_group_param_t *param)
{
    int err;

    if (unicast_group == NULL || param == NULL ||
            param->params_count > CONFIG_BT_BAP_UNICAST_CLIENT_GROUP_STREAM_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_unicast_group_reconfig_safe(unicast_group, param);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_unicast_group_add_streams(esp_ble_audio_bap_unicast_group_t *unicast_group,
                                                      esp_ble_audio_bap_unicast_group_stream_pair_param_t params[],
                                                      size_t num_param)
{
    int err;

    if (unicast_group == NULL || params == NULL || num_param == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_unicast_group_add_streams_safe(unicast_group, params, num_param);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_unicast_group_delete(esp_ble_audio_bap_unicast_group_t *unicast_group)
{
    int err;

    if (unicast_group == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_unicast_group_delete_safe(unicast_group);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_unicast_client_register_cb(esp_ble_audio_bap_unicast_client_cb_t *cb)
{
    int err;

    if (cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_unicast_client_register_cb_safe(cb);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_unicast_client_unregister_cb(esp_ble_audio_bap_unicast_client_cb_t *cb)
{
    int err;

    if (cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_unicast_client_unregister_cb_safe(cb);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_unicast_client_discover(uint16_t conn_handle, esp_ble_audio_dir_t dir)
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

    err = bt_bap_unicast_client_discover(conn, dir);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_bap_stream_config(uint16_t conn_handle,
                                          esp_ble_audio_bap_stream_t *stream,
                                          esp_ble_audio_bap_ep_t *ep,
                                          esp_ble_audio_codec_cfg_t *codec_cfg)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    if (stream == NULL || ep == NULL || codec_cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_bap_stream_config(conn, stream, ep, codec_cfg);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_bap_stream_qos(uint16_t conn_handle,
                                       esp_ble_audio_bap_unicast_group_t *group)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    if (group == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_bap_stream_qos(conn, group);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_bap_stream_enable(esp_ble_audio_bap_stream_t *stream,
                                          const uint8_t meta[], size_t meta_len)
{
    int err;

    if (stream == NULL || stream->ep == NULL ||
            stream->conn == NULL || meta == NULL || meta_len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_stream_enable_safe(stream, meta, meta_len);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_stream_connect(esp_ble_audio_bap_stream_t *stream)
{
    int err;

    if (stream == NULL || stream->ep == NULL || stream->conn == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_stream_connect_safe(stream);
    if (err) {
        return (err == -EALREADY) ? ESP_OK : ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_stream_stop(esp_ble_audio_bap_stream_t *stream)
{
    int err;

    if (stream == NULL || stream->ep == NULL || stream->conn == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_stream_stop_safe(stream);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_BAP_UNICAST_CLIENT */

#if CONFIG_BT_BAP_UNICAST
esp_err_t esp_ble_audio_bap_stream_reconfig(esp_ble_audio_bap_stream_t *stream,
                                            esp_ble_audio_codec_cfg_t *codec_cfg)
{
    int err;

    if (stream == NULL || stream->ep == NULL ||
            stream->conn == NULL || codec_cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_stream_reconfig_safe(stream, codec_cfg);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_stream_metadata(esp_ble_audio_bap_stream_t *stream,
                                            const uint8_t meta[], size_t meta_len)
{
    int err;

    if (stream == NULL || stream->ep == NULL ||
            stream->conn == NULL || meta == NULL || meta_len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_stream_metadata_safe(stream, meta, meta_len);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_stream_disable(esp_ble_audio_bap_stream_t *stream)
{
    int err;

    if (stream == NULL || stream->ep == NULL || stream->conn == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_stream_disable_safe(stream);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_stream_start(esp_ble_audio_bap_stream_t *stream)
{
    int err;

    if (stream == NULL || stream->ep == NULL || stream->conn == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_stream_start_safe(stream);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_stream_release(esp_ble_audio_bap_stream_t *stream)
{
    int err;

    if (stream == NULL || stream->ep == NULL || stream->conn == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_stream_release_safe(stream);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_BAP_UNICAST */

#if CONFIG_BT_BAP_BROADCAST_SOURCE
esp_err_t esp_ble_audio_bap_broadcast_source_register_cb(esp_ble_audio_bap_broadcast_source_cb_t *cb)
{
    int err;

    if (cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_broadcast_source_register_cb_safe(cb);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_broadcast_source_unregister_cb(esp_ble_audio_bap_broadcast_source_cb_t *cb)
{
    int err;

    if (cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_broadcast_source_unregister_cb_safe(cb);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_broadcast_source_create(esp_ble_audio_bap_broadcast_source_param_t *param,
                                                    esp_ble_audio_bap_broadcast_source_t **source)
{
    int err;

    if (param == NULL || source == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_broadcast_source_create_safe(param, source);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_broadcast_source_reconfig(esp_ble_audio_bap_broadcast_source_t *source,
                                                      esp_ble_audio_bap_broadcast_source_param_t *param)
{
    int err;

    if (source == NULL || param == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_broadcast_source_reconfig_safe(source, param);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_broadcast_source_update_metadata(esp_ble_audio_bap_broadcast_source_t *source,
                                                             const uint8_t meta[], size_t meta_len)
{
    int err;

    if (source == NULL || meta == NULL || meta_len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_broadcast_source_update_metadata_safe(source, meta, meta_len);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_broadcast_adv_add(esp_ble_audio_bap_broadcast_adv_info_t *info)
{
    if (info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    return esp_ble_iso_big_ext_adv_add(info);
}

esp_err_t esp_ble_audio_bap_broadcast_adv_delete(esp_ble_audio_bap_broadcast_adv_info_t *info)
{
    if (info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    return esp_ble_iso_big_ext_adv_delete(info);
}

esp_err_t esp_ble_audio_bap_broadcast_source_start(esp_ble_audio_bap_broadcast_source_t *source,
                                                   uint8_t adv_handle)
{
    esp_err_t ret = ESP_OK;
    void *adv;
    int err;

    if (source == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    bt_le_host_lock();

    adv = bt_le_ext_adv_find(adv_handle);
    if (adv == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_bap_broadcast_source_start(source, adv);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_bap_broadcast_source_stop(esp_ble_audio_bap_broadcast_source_t *source)
{
    int err;

    if (source == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_broadcast_source_stop_safe(source);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_broadcast_source_delete(esp_ble_audio_bap_broadcast_source_t *source)
{
    int err;

    if (source == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_broadcast_source_delete_safe(source);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_broadcast_source_get_base(esp_ble_audio_bap_broadcast_source_t *source,
                                                      struct net_buf_simple *base_buf)
{
    int err;

    if (source == NULL || base_buf == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_broadcast_source_get_base_safe(source, base_buf);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_BAP_BROADCAST_SOURCE */

#if CONFIG_BT_BAP_BROADCAST_SINK
esp_err_t esp_ble_audio_bap_broadcast_sink_register_cb(esp_ble_audio_bap_broadcast_sink_cb_t *cb)
{
    int err;

    if (cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_broadcast_sink_register_cb_safe(cb);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_broadcast_sink_create(uint16_t sync_handle,
                                                  uint32_t broadcast_id,
                                                  esp_ble_audio_bap_broadcast_sink_t **sink)
{
    void *per_adv_sync;
    int err;

    if (broadcast_id > ESP_BLE_AUDIO_BROADCAST_ID_MAX || sink == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    per_adv_sync = bt_le_per_adv_sync_find_safe(sync_handle);
    if (per_adv_sync == NULL) {
        return ESP_ERR_NOT_FOUND;
    }

    err = bt_bap_broadcast_sink_create_safe(per_adv_sync, broadcast_id, sink);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_broadcast_sink_sync(esp_ble_audio_bap_broadcast_sink_t *sink,
                                                uint32_t indexes_bitfield,
                                                esp_ble_audio_bap_stream_t *streams[],
                                                const uint8_t broadcast_code[ESP_BLE_ISO_BROADCAST_CODE_SIZE])
{
    int err;

    if (sink == NULL || indexes_bitfield == 0U ||
            indexes_bitfield > BIT_MASK(ESP_BLE_ISO_BIS_INDEX_MAX) || streams == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_broadcast_sink_sync_safe(sink, indexes_bitfield, streams, broadcast_code);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_broadcast_sink_stop(esp_ble_audio_bap_broadcast_sink_t *sink)
{
    int err;

    if (sink == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_broadcast_sink_stop_safe(sink);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_broadcast_sink_delete(esp_ble_audio_bap_broadcast_sink_t *sink)
{
    int err;

    if (sink == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_broadcast_sink_delete_safe(sink);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_BAP_BROADCAST_SINK */

#if CONFIG_BT_BAP_SCAN_DELEGATOR
esp_err_t esp_ble_audio_bap_scan_delegator_register(esp_ble_audio_bap_scan_delegator_cb_t *cb)
{
    int err;

    err = bt_bap_scan_delegator_register_safe(cb);
    if (err) {
        return ESP_FAIL;
    }

#if BLE_AUDIO_SVC_SEP_ADD
    err = bt_le_bass_init();
    if (err) {
        bt_bap_scan_delegator_unregister_safe();
        return ESP_FAIL;
    }
#endif /* BLE_AUDIO_SVC_SEP_ADD */

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_scan_delegator_unregister(void)
{
    int err;

    err = bt_bap_scan_delegator_unregister_safe();
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_scan_delegator_set_pa_state(uint8_t src_id,
                                                        esp_ble_audio_bap_pa_state_t pa_state)
{
    int err;

    err = bt_bap_scan_delegator_set_pa_state_safe(src_id, pa_state);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_scan_delegator_set_bis_sync_state(uint8_t src_id,
                                                              uint32_t bis_synced[CONFIG_BT_BAP_BASS_MAX_SUBGROUPS])
{
    int err;

    if (bis_synced == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_scan_delegator_set_bis_sync_state_safe(src_id, bis_synced);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_scan_delegator_add_src(const esp_ble_audio_bap_scan_delegator_add_src_param_t *param,
                                                   uint8_t *src_id)
{
    int ret;

    if (param == NULL || src_id == NULL ||
            param->broadcast_id > ESP_BLE_AUDIO_BROADCAST_ID_MAX ||
            param->addr.type > BT_ADDR_LE_RANDOM ||
            param->sid > BT_GAP_SID_MAX ||
            param->pa_state > ESP_BLE_AUDIO_BAP_PA_STATE_NO_PAST ||
            param->encrypt_state > ESP_BLE_AUDIO_BAP_BIG_ENC_STATE_BAD_CODE ||
            param->num_subgroups > CONFIG_BT_BAP_BASS_MAX_SUBGROUPS) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_bap_scan_delegator_add_src_safe(param);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *src_id = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_scan_delegator_mod_src(const esp_ble_audio_bap_scan_delegator_mod_src_param_t *param)
{
    int err;

    if (param == NULL ||
            param->broadcast_id > ESP_BLE_AUDIO_BROADCAST_ID_MAX ||
            param->num_subgroups > CONFIG_BT_BAP_BASS_MAX_SUBGROUPS) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_scan_delegator_mod_src_safe(param);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_scan_delegator_rem_src(uint8_t src_id)
{
    int err;

    err = bt_bap_scan_delegator_rem_src_safe(src_id);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_BAP_SCAN_DELEGATOR */

#if CONFIG_BT_BAP_BROADCAST_ASSISTANT
esp_err_t esp_ble_audio_bap_broadcast_assistant_discover(uint16_t conn_handle)
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

    err = bt_bap_broadcast_assistant_discover(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_bap_broadcast_assistant_scan_start(uint16_t conn_handle, bool start_scan)
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

    err = bt_bap_broadcast_assistant_scan_start(conn, start_scan);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_bap_broadcast_assistant_scan_stop(uint16_t conn_handle)
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

    err = bt_bap_broadcast_assistant_scan_stop(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_bap_broadcast_assistant_register_cb(esp_ble_audio_bap_broadcast_assistant_cb_t *cb)
{
    int err;

    if (cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_broadcast_assistant_register_cb_safe(cb);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_broadcast_assistant_unregister_cb(esp_ble_audio_bap_broadcast_assistant_cb_t *cb)
{
    int err;

    if (cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_broadcast_assistant_unregister_cb_safe(cb);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_broadcast_assistant_add_src(uint16_t conn_handle,
                                                        const esp_ble_audio_bap_broadcast_assistant_add_src_param_t *param)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    if (param == NULL ||
            param->broadcast_id > ESP_BLE_AUDIO_BROADCAST_ID_MAX ||
            param->addr.type > BT_ADDR_LE_RANDOM ||
            param->adv_sid > BT_GAP_SID_MAX ||
            param->num_subgroups > CONFIG_BT_BAP_BASS_MAX_SUBGROUPS) {
        return ESP_ERR_INVALID_ARG;
    }

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_bap_broadcast_assistant_add_src(conn, param);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_bap_broadcast_assistant_mod_src(uint16_t conn_handle,
                                                        const esp_ble_audio_bap_broadcast_assistant_mod_src_param_t *param)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    if (param == NULL ||
            param->num_subgroups > CONFIG_BT_BAP_BASS_MAX_SUBGROUPS) {
        return ESP_ERR_INVALID_ARG;
    }

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_bap_broadcast_assistant_mod_src(conn, param);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_bap_broadcast_assistant_set_broadcast_code(uint16_t conn_handle, uint8_t src_id,
                                                                   const uint8_t broadcast_code[ESP_BLE_ISO_BROADCAST_CODE_SIZE])
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    if (broadcast_code == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_bap_broadcast_assistant_set_broadcast_code(conn, src_id, broadcast_code);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_bap_broadcast_assistant_rem_src(uint16_t conn_handle, uint8_t src_id)
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

    err = bt_bap_broadcast_assistant_rem_src(conn, src_id);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_bap_broadcast_assistant_read_recv_state(uint16_t conn_handle, uint8_t idx)
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

    err = bt_bap_broadcast_assistant_read_recv_state(conn, idx);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_BAP_BROADCAST_ASSISTANT */

#if CONFIG_BT_BAP_STREAM
esp_err_t esp_ble_audio_bap_ep_get_info(const esp_ble_audio_bap_ep_t *ep,
                                        esp_ble_audio_bap_ep_info_t *info)
{
    int err;

    if (ep == NULL || info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_ep_get_info_safe(ep, info);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_stream_cb_register(esp_ble_audio_bap_stream_t *stream,
                                               esp_ble_audio_bap_stream_ops_t *ops)
{
    if (stream == NULL || ops == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    bt_bap_stream_cb_register_safe(stream, ops);

    return ESP_OK;
}
#endif /* CONFIG_BT_BAP_STREAM */

#if CONFIG_BT_AUDIO_TX
esp_err_t esp_ble_audio_bap_stream_send(esp_ble_audio_bap_stream_t *stream,
                                        const uint8_t *sdu, uint16_t sdu_len,
                                        uint16_t seq_num)
{
    struct net_buf buf = {
        /* Note: only data and len are used */
        .data = (void *)sdu,
        .len  = sdu_len,
    };
    int err;

    if (stream == NULL || (!sdu ^ !sdu_len)) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_stream_send_safe(stream, &buf, seq_num);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_stream_send_ts(esp_ble_audio_bap_stream_t *stream,
                                           const uint8_t *sdu, uint16_t sdu_len,
                                           uint16_t seq_num, uint32_t ts)
{
    struct net_buf buf = {
        /* Note: only data and len are used */
        .data = (void *)sdu,
        .len  = sdu_len,
    };
    int err;

    if (stream == NULL || (!sdu ^ !sdu_len)) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_stream_send_ts_safe(stream, &buf, seq_num, ts);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_stream_get_tx_sync(esp_ble_audio_bap_stream_t *stream,
                                               esp_ble_iso_tx_info_t *info)
{
    int err;

    if (stream == NULL || info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_stream_get_tx_sync_safe(stream, info);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_AUDIO_TX */

#if CONFIG_BT_BAP_BASE
const esp_ble_audio_bap_base_t *
esp_ble_audio_bap_base_get_base_from_ad(uint8_t data_type, const uint8_t *data, uint8_t data_len)
{
    struct bt_data ad = {
        .type = data_type,
        .data = data,
        .data_len = data_len,
    };

    if (data_type != BT_DATA_SVC_DATA16 || data == NULL || data_len == 0) {
        return NULL;
    }

    return bt_bap_base_get_base_from_ad(&ad);
}

esp_err_t esp_ble_audio_bap_base_get_size(const esp_ble_audio_bap_base_t *base,
                                          size_t *base_size)
{
    int ret;

    if (base == NULL || base_size == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_bap_base_get_size(base);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *base_size = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_base_get_pres_delay(const esp_ble_audio_bap_base_t *base,
                                                uint32_t *pres_delay)
{
    int ret;

    if (base == NULL || pres_delay == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_bap_base_get_pres_delay(base);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *pres_delay = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_base_get_subgroup_count(const esp_ble_audio_bap_base_t *base,
                                                    uint8_t *subgroup_count)
{
    int ret;

    if (base == NULL || subgroup_count == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_bap_base_get_subgroup_count(base);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *subgroup_count = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_base_get_bis_indexes(const esp_ble_audio_bap_base_t *base,
                                                 uint32_t *bis_indexes)
{
    int err;

    if (base == NULL || bis_indexes == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_base_get_bis_indexes(base, bis_indexes);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_base_foreach_subgroup(const esp_ble_audio_bap_base_t *base,
                                                  bool (*func)(const esp_ble_audio_bap_base_subgroup_t *subgroup,
                                                               void *user_data),
                                                  void *user_data)
{
    int err;

    if (base == NULL || func == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_base_foreach_subgroup(base, func, user_data);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_base_get_subgroup_codec_id(const esp_ble_audio_bap_base_subgroup_t *subgroup,
                                                       esp_ble_audio_bap_base_codec_id_t *codec_id)
{
    int err;

    if (subgroup == NULL || codec_id == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_base_get_subgroup_codec_id(subgroup, codec_id);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_base_get_subgroup_codec_data(const esp_ble_audio_bap_base_subgroup_t *subgroup,
                                                         uint8_t **data)
{
    int err;

    if (subgroup == NULL || data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_base_get_subgroup_codec_data(subgroup, data);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_base_get_subgroup_codec_meta(const esp_ble_audio_bap_base_subgroup_t *subgroup,
                                                         uint8_t **meta)
{
    int err;

    if (subgroup == NULL || meta == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_base_get_subgroup_codec_meta(subgroup, meta);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_base_subgroup_codec_to_codec_cfg(const esp_ble_audio_bap_base_subgroup_t *subgroup,
                                                             esp_ble_audio_codec_cfg_t *codec_cfg)
{
    int err;

    if (subgroup == NULL || codec_cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_base_subgroup_codec_to_codec_cfg(subgroup, codec_cfg);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_base_get_subgroup_bis_count(const esp_ble_audio_bap_base_subgroup_t *subgroup,
                                                        uint8_t *bis_count)
{
    int ret;

    if (subgroup == NULL || bis_count == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_bap_base_get_subgroup_bis_count(subgroup);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *bis_count = ret;

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_base_subgroup_get_bis_indexes(const esp_ble_audio_bap_base_subgroup_t *subgroup,
                                                          uint32_t *bis_indexes)
{
    int err;

    if (subgroup == NULL || bis_indexes == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_base_subgroup_get_bis_indexes(subgroup, bis_indexes);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_base_subgroup_foreach_bis(const esp_ble_audio_bap_base_subgroup_t *subgroup,
                                                      bool (*func)(const esp_ble_audio_bap_base_subgroup_bis_t *bis,
                                                                   void *user_data),
                                                      void *user_data)
{
    int err;

    if (subgroup == NULL || func == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_base_subgroup_foreach_bis(subgroup, func, user_data);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_bap_base_subgroup_bis_codec_to_codec_cfg(const esp_ble_audio_bap_base_subgroup_bis_t *bis,
                                                                 esp_ble_audio_codec_cfg_t *codec_cfg)
{
    int err;

    if (bis == NULL || codec_cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_bap_base_subgroup_bis_codec_to_codec_cfg(bis, codec_cfg);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_BAP_BASE */
