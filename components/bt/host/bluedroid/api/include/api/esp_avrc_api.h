/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_AVRC_API_H__
#define __ESP_AVRC_API_H__

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_bt_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_AVRC_TRANS_LABEL_MAX        15       /*!< max transaction label */

/// AVRC feature bit mask
typedef enum {
    ESP_AVRC_FEAT_RCTG = 0x0001,                 /*!< remote control target */
    ESP_AVRC_FEAT_RCCT = 0x0002,                 /*!< remote control controller */
    ESP_AVRC_FEAT_VENDOR = 0x0008,               /*!< remote control vendor dependent commands */
    ESP_AVRC_FEAT_BROWSE = 0x0010,               /*!< use browsing channel */
    ESP_AVRC_FEAT_META_DATA = 0x0040,            /*!< remote control metadata transfer command/response */
    ESP_AVRC_FEAT_ADV_CTRL = 0x0200,             /*!< remote control advanced control command/response */
} esp_avrc_features_t;

/// AVRC supported features flag retrieved in SDP record
typedef enum {
    ESP_AVRC_FEAT_FLAG_CAT1 = 0x0001,                             /*!< category 1 */
    ESP_AVRC_FEAT_FLAG_CAT2 = 0x0002,                             /*!< category 2 */
    ESP_AVRC_FEAT_FLAG_CAT3 = 0x0004,                             /*!< category 3 */
    ESP_AVRC_FEAT_FLAG_CAT4 = 0x0008,                             /*!< category 4 */
    ESP_AVRC_FEAT_FLAG_BROWSING = 0x0040,                         /*!< browsing */
    ESP_AVRC_FEAT_FLAG_COVER_ART_GET_IMAGE_PROP = 0x0080,         /*!< Cover Art GetImageProperties */
    ESP_AVRC_FEAT_FLAG_COVER_ART_GET_IMAGE = 0x0100,              /*!< Cover Art GetImage */
    ESP_AVRC_FEAT_FLAG_COVER_ART_GET_LINKED_THUMBNAIL = 0x0200,   /*!< Cover Art GetLinkedThumbnail */
} esp_avrc_feature_flag_t;

