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

#include "esp_err.h"
#include "esp_bt_defs.h"
#include <stdint.h>
#include <stdbool.h>

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
enum {
    ESP_AVRC_PT_CMD_PLAY = 0x44,                 /*!< play */
    ESP_AVRC_PT_CMD_STOP = 0x45,                 /*!< stop */
    ESP_AVRC_PT_CMD_PAUSE = 0x46,                /*!< pause */
    ESP_AVRC_PT_CMD_FORWARD = 0x4B,              /*!< forward */
    ESP_AVRC_PT_CMD_BACKWARD = 0x4C              /*!< backward */
};

/// AVRC passthrough command state
enum {
    ESP_AVRC_PT_CMD_STATE_PRESSED = 0,           /*!< key pressed */
    ESP_AVRC_PT_CMD_STATE_RELEASED = 1           /*!< key released */
};

/// AVRC Controller callback events
typedef enum {
    ESP_AVRC_CT_CONNECTION_STATE_EVT = 0,        /*!< connection state changed event */
    ESP_AVRC_CT_PASSTHROUGH_RSP_EVT = 1,         /*!< passthrough response event */
    ESP_AVRC_CT_MAX_EVT
} esp_avrc_ct_cb_event_t;

/// AVRC controller callback parameters
typedef union {
    /*< ESP_AVRC_CT_CONNECTION_STATE_EVT */
    struct avrc_ct_conn_stat_param {
        bool connected;
	uint32_t feat_mask;
        esp_bd_addr_t remote_bda;
    } conn_stat;
    
    /*< ESP_AVRC_CT_PASSTHROUGH_RSP_EVT */
    struct avrc_ct_psth_rsp_param {
        uint8_t tl;                               /*!< transaction label, 0 to 15 */
        uint8_t key_code;
        uint8_t key_state;                        /*!< 0 for  */
    } psth_rsp;
} esp_avrc_ct_cb_param_t;


/**
 * @brief AVRCP controler callback function type
 * @param event : Event type
 * @param param : Point to callback parameter, currently is union type
 */
typedef void (* esp_avrc_ct_cb_t)(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t *param);


/**
 * @brief           This function is called to register application callbacks
 *                  to AVRCP module; callbacks will provide the upstream events
 *                  (type esp_avrc_ct_cb_event_t) and paramters(type esp_avrc_ct_cb_param_t)
 *                  
 * @param[in]       callback: A2DP sink event callback function
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_avrc_ct_register_callback(esp_avrc_ct_cb_t callback);


/**
 *
 * @brief           This function is called to initialize the bluetooth AVRCP controller module
 *
 * @return          
 *                  - ESP_OK: success
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_avrc_ct_init(void);


/**
 *
 * @brief           This function is called to deinit AVRCP controller module
 *
 */
esp_err_t esp_avrc_ct_deinit(void);


/**
 * send pass through command to target
 */
esp_err_t esp_avrc_ct_send_passthrough_cmd(uint8_t tl, uint8_t key_code, uint8_t key_state);

#endif /* __ESP_AVRC_API_H__ */
