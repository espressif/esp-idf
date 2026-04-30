/*
 * SPDX-FileCopyrightText: 2019-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_BLE_AUDIO_MEDIA_PROXY_API_H_
#define ESP_BLE_AUDIO_MEDIA_PROXY_API_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "sdkconfig.h"
#include "esp_err.h"

#include <zephyr/bluetooth/services/ots.h>
#include <zephyr/bluetooth/audio/media_proxy.h>

#include "common/init.h"
#include "common/host.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!< Minimum playback speed, resulting in 25% speed */
#define ESP_BLE_AUDIO_MEDIA_PROXY_PLAYBACK_SPEED_MIN        MEDIA_PROXY_PLAYBACK_SPEED_MIN
/*!< Quarter playback speed, resulting in 25% speed */
#define ESP_BLE_AUDIO_MEDIA_PROXY_PLAYBACK_SPEED_QUARTER    MEDIA_PROXY_PLAYBACK_SPEED_QUARTER
/*!< Half playback speed, resulting in 50% speed */
#define ESP_BLE_AUDIO_MEDIA_PROXY_PLAYBACK_SPEED_HALF       MEDIA_PROXY_PLAYBACK_SPEED_HALF
/*!< Unity playback speed, resulting in 100% speed */
#define ESP_BLE_AUDIO_MEDIA_PROXY_PLAYBACK_SPEED_UNITY      MEDIA_PROXY_PLAYBACK_SPEED_UNITY
/*!< Double playback speed, resulting in 200% speed */
#define ESP_BLE_AUDIO_MEDIA_PROXY_PLAYBACK_SPEED_DOUBLE     MEDIA_PROXY_PLAYBACK_SPEED_DOUBLE
/*!< Max playback speed, resulting in 395.7% speed (nearly 400%) */
#define ESP_BLE_AUDIO_MEDIA_PROXY_PLAYBACK_SPEED_MAX        MEDIA_PROXY_PLAYBACK_SPEED_MAX

/*!< The current track is invalid, and no track has been selected. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_STATE_INACTIVE            MEDIA_PROXY_STATE_INACTIVE
/*!< The media player is playing the current track. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_STATE_PLAYING             MEDIA_PROXY_STATE_PLAYING
/*!< The current track is paused. The media player has a current track, but it is not being played */
#define ESP_BLE_AUDIO_MEDIA_PROXY_STATE_PAUSED              MEDIA_PROXY_STATE_PAUSED
/*!< The current track is fast forwarding or fast rewinding. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_STATE_SEEKING             MEDIA_PROXY_STATE_SEEKING
/*!< Used internally as the last state value */
#define ESP_BLE_AUDIO_MEDIA_PROXY_STATE_LAST                MEDIA_PROXY_STATE_LAST

/*!< Start playing the current track. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_PLAY                   MEDIA_PROXY_OP_PLAY
/*!< Pause playing the current track. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_PAUSE                  MEDIA_PROXY_OP_PAUSE
/*!< Fast rewind the current track. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_FAST_REWIND            MEDIA_PROXY_OP_FAST_REWIND
/*!< Fast forward the current track. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_FAST_FORWARD           MEDIA_PROXY_OP_FAST_FORWARD
/**
 * Stop current activity and return to the paused state and set
 * the current track position to the start of the current track.
 */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_STOP                   MEDIA_PROXY_OP_STOP

/*!< Set a new current track position relative to the current track position. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_MOVE_RELATIVE          MEDIA_PROXY_OP_MOVE_RELATIVE

/**
 * Set the current track position to the starting position of
 * the previous segment of the current track.
 */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_PREV_SEGMENT           MEDIA_PROXY_OP_PREV_SEGMENT
/**
 * Set the current track position to the starting position of
 * the next segment of the current track.
 */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_NEXT_SEGMENT           MEDIA_PROXY_OP_NEXT_SEGMENT
/**
 * Set the current track position to the starting position of
 * the first segment of the current track.
 */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_FIRST_SEGMENT          MEDIA_PROXY_OP_FIRST_SEGMENT
/**
 * Set the current track position to the starting position of
 * the last segment of the current track.
 */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_LAST_SEGMENT           MEDIA_PROXY_OP_LAST_SEGMENT