/// AVRC passthrough command code
typedef enum {
    ESP_AVRC_PT_CMD_SELECT         =   0x00,    /*!< select */
    ESP_AVRC_PT_CMD_UP             =   0x01,    /*!< up */
    ESP_AVRC_PT_CMD_DOWN           =   0x02,    /*!< down */
    ESP_AVRC_PT_CMD_LEFT           =   0x03,    /*!< left */
    ESP_AVRC_PT_CMD_RIGHT          =   0x04,    /*!< right */
    ESP_AVRC_PT_CMD_RIGHT_UP       =   0x05,    /*!< right-up */
    ESP_AVRC_PT_CMD_RIGHT_DOWN     =   0x06,    /*!< right-down */
    ESP_AVRC_PT_CMD_LEFT_UP        =   0x07,    /*!< left-up */
    ESP_AVRC_PT_CMD_LEFT_DOWN      =   0x08,    /*!< left-down */
    ESP_AVRC_PT_CMD_ROOT_MENU      =   0x09,    /*!< root menu */
    ESP_AVRC_PT_CMD_SETUP_MENU     =   0x0A,    /*!< setup menu */
    ESP_AVRC_PT_CMD_CONT_MENU      =   0x0B,    /*!< contents menu */
    ESP_AVRC_PT_CMD_FAV_MENU       =   0x0C,    /*!< favorite menu */
    ESP_AVRC_PT_CMD_EXIT           =   0x0D,    /*!< exit */
    ESP_AVRC_PT_CMD_0              =   0x20,    /*!< 0 */
    ESP_AVRC_PT_CMD_1              =   0x21,    /*!< 1 */
    ESP_AVRC_PT_CMD_2              =   0x22,    /*!< 2 */
    ESP_AVRC_PT_CMD_3              =   0x23,    /*!< 3 */
    ESP_AVRC_PT_CMD_4              =   0x24,    /*!< 4 */
    ESP_AVRC_PT_CMD_5              =   0x25,    /*!< 5 */
    ESP_AVRC_PT_CMD_6              =   0x26,    /*!< 6 */
    ESP_AVRC_PT_CMD_7              =   0x27,    /*!< 7 */
    ESP_AVRC_PT_CMD_8              =   0x28,    /*!< 8 */
    ESP_AVRC_PT_CMD_9              =   0x29,    /*!< 9 */
    ESP_AVRC_PT_CMD_DOT            =   0x2A,    /*!< dot */
    ESP_AVRC_PT_CMD_ENTER          =   0x2B,    /*!< enter */
    ESP_AVRC_PT_CMD_CLEAR          =   0x2C,    /*!< clear */
    ESP_AVRC_PT_CMD_CHAN_UP        =   0x30,    /*!< channel up */
    ESP_AVRC_PT_CMD_CHAN_DOWN      =   0x31,    /*!< channel down */
    ESP_AVRC_PT_CMD_PREV_CHAN      =   0x32,    /*!< previous channel */
    ESP_AVRC_PT_CMD_SOUND_SEL      =   0x33,    /*!< sound select */
    ESP_AVRC_PT_CMD_INPUT_SEL      =   0x34,    /*!< input select */
    ESP_AVRC_PT_CMD_DISP_INFO      =   0x35,    /*!< display information */
    ESP_AVRC_PT_CMD_HELP           =   0x36,    /*!< help */
    ESP_AVRC_PT_CMD_PAGE_UP        =   0x37,    /*!< page up */
    ESP_AVRC_PT_CMD_PAGE_DOWN      =   0x38,    /*!< page down */
    ESP_AVRC_PT_CMD_POWER          =   0x40,    /*!< power */
    ESP_AVRC_PT_CMD_VOL_UP         =   0x41,    /*!< volume up */
    ESP_AVRC_PT_CMD_VOL_DOWN       =   0x42,    /*!< volume down */
    ESP_AVRC_PT_CMD_MUTE           =   0x43,    /*!< mute */
    ESP_AVRC_PT_CMD_PLAY           =   0x44,    /*!< play */
    ESP_AVRC_PT_CMD_STOP           =   0x45,    /*!< stop */
    ESP_AVRC_PT_CMD_PAUSE          =   0x46,    /*!< pause */
    ESP_AVRC_PT_CMD_RECORD         =   0x47,    /*!< record */
    ESP_AVRC_PT_CMD_REWIND         =   0x48,    /*!< rewind */
    ESP_AVRC_PT_CMD_FAST_FORWARD   =   0x49,    /*!< fast forward */
    ESP_AVRC_PT_CMD_EJECT          =   0x4A,    /*!< eject */
    ESP_AVRC_PT_CMD_FORWARD        =   0x4B,    /*!< forward */
    ESP_AVRC_PT_CMD_BACKWARD       =   0x4C,    /*!< backward */
    ESP_AVRC_PT_CMD_ANGLE          =   0x50,    /*!< angle */
    ESP_AVRC_PT_CMD_SUBPICT        =   0x51,    /*!< subpicture */
    ESP_AVRC_PT_CMD_F1             =   0x71,    /*!< F1 */
    ESP_AVRC_PT_CMD_F2             =   0x72,    /*!< F2 */
    ESP_AVRC_PT_CMD_F3             =   0x73,    /*!< F3 */
    ESP_AVRC_PT_CMD_F4             =   0x74,    /*!< F4 */
    ESP_AVRC_PT_CMD_F5             =   0x75,    /*!< F5 */
    ESP_AVRC_PT_CMD_VENDOR         =   0x7E,    /*!< vendor unique */
} esp_avrc_pt_cmd_t;

/// AVRC passthrough command filter
typedef enum {
    ESP_AVRC_PSTH_FILTER_ALLOWED_CMD = 0,       /*!< all of the PASSTHROUGH commands that can possibly be used, immutable */
    ESP_AVRC_PSTH_FILTER_SUPPORTED_CMD = 1,     /*!< PASSTHROUGH commands selectively supported according to the current configuration */
    ESP_AVRC_PSTH_FILTER_SUPPORT_MAX,
} esp_avrc_psth_filter_t;

/// AVRC passthrough command bit mask
typedef struct {
    uint16_t bits[8];                           /*!< bit mask representation of PASSTHROUGH commands */
} esp_avrc_psth_bit_mask_t;

typedef enum {
    ESP_AVRC_BIT_MASK_OP_TEST = 0,      /*!< operation code to test a specific bit */
    ESP_AVRC_BIT_MASK_OP_SET = 1,       /*!< operation code to set a specific bit  */
    ESP_AVRC_BIT_MASK_OP_CLEAR = 2,     /*!< operation code to clear a specific bit */
} esp_avrc_bit_mask_op_t;

/// AVRC passthrough command state
typedef enum {
    ESP_AVRC_PT_CMD_STATE_PRESSED = 0,           /*!< key pressed */
    ESP_AVRC_PT_CMD_STATE_RELEASED = 1           /*!< key released */
} esp_avrc_pt_cmd_state_t;

