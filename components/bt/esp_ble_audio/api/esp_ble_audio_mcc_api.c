/*
 * SPDX-FileCopyrightText: 2019-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_ble_audio_mcs_defs.h"
#include "esp_ble_audio_mcc_api.h"

#if CONFIG_BT_MCC
esp_err_t esp_ble_audio_mcc_init(esp_ble_audio_mcc_cb_t *cb)
{
    int err;

    err = bt_mcc_init_safe(cb);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_mcc_discover_mcs(uint16_t conn_handle, bool subscribe)
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

    err = bt_mcc_discover_mcs(conn, subscribe);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_mcc_read_player_name(uint16_t conn_handle)
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

    err = bt_mcc_read_player_name(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

#if CONFIG_BT_MCC_READ_MEDIA_PLAYER_ICON_URL
esp_err_t esp_ble_audio_mcc_read_icon_url(uint16_t conn_handle)
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

    err = bt_mcc_read_icon_url(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_MCC_READ_MEDIA_PLAYER_ICON_URL */

#if CONFIG_BT_MCC_READ_TRACK_TITLE
esp_err_t esp_ble_audio_mcc_read_track_title(uint16_t conn_handle)
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

    err = bt_mcc_read_track_title(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_MCC_READ_TRACK_TITLE */

#if CONFIG_BT_MCC_READ_TRACK_DURATION
esp_err_t esp_ble_audio_mcc_read_track_duration(uint16_t conn_handle)
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

    err = bt_mcc_read_track_duration(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_MCC_READ_TRACK_DURATION */

#if CONFIG_BT_MCC_READ_TRACK_POSITION
esp_err_t esp_ble_audio_mcc_read_track_position(uint16_t conn_handle)
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

    err = bt_mcc_read_track_position(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_MCC_READ_TRACK_POSITION */

#if CONFIG_BT_MCC_SET_TRACK_POSITION
esp_err_t esp_ble_audio_mcc_set_track_position(uint16_t conn_handle, int32_t pos)
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

    err = bt_mcc_set_track_position(conn, pos);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_MCC_SET_TRACK_POSITION */

#if CONFIG_BT_MCC_READ_PLAYBACK_SPEED
esp_err_t esp_ble_audio_mcc_read_playback_speed(uint16_t conn_handle)
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

    err = bt_mcc_read_playback_speed(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_MCC_READ_PLAYBACK_SPEED */

#if CONFIG_BT_MCC_SET_PLAYBACK_SPEED
esp_err_t esp_ble_audio_mcc_set_playback_speed(uint16_t conn_handle, int8_t speed)
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

    err = bt_mcc_set_playback_speed(conn, speed);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_MCC_SET_PLAYBACK_SPEED */

#if CONFIG_BT_MCC_READ_SEEKING_SPEED
esp_err_t esp_ble_audio_mcc_read_seeking_speed(uint16_t conn_handle)
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

    err = bt_mcc_read_seeking_speed(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_MCC_READ_SEEKING_SPEED */

#if CONFIG_BT_MCC_READ_PLAYING_ORDER
esp_err_t esp_ble_audio_mcc_read_playing_order(uint16_t conn_handle)
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

    err = bt_mcc_read_playing_order(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_MCC_READ_PLAYING_ORDER */

#if CONFIG_BT_MCC_SET_PLAYING_ORDER
esp_err_t esp_ble_audio_mcc_set_playing_order(uint16_t conn_handle, uint8_t order)
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

    err = bt_mcc_set_playing_order(conn, order);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_MCC_SET_PLAYING_ORDER */

#if CONFIG_BT_MCC_READ_PLAYING_ORDER_SUPPORTED
esp_err_t esp_ble_audio_mcc_read_playing_orders_supported(uint16_t conn_handle)
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

    err = bt_mcc_read_playing_orders_supported(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_MCC_READ_PLAYING_ORDER_SUPPORTED */

#if CONFIG_BT_MCC_READ_MEDIA_STATE
esp_err_t esp_ble_audio_mcc_read_media_state(uint16_t conn_handle)
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

    err = bt_mcc_read_media_state(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_MCC_READ_MEDIA_STATE */

#if CONFIG_BT_MCC_SET_MEDIA_CONTROL_POINT
#define MCS_VALID_OP(opcode) \
        (IN_RANGE((opcode), ESP_BLE_AUDIO_MCS_OPC_PLAY, ESP_BLE_AUDIO_MCS_OPC_STOP) || \
         (opcode == ESP_BLE_AUDIO_MCS_OPC_MOVE_RELATIVE) || \
         IN_RANGE((opcode), ESP_BLE_AUDIO_MCS_OPC_PREV_SEGMENT, ESP_BLE_AUDIO_MCS_OPC_GOTO_SEGMENT) || \
         IN_RANGE((opcode), ESP_BLE_AUDIO_MCS_OPC_PREV_TRACK, ESP_BLE_AUDIO_MCS_OPC_GOTO_TRACK) || \
         IN_RANGE((opcode), ESP_BLE_AUDIO_MCS_OPC_PREV_GROUP, ESP_BLE_AUDIO_MCS_OPC_GOTO_GROUP))

esp_err_t esp_ble_audio_mcc_send_cmd(uint16_t conn_handle,
                                     const esp_ble_audio_mpl_cmd_t *cmd)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    if (cmd == NULL || MCS_VALID_OP(cmd->opcode) == false) {
        return ESP_ERR_INVALID_ARG;
    }

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_mcc_send_cmd(conn, cmd);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_MCC_SET_MEDIA_CONTROL_POINT */

#if CONFIG_BT_MCC_READ_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED
esp_err_t esp_ble_audio_mcc_read_opcodes_supported(uint16_t conn_handle)
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

    err = bt_mcc_read_opcodes_supported(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_MCC_READ_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED */

#if CONFIG_BT_MCC_READ_CONTENT_CONTROL_ID
esp_err_t esp_ble_audio_mcc_read_content_control_id(uint16_t conn_handle)
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

    err = bt_mcc_read_content_control_id(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_MCC_READ_CONTENT_CONTROL_ID */

#if CONFIG_BT_MCC_OTS
esp_err_t esp_ble_audio_mcc_read_icon_obj_id(uint16_t conn_handle)
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

    err = bt_mcc_read_icon_obj_id(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_mcc_read_segments_obj_id(uint16_t conn_handle)
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

    err = bt_mcc_read_segments_obj_id(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_mcc_read_current_track_obj_id(uint16_t conn_handle)
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

    err = bt_mcc_read_current_track_obj_id(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_mcc_set_current_track_obj_id(uint16_t conn_handle, uint64_t id)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    if (ESP_BLE_AUDIO_MCS_VALID_OBJ_ID(id) == false) {
        return ESP_ERR_INVALID_ARG;
    }

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_mcc_set_current_track_obj_id(conn, id);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_mcc_read_next_track_obj_id(uint16_t conn_handle)
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

    err = bt_mcc_read_next_track_obj_id(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_mcc_set_next_track_obj_id(uint16_t conn_handle, uint64_t id)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    if (ESP_BLE_AUDIO_MCS_VALID_OBJ_ID(id) == false) {
        return ESP_ERR_INVALID_ARG;
    }

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_mcc_set_next_track_obj_id(conn, id);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_mcc_read_current_group_obj_id(uint16_t conn_handle)
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

    err = bt_mcc_read_current_group_obj_id(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_mcc_set_current_group_obj_id(uint16_t conn_handle, uint64_t id)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    if (ESP_BLE_AUDIO_MCS_VALID_OBJ_ID(id) == false) {
        return ESP_ERR_INVALID_ARG;
    }

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_mcc_set_current_group_obj_id(conn, id);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_mcc_read_parent_group_obj_id(uint16_t conn_handle)
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

    err = bt_mcc_read_parent_group_obj_id(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_mcc_send_search(uint16_t conn_handle,
                                        const esp_ble_audio_mpl_search_t *search)
{
    esp_err_t ret = ESP_OK;
    void *conn;
    int err;

    if (search == NULL || IN_RANGE(search->len,
                                   ESP_BLE_AUDIO_SEARCH_LEN_MIN,
                                   ESP_BLE_AUDIO_SEARCH_LEN_MAX) == false) {
        return ESP_ERR_INVALID_ARG;
    }

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto unlock;
    }

    err = bt_mcc_send_search(conn, search);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_mcc_read_search_results_obj_id(uint16_t conn_handle)
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

    err = bt_mcc_read_search_results_obj_id(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_mcc_otc_read_object_metadata(uint16_t conn_handle)
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

    err = bt_mcc_otc_read_object_metadata(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_mcc_otc_read_icon_object(uint16_t conn_handle)
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

    err = bt_mcc_otc_read_icon_object(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_mcc_otc_read_track_segments_object(uint16_t conn_handle)
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

    err = bt_mcc_otc_read_track_segments_object(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_mcc_otc_read_current_track_object(uint16_t conn_handle)
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

    err = bt_mcc_otc_read_current_track_object(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_mcc_otc_read_next_track_object(uint16_t conn_handle)
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

    err = bt_mcc_otc_read_next_track_object(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_mcc_otc_read_current_group_object(uint16_t conn_handle)
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

    err = bt_mcc_otc_read_current_group_object(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

esp_err_t esp_ble_audio_mcc_otc_read_parent_group_object(uint16_t conn_handle)
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

    err = bt_mcc_otc_read_parent_group_object(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}

#if CONFIG_BT_MCC_SHELL
struct bt_ots_client *esp_ble_audio_mcc_otc_inst(uint16_t conn_handle)
{
    struct bt_ots_client *ret = NULL;
    void *conn;

    bt_le_host_lock();

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL) {
        goto unlock;
    }

    ret = bt_mcc_otc_inst(conn);

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_MCC_SHELL */
#endif /* CONFIG_BT_MCC_OTS */
#endif /* CONFIG_BT_MCC */
