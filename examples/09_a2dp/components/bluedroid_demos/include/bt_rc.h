/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_INCLUDE_BT_RC_H
#define ANDROID_INCLUDE_BT_RC_H

__BEGIN_DECLS

/* Macros */
#define BTRC_MAX_ATTR_STR_LEN       255
#define BTRC_UID_SIZE               8
#define BTRC_MAX_APP_SETTINGS       8
#define BTRC_MAX_FOLDER_DEPTH       4
#define BTRC_MAX_APP_ATTR_SIZE      16
#define BTRC_MAX_ELEM_ATTR_SIZE     7

typedef uint8_t btrc_uid_t[BTRC_UID_SIZE];

typedef enum {
    BTRC_FEAT_NONE = 0x00,    /* AVRCP 1.0 */
    BTRC_FEAT_METADATA = 0x01,    /* AVRCP 1.3 */
    BTRC_FEAT_ABSOLUTE_VOLUME = 0x02,    /* Supports TG role and volume sync */
    BTRC_FEAT_BROWSE = 0x04,    /* AVRCP 1.4 and up, with Browsing support */
} btrc_remote_features_t;

typedef enum {
    BTRC_PLAYSTATE_STOPPED = 0x00,    /* Stopped */
    BTRC_PLAYSTATE_PLAYING = 0x01,    /* Playing */
    BTRC_PLAYSTATE_PAUSED = 0x02,    /* Paused  */
    BTRC_PLAYSTATE_FWD_SEEK = 0x03,    /* Fwd Seek*/
    BTRC_PLAYSTATE_REV_SEEK = 0x04,    /* Rev Seek*/
    BTRC_PLAYSTATE_ERROR = 0xFF,    /* Error   */
} btrc_play_status_t;

typedef enum {
    BTRC_EVT_PLAY_STATUS_CHANGED = 0x01,
    BTRC_EVT_TRACK_CHANGE = 0x02,
    BTRC_EVT_TRACK_REACHED_END = 0x03,
    BTRC_EVT_TRACK_REACHED_START = 0x04,
    BTRC_EVT_PLAY_POS_CHANGED = 0x05,
    BTRC_EVT_APP_SETTINGS_CHANGED = 0x08,
} btrc_event_id_t;

typedef enum {
    BTRC_NOTIFICATION_TYPE_INTERIM = 0,
    BTRC_NOTIFICATION_TYPE_CHANGED = 1,
} btrc_notification_type_t;

typedef enum {
    BTRC_PLAYER_ATTR_EQUALIZER = 0x01,
    BTRC_PLAYER_ATTR_REPEAT = 0x02,
    BTRC_PLAYER_ATTR_SHUFFLE = 0x03,
    BTRC_PLAYER_ATTR_SCAN = 0x04,
} btrc_player_attr_t;

typedef enum {
    BTRC_MEDIA_ATTR_TITLE = 0x01,
    BTRC_MEDIA_ATTR_ARTIST = 0x02,
    BTRC_MEDIA_ATTR_ALBUM = 0x03,
    BTRC_MEDIA_ATTR_TRACK_NUM = 0x04,
    BTRC_MEDIA_ATTR_NUM_TRACKS = 0x05,
    BTRC_MEDIA_ATTR_GENRE = 0x06,
    BTRC_MEDIA_ATTR_PLAYING_TIME = 0x07,
} btrc_media_attr_t;

typedef enum {
    BTRC_PLAYER_VAL_OFF_REPEAT = 0x01,
    BTRC_PLAYER_VAL_SINGLE_REPEAT = 0x02,
    BTRC_PLAYER_VAL_ALL_REPEAT = 0x03,
    BTRC_PLAYER_VAL_GROUP_REPEAT = 0x04
} btrc_player_repeat_val_t;

typedef enum {
    BTRC_PLAYER_VAL_OFF_SHUFFLE = 0x01,
    BTRC_PLAYER_VAL_ALL_SHUFFLE = 0x02,
    BTRC_PLAYER_VAL_GROUP_SHUFFLE = 0x03
} btrc_player_shuffle_val_t;

typedef enum {
    BTRC_STS_BAD_CMD        = 0x00, /* Invalid command */
    BTRC_STS_BAD_PARAM      = 0x01, /* Invalid parameter */
    BTRC_STS_NOT_FOUND      = 0x02, /* Specified parameter is wrong or not found */
    BTRC_STS_INTERNAL_ERR   = 0x03, /* Internal Error */
    BTRC_STS_NO_ERROR       = 0x04  /* Operation Success */
} btrc_status_t;