/// AVRC Controller callback events
typedef enum {
    ESP_AVRC_CT_CONNECTION_STATE_EVT = 0,        /*!< connection state changed event */
    ESP_AVRC_CT_PASSTHROUGH_RSP_EVT = 1,         /*!< passthrough response event */
    ESP_AVRC_CT_METADATA_RSP_EVT = 2,            /*!< metadata response event */
    ESP_AVRC_CT_PLAY_STATUS_RSP_EVT = 3,         /*!< play status response event */
    ESP_AVRC_CT_CHANGE_NOTIFY_EVT = 4,           /*!< notification event */
    ESP_AVRC_CT_REMOTE_FEATURES_EVT = 5,         /*!< feature of remote device indication event */
    ESP_AVRC_CT_GET_RN_CAPABILITIES_RSP_EVT = 6,     /*!< supported notification events capability of peer device */
    ESP_AVRC_CT_SET_ABSOLUTE_VOLUME_RSP_EVT = 7, /*!< set absolute volume response event */
} esp_avrc_ct_cb_event_t;

/// AVRC Target callback events
typedef enum {
    ESP_AVRC_TG_CONNECTION_STATE_EVT = 0,          /*!< connection state changed event */
    ESP_AVRC_TG_REMOTE_FEATURES_EVT = 1,           /*!< feature of remote device indication event */
    ESP_AVRC_TG_PASSTHROUGH_CMD_EVT = 2,           /*!< passthrough command event */
    ESP_AVRC_TG_SET_ABSOLUTE_VOLUME_CMD_EVT = 3,   /*!< set absolute volume command from remote device */
    ESP_AVRC_TG_REGISTER_NOTIFICATION_EVT = 4,     /*!< register notification event */
    ESP_AVRC_TG_SET_PLAYER_APP_VALUE_EVT = 5,      /*!< set application attribute value, attribute refer to esp_avrc_ps_attr_ids_t */
} esp_avrc_tg_cb_event_t;

/// AVRC metadata attribute mask
typedef enum {
    ESP_AVRC_MD_ATTR_TITLE = 0x1,                 /*!< title of the playing track */
    ESP_AVRC_MD_ATTR_ARTIST = 0x2,                /*!< track artist */
    ESP_AVRC_MD_ATTR_ALBUM = 0x4,                 /*!< album name */
    ESP_AVRC_MD_ATTR_TRACK_NUM = 0x8,             /*!< track position on the album */
    ESP_AVRC_MD_ATTR_NUM_TRACKS = 0x10,           /*!< number of tracks on the album */
    ESP_AVRC_MD_ATTR_GENRE = 0x20,                /*!< track genre */
    ESP_AVRC_MD_ATTR_PLAYING_TIME = 0x40          /*!< total album playing time in miliseconds */
} esp_avrc_md_attr_mask_t;

/// AVRC event notification ids
typedef enum {
    ESP_AVRC_RN_PLAY_STATUS_CHANGE = 0x01,        /*!< track status change, eg. from playing to paused */
    ESP_AVRC_RN_TRACK_CHANGE = 0x02,              /*!< new track is loaded */
    ESP_AVRC_RN_TRACK_REACHED_END = 0x03,         /*!< current track reached end */
    ESP_AVRC_RN_TRACK_REACHED_START = 0x04,       /*!< current track reached start position */
    ESP_AVRC_RN_PLAY_POS_CHANGED = 0x05,          /*!< track playing position changed */
    ESP_AVRC_RN_BATTERY_STATUS_CHANGE = 0x06,     /*!< battery status changed */
    ESP_AVRC_RN_SYSTEM_STATUS_CHANGE = 0x07,      /*!< system status changed */
    ESP_AVRC_RN_APP_SETTING_CHANGE = 0x08,        /*!< application settings changed */
    ESP_AVRC_RN_NOW_PLAYING_CHANGE = 0x09,        /*!< now playing content changed */
    ESP_AVRC_RN_AVAILABLE_PLAYERS_CHANGE = 0x0a,  /*!< available players changed */
    ESP_AVRC_RN_ADDRESSED_PLAYER_CHANGE = 0x0b,   /*!< the addressed player changed */
    ESP_AVRC_RN_UIDS_CHANGE = 0x0c,               /*!< UIDs changed */
    ESP_AVRC_RN_VOLUME_CHANGE = 0x0d,             /*!< volume changed locally on TG */
    ESP_AVRC_RN_MAX_EVT
} esp_avrc_rn_event_ids_t;

/// AVRC target notification event notification capability
typedef enum {
    ESP_AVRC_RN_CAP_ALLOWED_EVT = 0,              /*!< all of the notification events that can possibly be supported, immutable */
    ESP_AVRC_RN_CAP_SUPPORTED_EVT = 1,            /*!< notification events selectively supported according to the current configuration */
    ESP_AVRC_RN_CAP_MAX,
} esp_avrc_rn_evt_cap_t;

/// AVRC target notification event capability bit mask
typedef struct {
    uint16_t bits;                                /*!< bit mask representation of PASSTHROUGH commands */
} esp_avrc_rn_evt_cap_mask_t;

