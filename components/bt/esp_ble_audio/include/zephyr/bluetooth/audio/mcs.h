/**
 * @file
 * @brief Bluetooth Media Control Service (MCS) APIs.
 */
/*
 * SPDX-FileCopyrightText: 2019-2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_MCS_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_MCS_H_

/**
 * @brief Media Control Service (MCS)
 *
 * @defgroup bt_mcs Media Control Service (MCS)
 *
 * @since 3.0
 * @version 0.8.0
 *
 * @ingroup bluetooth
 * @{
 *
 * Definitions and types related to the Media Control Service and Media Control
 * Profile specifications.
 */

#include <zephyr/sys/util.h>
#include <zephyr/sys/util_macro.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A characteristic value has changed while a Read Long Value Characteristic sub-procedure is in
 * progress.
 */
#define BT_MCS_ERR_LONG_VAL_CHANGED     0x80U

/**
 * @name Playback speeds
 *
 * The playback speed (s) is calculated by the value of 2 to the power of p divided by 64.
 * All values from -128 to 127 allowed, only some examples defined.
 * @{
 */
/** Minimum playback speed, resulting in 25 % speed */
#define BT_MCS_PLAYBACK_SPEED_MIN     -128
/** Quarter playback speed, resulting in 25 % speed */
#define BT_MCS_PLAYBACK_SPEED_QUARTER -128
/** Half playback speed, resulting in 50 % speed */
#define BT_MCS_PLAYBACK_SPEED_HALF     -64
/** Unity playback speed, resulting in 100 % speed */
#define BT_MCS_PLAYBACK_SPEED_UNITY      0
/** Double playback speed, resulting in 200 % speed */
#define BT_MCS_PLAYBACK_SPEED_DOUBLE    64
/** Max playback speed, resulting in 395.7 % speed (nearly 400 %) */
#define BT_MCS_PLAYBACK_SPEED_MAX      127
/** @} */

/**
 * @name Seeking speed
 *
 * The allowed values for seeking speed are the range -64 to -4
 * (endpoints included), the value 0, and the range 4 to 64
 * (endpoints included).
 * @{
 */
/** Maximum seeking speed - Can be negated */
#define BT_MCS_SEEKING_SPEED_FACTOR_MAX  64
/** Minimum seeking speed - Can be negated */
#define BT_MCS_SEEKING_SPEED_FACTOR_MIN   4
/** No seeking */
#define BT_MCS_SEEKING_SPEED_FACTOR_ZERO  0
/** @} */

/**
 * @name Playing orders
 * @{
 */
/** A single track is played once; there is no next track. */
#define BT_MCS_PLAYING_ORDER_SINGLE_ONCE    0x01U
/** A single track is played repeatedly; the next track is the current track. */
#define BT_MCS_PLAYING_ORDER_SINGLE_REPEAT  0x02U
/** The tracks within a group are played once in track order. */
#define BT_MCS_PLAYING_ORDER_INORDER_ONCE   0x03U
/** The tracks within a group are played in track order repeatedly. */
#define BT_MCS_PLAYING_ORDER_INORDER_REPEAT 0x04U
/** The tracks within a group are played once only from the oldest first. */
#define BT_MCS_PLAYING_ORDER_OLDEST_ONCE    0x05U
/** The tracks within a group are played from the oldest first repeatedly. */
#define BT_MCS_PLAYING_ORDER_OLDEST_REPEAT  0x06U
/** The tracks within a group are played once only from the newest first. */
#define BT_MCS_PLAYING_ORDER_NEWEST_ONCE    0x07U
/** The tracks within a group are played from the newest first repeatedly. */
#define BT_MCS_PLAYING_ORDER_NEWEST_REPEAT  0x08U
/** The tracks within a group are played in random order once. */
#define BT_MCS_PLAYING_ORDER_SHUFFLE_ONCE   0x09U
/** The tracks within a group are played in random order repeatedly. */
#define BT_MCS_PLAYING_ORDER_SHUFFLE_REPEAT 0x0AU
/** @} */

