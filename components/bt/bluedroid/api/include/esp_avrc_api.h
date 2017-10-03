// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __ESP_AVRC_API_H__
#define __ESP_AVRC_API_H__

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_bt_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/// AVRC feature bit mask
typedef enum {
    ESP_AVRC_FEAT_RCTG = 0x0001,                 /*!< remote control target */
    ESP_AVRC_FEAT_RCCT = 0x0002,                 /*!< remote control controller */
    ESP_AVRC_FEAT_VENDOR = 0x0008,               /*!< remote control vendor dependent commands */
    ESP_AVRC_FEAT_BROWSE = 0x0010,               /*!< use browsing channel */
    ESP_AVRC_FEAT_META_DATA = 0x0040,            /*!< remote control metadata transfer command/response */
    ESP_AVRC_FEAT_ADV_CTRL = 0x0200,             /*!< remote control advanced control commmand/response */
} esp_avrc_features_t;

/// AVRC passthrough command code
typedef enum {
    ESP_AVRC_PT_CMD_PLAY = 0x44,                 /*!< play */
    ESP_AVRC_PT_CMD_STOP = 0x45,                 /*!< stop */
    ESP_AVRC_PT_CMD_PAUSE = 0x46,                /*!< pause */
    ESP_AVRC_PT_CMD_FORWARD = 0x4B,              /*!< forward */
    ESP_AVRC_PT_CMD_BACKWARD = 0x4C,             /*!< backward */
    ESP_AVRC_PT_CMD_REWIND = 0x48,               /*!< rewind */
    ESP_AVRC_PT_CMD_FAST_FORWARD = 0x49          /*!< fast forward */
} esp_avrc_pt_cmd_t;

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
    ESP_AVRC_CT_MAX_EVT
} esp_avrc_ct_cb_event_t;

//AVRC metadata attribute mask
typedef enum {
    ESP_AVRC_MD_ATTR_TITLE = 0x1,                 /*!< title of the playing track */
    ESP_AVRC_MD_ATTR_ARTIST = 0x2,                /*!< track artist */
    ESP_AVRC_MD_ATTR_ALBUM = 0x4,                 /*!< album name */
    ESP_AVRC_MD_ATTR_TRACK_NUM = 0x8,             /*!< track position on the album */
    ESP_AVRC_MD_ATTR_NUM_TRACKS = 0x10,           /*!< number of tracks on the album */
    ESP_AVRC_MD_ATTR_GENRE = 0x20,                /*!< track genre */
    ESP_AVRC_MD_ATTR_PLAYING_TIME = 0x40          /*!< total album playing time in miliseconds */
} esp_avrc_md_attr_mask_t;

//AVRC event notification ids
typedef enum {
    ESP_AVRC_RN_PLAY_STATUS_CHANGE = 0x01,        /*!< track status change, eg. from playing to paused */
    ESP_AVRC_RN_TRACK_CHANGE = 0x02,              /*!< new track is loaded */
    ESP_AVRC_RN_TRACK_REACHED_END = 0x03,         /*!< current track reached end */
    ESP_AVRC_RN_TRACK_REACHED_START = 0x04,       /*!< current track reached start position */
    ESP_AVRC_RN_PLAY_POS_CHANGED = 0x05,          /*!< track playing position changed */
    ESP_AVRC_RN_BATTERY_STATUS_CHANGE = 0x06,     /*!< battery status changed */
    ESP_AVRC_RN_SYSTEM_STATUS_CHANGE = 0x07,      /*!< system status changed */
    ESP_AVRC_RN_APP_SETTING_CHANGE = 0x08,        /*!< application settings changed */
    ESP_AVRC_RN_MAX_EVT
} esp_avrc_rn_event_ids_t;

//AVRC player setting ids
typedef enum {
    ESP_AVRC_PS_EQUALIZER = 0x01,                 /*!< equalizer, on or off */
    ESP_AVRC_PS_REPEAT_MODE = 0x02,               /*!< repeat mode */
    ESP_AVRC_PS_SHUFFLE_MODE = 0x03,              /*!< shuffle mode */
    ESP_AVRC_PS_SCAN_MODE = 0x04,                 /*!< scan mode on or off */
    ESP_AVRC_PS_MAX_ATTR
} esp_avrc_ps_attr_ids_t;

//AVRC equalizer modes
typedef enum {
    ESP_AVRC_PS_EQUALIZER_OFF = 0x1,
    ESP_AVRC_PS_EQUALIZER_ON = 0x2
} esp_avrc_ps_eq_value_ids_t;

//AVRC repeat modes
typedef enum {
    ESP_AVRC_PS_REPEAT_OFF = 0x1,
    ESP_AVRC_PS_REPEAT_SINGLE = 0x2,
    ESP_AVRC_PS_REPEAT_GROUP = 0x3
} esp_avrc_ps_rpt_value_ids_t;


