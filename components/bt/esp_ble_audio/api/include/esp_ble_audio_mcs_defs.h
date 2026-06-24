/*
 * SPDX-FileCopyrightText: 2019-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_BLE_AUDIO_MCS_DEFS_H_
#define ESP_BLE_AUDIO_MCS_DEFS_H_

#include <zephyr/bluetooth/audio/mcs.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!< Minimum playback speed, resulting in 25% speed */
#define ESP_BLE_AUDIO_MCS_PLAYBACK_SPEED_MIN            BT_MCS_PLAYBACK_SPEED_MIN
/*!< Quarter playback speed, resulting in 25% speed */
#define ESP_BLE_AUDIO_MCS_PLAYBACK_SPEED_QUARTER        BT_MCS_PLAYBACK_SPEED_QUARTER
/*!< Half playback speed, resulting in 50% speed */
#define ESP_BLE_AUDIO_MCS_PLAYBACK_SPEED_HALF           BT_MCS_PLAYBACK_SPEED_HALF
/*!< Unity playback speed, resulting in 100% speed */
#define ESP_BLE_AUDIO_MCS_PLAYBACK_SPEED_UNITY          BT_MCS_PLAYBACK_SPEED_UNITY
/*!< Double playback speed, resulting in 200% speed */
#define ESP_BLE_AUDIO_MCS_PLAYBACK_SPEED_DOUBLE         BT_MCS_PLAYBACK_SPEED_DOUBLE
/*!< Max playback speed, resulting in 395.7% speed (nearly 400%) */
#define ESP_BLE_AUDIO_MCS_PLAYBACK_SPEED_MAX            BT_MCS_PLAYBACK_SPEED_MAX

/*!< Maximum seeking speed - Can be negated */
#define ESP_BLE_AUDIO_MCS_SEEKING_SPEED_FACTOR_MAX      BT_MCS_SEEKING_SPEED_FACTOR_MAX
/*!< Minimum seeking speed - Can be negated */
#define ESP_BLE_AUDIO_MCS_SEEKING_SPEED_FACTOR_MIN      BT_MCS_SEEKING_SPEED_FACTOR_MIN
/*!< No seeking */
#define ESP_BLE_AUDIO_MCS_SEEKING_SPEED_FACTOR_ZERO     BT_MCS_SEEKING_SPEED_FACTOR_ZERO

/*!< A single track is played once; there is no next track. */
#define ESP_BLE_AUDIO_MCS_PLAYING_ORDER_SINGLE_ONCE     BT_MCS_PLAYING_ORDER_SINGLE_ONCE
/*!< A single track is played repeatedly; the next track is the current track. */
#define ESP_BLE_AUDIO_MCS_PLAYING_ORDER_SINGLE_REPEAT   BT_MCS_PLAYING_ORDER_SINGLE_REPEAT
/*!< The tracks within a group are played once in track order. */
#define ESP_BLE_AUDIO_MCS_PLAYING_ORDER_INORDER_ONCE    BT_MCS_PLAYING_ORDER_INORDER_ONCE
/*!< The tracks within a group are played in track order repeatedly. */
#define ESP_BLE_AUDIO_MCS_PLAYING_ORDER_INORDER_REPEAT  BT_MCS_PLAYING_ORDER_INORDER_REPEAT
/*!< The tracks within a group are played once only from the oldest first. */
#define ESP_BLE_AUDIO_MCS_PLAYING_ORDER_OLDEST_ONCE     BT_MCS_PLAYING_ORDER_OLDEST_ONCE
/*!< The tracks within a group are played from the oldest first repeatedly. */
#define ESP_BLE_AUDIO_MCS_PLAYING_ORDER_OLDEST_REPEAT   BT_MCS_PLAYING_ORDER_OLDEST_REPEAT
/*!< The tracks within a group are played once only from the newest first. */
#define ESP_BLE_AUDIO_MCS_PLAYING_ORDER_NEWEST_ONCE     BT_MCS_PLAYING_ORDER_NEWEST_ONCE
/*!< The tracks within a group are played from the newest first repeatedly. */
#define ESP_BLE_AUDIO_MCS_PLAYING_ORDER_NEWEST_REPEAT   BT_MCS_PLAYING_ORDER_NEWEST_REPEAT
/*!< The tracks within a group are played in random order once. */
#define ESP_BLE_AUDIO_MCS_PLAYING_ORDER_SHUFFLE_ONCE    BT_MCS_PLAYING_ORDER_SHUFFLE_ONCE
/*!< The tracks within a group are played in random order repeatedly. */
#define ESP_BLE_AUDIO_MCS_PLAYING_ORDER_SHUFFLE_REPEAT  BT_MCS_PLAYING_ORDER_SHUFFLE_REPEAT

