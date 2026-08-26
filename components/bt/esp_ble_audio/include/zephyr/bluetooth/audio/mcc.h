/**
 * @file
 * @brief Bluetooth Media Control Client (MCC) APIs.
 */

/*
 * SPDX-FileCopyrightText: 2019-2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_MCC_
#define ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_MCC_

/**
 * @brief Bluetooth Media Control Client (MCC) interface
 *
 * Updated to the Media Control Profile specification revision 1.0
 *
 * @defgroup bt_mcc Media Control Client (MCC)
 *
 * @since 3.0
 * @version 0.8.0
 *
 * @ingroup bluetooth
 * @{
 */

#include <stdint.h>
#include <stdbool.h>

#include <zephyr/bluetooth/conn.h>
#include <zephyr/net_buf.h>
#include <zephyr/bluetooth/audio/media_proxy.h>

#ifdef __cplusplus
extern "C" {
#endif

/**** Callback functions ******************************************************/

/**
 * @brief Callback function for bt_mcc_discover_mcs()
 *
 * Called when a media control server is discovered
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 */
typedef void (*bt_mcc_discover_mcs_cb)(struct bt_conn *conn, int err);

/**
 * @brief Callback function for bt_mcc_read_player_name()
 *
 * Called when the player name is read or notified
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param name          Player name
 */
typedef void (*bt_mcc_read_player_name_cb)(struct bt_conn *conn, int err, const char *name);

/**
 * @brief Callback function for bt_mcc_read_icon_obj_id()
 *
 * Called when the icon object ID is read
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param icon_id       The ID of the Icon Object. This is a UINT48 in a uint64_t
 */
typedef void (*bt_mcc_read_icon_obj_id_cb)(struct bt_conn *conn, int err, uint64_t icon_id);

/**
 * @brief Callback function for bt_mcc_read_icon_url()
 *
 * Called when the icon URL is read
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param icon_url      The URL of the Icon
 */
typedef void (*bt_mcc_read_icon_url_cb)(struct bt_conn *conn, int err, const char *icon_url);

/**
 * @brief Callback function for track changed notifications
 *
 * Called when a track change is notified.
 *
 * The track changed characteristic is a special case.  It can not be read or
 * set, it can only be notified.
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 */
typedef void (*bt_mcc_track_changed_ntf_cb)(struct bt_conn *conn, int err);

/**
 * @brief Callback function for bt_mcc_read_track_title()
 *
 * Called when the track title is read or notified
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param title         The title of the track
 */
typedef void (*bt_mcc_read_track_title_cb)(struct bt_conn *conn, int err, const char *title);

/**
 * @brief Callback function for bt_mcc_read_track_duration()
 *
 * Called when the track duration is read or notified
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param dur           The duration of the track
 */
typedef void (*bt_mcc_read_track_duration_cb)(struct bt_conn *conn, int err, int32_t dur);

/**
 * @brief Callback function for bt_mcc_read_track_position()
 *
 * Called when the track position is read or notified
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param pos           The Track Position
 */
typedef void (*bt_mcc_read_track_position_cb)(struct bt_conn *conn, int err, int32_t pos);

/**
 * @brief Callback function for bt_mcc_set_track_position()
 *
 * Called when the track position is set
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param pos           The Track Position set (or attempted to set)
 */
typedef void (*bt_mcc_set_track_position_cb)(struct bt_conn *conn, int err, int32_t pos);

/**
 * @brief Callback function for bt_mcc_read_playback_speed()
 *
 * Called when the playback speed is read or notified
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param speed         The Playback Speed
 */
typedef void (*bt_mcc_read_playback_speed_cb)(struct bt_conn *conn, int err, int8_t speed);

/**
 * @brief Callback function for bt_mcc_set_playback_speed()
 *
 * Called when the playback speed is set
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param speed         The Playback Speed set (or attempted to set)
 */
typedef void (*bt_mcc_set_playback_speed_cb)(struct bt_conn *conn, int err, int8_t speed);

/**
 * @brief Callback function for bt_mcc_read_seeking_speed()
 *
 * Called when the seeking speed is read or notified
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param speed         The Seeking Speed
 */
typedef void (*bt_mcc_read_seeking_speed_cb)(struct bt_conn *conn, int err, int8_t speed);

/**
 * @brief Callback function for bt_mcc_read_segments_obj_id()
 *
 * Called when the track segments object ID is read
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param id            The Track Segments Object ID (UINT48)
 */
typedef void (*bt_mcc_read_segments_obj_id_cb)(struct bt_conn *conn, int err, uint64_t id);

/**
 * @brief Callback function for bt_mcc_read_current_track_obj_id()
 *
 * Called when the current track object ID is read or notified
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param id            The Current Track Object ID (UINT48)
 */
typedef void (*bt_mcc_read_current_track_obj_id_cb)(struct bt_conn *conn, int err, uint64_t id);

/**
 * @brief Callback function for bt_mcc_set_current_track_obj_id()
 *
 * Called when the current track object ID is set
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param id            The Object ID (UINT48) set (or attempted to set)
 */
typedef void (*bt_mcc_set_current_track_obj_id_cb)(struct bt_conn *conn, int err, uint64_t id);

/**
 * @brief Callback function for bt_mcc_read_next_track_obj_id_obj()
 *
 * Called when the next track object ID is read or notified
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param id            The Next Track Object ID (UINT48)
 */
typedef void (*bt_mcc_read_next_track_obj_id_cb)(struct bt_conn *conn, int err, uint64_t id);

/**
 * @brief Callback function for bt_mcc_set_next_track_obj_id()
 *
 * Called when the next track object ID is set
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param id            The Object ID (UINT48) set (or attempted to set)
 */
typedef void (*bt_mcc_set_next_track_obj_id_cb)(struct bt_conn *conn, int err, uint64_t id);

/**
 * @brief Callback function for bt_mcc_read_parent_group_obj_id()
 *
 * Called when the parent group object ID is read or notified
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param id            The Parent Group Object ID (UINT48)
 */
typedef void (*bt_mcc_read_parent_group_obj_id_cb)(struct bt_conn *conn, int err, uint64_t id);

/**
 * @brief Callback function for bt_mcc_read_current_group_obj_id()
 *
 * Called when the current group object ID is read or notified
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param id            The Current Group Object ID (UINT48)
 */
typedef void (*bt_mcc_read_current_group_obj_id_cb)(struct bt_conn *conn, int err, uint64_t id);

/**
 * @brief Callback function for bt_mcc_set_current_group_obj_id()
 *
 * Called when the current group object ID is set
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param obj_id        The Object ID (UINT48) set (or attempted to set)
 */
typedef void (*bt_mcc_set_current_group_obj_id_cb)(struct bt_conn *conn, int err, uint64_t obj_id);

/**
 * @brief Callback function for bt_mcc_read_playing_order()
 *
 * Called when the playing order is read or notified
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param order         The playback order
 */
typedef void (*bt_mcc_read_playing_order_cb)(struct bt_conn *conn, int err, uint8_t order);

/**
 * @brief Callback function for bt_mcc_set_playing_order()
 *
 * Called when the playing order is set
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param order         The Playing Order set (or attempted to set)
 */
typedef void (*bt_mcc_set_playing_order_cb)(struct bt_conn *conn, int err, uint8_t order);

/**
 * @brief Callback function for bt_mcc_read_playing_orders_supported()
 *
 * Called when the supported playing orders are read or notified
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param orders        The playing orders supported (bitmap)
 */
typedef void (*bt_mcc_read_playing_orders_supported_cb)(struct bt_conn *conn, int err,
                                                        uint16_t orders);

/**
 * @brief Callback function for bt_mcc_read_media_state()
 *
 * Called when the media state is read or notified
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param state         The Media State
 */
typedef void (*bt_mcc_read_media_state_cb)(struct bt_conn *conn, int err, uint8_t state);

/**
 * @brief Callback function for bt_mcc_send_cmd()
 *
 * Called when a command is sent, i.e. when the media control point is set
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param cmd           The command sent
 */
typedef void (*bt_mcc_send_cmd_cb)(struct bt_conn *conn, int err, const struct mpl_cmd *cmd);

/**
 * @brief Callback function for command notifications
 *
 * Called when the media control point is notified
 *
 * Notifications for commands (i.e. for writes to the media control point) use a
 * different parameter structure than what is used for sending commands (writing
 * to the media control point)
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param ntf           The command notification
 */
typedef void (*bt_mcc_cmd_ntf_cb)(struct bt_conn *conn, int err, const struct mpl_cmd_ntf *ntf);

/**
 * @brief Callback function for bt_mcc_read_opcodes_supported()
 *
 * Called when the supported opcodes (commands) are read or notified
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param opcodes       The supported opcodes
 */
typedef void (*bt_mcc_read_opcodes_supported_cb)(struct bt_conn *conn, int err,
                                                 uint32_t opcodes);

/**
 * @brief Callback function for bt_mcc_send_search()
 *
 * Called when a search is sent, i.e. when the search control point is set
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param search        The search set (or attempted to set)
 */
typedef void (*bt_mcc_send_search_cb)(struct bt_conn *conn, int err,
                                      const struct mpl_search *search);

/**
 * @brief Callback function for search notifications
 *
 * Called when the search control point is notified
 *
 * Notifications for searches (i.e. for writes to the search control point) use a
 * different parameter structure than what is used for sending searches (writing
 * to the search control point)
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param result_code   The search notification
 */
typedef void (*bt_mcc_search_ntf_cb)(struct bt_conn *conn, int err,
                                     uint8_t result_code);

/**
 * @brief Callback function for bt_mcc_read_search_results_obj_id()
 *
 * Called when the search results object ID is read or notified
 *
 * Note that the Search Results Object ID value may be zero, in case the
 * characteristic does not exist on the server.  (This will be the case if
 * there has not been a successful search.)
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param id            The Search Results Object ID (UINT48)
 */
typedef void (*bt_mcc_read_search_results_obj_id_cb)(struct bt_conn *conn,
                                                     int err, uint64_t id);

/**
 * @brief Callback function for bt_mcc_read_content_control_id()
 *
 * Called when the content control ID is read
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param ccid          The Content Control ID
 */
typedef void (*bt_mcc_read_content_control_id_cb)(struct bt_conn *conn,
                                                  int err, uint8_t ccid);

/**** Callback functions for the included Object Transfer service *************/

/**
 * @brief Callback function for object selected
 *
 * Called when an object is selected
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 */
typedef void (*bt_mcc_otc_obj_selected_cb)(struct bt_conn *conn, int err);

/**
 * @brief Callback function for bt_mcc_otc_read_object_metadata()
 *
 * Called when object metadata is read
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 */
typedef void (*bt_mcc_otc_obj_metadata_cb)(struct bt_conn *conn, int err);

/**
 * @brief Callback function for bt_mcc_otc_read_icon_object()
 *
 * Called when the icon object is read
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param buf           Buffer containing the object contents
 *
 * If err is EMSGSIZE, the object contents have been truncated.
 */
typedef void (*bt_mcc_otc_read_icon_object_cb)(struct bt_conn *conn, int err,
                                               struct net_buf_simple *buf);

/**
 * @brief Callback function for bt_mcc_otc_read_track_segments_object()
 *
 * Called when the track segments object is read
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param buf           Buffer containing the object contents
 *
 * If err is EMSGSIZE, the object contents have been truncated.
 */
typedef void (*bt_mcc_otc_read_track_segments_object_cb)(struct bt_conn *conn, int err,
                                                         struct net_buf_simple *buf);

/**
 * @brief Callback function for bt_mcc_otc_read_current_track_object()
 *
 * Called when the current track object is read
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param buf           Buffer containing the object contents
 *
 * If err is EMSGSIZE, the object contents have been truncated.
 */
typedef void (*bt_mcc_otc_read_current_track_object_cb)(struct bt_conn *conn, int err,
                                                        struct net_buf_simple *buf);

/**
 * @brief Callback function for bt_mcc_otc_read_next_track_object()
 *
 * Called when the next track object is read
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param buf           Buffer containing the object contents
 *
 * If err is EMSGSIZE, the object contents have been truncated.
 */
typedef void (*bt_mcc_otc_read_next_track_object_cb)(struct bt_conn *conn, int err,
                                                     struct net_buf_simple *buf);

/**
 * @brief Callback function for bt_mcc_otc_read_parent_group_object()
 *
 * Called when the parent group object is read
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param buf           Buffer containing the object contents
 *
 * If err is EMSGSIZE, the object contents have been truncated.
 */
typedef void (*bt_mcc_otc_read_parent_group_object_cb)(struct bt_conn *conn, int err,
                                                       struct net_buf_simple *buf);

/**
 * @brief Callback function for bt_mcc_otc_read_current_group_object()
 *
 * Called when the current group object is read
 *
 * @param conn          The connection that was used to initialise the media control client
 * @param err           Error value. 0 on success, GATT error or errno on fail
 * @param buf           Buffer containing the object contents
 *
 * If err is EMSGSIZE, the object contents have been truncated.
 */
typedef void (*bt_mcc_otc_read_current_group_object_cb)(struct bt_conn *conn, int err,
                                                        struct net_buf_simple *buf);

/**
 * @brief Media control client callbacks
 */
struct bt_mcc_cb {
    /** Callback when discovery has finished */
    bt_mcc_discover_mcs_cb                   discover_mcs;
    /** Callback when reading the player name */
    bt_mcc_read_player_name_cb               read_player_name;
    /** Callback when reading the icon object ID */
    bt_mcc_read_icon_obj_id_cb               read_icon_obj_id;
    /** Callback when reading the icon URL */
    bt_mcc_read_icon_url_cb                  read_icon_url;
    /** Callback when getting a track changed notification */
    bt_mcc_track_changed_ntf_cb              track_changed_ntf;
    /** Callback when reading the track title */
    bt_mcc_read_track_title_cb               read_track_title;
    /** Callback when reading the track duration */
    bt_mcc_read_track_duration_cb            read_track_duration;
    /** Callback when reading the track position */
    bt_mcc_read_track_position_cb            read_track_position;
    /** Callback when setting the track position */
    bt_mcc_set_track_position_cb             set_track_position;
    /** Callback when reading the playback speed */
    bt_mcc_read_playback_speed_cb            read_playback_speed;
    /** Callback when setting the playback speed */
    bt_mcc_set_playback_speed_cb             set_playback_speed;
    /** Callback when reading the seeking speed */
    bt_mcc_read_seeking_speed_cb             read_seeking_speed;
    /** Callback when reading the segments object ID */
    bt_mcc_read_segments_obj_id_cb           read_segments_obj_id;
    /** Callback when reading the current track object ID */
    bt_mcc_read_current_track_obj_id_cb      read_current_track_obj_id;
    /** Callback when setting the current track object ID */
    bt_mcc_set_current_track_obj_id_cb       set_current_track_obj_id;
    /** Callback when reading the next track object ID */
    bt_mcc_read_next_track_obj_id_cb         read_next_track_obj_id;
    /** Callback when setting the next track object ID */
    bt_mcc_set_next_track_obj_id_cb          set_next_track_obj_id;
    /** Callback when reading the current group object ID */
    bt_mcc_read_current_group_obj_id_cb      read_current_group_obj_id;
    /** Callback when setting the current group object ID */
    bt_mcc_set_current_group_obj_id_cb       set_current_group_obj_id;
    /** Callback when reading the parent group object ID */
    bt_mcc_read_parent_group_obj_id_cb       read_parent_group_obj_id;
    /** Callback when reading the playing order */
    bt_mcc_read_playing_order_cb             read_playing_order;
    /** Callback when setting the playing order */
    bt_mcc_set_playing_order_cb              set_playing_order;
    /** Callback when reading the supported playing orders */
    bt_mcc_read_playing_orders_supported_cb  read_playing_orders_supported;
    /** Callback when reading the media state */
    bt_mcc_read_media_state_cb               read_media_state;
    /** Callback when sending a command */
    bt_mcc_send_cmd_cb                       send_cmd;
    /** Callback command notifications */
    bt_mcc_cmd_ntf_cb                        cmd_ntf;
    /** Callback when reading the supported opcodes */
    bt_mcc_read_opcodes_supported_cb         read_opcodes_supported;
    /** Callback when sending the a search query */
    bt_mcc_send_search_cb                    send_search;
    /** Callback when receiving a search notification */
    bt_mcc_search_ntf_cb                     search_ntf;
    /** Callback when reading the search results object ID */
    bt_mcc_read_search_results_obj_id_cb     read_search_results_obj_id;
    /** Callback when reading the content control ID */
    bt_mcc_read_content_control_id_cb        read_content_control_id;
    /** Callback when selecting an object */
    bt_mcc_otc_obj_selected_cb               otc_obj_selected;
    /** Callback when receiving the current object metadata */
    bt_mcc_otc_obj_metadata_cb               otc_obj_metadata;
    /** Callback when reading the current icon object */
    bt_mcc_otc_read_icon_object_cb           otc_icon_object;
    /** Callback when reading the track segments object */
    bt_mcc_otc_read_track_segments_object_cb otc_track_segments_object;
    /** Callback when reading the current track object */
    bt_mcc_otc_read_current_track_object_cb  otc_current_track_object;
    /** Callback when reading the next track object */
    bt_mcc_otc_read_next_track_object_cb     otc_next_track_object;
    /** Callback when reading the current group object */
    bt_mcc_otc_read_current_group_object_cb  otc_current_group_object;
    /** Callback when reading the parent group object */
    bt_mcc_otc_read_parent_group_object_cb   otc_parent_group_object;
};

/**** Functions ***************************************************************/

/**
 * @brief Initialize Media Control Client
 *
 * @param cb    Callbacks to be used
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_init_safe(struct bt_mcc_cb *cb);

/**
 * @brief Discover Media Control Service
 *
 * Discover Media Control Service (MCS) on the server given by the connection
 * Optionally subscribe to notifications.
 *
 * Shall be called once, after media control client initialization and before
 * using other media control client functionality.
 *
 * @param conn  Connection to the peer device
 * @param subscribe   Whether to subscribe to notifications
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_discover_mcs(struct bt_conn *conn, bool subscribe);
int bt_mcc_discover_mcs_safe(struct bt_conn *conn, bool subscribe);

/**
 * @brief Read Media Player Name
 *
 * @param conn  Connection to the peer device
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_read_player_name(struct bt_conn *conn);
int bt_mcc_read_player_name_safe(struct bt_conn *conn);

/**
 * @brief Read Icon Object ID
 *
 * @param conn  Connection to the peer device
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_read_icon_obj_id(struct bt_conn *conn);
int bt_mcc_read_icon_obj_id_safe(struct bt_conn *conn);

/**
 * @brief Read Icon Object URL
 *
 * @param conn  Connection to the peer device
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_read_icon_url(struct bt_conn *conn);
int bt_mcc_read_icon_url_safe(struct bt_conn *conn);

/**
 * @brief Read Track Title
 *
 * @param conn  Connection to the peer device
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_read_track_title(struct bt_conn *conn);
int bt_mcc_read_track_title_safe(struct bt_conn *conn);

/**
 * @brief Read Track Duration
 *
 * @param conn  Connection to the peer device
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_read_track_duration(struct bt_conn *conn);
int bt_mcc_read_track_duration_safe(struct bt_conn *conn);

/**
 * @brief Read Track Position
 *
 * @param conn  Connection to the peer device
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_read_track_position(struct bt_conn *conn);
int bt_mcc_read_track_position_safe(struct bt_conn *conn);

/**
 * @brief Set Track position
 *
 * @param conn  Connection to the peer device
 * @param pos   Track position
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_set_track_position(struct bt_conn *conn, int32_t pos);
int bt_mcc_set_track_position_safe(struct bt_conn *conn, int32_t pos);

/**
 * @brief Read Playback speed
 *
 * @param conn  Connection to the peer device
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_read_playback_speed(struct bt_conn *conn);
int bt_mcc_read_playback_speed_safe(struct bt_conn *conn);

/**
 * @brief Set Playback Speed
 *
 * @param conn  Connection to the peer device
 * @param speed Playback speed
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_set_playback_speed(struct bt_conn *conn, int8_t speed);
int bt_mcc_set_playback_speed_safe(struct bt_conn *conn, int8_t speed);

/**
 * @brief Read Seeking speed
 *
 * @param conn  Connection to the peer device
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_read_seeking_speed(struct bt_conn *conn);
int bt_mcc_read_seeking_speed_safe(struct bt_conn *conn);

/**
 * @brief Read Track Segments Object ID
 *
 * @param conn  Connection to the peer device
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_read_segments_obj_id(struct bt_conn *conn);
int bt_mcc_read_segments_obj_id_safe(struct bt_conn *conn);

/**
 * @brief Read Current Track Object ID
 *
 * @param conn  Connection to the peer device
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_read_current_track_obj_id(struct bt_conn *conn);
int bt_mcc_read_current_track_obj_id_safe(struct bt_conn *conn);

/**
 * @brief Set Current Track Object ID
 *
 * Set the Current Track to the track given by the @p id parameter
 *
 * @param conn  Connection to the peer device
 * @param id    Object Transfer Service ID (UINT48) of the track to set as the current track
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_set_current_track_obj_id(struct bt_conn *conn, uint64_t id);
int bt_mcc_set_current_track_obj_id_safe(struct bt_conn *conn, uint64_t id);

/**
 * @brief Read Next Track Object ID
 *
 * @param conn  Connection to the peer device
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_read_next_track_obj_id(struct bt_conn *conn);
int bt_mcc_read_next_track_obj_id_safe(struct bt_conn *conn);

/**
 * @brief Set Next Track Object ID
 *
 * Set the Next Track to the track given by the @p id parameter
 *
 * @param conn  Connection to the peer device
 * @param id   Object Transfer Service ID (UINT48) of the track to set as the next track
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_set_next_track_obj_id(struct bt_conn *conn, uint64_t id);
int bt_mcc_set_next_track_obj_id_safe(struct bt_conn *conn, uint64_t id);

/**
 * @brief Read Current Group Object ID
 *
 * @param conn  Connection to the peer device
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_read_current_group_obj_id(struct bt_conn *conn);
int bt_mcc_read_current_group_obj_id_safe(struct bt_conn *conn);

/**
 * @brief Set Current Group Object ID
 *
 * Set the Current Group to the group given by the @p id parameter
 *
 * @param conn  Connection to the peer device
 * @param id   Object Transfer Service ID (UINT48) of the group to set as the current group
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_set_current_group_obj_id(struct bt_conn *conn, uint64_t id);
int bt_mcc_set_current_group_obj_id_safe(struct bt_conn *conn, uint64_t id);

/**
 * @brief Read Parent Group Object ID
 *
 * @param conn  Connection to the peer device
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_read_parent_group_obj_id(struct bt_conn *conn);
int bt_mcc_read_parent_group_obj_id_safe(struct bt_conn *conn);

/**
 * @brief Read Playing Order
 *
 * @param conn  Connection to the peer device
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_read_playing_order(struct bt_conn *conn);
int bt_mcc_read_playing_order_safe(struct bt_conn *conn);

/**
 * @brief Set Playing Order
 *
 * @param conn  Connection to the peer device
 * @param order Playing order
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_set_playing_order(struct bt_conn *conn, uint8_t order);
int bt_mcc_set_playing_order_safe(struct bt_conn *conn, uint8_t order);

/**
 * @brief Read Playing Orders Supported
 *
 * @param conn  Connection to the peer device
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_read_playing_orders_supported(struct bt_conn *conn);
int bt_mcc_read_playing_orders_supported_safe(struct bt_conn *conn);

/**
 * @brief Read Media State
 *
 * @param conn  Connection to the peer device
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_read_media_state(struct bt_conn *conn);
int bt_mcc_read_media_state_safe(struct bt_conn *conn);

/**
 * @brief Send a command
 *
 * Write a command (e.g. "play", "pause") to the server's media control point.
 *
 * @param conn  Connection to the peer device
 * @param cmd   The command to send
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_send_cmd(struct bt_conn *conn, const struct mpl_cmd *cmd);
int bt_mcc_send_cmd_safe(struct bt_conn *conn, const struct mpl_cmd *cmd);

/**
 * @brief Read Opcodes Supported
 *
 * @param conn  Connection to the peer device
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_read_opcodes_supported(struct bt_conn *conn);
int bt_mcc_read_opcodes_supported_safe(struct bt_conn *conn);

/**
 * @brief Send a Search command
 *
 * Write a search to the server's search control point.
 *
 * @param conn   Connection to the peer device
 * @param search The search
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_send_search(struct bt_conn *conn, const struct mpl_search *search);
int bt_mcc_send_search_safe(struct bt_conn *conn, const struct mpl_search *search);

/**
 * @brief Search Results Group Object ID
 *
 * @param conn  Connection to the peer device
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_read_search_results_obj_id(struct bt_conn *conn);
int bt_mcc_read_search_results_obj_id_safe(struct bt_conn *conn);

/**
 * @brief Read Content Control ID
 *
 * @param conn  Connection to the peer device
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_read_content_control_id(struct bt_conn *conn);
int bt_mcc_read_content_control_id_safe(struct bt_conn *conn);

/**
 * @brief Read the current object metadata
 *
 * @param conn  Connection to the peer device
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_otc_read_object_metadata(struct bt_conn *conn);
int bt_mcc_otc_read_object_metadata_safe(struct bt_conn *conn);

/**
 * @brief Read the Icon Object
 *
 * @param conn  Connection to the peer device
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_otc_read_icon_object(struct bt_conn *conn);
int bt_mcc_otc_read_icon_object_safe(struct bt_conn *conn);

/**
 * @brief Read the Track Segments Object
 *
 * @param conn  Connection to the peer device
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_otc_read_track_segments_object(struct bt_conn *conn);
int bt_mcc_otc_read_track_segments_object_safe(struct bt_conn *conn);

/**
 * @brief Read the Current Track Object
 *
 * @param conn  Connection to the peer device
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_otc_read_current_track_object(struct bt_conn *conn);
int bt_mcc_otc_read_current_track_object_safe(struct bt_conn *conn);

/**
 * @brief Read the Next Track Object
 *
 * @param conn  Connection to the peer device
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_otc_read_next_track_object(struct bt_conn *conn);
int bt_mcc_otc_read_next_track_object_safe(struct bt_conn *conn);

/**
 * @brief Read the Current Group Object
 *
 * @param conn  Connection to the peer device
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_otc_read_current_group_object(struct bt_conn *conn);
int bt_mcc_otc_read_current_group_object_safe(struct bt_conn *conn);

/**
 * @brief Read the Parent Group Object
 *
 * @param conn  Connection to the peer device
 *
 * @return 0 if success, errno on failure.
 */
int bt_mcc_otc_read_parent_group_object(struct bt_conn *conn);
int bt_mcc_otc_read_parent_group_object_safe(struct bt_conn *conn);

/**
 * @brief Look up MCC OTC instance
 *
 * @param conn  The connection to the MCC server.
 *
 * @return Pointer to a MCC OTC instance if found else NULL.
 *
 */
struct bt_ots_client *bt_mcc_otc_inst(struct bt_conn *conn);
struct bt_ots_client *bt_mcc_otc_inst_safe(struct bt_conn *conn);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_MCC__ */