/** @name Playing orders supported
 *
 * A bitmap, in the same order as the playing orders above.
 * Note that playing order 1 corresponds to bit 0, and so on.
 * @{
 */
/** A single track is played once; there is no next track. */
#define BT_MCS_PLAYING_ORDERS_SUPPORTED_SINGLE_ONCE    BIT(0U)
/** A single track is played repeatedly; the next track is the current track. */
#define BT_MCS_PLAYING_ORDERS_SUPPORTED_SINGLE_REPEAT  BIT(1U)
/** The tracks within a group are played once in track order. */
#define BT_MCS_PLAYING_ORDERS_SUPPORTED_INORDER_ONCE   BIT(2U)
/** The tracks within a group are played in track order repeatedly. */
#define BT_MCS_PLAYING_ORDERS_SUPPORTED_INORDER_REPEAT BIT(3U)
/** The tracks within a group are played once only from the oldest first. */
#define BT_MCS_PLAYING_ORDERS_SUPPORTED_OLDEST_ONCE    BIT(4U)
/** The tracks within a group are played from the oldest first repeatedly. */
#define BT_MCS_PLAYING_ORDERS_SUPPORTED_OLDEST_REPEAT  BIT(5U)
/** The tracks within a group are played once only from the newest first. */
#define BT_MCS_PLAYING_ORDERS_SUPPORTED_NEWEST_ONCE    BIT(6U)
/** The tracks within a group are played from the newest first repeatedly. */
#define BT_MCS_PLAYING_ORDERS_SUPPORTED_NEWEST_REPEAT  BIT(7U)
/** The tracks within a group are played in random order once. */
#define BT_MCS_PLAYING_ORDERS_SUPPORTED_SHUFFLE_ONCE   BIT(8U)
/** The tracks within a group are played in random order repeatedly. */
#define BT_MCS_PLAYING_ORDERS_SUPPORTED_SHUFFLE_REPEAT BIT(9U)
/** @} */

/**
 * @name Media states
 * @{
 */
/** The current track is invalid, and no track has been selected. */
#define BT_MCS_MEDIA_STATE_INACTIVE 0x00U
/** The media player is playing the current track. */
#define BT_MCS_MEDIA_STATE_PLAYING  0x01U
/** The current track is paused. The media player has a current track, but it is not being played */
#define BT_MCS_MEDIA_STATE_PAUSED   0x02U
/** The current track is fast forwarding or fast rewinding. */
#define BT_MCS_MEDIA_STATE_SEEKING  0x03U
/** @} */

/**
 * @name Media control point opcodes
 * @{
 */
/** Start playing the current track. */
#define BT_MCS_OPC_PLAY          0x01U
/** Pause playing the current track. */
#define BT_MCS_OPC_PAUSE         0x02U
/** Fast rewind the current track. */
#define BT_MCS_OPC_FAST_REWIND   0x03U
/** Fast forward the current track. */
#define BT_MCS_OPC_FAST_FORWARD  0x04U
/**
 * Stop current activity and return to the paused state and set the current track position to the
 * start of the current track.
 */
#define BT_MCS_OPC_STOP          0x05U

/** Set a new current track position relative to the current track position. */
#define BT_MCS_OPC_MOVE_RELATIVE 0x10U

/**
 * Set the current track position to the starting position of the previous segment of the current
 * track.
 */
#define BT_MCS_OPC_PREV_SEGMENT  0x20U
/**
 * Set the current track position to the starting position of
 * the next segment of the current track.
 */
#define BT_MCS_OPC_NEXT_SEGMENT  0x21U
/**
 * Set the current track position to the starting position of
 * the first segment of the current track.
 */
#define BT_MCS_OPC_FIRST_SEGMENT 0x22U
/**
 * Set the current track position to the starting position of
 * the last segment of the current track.
 */