/// AVRC notification response type
typedef enum {
    ESP_AVRC_RN_RSP_INTERIM = 13,     /*!< initial response to RegisterNotification, should be sent T_mtp(1000ms) from receiving the command */
    ESP_AVRC_RN_RSP_CHANGED = 15,     /*!< final response to RegisterNotification command */
} esp_avrc_rn_rsp_t;

/// AVRC player setting ids
typedef enum {
    ESP_AVRC_PS_EQUALIZER = 0x01,                 /*!< equalizer, on or off */
    ESP_AVRC_PS_REPEAT_MODE = 0x02,               /*!< repeat mode */
    ESP_AVRC_PS_SHUFFLE_MODE = 0x03,              /*!< shuffle mode */
    ESP_AVRC_PS_SCAN_MODE = 0x04,                 /*!< scan mode on or off */
    ESP_AVRC_PS_MAX_ATTR
} esp_avrc_ps_attr_ids_t;

/// AVRC equalizer modes
typedef enum {
    ESP_AVRC_PS_EQUALIZER_OFF = 0x1,              /*!< equalizer OFF */
    ESP_AVRC_PS_EQUALIZER_ON = 0x2                /*!< equalizer ON */
} esp_avrc_ps_eq_value_ids_t;

/// AVRC repeat modes
typedef enum {
    ESP_AVRC_PS_REPEAT_OFF = 0x1,                 /*!< repeat mode off */
    ESP_AVRC_PS_REPEAT_SINGLE = 0x2,              /*!< single track repeat */
    ESP_AVRC_PS_REPEAT_GROUP = 0x3                /*!< group repeat */
} esp_avrc_ps_rpt_value_ids_t;


/// AVRC shuffle modes
typedef enum {
    ESP_AVRC_PS_SHUFFLE_OFF = 0x1,                /*<! shuffle off */
    ESP_AVRC_PS_SHUFFLE_ALL = 0x2,                /*<! shuffle all tracks */
    ESP_AVRC_PS_SHUFFLE_GROUP = 0x3               /*<! group shuffle */
} esp_avrc_ps_shf_value_ids_t;

/// AVRC scan modes
typedef enum {
    ESP_AVRC_PS_SCAN_OFF = 0x1,                   /*!< scan off */
    ESP_AVRC_PS_SCAN_ALL = 0x2,                   /*!< all tracks scan */
    ESP_AVRC_PS_SCAN_GROUP = 0x3                  /*!< group scan */
} esp_avrc_ps_scn_value_ids_t;

/// AVCTP response codes
typedef enum {
    ESP_AVRC_RSP_NOT_IMPL   = 8,                  /*!< not implemented */
    ESP_AVRC_RSP_ACCEPT     = 9,                  /*!< accept */
    ESP_AVRC_RSP_REJECT     = 10,                 /*!< reject */
    ESP_AVRC_RSP_IN_TRANS   = 11,                 /*!< in transition */
    ESP_AVRC_RSP_IMPL_STBL  = 12,                 /*!< implemented/stable */
    ESP_AVRC_RSP_CHANGED    = 13,                 /*!< changed */
    ESP_AVRC_RSP_INTERIM    = 15,                 /*!< interim */
} esp_avrc_rsp_t;

/// AVRCP battery status
typedef enum {
    ESP_AVRC_BATT_NORMAL       = 0,               /*!< normal state */
    ESP_AVRC_BATT_WARNING      = 1,               /*!< unable to operate soon */
    ESP_AVRC_BATT_CRITICAL     = 2,               /*!< cannot operate any more */
    ESP_AVRC_BATT_EXTERNAL     = 3,               /*!< plugged to external power supply */
    ESP_AVRC_BATT_FULL_CHARGE  = 4,               /*!< when completely charged from external power supply */
} esp_avrc_batt_stat_t;

/// AVRCP current status of playback
typedef enum {
    ESP_AVRC_PLAYBACK_STOPPED = 0,                /*!< stopped */
    ESP_AVRC_PLAYBACK_PLAYING = 1,                /*!< playing */
    ESP_AVRC_PLAYBACK_PAUSED = 2,                 /*!< paused */
    ESP_AVRC_PLAYBACK_FWD_SEEK = 3,               /*!< forward seek */
    ESP_AVRC_PLAYBACK_REV_SEEK = 4,               /*!< reverse seek */
    ESP_AVRC_PLAYBACK_ERROR = 0xFF,               /*!< error */
} esp_avrc_playback_stat_t;

/// AVRCP notification parameters
typedef union
{
    uint8_t volume;                          /*!< response data for ESP_AVRC_RN_VOLUME_CHANGE, ranges 0..127 */
    esp_avrc_playback_stat_t playback;       /*!< response data for ESP_AVRC_RN_PLAY_STATUS_CHANGE */
    uint8_t elm_id[8];                       /*!< response data for ESP_AVRC_RN_TRACK_CHANGE */
    uint32_t play_pos;                       /*!< response data for ESP_AVRC_RN_PLAY_POS_CHANGED, in millisecond */
    esp_avrc_batt_stat_t batt;               /*!< response data for ESP_AVRC_RN_BATTERY_STATUS_CHANGE */
} esp_avrc_rn_param_t;