typedef struct {
    uint8_t num_attr;
    uint8_t attr_ids[BTRC_MAX_APP_SETTINGS];
    uint8_t attr_values[BTRC_MAX_APP_SETTINGS];
} btrc_player_settings_t;

typedef union
{
    btrc_play_status_t play_status;
    btrc_uid_t track; /* queue position in NowPlaying */
    uint32_t song_pos;
    btrc_player_settings_t player_setting;
} btrc_register_notification_t;

typedef struct {
    uint8_t id; /* can be attr_id or value_id */
    uint8_t text[BTRC_MAX_ATTR_STR_LEN];
} btrc_player_setting_text_t;

typedef struct {
    uint32_t attr_id;
    uint8_t text[BTRC_MAX_ATTR_STR_LEN];
} btrc_element_attr_val_t;

/** Callback for the controller's supported feautres */
typedef void (* btrc_remote_features_callback)(bt_bdaddr_t *bd_addr,
                                                      btrc_remote_features_t features);

/** Callback for play status request */
typedef void (* btrc_get_play_status_callback)();

/** Callback for list player application attributes (Shuffle, Repeat,...) */
typedef void (* btrc_list_player_app_attr_callback)();

/** Callback for list player application attributes (Shuffle, Repeat,...) */
typedef void (* btrc_list_player_app_values_callback)(btrc_player_attr_t attr_id);

/** Callback for getting the current player application settings value
**  num_attr: specifies the number of attribute ids contained in p_attrs
*/
typedef void (* btrc_get_player_app_value_callback) (uint8_t num_attr, btrc_player_attr_t *p_attrs);

/** Callback for getting the player application settings attributes' text
**  num_attr: specifies the number of attribute ids contained in p_attrs
*/
typedef void (* btrc_get_player_app_attrs_text_callback) (uint8_t num_attr, btrc_player_attr_t *p_attrs);

/** Callback for getting the player application settings values' text
**  num_attr: specifies the number of value ids contained in p_vals
*/
typedef void (* btrc_get_player_app_values_text_callback) (uint8_t attr_id, uint8_t num_val, uint8_t *p_vals);

/** Callback for setting the player application settings values */
typedef void (* btrc_set_player_app_value_callback) (btrc_player_settings_t *p_vals);

/** Callback to fetch the get element attributes of the current song
**  num_attr: specifies the number of attributes requested in p_attrs
*/
typedef void (* btrc_get_element_attr_callback) (uint8_t num_attr, btrc_media_attr_t *p_attrs);

/** Callback for register notification (Play state change/track change/...)
**  param: Is only valid if event_id is BTRC_EVT_PLAY_POS_CHANGED
*/
typedef void (* btrc_register_notification_callback) (btrc_event_id_t event_id, uint32_t param);

/* AVRCP 1.4 Enhancements */
/** Callback for volume change on CT
**  volume: Current volume setting on the CT (0-127)
*/
typedef void (* btrc_volume_change_callback) (uint8_t volume, uint8_t ctype);

/** Callback for passthrough commands */
typedef void (* btrc_passthrough_cmd_callback) (int id, int key_state);

/** BT-RC Target callback structure. */
typedef struct {
    /** set to sizeof(BtRcCallbacks) */
    size_t      size;
    btrc_remote_features_callback               remote_features_cb;
    btrc_get_play_status_callback               get_play_status_cb;
    btrc_list_player_app_attr_callback          list_player_app_attr_cb;
    btrc_list_player_app_values_callback        list_player_app_values_cb;
    btrc_get_player_app_value_callback          get_player_app_value_cb;
    btrc_get_player_app_attrs_text_callback     get_player_app_attrs_text_cb;
    btrc_get_player_app_values_text_callback    get_player_app_values_text_cb;
    btrc_set_player_app_value_callback          set_player_app_value_cb;
    btrc_get_element_attr_callback              get_element_attr_cb;
    btrc_register_notification_callback         register_notification_cb;
    btrc_volume_change_callback                 volume_change_cb;
    btrc_passthrough_cmd_callback               passthrough_cmd_cb;
} btrc_callbacks_t;