/**
 * Set the current track position to the starting position of
 * the nth segment of the current track.
 */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_GOTO_SEGMENT           MEDIA_PROXY_OP_GOTO_SEGMENT

/*!< Set the current track to the previous track based on the playing order. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_PREV_TRACK             MEDIA_PROXY_OP_PREV_TRACK
/*!< Set the current track to the next track based on the playing order. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_NEXT_TRACK             MEDIA_PROXY_OP_NEXT_TRACK
/*!< Set the current track to the first track based on the playing order. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_FIRST_TRACK            MEDIA_PROXY_OP_FIRST_TRACK
/*!< Set the current track to the last track based on the playing order. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_LAST_TRACK             MEDIA_PROXY_OP_LAST_TRACK
/*!< Set the current track to the nth track based on the playing order. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_GOTO_TRACK             MEDIA_PROXY_OP_GOTO_TRACK

/*!< Set the current group to the previous group in the sequence of groups. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_PREV_GROUP             MEDIA_PROXY_OP_PREV_GROUP
/*!< Set the current group to the next group in the sequence of groups. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_NEXT_GROUP             MEDIA_PROXY_OP_NEXT_GROUP
/*!< Set the current group to the first group in the sequence of groups. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_FIRST_GROUP            MEDIA_PROXY_OP_FIRST_GROUP
/*!< Set the current group to the last group in the sequence of groups. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_LAST_GROUP             MEDIA_PROXY_OP_LAST_GROUP
/*!< Set the current group to the nth group in the sequence of groups. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_GOTO_GROUP             MEDIA_PROXY_OP_GOTO_GROUP

/*!< Media player supported opcodes length */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OPCODES_SUPPORTED_LEN     MEDIA_PROXY_OPCODES_SUPPORTED_LEN

/*!< Support the Play opcode */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_SUP_PLAY               MEDIA_PROXY_OP_SUP_PLAY
/*!< Support the Pause opcode */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_SUP_PAUSE              MEDIA_PROXY_OP_SUP_PAUSE
/*!< Support the Fast Rewind opcode */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_SUP_FAST_REWIND        MEDIA_PROXY_OP_SUP_FAST_REWIND
/*!< Support the Fast Forward opcode */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_SUP_FAST_FORWARD       MEDIA_PROXY_OP_SUP_FAST_FORWARD
/*!< Support the Stop opcode */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_SUP_STOP               MEDIA_PROXY_OP_SUP_STOP

/*!< Support the Move Relative opcode */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_SUP_MOVE_RELATIVE      MEDIA_PROXY_OP_SUP_MOVE_RELATIVE

/*!< Support the Previous Segment opcode */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_SUP_PREV_SEGMENT       MEDIA_PROXY_OP_SUP_PREV_SEGMENT
/*!< Support the Next Segment opcode */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_SUP_NEXT_SEGMENT       MEDIA_PROXY_OP_SUP_NEXT_SEGMENT
/*!< Support the First Segment opcode */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_SUP_FIRST_SEGMENT      MEDIA_PROXY_OP_SUP_FIRST_SEGMENT
/*!< Support the Last Segment opcode */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_SUP_LAST_SEGMENT       MEDIA_PROXY_OP_SUP_LAST_SEGMENT
/*!< Support the Goto Segment opcode */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_SUP_GOTO_SEGMENT       MEDIA_PROXY_OP_SUP_GOTO_SEGMENT

/*!< Support the Previous Track opcode */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_SUP_PREV_TRACK         MEDIA_PROXY_OP_SUP_PREV_TRACK
/*!< Support the Next Track opcode */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_SUP_NEXT_TRACK         MEDIA_PROXY_OP_SUP_NEXT_TRACK
/*!< Support the First Track opcode */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_SUP_FIRST_TRACK        MEDIA_PROXY_OP_SUP_FIRST_TRACK
/*!< Support the Last Track opcode */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_SUP_LAST_TRACK         MEDIA_PROXY_OP_SUP_LAST_TRACK
/*!< Support the Goto Track opcode */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_SUP_GOTO_TRACK         MEDIA_PROXY_OP_SUP_GOTO_TRACK