/// AVRCP set app value parameters
typedef struct {
    uint8_t   attr_id;                       /*!< player application attribute id */
    uint8_t   attr_val;                      /*!< player application attribute value */
} esp_avrc_set_app_value_param_t;

/// AVRC controller callback parameters
typedef union {
    /**
     * @brief ESP_AVRC_CT_CONNECTION_STATE_EVT
     */
    struct avrc_ct_conn_stat_param {
        bool connected;                          /*!< whether AVRC connection is set up */
        esp_bd_addr_t remote_bda;                /*!< remote bluetooth device address */
    } conn_stat;                                 /*!< AVRC connection status */

    /**
     * @brief ESP_AVRC_CT_PASSTHROUGH_RSP_EVT
     */
    struct avrc_ct_psth_rsp_param {
        uint8_t tl;                              /*!< transaction label, 0 to 15 */
        uint8_t key_code;                        /*!< passthrough command code */
        uint8_t key_state;                       /*!< 0 for PRESSED, 1 for RELEASED */
        esp_avrc_rsp_t rsp_code;                 /*!< response code */
    } psth_rsp;                                  /*!< passthrough command response */

    /**
     * @brief ESP_AVRC_CT_METADATA_RSP_EVT
     */
    struct avrc_ct_meta_rsp_param {
        uint8_t attr_id;                         /*!< id of metadata attribute */
        uint8_t *attr_text;                      /*!< attribute itself */
        int attr_length;                         /*!< attribute character length */
    } meta_rsp;                                  /*!< metadata attributes response */

    /**
     * @brief ESP_AVRC_CT_CHANGE_NOTIFY_EVT
     */
    struct avrc_ct_change_notify_param {
        uint8_t event_id;                        /*!< id of AVRC event notification */
        esp_avrc_rn_param_t event_parameter;     /*!< event notification parameter */
    } change_ntf;                                /*!< notifications */

    /**
     * @brief ESP_AVRC_CT_REMOTE_FEATURES_EVT
     */
    struct avrc_ct_rmt_feats_param {
        uint32_t feat_mask;                      /*!< AVRC feature mask of remote device */
        uint16_t tg_feat_flag;                   /*!< feature flag of remote device as TG */
        esp_bd_addr_t remote_bda;                /*!< remote bluetooth device address */
    } rmt_feats;                                 /*!< AVRC features discovered from remote SDP server */

    /**
     * @brief ESP_AVRC_CT_GET_RN_CAPABILITIES_RSP_EVT
     */
    struct avrc_ct_get_rn_caps_rsp_param {
        uint8_t cap_count;                       /*!< number of items provided in event or company_id according to cap_id used */
        esp_avrc_rn_evt_cap_mask_t evt_set;      /*!< supported event_ids represented in bit-mask */
    } get_rn_caps_rsp;                           /*!< get supported event capabilities response from AVRCP target */

    /**
     * @brief ESP_AVRC_CT_SET_ABSOLUTE_VOLUME_RSP_EVT
     */
    struct avrc_ct_set_volume_rsp_param {
        uint8_t volume;                          /*!< the volume which has actually been set, range is 0 to 0x7f, means 0% to 100% */
    } set_volume_rsp;                            /*!< set absolute volume response event */
} esp_avrc_ct_cb_param_t;

/// AVRC target callback parameters
typedef union {
    /**
     * @brief ESP_AVRC_TG_CONNECTION_STATE_EVT
     */
    struct avrc_tg_conn_stat_param {
        bool connected;                          /*!< whether AVRC connection is set up */
        esp_bd_addr_t remote_bda;                /*!< remote bluetooth device address */
    } conn_stat;                                 /*!< AVRC connection status */

    /**
     * @brief ESP_AVRC_TG_REMOTE_FEATURES_EVT
     */
    struct avrc_tg_rmt_feats_param {
        uint32_t feat_mask;                      /*!< AVRC feature mask of remote device */
        uint16_t ct_feat_flag;                   /*!< feature flag of remote device as CT */
        esp_bd_addr_t remote_bda;                /*!< remote bluetooth device address */
    } rmt_feats;                                 /*!< AVRC features discovered through SDP */

    /**
     * @brief ESP_AVRC_TG_PASSTHROUGH_CMD_EVT
     */
    struct avrc_tg_psth_cmd_param {
        uint8_t key_code;                        /*!< passthrough command code */
        uint8_t key_state;                       /*!< 0 for PRESSED, 1 for RELEASED */
    } psth_cmd;                                  /*!< passthrough command */

    /**
     * @brief ESP_AVRC_TG_SET_ABSOLUTE_VOLUME_CMD_EVT
     */
    struct avrc_tg_set_abs_vol_param {
        uint8_t volume;                          /*!< volume ranges from 0 to 127 */
    } set_abs_vol;                               /*!< set absolute volume command targeted on audio sink */

    /**
     * @brief ESP_AVRC_TG_REGISTER_NOTIFICATION_EVT
     */
    struct avrc_tg_reg_ntf_param {
        uint8_t event_id;                        /*!< event id of AVRC RegisterNotification */
        uint32_t event_parameter;                /*!< event notification parameter */
    } reg_ntf;                                   /*!< register notification */

    /**
     * @brief ESP_AVRC_TG_SET_PLAYER_APP_VALUE_EVT
     */
    struct avrc_tg_set_app_value_param {
        uint8_t num_val;                        /*!< attribute num */
        esp_avrc_set_app_value_param_t *p_vals; /*!< point to the id and value of player application attribute */
    } set_app_value;                            /*!< set player application value */

} esp_avrc_tg_cb_param_t;

