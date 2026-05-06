/**
 * @file
 * @brief Bluetooth Media Proxy APIs
 */

/*
 * SPDX-FileCopyrightText: 2019-2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_MEDIA_PROXY_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_MEDIA_PROXY_H_

/**
 * @brief Media proxy module
 *
 * @defgroup bt_media_proxy Media Proxy
 *
 * @since 3.0
 * @version 0.8.0
 *
 * @ingroup bluetooth
 * @{
 *
 * The media proxy module is the connection point between media players
 * and media controllers.
 *
 * A media player has (access to) media content and knows how to
 * navigate and play this content. A media controller reads or gets
 * information from a player and controls the player by setting player
 * parameters and giving the player commands.
 *
 * The media proxy module allows media player implementations to make
 * themselves available to media controllers. And it allows
 * controllers to access, and get updates from, any player.
 *
 * The media proxy module allows both local and remote control of
 * local player instances: A media controller may be a local
 * application, or it may be a Media Control Service relaying requests
 * from a remote Media Control Client. There may be either local or
 * remote control, or both, or even multiple instances of each.
 */

#include <stdint.h>
#include <stdbool.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/sys/util_macro.h>

/* TODO: Remove dependency on mcs.h */
#include "mcs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Media player command
 */
struct mpl_cmd {
    /** The opcode. See the MEDIA_PROXY_OP_* values */
    uint8_t  opcode;
    /** Whether or not the @ref mpl_cmd.param is used */
    bool  use_param;
    /** A 32-bit signed parameter. The parameter value depends on the @ref mpl_cmd.opcode */
    int32_t param;
};

/**
 * @brief Media command notification
 */
struct mpl_cmd_ntf {
    /** The opcode that was sent */
    uint8_t requested_opcode;
    /** The result of the operation  */
    uint8_t result_code;
};

/**
 * @brief Search control item
 */
struct mpl_sci {
    uint8_t len;                     /**< Length of type and parameter */
    uint8_t type;                    /**< MEDIA_PROXY_SEARCH_TYPE_<...> */
    char    param[SEARCH_PARAM_MAX]; /**< Search parameter */
};

/**
 * @brief Search
 */
struct mpl_search {
    /** The length of the @ref mpl_search.search value */
    uint8_t len;
    /** Concatenated search control items - (type, length, param) */
    char search[SEARCH_LEN_MAX];
};

/**
 * @name Playback speed parameters
 *
 * All values from -128 to 127 allowed, only some defined
 * @{
 */
/** Minimum playback speed, resulting in 25 % speed */
#define MEDIA_PROXY_PLAYBACK_SPEED_MIN     -128
/** Quarter playback speed, resulting in 25 % speed */
#define MEDIA_PROXY_PLAYBACK_SPEED_QUARTER -128
/** Half playback speed, resulting in 50 % speed */
#define MEDIA_PROXY_PLAYBACK_SPEED_HALF    -64
/** Unity playback speed, resulting in 100 % speed */
#define MEDIA_PROXY_PLAYBACK_SPEED_UNITY   0
/** Double playback speed, resulting in 200 % speed */
#define MEDIA_PROXY_PLAYBACK_SPEED_DOUBLE  64
/** Max playback speed, resulting in 395.7 % speed (nearly 400 %) */
#define MEDIA_PROXY_PLAYBACK_SPEED_MAX     127
/** @} */

/**
 * @name Seeking speed factors
 *
 * The allowed values for seeking speed are the range -64 to -4
 * (endpoints included), the value 0, and the range 4 to 64
 * (endpoints included).
 * @{
 */
/** Maximum seeking speed - Can be negated */
#define MEDIA_PROXY_SEEKING_SPEED_FACTOR_MAX  64
/** Minimum seeking speed - Can be negated */
#define MEDIA_PROXY_SEEKING_SPEED_FACTOR_MIN  4
/** No seeking */
#define MEDIA_PROXY_SEEKING_SPEED_FACTOR_ZERO 0
/** @} */

/**
 * @name Playing orders
 * @{
 */
/** A single track is played once; there is no next track. */
#define MEDIA_PROXY_PLAYING_ORDER_SINGLE_ONCE    0x01
/** A single track is played repeatedly; the next track is the current track. */
#define MEDIA_PROXY_PLAYING_ORDER_SINGLE_REPEAT  0x02
/** The tracks within a group are played once in track order. */
#define MEDIA_PROXY_PLAYING_ORDER_INORDER_ONCE   0x03
/** The tracks within a group are played in track order repeatedly. */
#define MEDIA_PROXY_PLAYING_ORDER_INORDER_REPEAT 0x04
/** The tracks within a group are played once only from the oldest first. */
#define MEDIA_PROXY_PLAYING_ORDER_OLDEST_ONCE    0x05
/** The tracks within a group are played from the oldest first repeatedly. */
#define MEDIA_PROXY_PLAYING_ORDER_OLDEST_REPEAT  0x06
/** The tracks within a group are played once only from the newest first. */
#define MEDIA_PROXY_PLAYING_ORDER_NEWEST_ONCE    0x07
/** The tracks within a group are played from the newest first repeatedly. */
#define MEDIA_PROXY_PLAYING_ORDER_NEWEST_REPEAT  0x08
/** The tracks within a group are played in random order once. */
#define MEDIA_PROXY_PLAYING_ORDER_SHUFFLE_ONCE   0x09
/** The tracks within a group are played in random order repeatedly. */
#define MEDIA_PROXY_PLAYING_ORDER_SHUFFLE_REPEAT 0x0a
/** @} */

/**
 * @name Playing orders supported
 *
 * A bitmap, in the same order as the playing orders above.
 * Note that playing order 1 corresponds to bit 0, and so on.
 * @{
 */
/** A single track is played once; there is no next track. */
#define MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_SINGLE_ONCE    BIT(0)
/** A single track is played repeatedly; the next track is the current track. */
#define MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_SINGLE_REPEAT  BIT(1)
/** The tracks within a group are played once in track order. */
#define MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_INORDER_ONCE   BIT(2)
/** The tracks within a group are played in track order repeatedly. */
#define MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_INORDER_REPEAT BIT(3)
/** The tracks within a group are played once only from the oldest first. */
#define MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_OLDEST_ONCE    BIT(4)
/** The tracks within a group are played from the oldest first repeatedly. */
#define MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_OLDEST_REPEAT  BIT(5)
/** The tracks within a group are played once only from the newest first. */
#define MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_NEWEST_ONCE    BIT(6)
/** The tracks within a group are played from the newest first repeatedly. */
#define MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_NEWEST_REPEAT  BIT(7)
/** The tracks within a group are played in random order once. */
#define MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_SHUFFLE_ONCE   BIT(8)
/** The tracks within a group are played in random order repeatedly. */
#define MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_SHUFFLE_REPEAT BIT(9)
/** @} */

