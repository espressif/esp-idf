/*
 * SPDX-FileCopyrightText: 2019-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_BLE_AUDIO_MCC_API_H_
#define ESP_BLE_AUDIO_MCC_API_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "sdkconfig.h"
#include "esp_err.h"

#include <zephyr/bluetooth/services/ots.h>
#include <zephyr/bluetooth/audio/mcc.h>

#include "common/init.h"
#include "common/host.h"

#include "esp_ble_audio_media_proxy_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Media control client callbacks */
typedef struct bt_mcc_cb    esp_ble_audio_mcc_cb_t;

/**
 * @brief   Initialize Media Control Client.
 *
 * @param   cb  Callbacks to be used.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_init(esp_ble_audio_mcc_cb_t *cb);

/**
 * @brief   Discover Media Control Service.
 *
 * Discover Media Control Service (MCS) on the server given by the connection
 * Optionally subscribe to notifications.
 *
 * Shall be called once, after media control client initialization and before
 * using other media control client functionality.
 *
 * @param   conn_handle Connection handle.
 * @param   subscribe   Whether to subscribe to notifications.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_discover_mcs(uint16_t conn_handle, bool subscribe);

/**
 * @brief   Read Media Player Name.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_read_player_name(uint16_t conn_handle);

/**
 * @brief   Read Icon Object ID.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_read_icon_obj_id(uint16_t conn_handle);

/**
 * @brief   Read Icon Object URL.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_read_icon_url(uint16_t conn_handle);

/**
 * @brief   Read Track Title.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_read_track_title(uint16_t conn_handle);

/**
 * @brief   Read Track Duration.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_read_track_duration(uint16_t conn_handle);

/**
 * @brief   Read Track Position.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_read_track_position(uint16_t conn_handle);

/**
 * @brief   Set Track position.
 *
 * @param   conn_handle Connection handle.
 * @param   pos         Track position.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_set_track_position(uint16_t conn_handle, int32_t pos);

/**
 * @brief   Read Playback speed.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_read_playback_speed(uint16_t conn_handle);

/**
 * @brief   Set Playback Speed.
 *
 * @param   conn_handle Connection handle.
 * @param   speed       Playback speed.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_set_playback_speed(uint16_t conn_handle, int8_t speed);

/**
 * @brief   Read Seeking speed.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_read_seeking_speed(uint16_t conn_handle);

/**
 * @brief   Read Track Segments Object ID.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_read_segments_obj_id(uint16_t conn_handle);

/**
 * @brief   Read Current Track Object ID.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_read_current_track_obj_id(uint16_t conn_handle);

/**
 * @brief   Set Current Track Object ID.
 *
 * Set the Current Track to the track given by the @p id parameter.
 *
 * @param   conn_handle Connection handle.
 * @param   id          Object Transfer Service ID (UINT48) of the track to set as the current track.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_set_current_track_obj_id(uint16_t conn_handle, uint64_t id);

/**
 * @brief   Read Next Track Object ID.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_read_next_track_obj_id(uint16_t conn_handle);

/**
 * @brief   Set Next Track Object ID.
 *
 * @param   conn_handle Connection handle.
 * @param   id          Object Transfer Service ID (UINT48) of the track to set as the next track.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_set_next_track_obj_id(uint16_t conn_handle, uint64_t id);

/**
 * @brief   Read Current Group Object ID.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_read_current_group_obj_id(uint16_t conn_handle);

/**
 * @brief   Set Current Group Object ID.
 *
 * @param   conn_handle Connection handle.
 * @param   id          Object Transfer Service ID (UINT48) of the group to set as the current group.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_set_current_group_obj_id(uint16_t conn_handle, uint64_t id);

/**
 * @brief   Read Parent Group Object ID.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_read_parent_group_obj_id(uint16_t conn_handle);

/**
 * @brief   Read Playing Order.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_read_playing_order(uint16_t conn_handle);

/**
 * @brief   Set Playing Order.
 *
 * @param   conn_handle Connection handle.
 * @param   order       Playing order.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_set_playing_order(uint16_t conn_handle, uint8_t order);

/**
 * @brief   Read Playing Orders Supported.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_read_playing_orders_supported(uint16_t conn_handle);

/**
 * @brief   Read Media State.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_read_media_state(uint16_t conn_handle);

/**
 * @brief   Send a command.
 *
 * Write a command (e.g. "play", "pause") to the server's media control point.
 *
 * @param   conn_handle Connection handle.
 * @param   cmd         The command to send.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_send_cmd(uint16_t conn_handle,
                                     const esp_ble_audio_mpl_cmd_t *cmd);

/**
 * @brief   Read Opcodes Supported.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_read_opcodes_supported(uint16_t conn_handle);

/**
 * @brief   Send a Search command.
 *
 * Write a search to the server's search control point.
 *
 * @param   conn_handle Connection handle.
 * @param   search      The search.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_send_search(uint16_t conn_handle,
                                        const esp_ble_audio_mpl_search_t *search);

/**
 * @brief   Search Results Group Object ID.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_read_search_results_obj_id(uint16_t conn_handle);

/**
 * @brief   Read Content Control ID.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_read_content_control_id(uint16_t conn_handle);

/**
 * @brief   Read the current object metadata.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_otc_read_object_metadata(uint16_t conn_handle);

/**
 * @brief   Read the Icon Object.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_otc_read_icon_object(uint16_t conn_handle);

/**
 * @brief   Read the Track Segments Object.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_otc_read_track_segments_object(uint16_t conn_handle);

/**
 * @brief   Read the Current Track Object.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_otc_read_current_track_object(uint16_t conn_handle);

/**
 * @brief   Read the Next Track Object.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_otc_read_next_track_object(uint16_t conn_handle);

/**
 * @brief   Read the Current Group Object.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_otc_read_current_group_object(uint16_t conn_handle);

/**
 * @brief   Read the Parent Group Object.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_mcc_otc_read_parent_group_object(uint16_t conn_handle);

/**
 * @brief   Look up MCC OTC instance.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  Pointer to a MCC OTC instance if found else NULL.
 */
struct bt_ots_client *esp_ble_audio_mcc_otc_inst(uint16_t conn_handle);

#ifdef __cplusplus
}
#endif

#endif /* ESP_BLE_AUDIO_MCC_API_H_ */