/*!< Support the Previous Group opcode */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_SUP_PREV_GROUP         MEDIA_PROXY_OP_SUP_PREV_GROUP
/*!< Support the Next Group opcode */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_SUP_NEXT_GROUP         MEDIA_PROXY_OP_SUP_NEXT_GROUP
/*!< Support the First Group opcode */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_SUP_FIRST_GROUP        MEDIA_PROXY_OP_SUP_FIRST_GROUP
/*!< Support the Last Group opcode */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_SUP_LAST_GROUP         MEDIA_PROXY_OP_SUP_LAST_GROUP
/*!< Support the Goto Group opcode */
#define ESP_BLE_AUDIO_MEDIA_PROXY_OP_SUP_GOTO_GROUP         MEDIA_PROXY_OP_SUP_GOTO_GROUP

/*!< Action requested by the opcode write was completed successfully. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_CMD_SUCCESS               MEDIA_PROXY_CMD_SUCCESS
/*!< An invalid or unsupported opcode was used for the Media Control Point write. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_CMD_NOT_SUPPORTED         MEDIA_PROXY_CMD_NOT_SUPPORTED
/**
 * The Media Player State characteristic value is Inactive when the opcode
 * is received or the result of the requested action of the opcode results
 * in the Media Player State characteristic being set to Inactive.
 */
#define ESP_BLE_AUDIO_MEDIA_PROXY_CMD_PLAYER_INACTIVE       MEDIA_PROXY_CMD_PLAYER_INACTIVE
/**
 * The requested action of any Media Control Point write cannot be completed
 * successfully because of a condition within the player.
 */
#define ESP_BLE_AUDIO_MEDIA_PROXY_CMD_CANNOT_BE_COMPLETED   MEDIA_PROXY_CMD_CANNOT_BE_COMPLETED

/*!< Search for Track Name */
#define ESP_BLE_AUDIO_MEDIA_PROXY_SEARCH_TYPE_TRACK_NAME    MEDIA_PROXY_SEARCH_TYPE_TRACK_NAME
/*!< Search for Artist Name */
#define ESP_BLE_AUDIO_MEDIA_PROXY_SEARCH_TYPE_ARTIST_NAME   MEDIA_PROXY_SEARCH_TYPE_ARTIST_NAME
/*!< Search for Album Name */
#define ESP_BLE_AUDIO_MEDIA_PROXY_SEARCH_TYPE_ALBUM_NAME    MEDIA_PROXY_SEARCH_TYPE_ALBUM_NAME
/*!< Search for Group Name */
#define ESP_BLE_AUDIO_MEDIA_PROXY_SEARCH_TYPE_GROUP_NAME    MEDIA_PROXY_SEARCH_TYPE_GROUP_NAME
/*!< Search for Earliest Year */
#define ESP_BLE_AUDIO_MEDIA_PROXY_SEARCH_TYPE_EARLIEST_YEAR MEDIA_PROXY_SEARCH_TYPE_EARLIEST_YEAR
/*!< Search for Latest Year */
#define ESP_BLE_AUDIO_MEDIA_PROXY_SEARCH_TYPE_LATEST_YEAR   MEDIA_PROXY_SEARCH_TYPE_LATEST_YEAR
/*!< Search for Genre */
#define ESP_BLE_AUDIO_MEDIA_PROXY_SEARCH_TYPE_GENRE         MEDIA_PROXY_SEARCH_TYPE_GENRE
/*!< Search for Tracks only */
#define ESP_BLE_AUDIO_MEDIA_PROXY_SEARCH_TYPE_ONLY_TRACKS   MEDIA_PROXY_SEARCH_TYPE_ONLY_TRACKS
/*!< Search for Groups only */
#define ESP_BLE_AUDIO_MEDIA_PROXY_SEARCH_TYPE_ONLY_GROUPS   MEDIA_PROXY_SEARCH_TYPE_ONLY_GROUPS