/**
 * @name Media player states
 * @{
 */
/** The current track is invalid, and no track has been selected. */
#define MEDIA_PROXY_STATE_INACTIVE 0x00
/** The media player is playing the current track. */
#define MEDIA_PROXY_STATE_PLAYING  0x01
/** The current track is paused. The media player has a current track, but it is not being played */
#define MEDIA_PROXY_STATE_PAUSED   0x02
/** The current track is fast forwarding or fast rewinding. */
#define MEDIA_PROXY_STATE_SEEKING  0x03
/** Used internally as the last state value */
#define MEDIA_PROXY_STATE_LAST     0x04
/** @} */

/**
 * @name Media player command opcodes
 * @{
 */
/** Start playing the current track. */
#define MEDIA_PROXY_OP_PLAY         0x01
/** Pause playing the current track. */
#define MEDIA_PROXY_OP_PAUSE        0x02
/** Fast rewind the current track. */
#define MEDIA_PROXY_OP_FAST_REWIND  0x03
/** Fast forward the current track. */
#define MEDIA_PROXY_OP_FAST_FORWARD 0x04
/**
 * Stop current activity and return to the paused state and set the current track position to the
 * start of the current track.
 */
#define MEDIA_PROXY_OP_STOP         0x05

/** Set a new current track position relative to the current track position. */
#define MEDIA_PROXY_OP_MOVE_RELATIVE 0x10

/**
 * Set the current track position to the starting position of the previous segment of the current
 * track.
 */
#define MEDIA_PROXY_OP_PREV_SEGMENT  0x20
/**
 * Set the current track position to the starting position of
 * the next segment of the current track.
 */
#define MEDIA_PROXY_OP_NEXT_SEGMENT  0x21
/**
 * Set the current track position to the starting position of
 * the first segment of the current track.
 */
#define MEDIA_PROXY_OP_FIRST_SEGMENT 0x22
/**
 * Set the current track position to the starting position of
 * the last segment of the current track.
 */
#define MEDIA_PROXY_OP_LAST_SEGMENT  0x23
/**
 * Set the current track position to the starting position of
 * the nth segment of the current track.
 */
#define MEDIA_PROXY_OP_GOTO_SEGMENT  0x24

/** Set the current track to the previous track based on the playing order. */
#define MEDIA_PROXY_OP_PREV_TRACK  0x30
/** Set the current track to the next track based on the playing order. */
#define MEDIA_PROXY_OP_NEXT_TRACK  0x31
/** Set the current track to the first track based on the playing order. */
#define MEDIA_PROXY_OP_FIRST_TRACK 0x32
/** Set the current track to the last track based on the playing order. */
#define MEDIA_PROXY_OP_LAST_TRACK  0x33
/** Set the current track to the nth track based on the playing order. */
#define MEDIA_PROXY_OP_GOTO_TRACK  0x34

/** Set the current group to the previous group in the sequence of groups. */
#define MEDIA_PROXY_OP_PREV_GROUP  0x40
/** Set the current group to the next group in the sequence of groups. */
#define MEDIA_PROXY_OP_NEXT_GROUP  0x41
/** Set the current group to the first group in the sequence of groups. */
#define MEDIA_PROXY_OP_FIRST_GROUP 0x42
/** Set the current group to the last group in the sequence of groups. */
#define MEDIA_PROXY_OP_LAST_GROUP  0x43
/** Set the current group to the nth group in the sequence of groups. */
#define MEDIA_PROXY_OP_GOTO_GROUP  0x44
/** @} */

/**
 * @brief Media player supported opcodes length
 */
#define MEDIA_PROXY_OPCODES_SUPPORTED_LEN 4

/**
 * @brief Media player supported command opcodes
 * @{
 */
/** Support the Play opcode */
#define MEDIA_PROXY_OP_SUP_PLAY         BIT(0)
/** Support the Pause opcode */
#define MEDIA_PROXY_OP_SUP_PAUSE        BIT(1)
/** Support the Fast Rewind opcode */
#define MEDIA_PROXY_OP_SUP_FAST_REWIND  BIT(2)
/** Support the Fast Forward opcode */
#define MEDIA_PROXY_OP_SUP_FAST_FORWARD BIT(3)
/** Support the Stop opcode */
#define MEDIA_PROXY_OP_SUP_STOP         BIT(4)

/** Support the Move Relative opcode */
#define MEDIA_PROXY_OP_SUP_MOVE_RELATIVE BIT(5)

/** Support the Previous Segment opcode */
#define MEDIA_PROXY_OP_SUP_PREV_SEGMENT  BIT(6)
/** Support the Next Segment opcode */
#define MEDIA_PROXY_OP_SUP_NEXT_SEGMENT  BIT(7)
/** Support the First Segment opcode */
#define MEDIA_PROXY_OP_SUP_FIRST_SEGMENT BIT(8)
/** Support the Last Segment opcode */
#define MEDIA_PROXY_OP_SUP_LAST_SEGMENT  BIT(9)
/** Support the Goto Segment opcode */
#define MEDIA_PROXY_OP_SUP_GOTO_SEGMENT  BIT(10)

/** Support the Previous Track opcode */
#define MEDIA_PROXY_OP_SUP_PREV_TRACK  BIT(11)
/** Support the Next Track opcode */
#define MEDIA_PROXY_OP_SUP_NEXT_TRACK  BIT(12)
/** Support the First Track opcode */
#define MEDIA_PROXY_OP_SUP_FIRST_TRACK BIT(13)
/** Support the Last Track opcode */
#define MEDIA_PROXY_OP_SUP_LAST_TRACK  BIT(14)
/** Support the Goto Track opcode */
#define MEDIA_PROXY_OP_SUP_GOTO_TRACK  BIT(15)

/** Support the Previous Group opcode */
#define MEDIA_PROXY_OP_SUP_PREV_GROUP  BIT(16)
/** Support the Next Group opcode */
#define MEDIA_PROXY_OP_SUP_NEXT_GROUP  BIT(17)
/** Support the First Group opcode */
#define MEDIA_PROXY_OP_SUP_FIRST_GROUP BIT(18)
/** Support the Last Group opcode */
#define MEDIA_PROXY_OP_SUP_LAST_GROUP  BIT(19)
/** Support the Goto Group opcode */
#define MEDIA_PROXY_OP_SUP_GOTO_GROUP  BIT(20)
/** @} */

/**
 * @name Media player command result codes
 * @{
 */