/*!< The current track is invalid, and no track has been selected. */
#define ESP_BLE_AUDIO_MCS_MEDIA_STATE_INACTIVE          BT_MCS_MEDIA_STATE_INACTIVE
/*!< The media player is playing the current track. */
#define ESP_BLE_AUDIO_MCS_MEDIA_STATE_PLAYING           BT_MCS_MEDIA_STATE_PLAYING
/*!< The current track is paused. The media player has a current track, but it is not being played */
#define ESP_BLE_AUDIO_MCS_MEDIA_STATE_PAUSED            BT_MCS_MEDIA_STATE_PAUSED
/*!< The current track is fast forwarding or fast rewinding. */
#define ESP_BLE_AUDIO_MCS_MEDIA_STATE_SEEKING           BT_MCS_MEDIA_STATE_SEEKING

/*!< Start playing the current track. */
#define ESP_BLE_AUDIO_MCS_OPC_PLAY                      BT_MCS_OPC_PLAY
/*!< Pause playing the current track. */
#define ESP_BLE_AUDIO_MCS_OPC_PAUSE                     BT_MCS_OPC_PAUSE
/*!< Fast rewind the current track. */
#define ESP_BLE_AUDIO_MCS_OPC_FAST_REWIND               BT_MCS_OPC_FAST_REWIND
/*!< Fast forward the current track. */
#define ESP_BLE_AUDIO_MCS_OPC_FAST_FORWARD              BT_MCS_OPC_FAST_FORWARD
/**
 * Stop current activity and return to the paused state and set
 * the current track position to the start of the current track.
 */
#define ESP_BLE_AUDIO_MCS_OPC_STOP                      BT_MCS_OPC_STOP

/*!< Set a new current track position relative to the current track position. */
#define ESP_BLE_AUDIO_MCS_OPC_MOVE_RELATIVE             BT_MCS_OPC_MOVE_RELATIVE

/**
 * Set the current track position to the starting position of
 * the previous segment of the current track.
 */
#define ESP_BLE_AUDIO_MCS_OPC_PREV_SEGMENT              BT_MCS_OPC_PREV_SEGMENT
/**
 * Set the current track position to the starting position of
 * the next segment of the current track.
 */
#define ESP_BLE_AUDIO_MCS_OPC_NEXT_SEGMENT              BT_MCS_OPC_NEXT_SEGMENT
/**
 * Set the current track position to the starting position of
 * the first segment of the current track.
 */
#define ESP_BLE_AUDIO_MCS_OPC_FIRST_SEGMENT             BT_MCS_OPC_FIRST_SEGMENT
/**
 * Set the current track position to the starting position of
 * the last segment of the current track.
 */
#define ESP_BLE_AUDIO_MCS_OPC_LAST_SEGMENT              BT_MCS_OPC_LAST_SEGMENT
/**
 * Set the current track position to the starting position of
 * the nth segment of the current track.
 */
#define ESP_BLE_AUDIO_MCS_OPC_GOTO_SEGMENT              BT_MCS_OPC_GOTO_SEGMENT

/*!< Set the current track to the previous track based on the playing order. */
#define ESP_BLE_AUDIO_MCS_OPC_PREV_TRACK                BT_MCS_OPC_PREV_TRACK
/*!< Set the current track to the next track based on the playing order. */
#define ESP_BLE_AUDIO_MCS_OPC_NEXT_TRACK                BT_MCS_OPC_NEXT_TRACK
/*!< Set the current track to the first track based on the playing order. */
#define ESP_BLE_AUDIO_MCS_OPC_FIRST_TRACK               BT_MCS_OPC_FIRST_TRACK
/*!< Set the current track to the last track based on the playing order. */
#define ESP_BLE_AUDIO_MCS_OPC_LAST_TRACK                BT_MCS_OPC_LAST_TRACK
/*!< Set the current track to the nth track based on the playing order. */
#define ESP_BLE_AUDIO_MCS_OPC_GOTO_TRACK                BT_MCS_OPC_GOTO_TRACK

