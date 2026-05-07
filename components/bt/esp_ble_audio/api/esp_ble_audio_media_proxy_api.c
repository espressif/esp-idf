/*
 * SPDX-FileCopyrightText: 2019-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_ble_audio_mcs_defs.h"
#include "esp_ble_audio_media_proxy_api.h"

#if CONFIG_BT_MCTL_LOCAL_PLAYER_LOCAL_CONTROL
esp_err_t esp_ble_audio_media_proxy_ctrl_register(esp_ble_audio_media_proxy_ctrl_cbs_t *ctrl_cbs)
{
    int err;

    if (ctrl_cbs == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_register_safe(ctrl_cbs);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_MCTL_LOCAL_PLAYER_LOCAL_CONTROL */

#if CONFIG_BT_MCTL_REMOTE_PLAYER_CONTROL
esp_err_t esp_ble_audio_media_proxy_ctrl_discover_player(uint16_t conn_handle)
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

    err = bt_media_proxy_ctrl_discover_player(conn);
    if (err) {
        ret = ESP_FAIL;
    }

unlock:
    bt_le_host_unlock();
    return ret;
}
#endif /* CONFIG_BT_MCTL_REMOTE_PLAYER_CONTROL */

#if CONFIG_BT_MCTL_LOCAL_PLAYER_LOCAL_CONTROL || CONFIG_BT_MCTL_REMOTE_PLAYER_CONTROL
esp_err_t esp_ble_audio_media_proxy_ctrl_get_player_name(esp_ble_audio_media_player_t *player)
{
    int err;

    if (player == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_get_player_name_safe(player);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_media_proxy_ctrl_get_icon_id(esp_ble_audio_media_player_t *player)
{
    int err;

    if (player == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_get_icon_id_safe(player);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_media_proxy_ctrl_get_icon_url(esp_ble_audio_media_player_t *player)
{
    int err;

    if (player == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_get_icon_url_safe(player);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_media_proxy_ctrl_get_track_title(esp_ble_audio_media_player_t *player)
{
    int err;

    if (player == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_get_track_title_safe(player);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_media_proxy_ctrl_get_track_duration(esp_ble_audio_media_player_t *player)
{
    int err;

    if (player == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_get_track_duration_safe(player);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_media_proxy_ctrl_get_track_position(esp_ble_audio_media_player_t *player)
{
    int err;

    if (player == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_get_track_position_safe(player);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_media_proxy_ctrl_set_track_position(esp_ble_audio_media_player_t *player,
                                                            int32_t position)
{
    int err;

    if (player == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_set_track_position_safe(player, position);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_media_proxy_ctrl_get_playback_speed(esp_ble_audio_media_player_t *player)
{
    int err;

    if (player == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_get_playback_speed_safe(player);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_media_proxy_ctrl_set_playback_speed(esp_ble_audio_media_player_t *player,
                                                            int8_t speed)
{
    int err;

    if (player == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_set_playback_speed_safe(player, speed);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_media_proxy_ctrl_get_seeking_speed(esp_ble_audio_media_player_t *player)
{
    int err;

    if (player == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_get_seeking_speed_safe(player);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_media_proxy_ctrl_get_track_segments_id(esp_ble_audio_media_player_t *player)
{
    int err;

    if (player == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_get_track_segments_id_safe(player);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_media_proxy_ctrl_get_current_track_id(esp_ble_audio_media_player_t *player)
{
    int err;

    if (player == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_get_current_track_id_safe(player);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_media_proxy_ctrl_set_current_track_id(esp_ble_audio_media_player_t *player,
                                                              uint64_t id)
{
    int err;

    if (player == NULL || (IS_ENABLED(CONFIG_BT_MCTL_LOCAL_PLAYER_LOCAL_CONTROL) &&
                           ESP_BLE_AUDIO_MCS_VALID_OBJ_ID(id) == false)) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_set_current_track_id_safe(player, id);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_media_proxy_ctrl_get_next_track_id(esp_ble_audio_media_player_t *player)
{
    int err;

    if (player == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_get_next_track_id_safe(player);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_media_proxy_ctrl_set_next_track_id(esp_ble_audio_media_player_t *player,
                                                           uint64_t id)
{
    int err;

    if (player == NULL || ESP_BLE_AUDIO_MCS_VALID_OBJ_ID(id) == false) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_set_next_track_id_safe(player, id);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_media_proxy_ctrl_get_parent_group_id(esp_ble_audio_media_player_t *player)
{
    int err;

    if (player == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_get_parent_group_id_safe(player);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_media_proxy_ctrl_get_current_group_id(esp_ble_audio_media_player_t *player)
{
    int err;

    if (player == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_get_current_group_id_safe(player);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_media_proxy_ctrl_set_current_group_id(esp_ble_audio_media_player_t *player,
                                                              uint64_t id)
{
    int err;

    if (player == NULL || (IS_ENABLED(CONFIG_BT_MCTL_LOCAL_PLAYER_LOCAL_CONTROL) &&
                           ESP_BLE_AUDIO_MCS_VALID_OBJ_ID(id) == false)) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_set_current_group_id_safe(player, id);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_media_proxy_ctrl_get_playing_order(esp_ble_audio_media_player_t *player)
{
    int err;

    if (player == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_get_playing_order_safe(player);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_media_proxy_ctrl_set_playing_order(esp_ble_audio_media_player_t *player,
                                                           uint8_t order)
{
    int err;

    if (player == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_set_playing_order_safe(player, order);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_media_proxy_ctrl_get_playing_orders_supported(esp_ble_audio_media_player_t *player)
{
    int err;

    if (player == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_get_playing_orders_supported_safe(player);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_media_proxy_ctrl_get_media_state(esp_ble_audio_media_player_t *player)
{
    int err;

    if (player == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_get_media_state_safe(player);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_media_proxy_ctrl_send_command(esp_ble_audio_media_player_t *player,
                                                      const esp_ble_audio_mpl_cmd_t *command)
{
    int err;

    if (player == NULL || command == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_send_command_safe(player, command);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_media_proxy_ctrl_get_commands_supported(esp_ble_audio_media_player_t *player)
{
    int err;

    if (player == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_get_commands_supported_safe(player);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_media_proxy_ctrl_send_search(esp_ble_audio_media_player_t *player,
                                                     const esp_ble_audio_mpl_search_t *search)
{
    int err;

    if (player == NULL || search == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_send_search_safe(player, search);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_media_proxy_ctrl_get_search_results_id(esp_ble_audio_media_player_t *player)
{
    int err;

    if (player == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_get_search_results_id_safe(player);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_media_proxy_ctrl_get_content_ctrl_id(esp_ble_audio_media_player_t *player)
{
    int err;

    if (player == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_ctrl_get_content_ctrl_id_safe(player);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif /* CONFIG_BT_MCTL_LOCAL_PLAYER_LOCAL_CONTROL || CONFIG_BT_MCTL_REMOTE_PLAYER_CONTROL */

#if CONFIG_BT_MCTL_LOCAL_PLAYER_CONTROL
static bool pl_calls_is_valid(const esp_ble_audio_media_proxy_pl_calls_t *pl_calls)
{
    if (pl_calls == NULL) {
        return false;
    }

    if (pl_calls->get_player_name == NULL ||
            pl_calls->get_icon_url == NULL ||
            pl_calls->get_track_title == NULL ||
            pl_calls->get_track_duration == NULL ||
            pl_calls->get_track_position == NULL ||
            pl_calls->set_track_position == NULL ||
            pl_calls->get_playback_speed == NULL ||
            pl_calls->set_playback_speed == NULL ||
            pl_calls->get_seeking_speed == NULL ||
            pl_calls->get_playing_order == NULL ||
            pl_calls->set_playing_order == NULL ||
            pl_calls->get_playing_orders_supported == NULL ||
            pl_calls->get_media_state == NULL ||
            pl_calls->send_command == NULL ||
            pl_calls->get_commands_supported == NULL ||
            pl_calls->get_content_ctrl_id == NULL) {
        return false;
    }

#if CONFIG_BT_MPL_OBJECTS
    if (pl_calls->get_icon_id == NULL ||
            pl_calls->get_track_segments_id == NULL ||
            pl_calls->get_current_track_id == NULL ||
            pl_calls->set_current_track_id == NULL ||
            pl_calls->get_next_track_id == NULL ||
            pl_calls->set_next_track_id == NULL ||
            pl_calls->get_parent_group_id == NULL ||
            pl_calls->get_current_group_id == NULL ||
            pl_calls->set_current_group_id == NULL ||
            pl_calls->send_search == NULL ||
            pl_calls->get_search_results_id == NULL) {
        return false;
    }
#endif /* CONFIG_BT_MPL_OBJECTS */

    return true;
}

esp_err_t esp_ble_audio_media_proxy_pl_register(esp_ble_audio_media_proxy_pl_calls_t *pl_calls)
{
    int err;

    if (pl_calls_is_valid(pl_calls) == false) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_media_proxy_pl_register_safe(pl_calls);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_media_proxy_pl_init(void)
{
    int err;

    err = bt_media_proxy_pl_init_safe();
    if (err) {
        return ESP_FAIL;
    }

#if CONFIG_BT_MCS && BLE_AUDIO_SVC_SEP_ADD
    err = bt_le_media_proxy_pl_init();
    if (err) {
        return ESP_FAIL;
    }
#endif /* CONFIG_BT_MCS && BLE_AUDIO_SVC_SEP_ADD */

    return ESP_OK;
}
#endif /* CONFIG_BT_MCTL_LOCAL_PLAYER_CONTROL */

#if CONFIG_BT_MCS && CONFIG_BT_OTS
struct bt_ots *esp_ble_audio_mcs_get_ots(void)
{
    return bt_mcs_get_ots_safe();
}
#endif /* CONFIG_BT_MCS && CONFIG_BT_OTS */