/** Action requested by the opcode write was completed successfully. */
#define MEDIA_PROXY_CMD_SUCCESS             0x01
/** An invalid or unsupported opcode was used for the Media Control Point write. */
#define MEDIA_PROXY_CMD_NOT_SUPPORTED       0x02
/**
 * The Media Player State characteristic value is Inactive when the opcode is received or the
 * result of the requested action of the opcode results in the Media Player State characteristic
 * being set to Inactive.
 */
#define MEDIA_PROXY_CMD_PLAYER_INACTIVE     0x03
/**
 * The requested action of any Media Control Point write cannot be completed successfully because of
 * a condition within the player.
 */
#define MEDIA_PROXY_CMD_CANNOT_BE_COMPLETED 0x04
/** @} */

/**
 * @name Search operation type values
 * @{
 */
/** Search for Track Name */
#define MEDIA_PROXY_SEARCH_TYPE_TRACK_NAME    0x01
/** Search for Artist Name */
#define MEDIA_PROXY_SEARCH_TYPE_ARTIST_NAME   0x02
/** Search for Album Name */
#define MEDIA_PROXY_SEARCH_TYPE_ALBUM_NAME    0x03
/** Search for Group Name */
#define MEDIA_PROXY_SEARCH_TYPE_GROUP_NAME    0x04
/** Search for Earliest Year */
#define MEDIA_PROXY_SEARCH_TYPE_EARLIEST_YEAR 0x05
/** Search for Latest Year */
#define MEDIA_PROXY_SEARCH_TYPE_LATEST_YEAR   0x06
/** Search for Genre */
#define MEDIA_PROXY_SEARCH_TYPE_GENRE         0x07
/** Search for Tracks only */
#define MEDIA_PROXY_SEARCH_TYPE_ONLY_TRACKS   0x08
/** Search for Groups only */
#define MEDIA_PROXY_SEARCH_TYPE_ONLY_GROUPS   0x09
/** @} */

/**
 * @name Search notification result codes
 *
 * @{
 */
/** Search request was accepted; search has started. */
#define MEDIA_PROXY_SEARCH_SUCCESS 0x01
/** Search request was invalid; no search started. */
#define MEDIA_PROXY_SEARCH_FAILURE 0x02
/** @} */

/**
 * @name Group object object types
 *
 * @{
 */
/** Group object type is track */
#define MEDIA_PROXY_GROUP_OBJECT_TRACK_TYPE 0x00
/** Group object type is group */
#define MEDIA_PROXY_GROUP_OBJECT_GROUP_TYPE 0x01
/** @} */

/**
 * @brief Opaque media player instance
 */
struct media_player;

/* PUBLIC API FOR CONTROLLERS */

/**
 * @brief Callbacks to a controller, from the media proxy
 *
 * Given by a controller when registering
 */
struct media_proxy_ctrl_cbs {

    /**
     * @brief Media Player Instance callback
     *
     * Called when the local Media Player instance is registered or read (TODO).
     * Also called if the local player instance is already registered when
     * the controller is registered.
     * Provides the controller with the pointer to the local player instance.
     *
     * @param player   Media player instance pointer
     * @param err      Error value. 0 on success, or errno on negative value.
     */
    void (*local_player_instance)(struct media_player *player, int err);

    /**
     * @brief Discover Player Instance callback
     *
     * Called when a remote player instance has been discovered.
     * The instance has been discovered, and will be accessed, using Bluetooth,
     * via media control client and a remote media control service.
     *
     * @param player   Instance pointer to the remote player
     * @param err      Error value. 0 on success, GATT error on positive value
     *                 or errno on negative value.
     */
    void (*discover_player)(struct media_player *player, int err);

    /**
     * @brief Media Player Name receive callback
     *
     * Called when the Media Player Name is read or changed
     * See also media_proxy_ctrl_name_get()
     *
     * @param player   Media player instance pointer
     * @param err      Error value. 0 on success, GATT error on positive value
     *                 or errno on negative value.
     * @param name     The name of the media player
     */
    void (*player_name_recv)(struct media_player *player, int err, const char *name);

    /**
     * @brief Media Player Icon Object ID receive callback
     *
     * Called when the Media Player Icon Object ID is read
     * See also media_proxy_ctrl_get_icon_id()
     *
     * @param player   Media player instance pointer
     * @param err      Error value. 0 on success, GATT error on positive value
     *                 or errno on negative value.
     * @param id       The ID of the Icon object in the Object Transfer Service (48 bits)
     */
    void (*icon_id_recv)(struct media_player *player, int err, uint64_t id);

    /**
     * @brief Media Player Icon URL receive callback
     *
     * Called when the Media Player Icon URL is read
     * See also media_proxy_ctrl_get_icon_url()
     *
     * @param player   Media player instance pointer
     * @param err      Error value. 0 on success, GATT error on positive value
     *                 or errno on negative value.
     * @param url      The URL of the icon
     */
    void (*icon_url_recv)(struct media_player *player, int err, const char *url);

    /**
     * @brief Track changed receive callback
     *
     * Called when the Current Track is changed
     *
     * @param player   Media player instance pointer
     * @param err      Error value. 0 on success, GATT error on positive value
     *                 or errno on negative value.
     */
    void (*track_changed_recv)(struct media_player *player, int err);

    /**
     * @brief Track Title receive callback
     *
     * Called when the Track Title is read or changed
     * See also media_proxy_ctrl_get_track_title()
     *
     * @param player   Media player instance pointer
     * @param err      Error value. 0 on success, GATT error on positive value
     *                 or errno on negative value.
     * @param title    The title of the current track
     */
    void (*track_title_recv)(struct media_player *player, int err, const char *title);

    /**
     * @brief Track Duration receive callback
     *
     * Called when the Track Duration is read or changed
     * See also media_proxy_ctrl_get_track_duration()
     *
     * @param player     Media player instance pointer
     * @param err        Error value. 0 on success, GATT error on positive value
     *                   or errno on negative value.
     * @param duration   The duration of the current track
     */
    void (*track_duration_recv)(struct media_player *player, int err, int32_t duration);

    /**
     * @brief Track Position receive callback
     *
     * Called when the Track Position is read or changed
     * See also media_proxy_ctrl_get_track_position() and
     * media_proxy_ctrl_set_track_position()
     *
     * @param player     Media player instance pointer
     * @param err        Error value. 0 on success, GATT error on positive value
     *                   or errno on negative value.
     * @param position   The player's position in the track
     */
    void (*track_position_recv)(struct media_player *player, int err, int32_t position);

    /**
     * @brief Track Position write callback
     *
     * Called when the Track Position is written
     * See also media_proxy_ctrl_set_track_position().
     *
     * @param player     Media player instance pointer
     * @param err        Error value. 0 on success, GATT error on positive value
     *                   or errno on negative value.
     * @param position   The position given attempted to write
     */
    void (*track_position_write)(struct media_player *player, int err, int32_t position);