/** Represents the standard BT-RC AVRCP Target interface. */
typedef struct {

    /** set to sizeof(BtRcInterface) */
    size_t          size;
    /**
     * Register the BtRc callbacks
     */
    bt_status_t (*init)( btrc_callbacks_t* callbacks );

    /** Respose to GetPlayStatus request. Contains the current
    **  1. Play status
    **  2. Song duration/length
    **  3. Song position
    */
    bt_status_t (*get_play_status_rsp)( btrc_play_status_t play_status, uint32_t song_len, uint32_t song_pos);

    /** Lists the support player application attributes (Shuffle/Repeat/...)
    **  num_attr: Specifies the number of attributes contained in the pointer p_attrs
    */
    bt_status_t (*list_player_app_attr_rsp)( int num_attr, btrc_player_attr_t *p_attrs);

    /** Lists the support player application attributes (Shuffle Off/On/Group)
    **  num_val: Specifies the number of values contained in the pointer p_vals
    */
    bt_status_t (*list_player_app_value_rsp)( int num_val, uint8_t *p_vals);

    /** Returns the current application attribute values for each of the specified attr_id */
    bt_status_t (*get_player_app_value_rsp)( btrc_player_settings_t *p_vals);

    /** Returns the application attributes text ("Shuffle"/"Repeat"/...)
    **  num_attr: Specifies the number of attributes' text contained in the pointer p_attrs
    */
    bt_status_t (*get_player_app_attr_text_rsp)( int num_attr, btrc_player_setting_text_t *p_attrs);

    /** Returns the application attributes text ("Shuffle"/"Repeat"/...)
    **  num_attr: Specifies the number of attribute values' text contained in the pointer p_vals
    */
    bt_status_t (*get_player_app_value_text_rsp)( int num_val, btrc_player_setting_text_t *p_vals);

    /** Returns the current songs' element attributes text ("Title"/"Album"/"Artist")
    **  num_attr: Specifies the number of attributes' text contained in the pointer p_attrs
    */
    bt_status_t (*get_element_attr_rsp)( uint8_t num_attr, btrc_element_attr_val_t *p_attrs);

    /** Response to set player attribute request ("Shuffle"/"Repeat")
    **  rsp_status: Status of setting the player attributes for the current media player
    */
    bt_status_t (*set_player_app_value_rsp)(btrc_status_t rsp_status);

    /* Response to the register notification request (Play state change/track change/...).
    ** event_id: Refers to the event_id this notification change corresponds too
    ** type: Response type - interim/changed
    ** p_params: Based on the event_id, this parameter should be populated
    */
    bt_status_t (*register_notification_rsp)(btrc_event_id_t event_id,
                                             btrc_notification_type_t type,
                                             btrc_register_notification_t *p_param);

    /* AVRCP 1.4 enhancements */

    /**Send current volume setting to remote side. Support limited to SetAbsoluteVolume
    ** This can be enhanced to support Relative Volume (AVRCP 1.0).
    ** With RelateVolume, we will send VOLUME_UP/VOLUME_DOWN opposed to absolute volume level
    ** volume: Should be in the range 0-127. bit7 is reseved and cannot be set
    */
    bt_status_t (*set_volume)(uint8_t volume);

    /** Closes the interface. */
    void  (*cleanup)( void );
} btrc_interface_t;


typedef void (* btrc_passthrough_rsp_callback) (int id, int key_state);

typedef void (* btrc_connection_state_callback) (bool state, bt_bdaddr_t *bd_addr);

/** BT-RC Controller callback structure. */
typedef struct {
    /** set to sizeof(BtRcCallbacks) */
    size_t      size;
    btrc_passthrough_rsp_callback               passthrough_rsp_cb;
    btrc_connection_state_callback              connection_state_cb;
} btrc_ctrl_callbacks_t;

/** Represents the standard BT-RC AVRCP Controller interface. */
typedef struct {

    /** set to sizeof(BtRcInterface) */
    size_t          size;
    /**
     * Register the BtRc callbacks
     */
    bt_status_t (*init)( btrc_ctrl_callbacks_t* callbacks );

    /** send pass through command to target */
    bt_status_t (*send_pass_through_cmd) ( bt_bdaddr_t *bd_addr, uint8_t key_code, uint8_t key_state );

    /** Closes the interface. */
    void  (*cleanup)( void );
} btrc_ctrl_interface_t;

__END_DECLS

#endif /* ANDROID_INCLUDE_BT_RC_H */
