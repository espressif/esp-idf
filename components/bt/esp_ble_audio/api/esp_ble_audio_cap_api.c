/*
 * SPDX-FileCopyrightText: 2022-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_ble_audio_cap_api.h"

#if CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER
esp_err_t esp_ble_audio_cap_acceptor_register(const esp_ble_audio_csip_set_member_register_param_t *param,
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

    err = bt_cap_acceptor_register_safe(param, svc_inst);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER */

#if CONFIG_BT_CAP
esp_err_t esp_ble_audio_cap_stream_ops_register(esp_ble_audio_cap_stream_t *stream,
                                                esp_ble_audio_bap_stream_ops_t *ops)
{
    if (stream == NULL || ops == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    bt_cap_stream_ops_register_safe(stream, ops);

    return ESP_OK;
}

#if CONFIG_BT_AUDIO_TX
esp_err_t esp_ble_audio_cap_stream_send(esp_ble_audio_cap_stream_t *stream,
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

    err = bt_cap_stream_send_safe(stream, &buf, seq_num);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_cap_stream_send_ts(esp_ble_audio_cap_stream_t *stream,
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

    err = bt_cap_stream_send_ts_safe(stream, &buf, seq_num, ts);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_cap_stream_get_tx_sync(esp_ble_audio_cap_stream_t *stream,
                                               esp_ble_iso_tx_info_t *info)
{
    int err;

    if (stream == NULL || info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_stream_get_tx_sync_safe(stream, info);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_AUDIO_TX */
#endif /* CONFIG_BT_CAP */

#if CONFIG_BT_BAP_UNICAST_CLIENT
esp_err_t esp_ble_audio_cap_unicast_group_create(const esp_ble_audio_cap_unicast_group_param_t *param,
                                                 esp_ble_audio_cap_unicast_group_t **unicast_group)
{
    int err;

    if (param == NULL || unicast_group == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_unicast_group_create_safe(param, unicast_group);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_cap_unicast_group_reconfig(esp_ble_audio_cap_unicast_group_t *unicast_group,
                                                   const esp_ble_audio_cap_unicast_group_param_t *param)
{
    int err;

    if (unicast_group == NULL || param == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_unicast_group_reconfig_safe(unicast_group, param);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_cap_unicast_group_add_streams(esp_ble_audio_cap_unicast_group_t *unicast_group,
                                                      const esp_ble_audio_cap_unicast_group_stream_pair_param_t params[],
                                                      size_t num_param)
{
    int err;

    if (unicast_group == NULL || params == NULL || num_param == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_unicast_group_add_streams_safe(unicast_group, params, num_param);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_cap_unicast_group_delete(esp_ble_audio_cap_unicast_group_t *unicast_group)
{
    int err;

    if (unicast_group == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_unicast_group_delete_safe(unicast_group);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_BAP_UNICAST_CLIENT */

#if CONFIG_BT_CAP_INITIATOR
esp_err_t esp_ble_audio_cap_initiator_register_cb(const esp_ble_audio_cap_initiator_cb_t *cb)
{
    int err;

    if (cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_initiator_register_cb_safe(cb);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_cap_initiator_unregister_cb(const esp_ble_audio_cap_initiator_cb_t *cb)
{
    int err;

    if (cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_initiator_unregister_cb_safe(cb);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

#if CONFIG_BT_BAP_UNICAST_CLIENT
esp_err_t esp_ble_audio_cap_initiator_unicast_discover(uint16_t conn_handle)
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

    err = bt_cap_initiator_unicast_discover(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_cap_initiator_unicast_audio_start(const esp_ble_audio_cap_unicast_audio_start_param_t *param)
{
    int err;

    if (param == NULL || param->count == 0 || param->stream_params == NULL ||
            param->count > CONFIG_BT_BAP_UNICAST_CLIENT_GROUP_STREAM_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_initiator_unicast_audio_start_safe(param);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_cap_initiator_unicast_audio_update(const esp_ble_audio_cap_unicast_audio_update_param_t *param)
{
    int err;

    if (param == NULL || param->count == 0 || param->stream_params == NULL ||
            param->count > CONFIG_BT_BAP_UNICAST_CLIENT_GROUP_STREAM_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_initiator_unicast_audio_update_safe(param);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_cap_initiator_unicast_audio_stop(const esp_ble_audio_cap_unicast_audio_stop_param_t *param)
{
    int err;

    if (param == NULL || param->count == 0 || param->streams == NULL ||
            param->count > CONFIG_BT_BAP_UNICAST_CLIENT_GROUP_STREAM_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_initiator_unicast_audio_stop_safe(param);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_cap_initiator_unicast_audio_cancel(void)
{
    int err;

    err = bt_cap_initiator_unicast_audio_cancel_safe();
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_BAP_UNICAST_CLIENT */

#if CONFIG_BT_BAP_BROADCAST_SOURCE
esp_err_t esp_ble_audio_cap_initiator_broadcast_audio_create(const esp_ble_audio_cap_initiator_broadcast_create_param_t *param,
                                                             esp_ble_audio_cap_broadcast_source_t **broadcast_source)
{
    int err;

    if (param == NULL || broadcast_source == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_initiator_broadcast_audio_create_safe(param, broadcast_source);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_cap_initiator_broadcast_audio_start(esp_ble_audio_cap_broadcast_source_t *broadcast_source,
                                                            uint8_t adv_handle)
{
    esp_err_t ret = ESP_OK;
    void *adv;
    int err;

    if (broadcast_source == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    bt_le_host_lock();

    adv = bt_le_ext_adv_find(adv_handle);
    if (adv == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_cap_initiator_broadcast_audio_start(broadcast_source, adv);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_cap_initiator_broadcast_audio_update(esp_ble_audio_cap_broadcast_source_t *broadcast_source,
                                                             const uint8_t meta[], size_t meta_len)
{
    int err;

    if (broadcast_source == NULL || meta == NULL || meta_len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_initiator_broadcast_audio_update_safe(broadcast_source, meta, meta_len);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_cap_initiator_broadcast_audio_stop(esp_ble_audio_cap_broadcast_source_t *broadcast_source)
{
    int err;

    if (broadcast_source == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_initiator_broadcast_audio_stop_safe(broadcast_source);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_cap_initiator_broadcast_audio_delete(esp_ble_audio_cap_broadcast_source_t *broadcast_source)
{
    int err;

    if (broadcast_source == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_initiator_broadcast_audio_delete_safe(broadcast_source);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_cap_initiator_broadcast_get_base(esp_ble_audio_cap_broadcast_source_t *broadcast_source,
                                                         struct net_buf_simple *base_buf)
{
    int err;

    if (broadcast_source == NULL || base_buf == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_initiator_broadcast_get_base_safe(broadcast_source, base_buf);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_BAP_BROADCAST_SOURCE */
#endif /* CONFIG_BT_CAP_INITIATOR */

#if CONFIG_BT_CAP_HANDOVER
esp_err_t esp_ble_audio_cap_handover_register_cb(const esp_ble_audio_cap_handover_cb_t *cb)
{
    int err;

    if (cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_handover_register_cb_safe(cb);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_cap_handover_unregister_cb(const esp_ble_audio_cap_handover_cb_t *cb)
{
    int err;

    if (cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_handover_unregister_cb_safe(cb);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_cap_handover_unicast_to_broadcast(
    const esp_ble_audio_cap_handover_unicast_to_broadcast_param_t *param)
{
    int err;

    if (param == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_handover_unicast_to_broadcast_safe(param);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_cap_handover_broadcast_to_unicast(
    const esp_ble_audio_cap_handover_broadcast_to_unicast_param_t *param)
{
    int err;

    if (param == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_handover_broadcast_to_unicast_safe(param);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_CAP_HANDOVER */

#if CONFIG_BT_CAP_COMMANDER
esp_err_t esp_ble_audio_cap_commander_register_cb(const esp_ble_audio_cap_commander_cb_t *cb)
{
    int err;

    if (cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_commander_register_cb_safe(cb);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_cap_commander_unregister_cb(const esp_ble_audio_cap_commander_cb_t *cb)
{
    int err;

    if (cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_commander_unregister_cb_safe(cb);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_cap_commander_discover(uint16_t conn_handle)
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

    err = bt_cap_commander_discover(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_cap_commander_cancel(void)
{
    int err;

    err = bt_cap_commander_cancel_safe();
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

#if CONFIG_BT_BAP_BROADCAST_ASSISTANT
esp_err_t esp_ble_audio_cap_commander_broadcast_reception_start(
    const esp_ble_audio_cap_commander_broadcast_reception_start_param_t *param)
{
    int err;

    if (param == NULL || param->count == 0 ||
            param->count > CONFIG_BT_MAX_CONN || param->param == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_commander_broadcast_reception_start_safe(param);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_cap_commander_broadcast_reception_stop(
    const esp_ble_audio_cap_commander_broadcast_reception_stop_param_t *param)
{
    int err;

    if (param == NULL || param->count == 0 ||
            param->count > CONFIG_BT_MAX_CONN || param->param == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_commander_broadcast_reception_stop_safe(param);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_cap_commander_distribute_broadcast_code(
    const esp_ble_audio_cap_commander_distribute_broadcast_code_param_t *param)
{
    int err;

    if (param == NULL || param->count == 0 ||
        param->count > CONFIG_BT_MAX_CONN || param->param == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_commander_distribute_broadcast_code_safe(param);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_BAP_BROADCAST_ASSISTANT */

#if CONFIG_BT_VCP_VOL_CTLR
esp_err_t esp_ble_audio_cap_commander_change_volume(
    const esp_ble_audio_cap_commander_change_volume_param_t *param)
{
    int err;

    if (param == NULL || param->count == 0 ||
            param->count > CONFIG_BT_MAX_CONN || param->members == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_commander_change_volume_safe(param);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

#if CONFIG_BT_VCP_VOL_CTLR_VOCS
esp_err_t esp_ble_audio_cap_commander_change_volume_offset(
    const esp_ble_audio_cap_commander_change_volume_offset_param_t *param)
{
    int err;

    if (param == NULL || param->count == 0 ||
            param->count > CONFIG_BT_MAX_CONN || param->param == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_commander_change_volume_offset_safe(param);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_VCP_VOL_CTLR_VOCS */

esp_err_t esp_ble_audio_cap_commander_change_volume_mute_state(
    const esp_ble_audio_cap_commander_change_volume_mute_state_param_t *param)
{
    int err;

    if (param == NULL || param->count == 0 ||
            param->count > CONFIG_BT_MAX_CONN || param->members == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_commander_change_volume_mute_state_safe(param);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_VCP_VOL_CTLR */

#if CONFIG_BT_MICP_MIC_CTLR
esp_err_t esp_ble_audio_cap_commander_change_microphone_mute_state(
    const esp_ble_audio_cap_commander_change_microphone_mute_state_param_t *param)
{
    int err;

    if (param == NULL || param->count == 0 ||
            param->count > CONFIG_BT_MAX_CONN || param->members == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_commander_change_microphone_mute_state_safe(param);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

#if CONFIG_BT_MICP_MIC_CTLR_AICS
esp_err_t esp_ble_audio_cap_commander_change_microphone_gain_setting(
    const esp_ble_audio_cap_commander_change_microphone_gain_setting_param_t *param)
{
    int err;

    if (param == NULL || param->count == 0 ||
            param->count > CONFIG_BT_MAX_CONN || param->param == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_cap_commander_change_microphone_gain_setting_safe(param);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_MICP_MIC_CTLR_AICS */
#endif /* CONFIG_BT_MICP_MIC_CTLR */
#endif /* CONFIG_BT_CAP_COMMANDER */