    /**
     * @brief Playback Speed receive callback
     *
     * Called when the Playback Speed is read or changed
     * See also media_proxy_ctrl_get_playback_speed() and
     * media_proxy_ctrl_set_playback_speed()
     *
     * @param player   Media player instance pointer
     * @param err      Error value. 0 on success, GATT error on positive value
     *                 or errno on negative value.
     * @param speed    The playback speed parameter
     */
    void (*playback_speed_recv)(struct media_player *player, int err, int8_t speed);

    /**
     * @brief Playback Speed write callback
     *
     * Called when the Playback Speed is written
     * See also media_proxy_ctrl_set_playback_speed()
     *
     * @param player   Media player instance pointer
     * @param err      Error value. 0 on success, GATT error on positive value
     *                 or errno on negative value.
     * @param speed    The playback speed parameter attempted to write
     */
    void (*playback_speed_write)(struct media_player *player, int err, int8_t speed);

    /**
     * @brief Seeking Speed receive callback
     *
     * Called when the Seeking Speed is read or changed
     * See also media_proxy_ctrl_get_seeking_speed()
     *
     * @param player   Media player instance pointer
     * @param err      Error value. 0 on success, GATT error on positive value
     *                 or errno on negative value.
     * @param speed    The seeking speed factor
     */
    void (*seeking_speed_recv)(struct media_player *player, int err, int8_t speed);

    /**
     * @brief Track Segments Object ID receive callback
     *
     * Called when the Track Segments Object ID is read
     * See also media_proxy_ctrl_get_track_segments_id()
     *
     * @param player   Media player instance pointer
     * @param err      Error value. 0 on success, GATT error on positive value
     *                 or errno on negative value.
     * @param id       The ID of the track segments object in Object Transfer Service (48 bits)
     */
    void (*track_segments_id_recv)(struct media_player *player, int err, uint64_t id);

    /**
     * @brief Current Track Object ID receive callback
     *
     * Called when the Current Track Object ID is read or changed
     * See also media_proxy_ctrl_get_current_track_id() and
     * media_proxy_ctrl_set_current_track_id()
     *
     * @param player   Media player instance pointer
     * @param err      Error value. 0 on success, GATT error on positive value
     *                 or errno on negative value.
     * @param id       The ID of the current track object in Object Transfer Service (48 bits)
     */
    void (*current_track_id_recv)(struct media_player *player, int err, uint64_t id);

    /**
     * @brief Current Track Object ID write callback
     *
     * Called when the Current Track Object ID is written
     * See also media_proxy_ctrl_set_current_track_id()
     *
     * @param player   Media player instance pointer
     * @param err      Error value. 0 on success, GATT error on positive value
     *                 or errno on negative value.
     * @param id       The ID (48 bits) attempted to write
     */
    void (*current_track_id_write)(struct media_player *player, int err, uint64_t id);

    /**
     * @brief Next Track Object ID receive callback
     *
     * Called when the Next Track Object ID is read or changed
     * See also media_proxy_ctrl_get_next_track_id() and
     * media_proxy_ctrl_set_next_track_id()
     *
     * @param player   Media player instance pointer
     * @param err      Error value. 0 on success, GATT error on positive value
     *                 or errno on negative value.
     * @param id       The ID of the next track object in Object Transfer Service (48 bits)
     */
    void (*next_track_id_recv)(struct media_player *player, int err, uint64_t id);

    /**
     * @brief Next Track Object ID write callback
     *
     * Called when the Next Track Object ID is written
     * See also media_proxy_ctrl_set_next_track_id()
     *
     * @param player   Media player instance pointer
     * @param err      Error value. 0 on success, GATT error on positive value
     *                 or errno on negative value.
     * @param id       The ID (48 bits) attempted to write
     */
    void (*next_track_id_write)(struct media_player *player, int err, uint64_t id);

    /**
     * @brief Parent Group Object ID receive callback
     *
     * Called when the Parent Group Object ID is read or changed
     * See also media_proxy_ctrl_get_parent_group_id()
     *
     * @param player   Media player instance pointer
     * @param err      Error value. 0 on success, GATT error on positive value
     *                 or errno on negative value.
     * @param id       The ID of the parent group object in Object Transfer Service (48 bits)
     */
    void (*parent_group_id_recv)(struct media_player *player, int err, uint64_t id);

    /**
     * @brief Current Group Object ID receive callback
     *
     * Called when the Current Group Object ID is read or changed
     * See also media_proxy_ctrl_get_current_group_id() and
     * media_proxy_ctrl_set_current_group_id()
     *
     * @param player   Media player instance pointer
     * @param err      Error value. 0 on success, GATT error on positive value
     *                 or errno on negative value.
     * @param id       The ID of the current group object in Object Transfer Service (48 bits)
     */
    void (*current_group_id_recv)(struct media_player *player, int err, uint64_t id);

    /**
     * @brief Current Group Object ID write callback
     *
     * Called when the Current Group Object ID is written
     * See also media_proxy_ctrl_set_current_group_id()
     *
     * @param player   Media player instance pointer
     * @param err      Error value. 0 on success, GATT error on positive value
     *                 or errno on negative value.
     * @param id       The ID (48 bits) attempted to write
     */
    void (*current_group_id_write)(struct media_player *player, int err, uint64_t id);

    /**
     * @brief Playing Order receive callback
     *
     * Called when the Playing Order is read or changed
     * See also media_proxy_ctrl_get_playing_order() and
     * media_proxy_ctrl_set_playing_order()
     *
     * @param player   Media player instance pointer
     * @param err      Error value. 0 on success, GATT error on positive value
     *                 or errno on negative value.
     * @param order    The playing order
     */
    void (*playing_order_recv)(struct media_player *player, int err, uint8_t order);

    /**
     * @brief Playing Order write callback
     *
     * Called when the Playing Order is written
     * See also media_proxy_ctrl_set_playing_order()
     *
     * @param player   Media player instance pointer
     * @param err      Error value. 0 on success, GATT error on positive value
     *                 or errno on negative value.
     * @param order    The playing order attempted to write
     */
    void (*playing_order_write)(struct media_player *player, int err, uint8_t order);

    /**
     * @brief Playing Orders Supported receive callback
     *
     * Called when the Playing Orders Supported is read
     * See also media_proxy_ctrl_get_playing_orders_supported()
     *
     * @param player   Media player instance pointer
     * @param err      Error value. 0 on success, GATT error on positive value
     *                 or errno on negative value.
     * @param orders   The playing orders supported
     */
    void (*playing_orders_supported_recv)(struct media_player *player, int err,
                                          uint16_t orders);