/*!< Search request was accepted; search has started. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_SEARCH_SUCCESS            MEDIA_PROXY_SEARCH_SUCCESS
/*!< Search request was invalid; no search started. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_SEARCH_FAILURE            MEDIA_PROXY_SEARCH_FAILURE

/*!< Group object type is track */
#define ESP_BLE_AUDIO_MEDIA_PROXY_GROUP_OBJECT_TRACK_TYPE   MEDIA_PROXY_GROUP_OBJECT_TRACK_TYPE
/*!< Group object type is group */
#define ESP_BLE_AUDIO_MEDIA_PROXY_GROUP_OBJECT_GROUP_TYPE   MEDIA_PROXY_GROUP_OBJECT_GROUP_TYPE

/*!< Maximum seeking speed - Can be negated */
#define ESP_BLE_AUDIO_MEDIA_PROXY_SEEKING_SPEED_FACTOR_MAX  MEDIA_PROXY_SEEKING_SPEED_FACTOR_MAX
/*!< Minimum seeking speed - Can be negated */
#define ESP_BLE_AUDIO_MEDIA_PROXY_SEEKING_SPEED_FACTOR_MIN  MEDIA_PROXY_SEEKING_SPEED_FACTOR_MIN
/*!< No seeking */
#define ESP_BLE_AUDIO_MEDIA_PROXY_SEEKING_SPEED_FACTOR_ZERO MEDIA_PROXY_SEEKING_SPEED_FACTOR_ZERO

/*!< A single track is played once; there is no next track. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_PLAYING_ORDER_SINGLE_ONCE     MEDIA_PROXY_PLAYING_ORDER_SINGLE_ONCE
/*!< A single track is played repeatedly; the next track is the current track. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_PLAYING_ORDER_SINGLE_REPEAT   MEDIA_PROXY_PLAYING_ORDER_SINGLE_REPEAT
/*!< The tracks within a group are played once in track order. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_PLAYING_ORDER_INORDER_ONCE    MEDIA_PROXY_PLAYING_ORDER_INORDER_ONCE
/*!< The tracks within a group are played in track order repeatedly. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_PLAYING_ORDER_INORDER_REPEAT  MEDIA_PROXY_PLAYING_ORDER_INORDER_REPEAT
/*!< The tracks within a group are played once only from the oldest first. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_PLAYING_ORDER_OLDEST_ONCE     MEDIA_PROXY_PLAYING_ORDER_OLDEST_ONCE
/*!< The tracks within a group are played from the oldest first repeatedly. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_PLAYING_ORDER_OLDEST_REPEAT   MEDIA_PROXY_PLAYING_ORDER_OLDEST_REPEAT
/*!< The tracks within a group are played once only from the newest first. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_PLAYING_ORDER_NEWEST_ONCE     MEDIA_PROXY_PLAYING_ORDER_NEWEST_ONCE
/*!< The tracks within a group are played from the newest first repeatedly. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_PLAYING_ORDER_NEWEST_REPEAT   MEDIA_PROXY_PLAYING_ORDER_NEWEST_REPEAT
/*!< The tracks within a group are played in random order once. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_PLAYING_ORDER_SHUFFLE_ONCE    MEDIA_PROXY_PLAYING_ORDER_SHUFFLE_ONCE
/*!< The tracks within a group are played in random order repeatedly. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_PLAYING_ORDER_SHUFFLE_REPEAT  MEDIA_PROXY_PLAYING_ORDER_SHUFFLE_REPEAT

/*!< A single track is played once; there is no next track. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_SINGLE_ONCE      MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_SINGLE_ONCE
/*!< A single track is played repeatedly; the next track is the current track. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_SINGLE_REPEAT    MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_SINGLE_REPEAT
/*!< The tracks within a group are played once in track order. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_INORDER_ONCE     MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_INORDER_ONCE
/*!< The tracks within a group are played in track order repeatedly. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_INORDER_REPEAT   MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_INORDER_REPEAT
/*!< The tracks within a group are played once only from the oldest first. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_OLDEST_ONCE      MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_OLDEST_ONCE
/*!< The tracks within a group are played from the oldest first repeatedly. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_OLDEST_REPEAT    MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_OLDEST_REPEAT
/*!< The tracks within a group are played once only from the newest first. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_NEWEST_ONCE      MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_NEWEST_ONCE
/*!< The tracks within a group are played from the newest first repeatedly. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_NEWEST_REPEAT    MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_NEWEST_REPEAT
/*!< The tracks within a group are played in random order once. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_SHUFFLE_ONCE     MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_SHUFFLE_ONCE
/*!< The tracks within a group are played in random order repeatedly. */
#define ESP_BLE_AUDIO_MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_SHUFFLE_REPEAT   MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_SHUFFLE_REPEAT