#define BT_MCS_OPC_LAST_SEGMENT  0x23U
/**
 * Set the current track position to the starting position of
 * the nth segment of the current track.
 */
#define BT_MCS_OPC_GOTO_SEGMENT  0x24U

/** Set the current track to the previous track based on the playing order. */
#define BT_MCS_OPC_PREV_TRACK    0x30U
/** Set the current track to the next track based on the playing order. */
#define BT_MCS_OPC_NEXT_TRACK    0x31U
/** Set the current track to the first track based on the playing order. */
#define BT_MCS_OPC_FIRST_TRACK   0x32U
/** Set the current track to the last track based on the playing order. */
#define BT_MCS_OPC_LAST_TRACK    0x33U
/** Set the current track to the nth track based on the playing order. */
#define BT_MCS_OPC_GOTO_TRACK    0x34U

/** Set the current group to the previous group in the sequence of groups. */
#define BT_MCS_OPC_PREV_GROUP    0x40U
/** Set the current group to the next group in the sequence of groups. */
#define BT_MCS_OPC_NEXT_GROUP    0x41U
/** Set the current group to the first group in the sequence of groups. */
#define BT_MCS_OPC_FIRST_GROUP   0x42U
/** Set the current group to the last group in the sequence of groups. */
#define BT_MCS_OPC_LAST_GROUP    0x43U
/** Set the current group to the nth group in the sequence of groups. */
#define BT_MCS_OPC_GOTO_GROUP    0x44U
/** @} */

/** Media control point supported opcodes length */
#define BT_MCS_OPCODES_SUPPORTED_LEN 4U

/**
 * @name Media control point supported opcodes values
 * @{
 */
/** Support the Play opcode */
#define BT_MCS_OPC_SUP_PLAY          BIT(0U)
/** Support the Pause opcode */
#define BT_MCS_OPC_SUP_PAUSE         BIT(1U)
/** Support the Fast Rewind opcode */
#define BT_MCS_OPC_SUP_FAST_REWIND   BIT(2U)
/** Support the Fast Forward opcode */
#define BT_MCS_OPC_SUP_FAST_FORWARD  BIT(3U)
/** Support the Stop opcode */
#define BT_MCS_OPC_SUP_STOP          BIT(4U)

/** Support the Move Relative opcode */
#define BT_MCS_OPC_SUP_MOVE_RELATIVE BIT(5U)

/** Support the Previous Segment opcode */
#define BT_MCS_OPC_SUP_PREV_SEGMENT  BIT(6U)
/** Support the Next Segment opcode */
#define BT_MCS_OPC_SUP_NEXT_SEGMENT  BIT(7U)
/** Support the First Segment opcode */
#define BT_MCS_OPC_SUP_FIRST_SEGMENT BIT(8U)
/** Support the Last Segment opcode */
#define BT_MCS_OPC_SUP_LAST_SEGMENT  BIT(9U)
/** Support the Goto Segment opcode */
#define BT_MCS_OPC_SUP_GOTO_SEGMENT  BIT(10U)

/** Support the Previous Track opcode */
#define BT_MCS_OPC_SUP_PREV_TRACK    BIT(11U)
/** Support the Next Track opcode */
#define BT_MCS_OPC_SUP_NEXT_TRACK    BIT(12U)
/** Support the First Track opcode */
#define BT_MCS_OPC_SUP_FIRST_TRACK   BIT(13U)
/** Support the Last Track opcode */
#define BT_MCS_OPC_SUP_LAST_TRACK    BIT(14U)
/** Support the Goto Track opcode */
#define BT_MCS_OPC_SUP_GOTO_TRACK    BIT(15U)