    /**
     * @brief Media State receive callback
     *
     * Called when the Media State is read or changed
     * See also media_proxy_ctrl_get_media_state() and
     * media_proxy_ctrl_send_command()
     *
     * @param player   Media player instance pointer
     * @param err      Error value. 0 on success, GATT error on positive value
     *                 or errno on negative value.
     * @param state    The media player state
     */
    void (*media_state_recv)(struct media_player *player, int err, uint8_t state);

    /**
     * @brief Command send callback
     *
     * Called when a command has been sent
     * See also media_proxy_ctrl_send_command()
     *
     * @param player   Media player instance pointer
     * @param err      Error value. 0 on success, GATT error on positive value
     *                 or errno on negative value.
     * @param cmd      The command sent
     */
    void (*command_send)(struct media_player *player, int err, const struct mpl_cmd *cmd);

    /**
     * @brief Command result receive callback
     *
     * Called when a command result has been received
     * See also media_proxy_ctrl_send_command()
     *
     * @param player   Media player instance pointer
     * @param err      Error value. 0 on success, GATT error on positive value
     *                 or errno on negative value.
     * @param result   The result received
     */
    void (*command_recv)(struct media_player *player, int err,
                         const struct mpl_cmd_ntf *result);

    /**
     * @brief Commands supported receive callback
     *
     * Called when the Commands Supported is read or changed
     * See also media_proxy_ctrl_get_commands_supported()
     *
     * @param player       Media player instance pointer
     * @param err          Error value. 0 on success, GATT error on positive value
     *                     or errno on negative value.
     * @param opcodes      The supported command opcodes (bitmap)
     */
    void (*commands_supported_recv)(struct media_player *player, int err, uint32_t opcodes);

    /**
     * @brief Search send callback
     *
     * Called when a search has been sent
     * See also media_proxy_ctrl_send_search()
     *
     * @param player        Media player instance pointer
     * @param err           Error value. 0 on success, GATT error on positive value
     *                      or errno on negative value.
     * @param search        The search sent
     */
    void (*search_send)(struct media_player *player, int err, const struct mpl_search *search);

    /**
     * @brief Search result code receive callback
     *
     * Called when a search result code has been received
     * See also media_proxy_ctrl_send_search()
     *
     * The search result code tells whether the search was successful or not.
     * For a successful search, the actual results of the search (i.e. what was found
     * as a result of the search)can be accessed using the Search Results Object ID.
     * The Search Results Object ID has a separate callback - search_results_id_recv().
     *
     * @param player        Media player instance pointer
     * @param err           Error value. 0 on success, GATT error on positive value
     *                      or errno on negative value.
     * @param result_code   Search result code
     */
    void (*search_recv)(struct media_player *player, int err, uint8_t result_code);

    /**
     * @brief Search Results Object ID receive callback
     * See also media_proxy_ctrl_get_search_results_id()
     *
     * Called when the Search Results Object ID is read or changed
     *
     * @param player   Media player instance pointer
     * @param err      Error value. 0 on success, GATT error on positive value
     *                 or errno on negative value.
     * @param id       The ID of the search results object in Object Transfer Service (48 bits)
     */
    void (*search_results_id_recv)(struct media_player *player, int err, uint64_t id);

    /**
     * @brief Content Control ID receive callback
     *
     * Called when the Content Control ID is read
     * See also media_proxy_ctrl_get_content_ctrl_id()
     *
     * @param player   Media player instance pointer
     * @param err      Error value. 0 on success, GATT error on positive value
     *                 or errno on negative value.
     * @param ccid     The content control ID
     */
    void (*content_ctrl_id_recv)(struct media_player *player, int err, uint8_t ccid);
};

/**
 * @brief Register a controller with the media_proxy
 *
 * @param ctrl_cbs   Callbacks to the controller
 *
 * @return 0 if success, errno on failure
 */
int bt_media_proxy_ctrl_register_safe(struct media_proxy_ctrl_cbs *ctrl_cbs);

/**
 * @brief Discover a remote media player
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
 * @param conn   The connection to do discovery for
 *
 * @return 0 if success, errno on failure
 */
int bt_media_proxy_ctrl_discover_player(struct bt_conn *conn);
int bt_media_proxy_ctrl_discover_player_safe(struct bt_conn *conn);

/**
 * @brief Read Media Player Name
 *
 * @param player   Media player instance pointer
 *
 * @return 0 if success, errno on failure.
 */
int bt_media_proxy_ctrl_get_player_name_safe(struct media_player *player);

/**
 * @brief Read Icon Object ID
 *
 * Get an ID (48 bit) that can be used to retrieve the Icon
 * Object from an Object Transfer Service
 *
 * See the Media Control Service spec v1.0 sections 3.2 and
 * 4.1 for a description of the Icon Object.
 *
 * Requires Object Transfer Service
 *
 * @param player   Media player instance pointer
 *
 * @return 0 if success, errno on failure.
 */
int bt_media_proxy_ctrl_get_icon_id_safe(struct media_player *player);

/**
 * @brief Read Icon URL
 *
 * Get a URL to the media player's icon.
 *
 * @param player   Media player instance pointer
 */
int bt_media_proxy_ctrl_get_icon_url_safe(struct media_player *player);

/**
 * @brief Read Track Title
 *
 * @param player   Media player instance pointer
 *
 * @return 0 if success, errno on failure.
 */
int bt_media_proxy_ctrl_get_track_title_safe(struct media_player *player);

/**
 * @brief Read Track Duration
 *
 * The duration of a track is measured in hundredths of a
 * second.
 *
 * @param player   Media player instance pointer
 *
 * @return 0 if success, errno on failure.
 */
int bt_media_proxy_ctrl_get_track_duration_safe(struct media_player *player);

/**
 * @brief Read Track Position
 *
 * The position of the player (the playing position) is
 * measured in hundredths of a second from the beginning of
 * the track
 *
 * @param player   Media player instance pointer
 *
 * @return 0 if success, errno on failure.
 */
int bt_media_proxy_ctrl_get_track_position_safe(struct media_player *player);

/**
 * @brief Set Track Position
 *
 * Set the playing position of the media player in the current
 * track. The position is given in hundredths of a second,
 * from the beginning of the track of the track for positive
 * values, and (backwards) from the end of the track for
 * negative values.
 *
 * @param player     Media player instance pointer
 * @param position   The track position to set
 *
 * @return 0 if success, errno on failure.
 */
int bt_media_proxy_ctrl_set_track_position_safe(struct media_player *player, int32_t position);

/**
 * @brief Get Playback Speed
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
 * @param player   Media player instance pointer
 *
 * @return 0 if success, errno on failure.
 */
int bt_media_proxy_ctrl_get_playback_speed_safe(struct media_player *player);

/**
 * @brief Set Playback Speed
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
 * @param player   Media player instance pointer
 * @param speed    The playback speed parameter to set
 *
 * @return 0 if success, errno on failure.
 */