/*!< Set the current group to the previous group in the sequence of groups. */
#define ESP_BLE_AUDIO_MCS_OPC_PREV_GROUP                BT_MCS_OPC_PREV_GROUP
/*!< Set the current group to the next group in the sequence of groups. */
#define ESP_BLE_AUDIO_MCS_OPC_NEXT_GROUP                BT_MCS_OPC_NEXT_GROUP
/*!< Set the current group to the first group in the sequence of groups. */
#define ESP_BLE_AUDIO_MCS_OPC_FIRST_GROUP               BT_MCS_OPC_FIRST_GROUP
/*!< Set the current group to the last group in the sequence of groups. */
#define ESP_BLE_AUDIO_MCS_OPC_LAST_GROUP                BT_MCS_OPC_LAST_GROUP
/*!< Set the current group to the nth group in the sequence of groups. */
#define ESP_BLE_AUDIO_MCS_OPC_GOTO_GROUP                BT_MCS_OPC_GOTO_GROUP

/*!< Media control point supported opcodes length */
#define ESP_BLE_AUDIO_MCS_OPCODES_SUPPORTED_LEN         BT_MCS_OPCODES_SUPPORTED_LEN

/*!< Support the Play opcode */
#define ESP_BLE_AUDIO_MCS_OPC_SUP_PLAY                  BT_MCS_OPC_SUP_PLAY
/*!< Support the Pause opcode */
#define ESP_BLE_AUDIO_MCS_OPC_SUP_PAUSE                 BT_MCS_OPC_SUP_PAUSE
/*!< Support the Fast Rewind opcode */
#define ESP_BLE_AUDIO_MCS_OPC_SUP_FAST_REWIND           BT_MCS_OPC_SUP_FAST_REWIND
/*!< Support the Fast Forward opcode */
#define ESP_BLE_AUDIO_MCS_OPC_SUP_FAST_FORWARD          BT_MCS_OPC_SUP_FAST_FORWARD
/*!< Support the Stop opcode */
#define ESP_BLE_AUDIO_MCS_OPC_SUP_STOP                  BT_MCS_OPC_SUP_STOP
/*!< Support the Move Relative opcode */
#define ESP_BLE_AUDIO_MCS_OPC_SUP_MOVE_RELATIVE         BT_MCS_OPC_SUP_MOVE_RELATIVE
/*!< Support the Previous Segment opcode */
#define ESP_BLE_AUDIO_MCS_OPC_SUP_PREV_SEGMENT          BT_MCS_OPC_SUP_PREV_SEGMENT
/*!< Support the Next Segment opcode */
#define ESP_BLE_AUDIO_MCS_OPC_SUP_NEXT_SEGMENT          BT_MCS_OPC_SUP_NEXT_SEGMENT
/*!< Support the First Segment opcode */
#define ESP_BLE_AUDIO_MCS_OPC_SUP_FIRST_SEGMENT         BT_MCS_OPC_SUP_FIRST_SEGMENT
/*!< Support the Last Segment opcode */
#define ESP_BLE_AUDIO_MCS_OPC_SUP_LAST_SEGMENT          BT_MCS_OPC_SUP_LAST_SEGMENT
/*!< Support the Goto Segment opcode */
#define ESP_BLE_AUDIO_MCS_OPC_SUP_GOTO_SEGMENT          BT_MCS_OPC_SUP_GOTO_SEGMENT

/*!< Support the Previous Track opcode */
#define ESP_BLE_AUDIO_MCS_OPC_SUP_PREV_TRACK            BT_MCS_OPC_SUP_PREV_TRACK
/*!< Support the Next Track opcode */
#define ESP_BLE_AUDIO_MCS_OPC_SUP_NEXT_TRACK            BT_MCS_OPC_SUP_NEXT_TRACK
/*!< Support the First Track opcode */
#define ESP_BLE_AUDIO_MCS_OPC_SUP_FIRST_TRACK           BT_MCS_OPC_SUP_FIRST_TRACK
/*!< Support the Last Track opcode */
#define ESP_BLE_AUDIO_MCS_OPC_SUP_LAST_TRACK            BT_MCS_OPC_SUP_LAST_TRACK
/*!< Support the Goto Track opcode */
#define ESP_BLE_AUDIO_MCS_OPC_SUP_GOTO_TRACK            BT_MCS_OPC_SUP_GOTO_TRACK