/**
 * @brief           AVRCP controller callback function type
 *
 * @param           event : Event type
 *
 * @param           param : Pointer to callback parameter union
 */
typedef void (* esp_avrc_ct_cb_t)(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t *param);

/**
 * @brief           AVRCP target callback function type
 *
 * @param           event : Event type
 *
 * @param           param : Pointer to callback parameter union
 */
typedef void (* esp_avrc_tg_cb_t)(esp_avrc_tg_cb_event_t event, esp_avrc_tg_cb_param_t *param);


/**
 * @brief           Register application callbacks to AVRCP module. This function should be
 *                  called after esp_bluedroid_enable() completes successfully.
 *
 * @param[in]       callback: AVRCP controller callback function
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_avrc_ct_register_callback(esp_avrc_ct_cb_t callback);

/**
 *
 * @brief           Initialize the bluetooth AVRCP controller module, This function should be called
 *                  after esp_bluedroid_enable() completes successfully. Note: AVRC cannot work independently,
 *                  AVRC should be used along with A2DP and AVRC should be initialized before A2DP.
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_avrc_ct_init(void);

/**
 *
 * @brief           De-initialize AVRCP controller module. This function should be called after
 *                  after esp_bluedroid_enable() completes successfully. Note: AVRC cannot work independently,
 *                  AVRC should be used along with A2DP and AVRC should be deinitialized before A2DP.
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 */
esp_err_t esp_avrc_ct_deinit(void);

/**
 *
 * @brief           Send player application settings command to AVRCP target. This function should be called
 *                  after ESP_AVRC_CT_CONNECTION_STATE_EVT is received and AVRCP connection is established.
 *
 * @param[in]       tl : transaction label, 0 to 15, consecutive commands should use different values
 *
 * @param[in]       attr_id : player application setting attribute IDs from one of esp_avrc_ps_attr_ids_t
 *
 * @param[in]       value_id : attribute value defined for the specific player application setting attribute
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 */
esp_err_t esp_avrc_ct_send_set_player_value_cmd(uint8_t tl, uint8_t attr_id, uint8_t value_id);

/**
 * @brief           Send GetCapabilities PDU to AVRCP target to retrieve remote device's supported
 *                  notification event_ids. This function should be called after
 *                  ESP_AVRC_CT_CONNECTION_STATE_EVT is received and AVRCP connection is established.
 *
 * @param[in]       tl : transaction label, 0 to 15, consecutive commands should use different values
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 */
esp_err_t esp_avrc_ct_send_get_rn_capabilities_cmd(uint8_t tl);

/**
 * @brief           Send register notification command to AVRCP target. This function should be called after
 *                  ESP_AVRC_CT_CONNECTION_STATE_EVT is received and AVRCP connection is established.
 *
 * @param[in]       tl : transaction label, 0 to 15, consecutive commands should use different values.
 *
 * @param[in]       event_id : id of events, e.g. ESP_AVRC_RN_PLAY_STATUS_CHANGE, ESP_AVRC_RN_TRACK_CHANGE, etc.
 *
 * @param[in]       event_parameter : playback interval for ESP_AVRC_RN_PLAY_POS_CHANGED;
 *                                    For other events , value of this parameter is ignored.
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_ERR_NOT_SUPPORTED: if the event_id is not supported in current implementation
 *                  - ESP_FAIL: others
 */
esp_err_t esp_avrc_ct_send_register_notification_cmd(uint8_t tl, uint8_t event_id, uint32_t event_parameter);