int bt_media_proxy_ctrl_set_playback_speed_safe(struct media_player *player, int8_t speed);

/**
 * @brief Get Seeking Speed
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
 * @param player   Media player instance pointer
 *
 * @return 0 if success, errno on failure.
 */
int bt_media_proxy_ctrl_get_seeking_speed_safe(struct media_player *player);

/**
 * @brief Read Current Track Segments Object ID
 *
 * Get an ID (48 bit) that can be used to retrieve the Current
 * Track Segments Object from an Object Transfer Service
 *
 * See the Media Control Service spec v1.0 sections 3.10 and
 * 4.2 for a description of the Track Segments Object.
 *
 * Requires Object Transfer Service
 *
 * @param player   Media player instance pointer
 *
 * @return 0 if success, errno on failure.
 */
int bt_media_proxy_ctrl_get_track_segments_id_safe(struct media_player *player);

/**
 * @brief Read Current Track Object ID
 *
 * Get an ID (48 bit) that can be used to retrieve the Current
 * Track Object from an Object Transfer Service
 *
 * See the Media Control Service spec v1.0 sections 3.11 and
 * 4.3 for a description of the Current Track Object.
 *
 * Requires Object Transfer Service
 *
 * @param player   Media player instance pointer
 *
 * @return 0 if success, errno on failure.
 */
int bt_media_proxy_ctrl_get_current_track_id_safe(struct media_player *player);

/**
 * @brief Set Current Track Object ID
 *
 * Change the player's current track to the track given by the ID.
 * (Behaves similarly to the goto track command.)
 *
 * Requires Object Transfer Service
 *
 * @param player   Media player instance pointer
 * @param id       The ID of a track object
 *
 * @return 0 if success, errno on failure.
 */
int bt_media_proxy_ctrl_set_current_track_id_safe(struct media_player *player, uint64_t id);

/**
 * @brief Read Next Track Object ID
 *
 * Get an ID (48 bit) that can be used to retrieve the Next
 * Track Object from an Object Transfer Service
 *
 * Requires Object Transfer Service
 *
 * @param player   Media player instance pointer
 *
 * @return 0 if success, errno on failure.
 */
int bt_media_proxy_ctrl_get_next_track_id_safe(struct media_player *player);

/**
 * @brief Set Next Track Object ID
 *
 * Change the player's next track to the track given by the ID.
 *
 * Requires Object Transfer Service
 *
 * @param player   Media player instance pointer
 * @param id       The ID of a track object
 *
 * @return 0 if success, errno on failure.
 */
int bt_media_proxy_ctrl_set_next_track_id_safe(struct media_player *player, uint64_t id);

/**
 * @brief Read Parent Group Object ID
 *
 * Get an ID (48 bit) that can be used to retrieve the Parent
 * Track Object from an Object Transfer Service
 *
 * The parent group is the parent of the current group.
 *
 * See the Media Control Service spec v1.0 sections 3.14 and
 * 4.4 for a description of the Current Track Object.
 *
 * Requires Object Transfer Service
 *
 * @param player   Media player instance pointer
 *
 * @return 0 if success, errno on failure.
 */
int bt_media_proxy_ctrl_get_parent_group_id_safe(struct media_player *player);

/**
 * @brief Read Current Group Object ID
 *
 * Get an ID (48 bit) that can be used to retrieve the Current
 * Track Object from an Object Transfer Service
 *
 * See the Media Control Service spec v1.0 sections 3.14 and
 * 4.4 for a description of the Current Group Object.
 *
 * Requires Object Transfer Service
 *
 * @param player   Media player instance pointer
 *
 * @return 0 if success, errno on failure.
 */
int bt_media_proxy_ctrl_get_current_group_id_safe(struct media_player *player);

/**
 * @brief Set Current Group Object ID
 *
 * Change the player's current group to the group given by the
 * ID, and the current track to the first track in that group.
 *
 * Requires Object Transfer Service
 *
 * @param player   Media player instance pointer
 * @param id       The ID of a group object
 *
 * @return 0 if success, errno on failure.
 */
int bt_media_proxy_ctrl_set_current_group_id_safe(struct media_player *player, uint64_t id);

/**
 * @brief Read Playing Order
 *
 * @param player   Media player instance pointer
 *
 * @return 0 if success, errno on failure.
 */
int bt_media_proxy_ctrl_get_playing_order_safe(struct media_player *player);

/**
 * @brief Set Playing Order
 *
 * Set the media player's playing order
 *
 * @param player   Media player instance pointer
 * @param order    The playing order to set
 *
 * @return 0 if success, errno on failure.
 */
int bt_media_proxy_ctrl_set_playing_order_safe(struct media_player *player, uint8_t order);

/**
 * @brief Read Playing Orders Supported
 *
 * Read a bitmap containing the media player's supported
 * playing orders.
 *
 * @param player   Media player instance pointer
 *
 * @return 0 if success, errno on failure.
 */
int bt_media_proxy_ctrl_get_playing_orders_supported_safe(struct media_player *player);

/**
 * @brief Read Media State
 *
 * Read the media player's state
 *
 * @param player   Media player instance pointer
 *
 * @return 0 if success, errno on failure.
 */
int bt_media_proxy_ctrl_get_media_state_safe(struct media_player *player);

/**
 * @brief Send Command
 *
 * Send a command to the media player.
 * Commands may cause the media player to change its state
 * May result in two callbacks - one for the actual sending of the command to the
 * player, one for the result of the command from the player.
 *
 * @param player      Media player instance pointer
 * @param command     The command to send
 *
 * @return 0 if success, errno on failure.
 */
int bt_media_proxy_ctrl_send_command_safe(struct media_player *player, const struct mpl_cmd *command);

/**
 * @brief Read Commands Supported
 *
 * Read a bitmap containing the media player's supported
 * command opcodes.
 *
 * @param player   Media player instance pointer
 *
 * @return 0 if success, errno on failure.
 */
int bt_media_proxy_ctrl_get_commands_supported_safe(struct media_player *player);

/**
 * @brief Set Search
 *
 * Write a search to the media player.
 * If the search is successful, the search results will be available as a group object
 * in the Object Transfer Service (OTS).
 *
 * May result in up to three callbacks
 * - one for the actual sending of the search to the player
 * - one for the result code for the search from the player
 * - if the search is successful, one for the search results object ID in the OTs
 *
 * Requires Object Transfer Service
 *
 * @param player   Media player instance pointer
 * @param search   The search to write
 *
 * @return 0 if success, errno on failure.
 */
int bt_media_proxy_ctrl_send_search_safe(struct media_player *player, const struct mpl_search *search);

