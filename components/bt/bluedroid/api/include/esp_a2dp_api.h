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

#ifndef __ESP_A2DP_API_H__
#define __ESP_A2DP_API_H__

#include "esp_err.h"
#include "esp_bt_defs.h"

/// Media codec types supported by A2DP
#define ESP_A2D_MCT_SBC         (0)             /*!< SBC */
#define ESP_A2D_MCT_M12         (0x01)          /*!< MPEG-1, 2 Audio */
#define ESP_A2D_MCT_M24         (0x02)          /*!< MPEG-2, 4 AAC */
#define ESP_A2D_MCT_ATRAC       (0x04)          /*!< ATRAC family */
#define ESP_A2D_MCT_NON_A2DP    (0xff)
typedef uint8_t esp_a2d_mct_t;

/**
 * @brief Codec specific information elements as defined in A2DP spec.
 */

/**
 * @brief SBC codec specific information
 */
typedef struct {
#define ESP_A2D_CIE_LEN_SBC          (4)
    uint8_t oct[ESP_A2D_CIE_LEN_SBC];
} esp_a2d_cie_sbc_t;

/**
 * @brief MPEG-1,2 Audio codec specific information
 */
typedef struct {
#define ESP_A2D_CIE_LEN_M12          (4)
    uint8_t oct[ESP_A2D_CIE_LEN_M12];
} esp_a2d_cie_m12_t;

/**
 * @brief MPEG-2,4 AAC codec specific information
 */
typedef struct {
#define ESP_A2D_CIE_LEN_M24          (6)
    uint8_t oct[ESP_A2D_CIE_LEN_M24];
} esp_a2d_cie_m24_t;

/**
 * @brief ATRAC family codec specific information
 */
typedef struct {
#define ESP_A2D_CIE_LEN_ATRAC        (7)
    uint8_t oct[ESP_A2D_CIE_LEN_ATRAC];
} esp_a2d_cie_atrac_t;

/**
 * @brief A2DP media codec capabilities union
 */
typedef struct {
    esp_a2d_mct_t type;         /*!< A2DP media codec type */
    union {
        esp_a2d_cie_sbc_t sbc;
        esp_a2d_cie_m12_t m12;
        esp_a2d_cie_m24_t m24;
        esp_a2d_cie_atrac_t atrac;
    } cie;
} esp_a2d_mcc_t;

/// Bluetooth A2DP connection states
typedef enum {
    ESP_A2D_CONNECTION_STATE_DISCONNECTED = 0,
    ESP_A2D_CONNECTION_STATE_CONNECTING,
    ESP_A2D_CONNECTION_STATE_CONNECTED,
    ESP_A2D_CONNECTION_STATE_DISCONNECTING
} esp_a2d_connection_state_t;

/// Bluetooth A2DP disconnection reason
typedef enum {
    ESP_A2D_DISC_RSN_NORMAL = 0,
    ESP_A2D_DISC_RSN_ABNORMAL
} esp_a2d_disc_rsn_t;

/// Bluetooth A2DP datapath states
typedef enum {
    ESP_A2D_AUDIO_STATE_REMOTE_SUSPEND = 0,
    ESP_A2D_AUDIO_STATE_STOPPED,
    ESP_A2D_AUDIO_STATE_STARTED,
} esp_a2d_audio_state_t;

/// A2DP callback events
typedef enum {
    ESP_A2D_CONNECTION_STATE_EVT = 0,           /*!< connection state changed event */
    ESP_A2D_AUDIO_STATE_EVT = 1,                /*!< audio stream transmission state changed event */
    ESP_A2D_AUDIO_CFG_EVT = 2                   /*!< audio codec configuration received */
} esp_a2d_cb_event_t;

/// A2DP state callback parameters
typedef union {
    /*< ESP_A2D_CONNECTION_STATE_EVT */
    struct a2d_conn_stat_param {
        esp_a2d_connection_state_t state;       /*!< one of values from esp_a2d_connection_state_t */
        esp_bd_addr_t remote_bda;
        esp_a2d_disc_rsn_t disc_rsn;            /* disconnection reason */
    } conn_stat;
    
    /*< ESP_A2D_AUDIO_STATE_EVT */
    struct a2d_audio_stat_param {
        esp_a2d_audio_state_t state;            /*!< one of the values from esp_a2d_audio_state_t */
        esp_bd_addr_t remote_bda;
    } audio_stat;
    
    /*< ESP_A2D_AUDIO_CFG_EVT */
    struct a2d_audio_cfg_param {
        esp_bd_addr_t remote_bda;
        esp_a2d_mcc_t mcc;                      /*!< A2DP media codec capability information */
    } audio_cfg;
} esp_a2d_cb_param_t;

/**
 * @brief           A2DP profile callback function, data is ou
 * @param           event : Event type
 * @param           param : Point to callback parameter
 */
typedef void (* esp_a2d_cb_t)(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param);

/**
 * @brief           A2DP profile data callback function, data is ou
 *                 
 * @param[in]       buf: data received and decoded to PCM format, buf points to a static memory
 *                  and can be overwritten in later times
 * @param[in]       len: size(in bytes) in buf
 *
 */
typedef void (* esp_a2d_data_cb_t)(const uint8_t *buf, uint32_t len);


/**
 * @brief           This function is called to register application callbacks
 *                  with A2DP module; callbacks will provide the upstream events
 *                  (type esp_a2d_cb_event_t) and paramters(type esp_a2d_cb_param_t)
 *                  
 * @param[in]       callback: A2DP sink event callback function
 *
 * @return
 *                  - ESP_OK: success
 *
 */
esp_err_t esp_a2d_register_callback(esp_a2d_cb_t callback);


/**
 * @brief           This function is called to register A2DP sink data output function
 *                  for now only supports SBC codec, and the output is PCM data stream
 *                  
 * @param[in]       callback: A2DP data callback function
 *
 * @return
 *                  - ESP_OK: success
 *
 */
esp_err_t esp_a2d_register_data_callback(esp_a2d_data_cb_t cb);


/**
 *
 * @brief           This function is called to initialize the bluetooth A2DP sink module
 *
 * @return          
 *                  - ESP_OK: success
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_a2d_sink_init(void);


/**
 *
 * @brief           This function is called to deinit and free the resources for A2DP sink module
 *
 */
void esp_a2d_sink_deinit(void);


/**
 *
 * @brief           This function is called to connect with the remote bluetooth device
 *
 * @return          
 *                  - ESP_OK: connect request is sent to lower layer
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_a2d_sink_connect(esp_bd_addr_t remote_bda);


/**
 *
 * @brief           This function is called to disconnect with the remote bluetooth device
 *
 * @return          
 *                  - ESP_OK: disconnect request is sent to lower layer
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_a2d_sink_disconnect(esp_bd_addr_t remote_bda);

#endif /* __ESP_A2DP_API_H__ */