/*!< Support the Previous Group opcode */
#define ESP_BLE_AUDIO_MCS_OPC_SUP_PREV_GROUP            BT_MCS_OPC_SUP_PREV_GROUP
/*!< Support the Next Group opcode */
#define ESP_BLE_AUDIO_MCS_OPC_SUP_NEXT_GROUP            BT_MCS_OPC_SUP_NEXT_GROUP
/*!< Support the First Group opcode */
#define ESP_BLE_AUDIO_MCS_OPC_SUP_FIRST_GROUP           BT_MCS_OPC_SUP_FIRST_GROUP
/*!< Support the Last Group opcode */
#define ESP_BLE_AUDIO_MCS_OPC_SUP_LAST_GROUP            BT_MCS_OPC_SUP_LAST_GROUP
/*!< Support the Goto Group opcode */
#define ESP_BLE_AUDIO_MCS_OPC_SUP_GOTO_GROUP            BT_MCS_OPC_SUP_GOTO_GROUP

/** Action requested by the opcode write was completed successfully. */
#define ESP_BLE_AUDIO_MCS_OPC_NTF_SUCCESS               BT_MCS_OPC_NTF_SUCCESS
/** An invalid or unsupported opcode was used for the Media Control Point write. */
#define ESP_BLE_AUDIO_MCS_OPC_NTF_NOT_SUPPORTED         BT_MCS_OPC_NTF_NOT_SUPPORTED
/**
 * The Media Player State characteristic value is Inactive when the opcode is
 * received or the result of the requested action of the opcode results in the
 * Media Player State characteristic being set to Inactive.
 */
#define ESP_BLE_AUDIO_MCS_OPC_NTF_PLAYER_INACTIVE       BT_MCS_OPC_NTF_PLAYER_INACTIVE
/**
 * The requested action of any Media Control Point write cannot be completed
 * successfully because of a condition within the player.
 */
#define ESP_BLE_AUDIO_MCS_OPC_NTF_CANNOT_BE_COMPLETED   BT_MCS_OPC_NTF_CANNOT_BE_COMPLETED

/*!< Search for Track Name */
#define ESP_BLE_AUDIO_MCS_SEARCH_TYPE_TRACK_NAME        BT_MCS_SEARCH_TYPE_TRACK_NAME
/*!< Search for Artist Name */
#define ESP_BLE_AUDIO_MCS_SEARCH_TYPE_ARTIST_NAME       BT_MCS_SEARCH_TYPE_ARTIST_NAME
/*!< Search for Album Name */
#define ESP_BLE_AUDIO_MCS_SEARCH_TYPE_ALBUM_NAME        BT_MCS_SEARCH_TYPE_ALBUM_NAME
/*!< Search for Group Name */
#define ESP_BLE_AUDIO_MCS_SEARCH_TYPE_GROUP_NAME        BT_MCS_SEARCH_TYPE_GROUP_NAME
/*!< Search for Earliest Year */
#define ESP_BLE_AUDIO_MCS_SEARCH_TYPE_EARLIEST_YEAR     BT_MCS_SEARCH_TYPE_EARLIEST_YEAR
/*!< Search for Latest Year */
#define ESP_BLE_AUDIO_MCS_SEARCH_TYPE_LATEST_YEAR       BT_MCS_SEARCH_TYPE_LATEST_YEAR
/*!< Search for Genre */
#define ESP_BLE_AUDIO_MCS_SEARCH_TYPE_GENRE             BT_MCS_SEARCH_TYPE_GENRE
/*!< Search for Tracks only */
#define ESP_BLE_AUDIO_MCS_SEARCH_TYPE_ONLY_TRACKS       BT_MCS_SEARCH_TYPE_ONLY_TRACKS
/*!< Search for Groups only */
#define ESP_BLE_AUDIO_MCS_SEARCH_TYPE_ONLY_GROUPS       BT_MCS_SEARCH_TYPE_ONLY_GROUPS