/**
 * @brief Read Search Results Object ID
 *
 * Get an ID (48 bit) that can be used to retrieve the Search
 * Results Object from an Object Transfer Service
 *
 * The search results object is a group object.
 * The search results object only exists if a successful
 * search operation has been done.
 *
 * Requires Object Transfer Service
 *
 * @param player   Media player instance pointer
 *
 * @return 0 if success, errno on failure.
 */
int bt_media_proxy_ctrl_get_search_results_id_safe(struct media_player *player);

/**
 * @brief Read Content Control ID
 *
 * The content control ID identifies a content control service
 * on a device, and links it to the corresponding audio
 * stream.
 *
 * @param player   Media player instance pointer
 *
 * @return 0 if success, errno on failure.
 */
int bt_media_proxy_ctrl_get_content_ctrl_id_safe(struct media_player *player);

/**
 * @brief Available calls in a player, that the media proxy can call
 *
 * Given by a player when registering.
 */
struct media_proxy_pl_calls {

    /**
     * @brief Read Media Player Name
     *
     * @return The name of the media player
     */
    const char *(*get_player_name)(void);

    /**
     * @brief Read Icon Object ID
     *
     * Get an ID (48 bit) that can be used to retrieve the Icon
     * Object from an Object Transfer Service
     *
     * See the Media Control Service spec v1.0 sections 3.2 and
     * 4.1 for a description of the Icon Object.
     *
     * @return The Icon Object ID
     */
    uint64_t (*get_icon_id)(void);

    /**
     * @brief Read Icon URL
     *
     * Get a URL to the media player's icon.
     *
     * @return The URL of the Icon
     */
    const char *(*get_icon_url)(void);

    /**
     * @brief Read Track Title
     *
     * @return The title of the current track
     */
    const char *(*get_track_title)(void);

    /**
     * @brief Read Track Duration
     *
     * The duration of a track is measured in hundredths of a
     * second.
     *
     * @return The duration of the current track
     */
    int32_t (*get_track_duration)(void);

    /**
     * @brief Read Track Position
     *
     * The position of the player (the playing position) is
     * measured in hundredths of a second from the beginning of
     * the track
     *
     * @return The position of the player in the current track
     */
    int32_t (*get_track_position)(void);

    /**
     * @brief Set Track Position
     *
     * Set the playing position of the media player in the current
     * track. The position is given in hundredths of a second,
     * from the beginning of the track of the track for positive
     * values, and (backwards) from the end of the track for
     * negative values.
     *
     * @param position    The player position to set
     */
    void (*set_track_position)(int32_t position);

    /**
     * @brief Get Playback Speed
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
     * @return The playback speed parameter
     */
    int8_t (*get_playback_speed)(void);

    /**
     * @brief Set Playback Speed
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
     * @param speed The playback speed parameter to set
     */
    void (*set_playback_speed)(int8_t speed);

    /**
     * @brief Get Seeking Speed
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
     * @return The seeking speed factor
     */
    int8_t (*get_seeking_speed)(void);

    /**
     * @brief Read Current Track Segments Object ID
     *
     * Get an ID (48 bit) that can be used to retrieve the Current
     * Track Segments Object from an Object Transfer Service
     *
     * See the Media Control Service spec v1.0 sections 3.10 and
     * 4.2 for a description of the Track Segments Object.
     *
     * @return Current The Track Segments Object ID
     */
    uint64_t (*get_track_segments_id)(void);

    /**
     * @brief Read Current Track Object ID
     *
     * Get an ID (48 bit) that can be used to retrieve the Current
     * Track Object from an Object Transfer Service
     *
     * See the Media Control Service spec v1.0 sections 3.11 and
     * 4.3 for a description of the Current Track Object.
     *
     * @return The Current Track Object ID
     */
    uint64_t (*get_current_track_id)(void);

    /**
     * @brief Set Current Track Object ID
     *
     * Change the player's current track to the track given by the ID.
     * (Behaves similarly to the goto track command.)
     *
     * @param id    The ID of a track object
     */
    void (*set_current_track_id)(uint64_t id);

    /**
     * @brief Read Next Track Object ID
     *
     * Get an ID (48 bit) that can be used to retrieve the Next
     * Track Object from an Object Transfer Service
     *
     * @return The Next Track Object ID
     */
    uint64_t (*get_next_track_id)(void);

    /**
     * @brief Set Next Track Object ID
     *
     * Change the player's next track to the track given by the ID.
     *
     * @param id    The ID of a track object
     */
    void (*set_next_track_id)(uint64_t id);

    /**
     * @brief Read Parent Group Object ID
     *
     * Get an ID (48 bit) that can be used to retrieve the Parent
     * Track Object from an Object Transfer Service
     *
     * The parent group is the parent of the current group.
     *
     * See the Media Control Service spec v1.0 sections 3.14 and
     * 4.4 for a description of the Current Track Object.
     *
     * @return The Current Group Object ID
     */
    uint64_t (*get_parent_group_id)(void);

    /**
     * @brief Read Current Group Object ID
     *
     * Get an ID (48 bit) that can be used to retrieve the Current
     * Track Object from an Object Transfer Service
     *
     * See the Media Control Service spec v1.0 sections 3.14 and
     * 4.4 for a description of the Current Group Object.
     *
     * @return The Current Group Object ID
     */
    uint64_t (*get_current_group_id)(void);

    /**
     * @brief Set Current Group Object ID
     *
     * Change the player's current group to the group given by the
     * ID, and the current track to the first track in that group.
     *
     * @param id    The ID of a group object
     */
    void (*set_current_group_id)(uint64_t id);

    /**
     * @brief Read Playing Order
     *
     * return The media player's current playing order
     */
    uint8_t (*get_playing_order)(void);

    /**
     * @brief Set Playing Order
     *
     * Set the media player's playing order.
     * See the MEDIA_PROXY_PLAYING_ORDER_* defines.
     *
     * @param order The playing order to set
     */
    void (*set_playing_order)(uint8_t order);

    /**
     * @brief Read Playing Orders Supported
     *
     * Read a bitmap containing the media player's supported
     * playing orders.
     * See the MEDIA_PROXY_PLAYING_ORDERS_SUPPORTED_* defines.
     *
     * @return The media player's supported playing orders
     */
    uint16_t (*get_playing_orders_supported)(void);

    /**
     * @brief Read Media State
     *
     * Read the media player's state
     * See the MEDIA_PROXY_MEDIA_STATE_* defines.
     *
     * @return The media player's state
     */
    uint8_t (*get_media_state)(void);

    /**
     * @brief Send Command
     *
     * Send a command to the media player.
     * For command opcodes (play, pause, ...) - see the MEDIA_PROXY_OP_*
     * defines.
     *
     * @param command   The command to send
     */
    void (*send_command)(const struct mpl_cmd *command);