/** Media player command */
typedef struct mpl_cmd      esp_ble_audio_mpl_cmd_t;

/** Media command notification */
typedef struct mpl_cmd_ntf  esp_ble_audio_mpl_cmd_ntf_t;

/** Search control item */
typedef struct mpl_sci      esp_ble_audio_mpl_sci_t;

/** Search */
typedef struct mpl_search   esp_ble_audio_mpl_search_t;

/** Media player instance */
typedef struct media_player esp_ble_audio_media_player_t;

/** Callbacks to a controller, from the media proxy */
typedef struct media_proxy_ctrl_cbs esp_ble_audio_media_proxy_ctrl_cbs_t;

/** Available calls in a player, that the media proxy can call */
typedef struct media_proxy_pl_calls esp_ble_audio_media_proxy_pl_calls_t;

/**
 * @brief   Register a controller with the media_proxy.
 *
 * @param   ctrl_cbs    Callbacks to the controller.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_register(esp_ble_audio_media_proxy_ctrl_cbs_t *ctrl_cbs);

/**
 * @brief   Discover a remote media player.
 *
 * Discover a remote media player instance.
 * The remote player instance will be discovered, and accessed, using Bluetooth,
 * via the media control client and a remote media control service.
 * This call will start a GATT discovery of the Media Control Service on the peer,
 * and setup handles and subscriptions.
 *
 * This shall be called once before any other actions can be executed for the
 * remote player. The remote player instance will be returned in the
 * discover_player() callback.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_discover_player(uint16_t conn_handle);

/**
 * @brief   Read Media Player Name.
 *
 * @param   player  Media player instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_get_player_name(esp_ble_audio_media_player_t *player);

/**
 * @brief   Read Icon Object ID.
 *
 * Get an ID (48 bit) that can be used to retrieve the Icon
 * Object from an Object Transfer Service.
 *
 * See the Media Control Service spec v1.0 sections 3.2 and
 * 4.1 for a description of the Icon Object.
 *
 * Requires Object Transfer Service
 *
 * @param   player  Media player instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_get_icon_id(esp_ble_audio_media_player_t *player);

/**
 * @brief   Read Icon URL.
 *
 * Get a URL to the media player's icon.
 *
 * @param   player  Media player instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_get_icon_url(esp_ble_audio_media_player_t *player);

/**
 * @brief   Read Track Title.
 *
 * @param   player  Media player instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_get_track_title(esp_ble_audio_media_player_t *player);

/**
 * @brief   Read Track Duration.
 *
 * The duration of a track is measured in hundredths of a second.
 *
 * @param   player  Media player instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_get_track_duration(esp_ble_audio_media_player_t *player);

/**
 * @brief   Read Track Position.
 *
 * The position of the player (the playing position) is measured
 * in hundredths of a second from the beginning of the track.
 *
 * @param   player  Media player instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_get_track_position(esp_ble_audio_media_player_t *player);

/**
 * @brief   Set Track Position.
 *
 * Set the playing position of the media player in the current
 * track. The position is given in hundredths of a second,
 * from the beginning of the track of the track for positive
 * values, and (backwards) from the end of the track for
 * negative values.
 *
 * @param   player      Media player instance pointer.
 * @param   position    The track position to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_set_track_position(esp_ble_audio_media_player_t *player,
                                                            int32_t position);

/**
 * @brief   Get Playback Speed.
 *
 * The playback speed parameter is related to the actual
 * playback speed as follows:
 * actual playback speed = 2^(speed_parameter/64)
 *
 * A speed parameter of 0 corresponds to unity speed playback
 * (i.e. playback at "normal" speed). A speed parameter of
 * -128 corresponds to playback at one fourth of normal speed,
 * 127 corresponds to playback at almost four times the normal
 * speed.
 *
 * @param   player  Media player instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_get_playback_speed(esp_ble_audio_media_player_t *player);

/**
 * @brief   Set Playback Speed.
 *
 * See the get_playback_speed() function for an explanation of
 * the playback speed parameter.
 *
 * Note that the media player may not support all possible
 * values of the playback speed parameter. If the value given
 * is not supported, and is higher than the current value, the
 * player should set the playback speed to the next higher
 * supported value. (And correspondingly to the next lower
 * supported value for given values lower than the current
 * value.)
 *
 * @param   player  Media player instance pointer.
 * @param   speed   The playback speed parameter to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_set_playback_speed(esp_ble_audio_media_player_t *player,
                                                            int8_t speed);

/**
 * @brief   Get Seeking Speed.
 *
 * The seeking speed gives the speed with which the player is
 * seeking. It is a factor, relative to real-time playback
 * speed - a factor four means seeking happens at four times
 * the real-time playback speed. Positive values are for
 * forward seeking, negative values for backwards seeking.
 *
 * The seeking speed is not settable - a non-zero seeking speed
 * is the result of "fast rewind" of "fast forward" commands.
 *
 * @param   player  Media player instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_get_seeking_speed(esp_ble_audio_media_player_t *player);

/**
 * @brief   Read Current Track Segments Object ID.
 *
 * Get an ID (48 bit) that can be used to retrieve the Current
 * Track Segments Object from an Object Transfer Service.
 *
 * See the Media Control Service spec v1.0 sections 3.10 and
 * 4.2 for a description of the Track Segments Object.
 *
 * Requires Object Transfer Service.
 *
 * @param   player  Media player instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_get_track_segments_id(esp_ble_audio_media_player_t *player);

/**
 * @brief   Read Current Track Object ID.
 *
 * Get an ID (48 bit) that can be used to retrieve the Current
 * Track Object from an Object Transfer Service.
 *
 * See the Media Control Service spec v1.0 sections 3.11 and
 * 4.3 for a description of the Current Track Object.
 *
 * Requires Object Transfer Service.
 *
 * @param   player  Media player instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_get_current_track_id(esp_ble_audio_media_player_t *player);

/**
 * @brief   Set Current Track Object ID.
 *
 * Change the player's current track to the track given by the ID.
 * (Behaves similarly to the goto track command.)
 *
 * Requires Object Transfer Service.
 *
 * @param   player  Media player instance pointer.
 * @param   id      The ID of a track object.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_set_current_track_id(esp_ble_audio_media_player_t *player,
                                                              uint64_t id);

/**
 * @brief   Read Next Track Object ID.
 *
 * Get an ID (48 bit) that can be used to retrieve the Next
 * Track Object from an Object Transfer Service.
 *
 * Requires Object Transfer Service.
 *
 * @param   player  Media player instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_get_next_track_id(esp_ble_audio_media_player_t *player);

/**
 * @brief   Set Next Track Object ID.
 *
 * Change the player's next track to the track given by the ID.
 *
 * Requires Object Transfer Service.
 *
 * @param   player  Media player instance pointer.
 * @param   id      The ID of a track object.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_set_next_track_id(esp_ble_audio_media_player_t *player,
                                                           uint64_t id);

/**
 * @brief   Read Parent Group Object ID.
 *
 * Get an ID (48 bit) that can be used to retrieve the Parent
 * Track Object from an Object Transfer Service.
 *
 * The parent group is the parent of the current group.
 *
 * See the Media Control Service spec v1.0 sections 3.14 and
 * 4.4 for a description of the Current Track Object.
 *
 * Requires Object Transfer Service.
 *
 * @param   player  Media player instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_get_parent_group_id(esp_ble_audio_media_player_t *player);

/**
 * @brief   Read Current Group Object ID.
 *
 * Get an ID (48 bit) that can be used to retrieve the Current
 * Track Object from an Object Transfer Service
 *
 * See the Media Control Service spec v1.0 sections 3.14 and
 * 4.4 for a description of the Current Group Object.
 *
 * Requires Object Transfer Service.
 *
 * @param   player  Media player instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_get_current_group_id(esp_ble_audio_media_player_t *player);

/**
 * @brief   Set Current Group Object ID.
 *
 * Change the player's current group to the group given by the
 * ID, and the current track to the first track in that group.
 *
 * Requires Object Transfer Service.
 *
 * @param   player  Media player instance pointer.
 * @param   id      The ID of a group object.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_set_current_group_id(esp_ble_audio_media_player_t *player,
                                                              uint64_t id);

/**
 * @brief   Read Playing Order.
 *
 * @param   player  Media player instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_get_playing_order(esp_ble_audio_media_player_t *player);

/**
 * @brief   Set Playing Order.
 *
 * Set the media player's playing order.
 *
 * @param   player  Media player instance pointer.
 * @param   order   The playing order to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_set_playing_order(esp_ble_audio_media_player_t *player,
                                                           uint8_t order);

/**
 * @brief   Read Playing Orders Supported.
 *
 * Read a bitmap containing the media player's supported playing orders.
 *
 * @param   player  Media player instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_get_playing_orders_supported(esp_ble_audio_media_player_t *player);

/**
 * @brief   Read Media State.
 *
 * Read the media player's state.
 *
 * @param   player  Media player instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_get_media_state(esp_ble_audio_media_player_t *player);

/**
 * @brief   Send Command.
 *
 * Send a command to the media player.
 * Commands may cause the media player to change its state.
 * May result in two callbacks - one for the actual sending of the command
 * to the player, one for the result of the command from the player.
 *
 * @param   player  Media player instance pointer.
 * @param   command The command to send.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_send_command(esp_ble_audio_media_player_t *player,
                                                      const esp_ble_audio_mpl_cmd_t *command);

/**
 * @brief   Read Commands Supported.
 *
 * Read a bitmap containing the media player's supported command opcodes.
 *
 * @param   player  Media player instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_get_commands_supported(esp_ble_audio_media_player_t *player);

/**
 * @brief   Set Search.
 *
 * Write a search to the media player.
 * If the search is successful, the search results will be available as
 * a group object in the Object Transfer Service (OTS).
 *
 * May result in up to three callbacks:
 * - one for the actual sending of the search to the player;
 * - one for the result code for the search from the player;
 * - if the search is successful, one for the search results object ID
 *   in the OTs.
 *
 * Requires Object Transfer Service.
 *
 * @param   player  Media player instance pointer.
 * @param   search  The search to write.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_send_search(esp_ble_audio_media_player_t *player,
                                                     const esp_ble_audio_mpl_search_t *search);

/**
 * @brief   Read Search Results Object ID.
 *
 * Get an ID (48 bit) that can be used to retrieve the Search
 * Results Object from an Object Transfer Service.
 *
 * The search results object is a group object.
 * The search results object only exists if a successful
 * search operation has been done.
 *
 * Requires Object Transfer Service.
 *
 * @param   player  Media player instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_get_search_results_id(esp_ble_audio_media_player_t *player);

/**
 * @brief   Read Content Control ID.
 *
 * The content control ID identifies a content control service
 * on a device, and links it to the corresponding audio stream.
 *
 * @param   player  Media player instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_ctrl_get_content_ctrl_id(esp_ble_audio_media_player_t *player);

/**
 * @brief   Register a player with the media proxy.
 *
 * Register a player with the media proxy module, for use by media
 * controllers.
 *
 * The media proxy may call any non-NULL function pointers in the
 * supplied media_proxy_pl_calls structure.
 *
 * @param   pl_calls    Function pointers to the media player's calls.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_pl_register(esp_ble_audio_media_proxy_pl_calls_t *pl_calls);

/**
 * @brief   Initialize player.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_media_proxy_pl_init(void);

/**
 * @brief   Get the pointer of the Object Transfer Service used by the Media Control Service.
 *
 * @return  Pointer to an OTS instance if found else NULL.
 */
struct bt_ots *esp_ble_audio_mcs_get_ots(void);

#ifdef __cplusplus
}
#endif

#endif /* ESP_BLE_AUDIO_MEDIA_PROXY_API_H_ */