/*!< Search control point minimum length */
#define ESP_BLE_AUDIO_SEARCH_LEN_MIN                    SEARCH_LEN_MIN
/*!< Search control point maximum length */
#define ESP_BLE_AUDIO_SEARCH_LEN_MAX                    SEARCH_LEN_MAX

/*!< Search control point item (SCI) minimum length */
#define ESP_BLE_AUDIO_SEARCH_SCI_LEN_MIN                SEARCH_SCI_LEN_MIN

/*!< Search parameters maximum length */
#define ESP_BLE_AUDIO_SEARCH_PARAM_MAX                  SEARCH_PARAM_MAX

/*!< Search request was accepted; search has started. */
#define ESP_BLE_AUDIO_MCS_SCP_NTF_SUCCESS               BT_MCS_SCP_NTF_SUCCESS
/*!< Search request was invalid; no search started. */
#define ESP_BLE_AUDIO_MCS_SCP_NTF_FAILURE               BT_MCS_SCP_NTF_FAILURE

/*!< Group object type is track */
#define ESP_BLE_AUDIO_MCS_GROUP_OBJECT_TRACK_TYPE       BT_MCS_GROUP_OBJECT_TRACK_TYPE
/*!< Group object type is group */
#define ESP_BLE_AUDIO_MCS_GROUP_OBJECT_GROUP_TYPE       BT_MCS_GROUP_OBJECT_GROUP_TYPE

/*!< A single track is played once; there is no next track. */
#define ESP_BLE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_SINGLE_ONCE      BT_MCS_PLAYING_ORDERS_SUPPORTED_SINGLE_ONCE
/*!< A single track is played repeatedly; the next track is the current track. */
#define ESP_BLE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_SINGLE_REPEAT    BT_MCS_PLAYING_ORDERS_SUPPORTED_SINGLE_REPEAT
/*!< The tracks within a group are played once in track order. */
#define ESP_BLE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_INORDER_ONCE     BT_MCS_PLAYING_ORDERS_SUPPORTED_INORDER_ONCE
/*!< The tracks within a group are played in track order repeatedly. */
#define ESP_BLE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_INORDER_REPEAT   BT_MCS_PLAYING_ORDERS_SUPPORTED_INORDER_REPEAT
/*!< The tracks within a group are played once only from the oldest first. */
#define ESP_BLE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_OLDEST_ONCE      BT_MCS_PLAYING_ORDERS_SUPPORTED_OLDEST_ONCE
/*!< The tracks within a group are played from the oldest first repeatedly. */
#define ESP_BLE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_OLDEST_REPEAT    BT_MCS_PLAYING_ORDERS_SUPPORTED_OLDEST_REPEAT
/*!< The tracks within a group are played once only from the newest first. */
#define ESP_BLE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_NEWEST_ONCE      BT_MCS_PLAYING_ORDERS_SUPPORTED_NEWEST_ONCE
/*!< The tracks within a group are played from the newest first repeatedly. */
#define ESP_BLE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_NEWEST_REPEAT    BT_MCS_PLAYING_ORDERS_SUPPORTED_NEWEST_REPEAT
/*!< The tracks within a group are played in random order once. */
#define ESP_BLE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_SHUFFLE_ONCE     BT_MCS_PLAYING_ORDERS_SUPPORTED_SHUFFLE_ONCE
/*!< The tracks within a group are played in random order repeatedly. */
#define ESP_BLE_AUDIO_MCS_PLAYING_ORDERS_SUPPORTED_SHUFFLE_REPEAT   BT_MCS_PLAYING_ORDERS_SUPPORTED_SHUFFLE_REPEAT

/**
 * Check if an Object ID is valid for MCS.
 * This differs from BT_OTS_VALID_OBJ_ID as MCS does not use the directory list object.
 *
 * @param   id              Object ID to validate.
 */
#define ESP_BLE_AUDIO_MCS_VALID_OBJ_ID(id) \
    (IN_RANGE((id), BT_OTS_OBJ_ID_MIN, BT_OTS_OBJ_ID_MAX))

#ifdef __cplusplus
}
#endif

#endif /* ESP_BLE_AUDIO_MCS_DEFS_H_ */
