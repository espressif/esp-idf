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
    ESP_AVRC_PT_CMD_BACKWARD = 0x4C              /*!< backward */
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
    ESP_AVRC_CT_MAX_EVT
} esp_avrc_ct_cb_event_t;

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


/**
 * @brief           Send passthrough command to AVRCP target, This function should be called after
 *                  ESP_AVRC_CT_CONNECTION_STATE_EVT is received and AVRCP connection is established
 *
 * @param[in]       tl : transaction label, 0 to 15, consecutive commands should use different values.
 * @param[in]       key_code : passthrough command code, e.g. ESP_AVRC_PT_CMD_PLAY, ESP_AVRC_PT_CMD_STOP, etc.
 * @param[in]       key_state : passthrough command key state, ESP_AVRC_PT_CMD_STATE_PRESSED or
 *                  ESP_AVRC_PT_CMD_STATE_PRESSED
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