/** Support the Previous Group opcode */
#define BT_MCS_OPC_SUP_PREV_GROUP    BIT(16U)
/** Support the Next Group opcode */
#define BT_MCS_OPC_SUP_NEXT_GROUP    BIT(17U)
/** Support the First Group opcode */
#define BT_MCS_OPC_SUP_FIRST_GROUP   BIT(18U)
/** Support the Last Group opcode */
#define BT_MCS_OPC_SUP_LAST_GROUP    BIT(19U)
/** Support the Goto Group opcode */
#define BT_MCS_OPC_SUP_GOTO_GROUP    BIT(20U)
/** @} */

/**
 * @name Media control point notification result codes
 * @{
 */
/** Action requested by the opcode write was completed successfully. */
#define BT_MCS_OPC_NTF_SUCCESS             0x01U
/** An invalid or unsupported opcode was used for the Media Control Point write. */
#define BT_MCS_OPC_NTF_NOT_SUPPORTED       0x02U
/**
 * The Media Player State characteristic value is Inactive when the opcode is received or the
 * result of the requested action of the opcode results in the Media Player State characteristic
 * being set to Inactive.
 */
#define BT_MCS_OPC_NTF_PLAYER_INACTIVE     0x03U
/**
 * The requested action of any Media Control Point write cannot be completed successfully because of
 * a condition within the player.
 */
#define BT_MCS_OPC_NTF_CANNOT_BE_COMPLETED 0x04U
/** @} */

/**
 * @name Search control point type values
 *
 * Reference: Media Control Service spec v1.0 section 3.20.2
 * @{
 */
/** Search for Track Name */
#define BT_MCS_SEARCH_TYPE_TRACK_NAME    0x01U
/** Search for Artist Name */
#define BT_MCS_SEARCH_TYPE_ARTIST_NAME   0x02U
/** Search for Album Name */
#define BT_MCS_SEARCH_TYPE_ALBUM_NAME    0x03U
/** Search for Group Name */
#define BT_MCS_SEARCH_TYPE_GROUP_NAME    0x04U
/** Search for Earliest Year */
#define BT_MCS_SEARCH_TYPE_EARLIEST_YEAR 0x05U
/** Search for Latest Year */
#define BT_MCS_SEARCH_TYPE_LATEST_YEAR   0x06U
/** Search for Genre */
#define BT_MCS_SEARCH_TYPE_GENRE         0x07U
/** Search for Tracks only */
#define BT_MCS_SEARCH_TYPE_ONLY_TRACKS   0x08U
/** Search for Groups only */
#define BT_MCS_SEARCH_TYPE_ONLY_GROUPS   0x09U
/** @} */

/**
 * @brief Search control point minimum length
 *
 * At least one search control item (SCI), consisting of the length octet and the type octet.
 * (The * parameter field may be empty.)
 */
#define SEARCH_LEN_MIN 2U

/** Search control point maximum length */
#define SEARCH_LEN_MAX 64U

/**
 * @brief Search control point item (SCI) minimum length
 *
 * An SCI length can be as little as one byte, for an SCI that has only the type field.
 * (The SCI len is the length of type + param.)
 */
#define SEARCH_SCI_LEN_MIN 1   /* An SCI length can be as little as one byte,
                * for an SCI that has only the type field.
                * (The SCI len is the length of type + param.)
                */

/** Search parameters maximum length  */
#define SEARCH_PARAM_MAX 62U

/**
 * @name Search notification result codes
 *
 * Reference: Media Control Service spec v1.0 section 3.20.2
 * @{
 */
/** Search request was accepted; search has started. */
#define BT_MCS_SCP_NTF_SUCCESS  0x01U
/** Search request was invalid; no search started. */
#define BT_MCS_SCP_NTF_FAILURE  0x02U
/** @} */

/**
 * @name Group object object types
 *
 * Reference: Media Control Service spec v1.0 section 4.4.1
 * @{
 */
/** Group object type is track */
#define BT_MCS_GROUP_OBJECT_TRACK_TYPE 0x00U
/** Group object type is group */
#define BT_MCS_GROUP_OBJECT_GROUP_TYPE 0x01U
/** @} */

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_MCS_H_ */