    /**
     * @brief Read Commands Supported
     *
     * Read a bitmap containing the media player's supported
     * command opcodes.
     * See the MEDIA_PROXY_OP_SUP_* defines.
     *
     * @return The media player's supported command opcodes
     */
    uint32_t (*get_commands_supported)(void);

    /**
     * @brief Set Search
     *
     * Write a search to the media player.
     * (For the formatting of a search, see the Media Control
     * Service spec and the mcs.h file.)
     *
     * @param search    The search to write
     */
    void (*send_search)(const struct mpl_search *search);

    /**
     * @brief Read Search Results Object ID
     *
     * Get an ID (48 bit) that can be used to retrieve the Search
     * Results Object from an Object Transfer Service
     *
     * The search results object is a group object.
     * The search results object only exists if a successful
     * search operation has been done.
     *
     * @return The Search Results Object ID
     */
    uint64_t (*get_search_results_id)(void);

    /**
     * @brief Read Content Control ID
     *
     * The content control ID identifies a content control service
     * on a device, and links it to the corresponding audio
     * stream.
     *
     * @return The content control ID for the media player
     */
    uint8_t (*get_content_ctrl_id)(void);
};

/**
 * @brief Register a player with the media proxy
 *
 * Register a player with the media proxy module, for use by media
 * controllers.
 *
 * The media proxy may call any non-NULL function pointers in the
 * supplied media_proxy_pl_calls structure.
 *
 * @param pl_calls  Function pointers to the media player's calls
 *
 * @return 0 if success, errno on failure
 */
int bt_media_proxy_pl_register_safe(struct media_proxy_pl_calls *pl_calls);

/**
 * @brief Initialize player
 *
 * TODO: Move to player header file
 */
int bt_media_proxy_pl_init_safe(void);

/**
 * @brief Get the pointer of the Object Transfer Service used by the Media Control Service
 *
 * TODO: Find best location for this call, and move this one also
 */
struct bt_ots *bt_mcs_get_ots_safe(void);

/**
 * @brief Player name changed callback
 *
 * To be called when the player's name is changed.
 *
 * @param name The name of the player
 */
void bt_media_proxy_pl_name_cb_safe(const char *name);

/**
 * @brief Player icon URL changed callback
 *
 * To be called when the player's icon URL is changed.
 *
 * @param url The URL of the player's icon
 */
void bt_media_proxy_pl_icon_url_cb_safe(const char *url);

/**
 * @brief Track changed callback
 *
 * To be called when the player's current track is changed
 */
void bt_media_proxy_pl_track_changed_cb_safe(void);

/**
 * @brief Track title callback
 *
 * To be called when the player's current track is changed
 *
 * @param title The title of the track
 */
void bt_media_proxy_pl_track_title_cb_safe(char *title);

/**
 * @brief Track duration callback
 *
 * To be called when the current track's duration is changed (e.g. due
 * to a track change)
 *
 * The track duration is given in hundredths of a second.
 *
 * @param duration  The track duration
 */
void bt_media_proxy_pl_track_duration_cb_safe(int32_t duration);

/**
 * @brief Track position callback
 *
 * To be called when the media player's position in the track is
 * changed, or when the player is paused or similar.
 *
 * Exception: This callback should not be called when the position
 * changes during regular playback, i.e. while the player is playing
 * and playback happens at a constant speed.
 *
 * The track position is given in hundredths of a second from the
 * start of the track.
 *
 *  @param position The media player's position in the track
 */
void bt_media_proxy_pl_track_position_cb_safe(int32_t position);

/**
 * @brief Playback speed callback
 *
 * To be called when the playback speed is changed.
 *
 * @param speed The playback speed parameter
 */
void bt_media_proxy_pl_playback_speed_cb_safe(int8_t speed);

/**
 * @brief Seeking speed callback
 *
 * To be called when the seeking speed is changed.
 *
 * @param speed The seeking speed factor
 */
void bt_media_proxy_pl_seeking_speed_cb_safe(int8_t speed);

/**
 * @brief Current track object ID callback
 *
 * To be called when the ID of the current track is changed (e.g. due
 * to a track change).
 *
 * @param id The ID of the current track object in the OTS
 */
void bt_media_proxy_pl_current_track_id_cb_safe(uint64_t id);

/**
 * @brief Next track object ID callback
 *
 * To be called when the ID of the current track is changes
 *
 * @param id The ID of the next track object in the OTS
 */
void bt_media_proxy_pl_next_track_id_cb_safe(uint64_t id);

/**
 * @brief Parent group object ID callback
 *
 * To be called when the ID of the parent group is changed
 *
 * @param id The ID of the parent group object in the OTS
 */
void bt_media_proxy_pl_parent_group_id_cb_safe(uint64_t id);

/**
 * @brief Current group object ID callback
 *
 * To be called when the ID of the current group is changed
 *
 * @param id The ID of the current group object in the OTS
 */
void bt_media_proxy_pl_current_group_id_cb_safe(uint64_t id);

/**
 * @brief Playing order callback
 *
 * To be called when the playing order is changed
 *
 * @param order The playing order
 */
void bt_media_proxy_pl_playing_order_cb_safe(uint8_t order);

/**
 * @brief Media state callback
 *
 * To be called when the media state is changed
 *
 * @param state The media player's state
 */
void bt_media_proxy_pl_media_state_cb_safe(uint8_t state);

/**
 * @brief Command callback
 *
 * To be called when a command has been sent, to notify whether the
 * command was successfully performed or not.
 * See the MEDIA_PROXY_CMD_* result code defines.
 *
 * @param cmd_ntf   The result of the command
 */
void bt_media_proxy_pl_command_cb_safe(const struct mpl_cmd_ntf *cmd_ntf);

/**
 * @brief Commands supported callback
 *
 * To be called when the set of commands supported is changed
 *
 * @param opcodes   The supported commands opcodes
 */
void bt_media_proxy_pl_commands_supported_cb_safe(uint32_t opcodes);

/**
 * @brief Search callback
 *
 * To be called when a search has been set to notify whether the
 * search was successfully performed or not.
 * See the MEDIA_PROXY_SEARCH_* result code defines.
 *
 * The actual results of the search, if successful, can be found in
 * the search results object.
 *
 * @param result_code   The result (success or failure) of the search
 */
void bt_media_proxy_pl_search_cb_safe(uint8_t result_code);

/**
 * @brief Search Results object ID callback
 *
 * To be called when the ID of the search results is changed
 * (typically as the result of a new successful search).
 *
 * @param id    The ID of the search results object in the OTS
 */
void bt_media_proxy_pl_search_results_id_cb_safe(uint64_t id);

#ifdef __cplusplus
}
#endif

/** @} */ /* End of group bt_media_proxy */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_MEDIA_PROXY_H_ */