/**
 * @brief           Send set absolute volume command to AVRCP target. This function should be called after
 *                  ESP_AVRC_CT_CONNECTION_STATE_EVT is received and AVRCP connection is established.
 *
 * @param[in]       tl : transaction label, 0 to 15, consecutive commands should use different values
 *
 * @param[in]       volume : volume, 0 to 0x7f, means 0% to 100%
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_ERR_NOT_SUPPORTED: if the event_id is not supported in current implementation
 *                  - ESP_FAIL: others
 */
esp_err_t esp_avrc_ct_send_set_absolute_volume_cmd(uint8_t tl, uint8_t volume);

/**
 * @brief           Send metadata command to AVRCP target. This function should be called after
 *                  ESP_AVRC_CT_CONNECTION_STATE_EVT is received and AVRCP connection is established.
 *
 * @param[in]       tl : transaction label, 0 to 15, consecutive commands should use different values.
 *
 * @param[in]       attr_mask : mask of attributes, e.g. ESP_AVRC_MD_ATTR_ID_TITLE | ESP_AVRC_MD_ATTR_ID_ARTIST.
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 */
esp_err_t esp_avrc_ct_send_metadata_cmd(uint8_t tl, uint8_t attr_mask);


/**
 * @brief           Send passthrough command to AVRCP target. This function should be called after
 *                  ESP_AVRC_CT_CONNECTION_STATE_EVT is received and AVRCP connection is established.
 *
 * @param[in]       tl : transaction label, 0 to 15, consecutive commands should use different values.
 *
 * @param[in]       key_code : passthrough command code, e.g. ESP_AVRC_PT_CMD_PLAY, ESP_AVRC_PT_CMD_STOP, etc.
 *
 * @param[in]       key_state : passthrough command key state, ESP_AVRC_PT_CMD_STATE_PRESSED or
 *                  ESP_AVRC_PT_CMD_STATE_RELEASED
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 */
esp_err_t esp_avrc_ct_send_passthrough_cmd(uint8_t tl, uint8_t key_code, uint8_t key_state);


/**
 * @brief           Register application callbacks to AVRCP target module. This function should be
 *                  called after esp_bluedroid_enable() completes successfully.
 *
 * @param[in]       callback: AVRCP target callback function
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_avrc_tg_register_callback(esp_avrc_tg_cb_t callback);

/**
 *
 * @brief           Initialize the bluetooth AVRCP target module, This function should be called
 *                  after esp_bluedroid_enable() completes successfully. Note: AVRC cannot work independently,
 *                  AVRC should be used along with A2DP and AVRC should be initialized before A2DP.
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_avrc_tg_init(void);

/**
 *
 * @brief           De-initialize AVRCP target module. This function should be called after
 *                  after esp_bluedroid_enable() completes successfully. Note: AVRC cannot work independently,
 *                  AVRC should be used along with A2DP and AVRC should be deinitialized before A2DP.
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 */
esp_err_t esp_avrc_tg_deinit(void);

/**
 *
 * @brief           Get the current filter of remote passthrough commands on AVRC target. Filter is given by
 *                  filter type and bit mask for the passthrough commands. This function should be called
 *                  after esp_avrc_tg_init().
 *                  For filter type ESP_AVRC_PSTH_FILTER_ALLOWED_CMD, the retrieved command set is constant and
 *                  it covers all of the passthrough commands that can possibly be supported.
 *                  For filter type ESP_AVRC_PSTH_FILTER_SUPPORT_COMMANDS, the retrieved command set covers the
 *                  passthrough commands selected to be supported according to current configuration. The
 *                  configuration can be changed using esp_avrc_tg_set_psth_cmd_filter().
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not enabled or AVRC TG is not initialized
 *                  - ESP_ERR_INVALID_ARG: if filter type is invalid or cmd_set is NULL
 *                  - ESP_FAIL: otherwise
 */
esp_err_t esp_avrc_tg_get_psth_cmd_filter(esp_avrc_psth_filter_t filter, esp_avrc_psth_bit_mask_t *cmd_set);

/**
 *
 * @brief           Set the filter of remote passthrough commands on AVRC target. Filter is given by
 *                  filter type and bit mask for the passthrough commands. This function should be called
 *                  after esp_avrc_tg_init().
 *
 *                  If filter type is ESP_AVRC_PSTH_FILTER_SUPPORT_CMD, the passthrough commands which
 *                  are set "1" as given in cmd_set will generate ESP_AVRC_CT_PASSTHROUGH_RSP_EVT callback
 *                  event and are auto-accepted in the protocol stack, other commands are replied with response
 *                  type "NOT IMPLEMENTED" (8). The set of supported commands should be a subset of allowed
 *                  command set. The allowed command set can be retrieved using esp_avrc_tg_get_psth_cmd_filter()
 *                  with filter type "ESP_AVRC_PSTH_FILTER_ALLOWED_CMD".
 *
 *                  Filter type "ESP_AVRC_PSTH_FILTER_ALLOWED_CMD" does not apply to this function.
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not enabled
 *                  - ESP_ERR_INVALID_ARG: if filter type is invalid or cmd_set is NULL
 *                  - ESP_ERR_NOT_SUPPORTED:: if filter type is ESP_AVRC_PSTH_FILTER_ALLOWED_CMD, or cmd_set
 *                    includes commands that are not allowed
 *
 */