//AVRC shuffle modes
typedef enum {
    ESP_AVRC_PS_SHUFFLE_OFF = 0x1,
    ESP_AVRC_PS_SHUFFLE_ALL = 0x2,
    ESP_AVRC_PS_SHUFFLE_GROUP = 0x3
} esp_avrc_ps_shf_value_ids_t;

//AVRC scan modes
typedef enum {
    ESP_AVRC_PS_SCAN_OFF = 0x1,
    ESP_AVRC_PS_SCAN_ALL = 0x2,
    ESP_AVRC_PS_SCAN_GROUP = 0x3
} esp_avrc_ps_scn_value_ids_t;

/// AVRC controller callback parameters
typedef union {
    /**
     * @brief ESP_AVRC_CT_CONNECTION_STATE_EVT
     */
    struct avrc_ct_conn_stat_param {
        bool connected;                          /*!< whether AVRC connection is set up */
        uint32_t feat_mask;                      /*!< AVRC feature mask of remote device */
        esp_bd_addr_t remote_bda;                /*!< remote bluetooth device address */
    } conn_stat;                                 /*!< AVRC connection status */

    /**
     * @brief ESP_AVRC_CT_PASSTHROUGH_RSP_EVT
     */
    struct avrc_ct_psth_rsp_param {
        uint8_t tl;                              /*!< transaction label, 0 to 15 */
        uint8_t key_code;                        /*!< passthrough command code */
        uint8_t key_state;                       /*!< 0 for PRESSED, 1 for RELEASED */
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
        uint32_t event_parameter;                /*!< event notification parameter */
    } change_ntf;                                /*!< notifications */

} esp_avrc_ct_cb_param_t;


/**
 * @brief           AVRCP controller callback function type
 * @param           event : Event type
 * @param           param : Pointer to callback parameter union
 */
typedef void (* esp_avrc_ct_cb_t)(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t *param);


/**
 * @brief           Register application callbacks to AVRCP module; for now only AVRCP Controller
 *                  role is supported. This function should be called after esp_bluedroid_enable()
 *                  completes successfully
 *
 * @param[in]       callback: AVRCP controller callback function
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_avrc_ct_register_callback(esp_avrc_ct_cb_t callback);


/**
 *
 * @brief           Initialize the bluetooth AVRCP controller module, This function should be called
 *                  after esp_bluedroid_enable() completes successfully
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_avrc_ct_init(void);


/**
 *
 * @brief           De-initialize AVRCP controller module. This function should be called after
 *                  after esp_bluedroid_enable() completes successfully
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 */
esp_err_t esp_avrc_ct_deinit(void);

/* add description */
esp_err_t esp_avrc_ct_send_set_player_value_cmd(uint8_t tl, uint8_t attr_id, uint8_t value_id);

/**
 * @brief           Send register notification command to AVRCP target, This function should be called after
 *                  ESP_AVRC_CT_CONNECTION_STATE_EVT is received and AVRCP connection is established
 *
 * @param[in]       tl : transaction label, 0 to 15, consecutive commands should use different values.
 * @param[in]       event_id : id of events, e.g. ESP_AVRC_RN_PLAY_STATUS_CHANGE, ESP_AVRC_RN_TRACK_CHANGE, etc.
 * @param[in]       event_parameter : special parameters, eg. playback interval for ESP_AVRC_RN_PLAY_POS_CHANGED
 * @return
 *                  - ESP_OK: success
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 */
esp_err_t esp_avrc_ct_send_register_notification_cmd(uint8_t tl, uint8_t event_id, uint32_t event_parameter);


/**
 * @brief           Send metadata command to AVRCP target, This function should be called after
 *                  ESP_AVRC_CT_CONNECTION_STATE_EVT is received and AVRCP connection is established
 *
 * @param[in]       tl : transaction label, 0 to 15, consecutive commands should use different values.
 * @param[in]       attr_mask : mask of attributes, e.g. ESP_AVRC_MD_ATTR_ID_TITLE | ESP_AVRC_MD_ATTR_ID_ARTIST.
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 */
esp_err_t esp_avrc_ct_send_metadata_cmd(uint8_t tl, uint8_t attr_mask);


/**
 * @brief           Send passthrough command to AVRCP target, This function should be called after
 *                  ESP_AVRC_CT_CONNECTION_STATE_EVT is received and AVRCP connection is established
 *
 * @param[in]       tl : transaction label, 0 to 15, consecutive commands should use different values.
 * @param[in]       key_code : passthrough command code, e.g. ESP_AVRC_PT_CMD_PLAY, ESP_AVRC_PT_CMD_STOP, etc.
 * @param[in]       key_state : passthrough command key state, ESP_AVRC_PT_CMD_STATE_PRESSED or
 *                  ESP_AVRC_PT_CMD_STATE_RELEASED
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 */
esp_err_t esp_avrc_ct_send_passthrough_cmd(uint8_t tl, uint8_t key_code, uint8_t key_state);


#ifdef __cplusplus
}
#endif

#endif /* __ESP_AVRC_API_H__ */