esp_err_t esp_avrc_tg_set_psth_cmd_filter(esp_avrc_psth_filter_t filter, const esp_avrc_psth_bit_mask_t *cmd_set);

/**
 * @brief           Operate on the type esp_avrc_psth_bit_mask_t with regard to a specific PASSTHROUGH command.
 *
 * @param[in]       op: operation requested on the bit mask field
 *
 * @param[in]       psth: pointer to passthrough command bit mask structure
 *
 * @param[in]       cmd: passthrough command code
 *
 * @return          For operation ESP_AVRC_BIT_MASK_OP_SET or ESP_AVRC_BIT_MASK_OP_CLEAR, return
 *                  true for a successful operation, otherwise return false.
 *                  For operation ESP_AVRC_BIT_MASK_OP_TEST, return true if the corresponding bit
 *                  is set, otherwise false.
 *
 */
bool esp_avrc_psth_bit_mask_operation(esp_avrc_bit_mask_op_t op, esp_avrc_psth_bit_mask_t *psth, esp_avrc_pt_cmd_t cmd);

/**
 *
 * @brief           Get the requested event notification capabilies on local AVRC target. The capability is returned
 *                  in a bit mask representation in evt_set. This function should be called after esp_avrc_tg_init().
 *
 *                  For capability type "ESP_AVRC_RN_CAP_ALLOWED_EVT, the retrieved event set is constant and
 *                  it covers all of the notifcation events that can possibly be supported with current
 *                  implementation.
 *
 *                  For capability type ESP_AVRC_RN_CAP_SUPPORTED_EVT, the event set covers the notification
 *                  events selected to be supported under current configuration, The configuration can be
 *                  changed using esp_avrc_tg_set_rn_evt_cap().
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not enabled or AVRC TG is not initialized
 *                  - ESP_ERR_INVALID_ARG: if cap is invalid or evt_set is NULL
 *                  - ESP_FAIL: otherwise
 */
esp_err_t esp_avrc_tg_get_rn_evt_cap(esp_avrc_rn_evt_cap_t cap, esp_avrc_rn_evt_cap_mask_t *evt_set);

/**
 *
 * @brief           Set the event notification capabilities on local AVRCP target. The capability is given in a
 *                  bit mask representation in evt_set and must be a subset of allowed event IDs with current
 *                  implementation. This function should be called after esp_avrc_tg_init().
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not enabled
 *                  - ESP_ERR_INVALID_ARG: if evt_set is NULL
 *
 */
esp_err_t esp_avrc_tg_set_rn_evt_cap(const esp_avrc_rn_evt_cap_mask_t *evt_set);

/**
 * @brief           Operate on the type esp_avrc_rn_evt_cap_mask_t with regard to a specific event.
 *
 * @param[in]       op: operation requested on the bit mask field
 *
 * @param[in]       events: pointer to event notification capability bit mask structure
 *
 * @param[in]       event_id: notification event code
 *
 * @return          For operation ESP_AVRC_BIT_MASK_OP_SET or ESP_AVRC_BIT_MASK_OP_CLEAR, return
 *                  true for a successful operation, otherwise return false.
 *
 *                  For operation ESP_AVRC_BIT_MASK_OP_TEST, return true if the corresponding bit
 *                  is set, otherwise false.
 *
 */
bool esp_avrc_rn_evt_bit_mask_operation(esp_avrc_bit_mask_op_t op, esp_avrc_rn_evt_cap_mask_t *events,
                                        esp_avrc_rn_event_ids_t event_id);

/**
 *
 * @brief           Send RegisterNotification Response to remote AVRCP controller. Local event notification
 *                  capability can be set using esp_avrc_tg_set_rn_evt_cap(), in a bit mask representation
 *                  in evt_set. This function should be called after esp_avrc_tg_init().
 *
 * @param[in]       event_id: notification event ID that remote AVRCP CT registers
 *
 * @param[in]       rsp: notification response code
 *
 * @param[in]       param: parameters included in the specific notification
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not enabled or AVRC TG is not initialized
 *                  - ESP_ERR_INVALID_ARG: if evt_set is NULL
 *
 */
esp_err_t esp_avrc_tg_send_rn_rsp(esp_avrc_rn_event_ids_t event_id, esp_avrc_rn_rsp_t rsp,
                                  esp_avrc_rn_param_t *param);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_AVRC_API_H__ */
