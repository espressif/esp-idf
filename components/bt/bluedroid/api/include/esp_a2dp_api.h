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

#ifdef __cplusplus
extern "C" {
#endif

/// Media codec types supported by A2DP
#define ESP_A2D_MCT_SBC         (0)             /*!< SBC */
#define ESP_A2D_MCT_M12         (0x01)          /*!< MPEG-1, 2 Audio */
#define ESP_A2D_MCT_M24         (0x02)          /*!< MPEG-2, 4 AAC */
#define ESP_A2D_MCT_ATRAC       (0x04)          /*!< ATRAC family */
#define ESP_A2D_MCT_NON_A2DP    (0xff)

typedef uint8_t esp_a2d_mct_t;

/// A2DP media codec capabilities union
typedef struct {
    esp_a2d_mct_t type;                        /*!< A2DP media codec type */
#define ESP_A2D_CIE_LEN_SBC          (4)
#define ESP_A2D_CIE_LEN_M12          (4)
#define ESP_A2D_CIE_LEN_M24          (6)
#define ESP_A2D_CIE_LEN_ATRAC        (7)
    union {
        uint8_t sbc[ESP_A2D_CIE_LEN_SBC];
        uint8_t m12[ESP_A2D_CIE_LEN_M12];
        uint8_t m24[ESP_A2D_CIE_LEN_M24];
        uint8_t atrac[ESP_A2D_CIE_LEN_ATRAC];
    } cie;                                     /*!< A2DP codec information element */
} __attribute__((packed)) esp_a2d_mcc_t;

/// Bluetooth A2DP connection states
typedef enum {
    ESP_A2D_CONNECTION_STATE_DISCONNECTED = 0, /*!< connection released  */
    ESP_A2D_CONNECTION_STATE_CONNECTING,       /*!< connecting remote device */
    ESP_A2D_CONNECTION_STATE_CONNECTED,        /*!< connection established */
    ESP_A2D_CONNECTION_STATE_DISCONNECTING     /*!< disconnecting remote device */
} esp_a2d_connection_state_t;

/// Bluetooth A2DP disconnection reason
typedef enum {
    ESP_A2D_DISC_RSN_NORMAL = 0,               /*!< Finished disconnection that is initiated by local or remote device */
    ESP_A2D_DISC_RSN_ABNORMAL                  /*!< Abnormal disconnection caused by signal loss */
} esp_a2d_disc_rsn_t;

/// Bluetooth A2DP datapath states
typedef enum {
    ESP_A2D_AUDIO_STATE_REMOTE_SUSPEND = 0,    /*!< audio stream datapath suspended by remote device */
    ESP_A2D_AUDIO_STATE_STOPPED,               /*!< audio stream datapath stopped */
    ESP_A2D_AUDIO_STATE_STARTED,               /*!< audio stream datapath started */
} esp_a2d_audio_state_t;

/// A2DP media control command acknowledgement code
typedef enum {
    ESP_A2D_MEDIA_CTRL_ACK_SUCCESS = 0,        /*!< media control command is acknowledged with success */
    ESP_A2D_MEDIA_CTRL_ACK_FAILURE,            /*!< media control command is acknowledged with failure */
    ESP_A2D_MEDIA_CTRL_ACK_BUSY,               /*!< media control command is rejected, as previous command is not yet acknowledged */
} esp_a2d_media_ctrl_ack_t;

/// A2DP media control commands
typedef enum {
    ESP_A2D_MEDIA_CTRL_NONE = 0,               /*!< dummy command */
    ESP_A2D_MEDIA_CTRL_CHECK_SRC_RDY,          /*!< check whether AVDTP is connected, only used in A2DP source */
    ESP_A2D_MEDIA_CTRL_START,                  /*!< command to set up media transmission channel */
    ESP_A2D_MEDIA_CTRL_STOP,                   /*!< command to stop media transmission */
    ESP_A2D_MEDIA_CTRL_SUSPEND,                /*!< command to suspend media transmission  */
} esp_a2d_media_ctrl_t;

/// A2DP callback events
typedef enum {
    ESP_A2D_CONNECTION_STATE_EVT = 0,          /*!< connection state changed event */
    ESP_A2D_AUDIO_STATE_EVT,                   /*!< audio stream transmission state changed event */
    ESP_A2D_AUDIO_CFG_EVT,                     /*!< audio codec is configured, only used for A2DP SINK */
    ESP_A2D_MEDIA_CTRL_ACK_EVT,                /*!< acknowledge event in response to media control commands */
} esp_a2d_cb_event_t;

/// A2DP state callback parameters
typedef union {
    /**
     * @brief  ESP_A2D_CONNECTION_STATE_EVT
     */
    struct a2d_conn_stat_param {
        esp_a2d_connection_state_t state;      /*!< one of values from esp_a2d_connection_state_t */
        esp_bd_addr_t remote_bda;              /*!< remote bluetooth device address */
        esp_a2d_disc_rsn_t disc_rsn;           /*!< reason of disconnection for "DISCONNECTED" */
    } conn_stat;                               /*!< A2DP connection status */

    /**
     * @brief ESP_A2D_AUDIO_STATE_EVT
     */
    struct a2d_audio_stat_param {
        esp_a2d_audio_state_t state;           /*!< one of the values from esp_a2d_audio_state_t */
        esp_bd_addr_t remote_bda;              /*!< remote bluetooth device address */
    } audio_stat;                              /*!< audio stream playing state */

    /**
     * @brief ESP_A2D_AUDIO_CFG_EVT
     */
    struct a2d_audio_cfg_param {
        esp_bd_addr_t remote_bda;              /*!< remote bluetooth device address */
        esp_a2d_mcc_t mcc;                     /*!< A2DP media codec capability information */
    } audio_cfg;                               /*!< media codec configuration infomation */

    /**
     * @brief ESP_A2D_MEDIA_CTRL_ACK_EVT
     */
    struct media_ctrl_stat_param {
        esp_a2d_media_ctrl_t cmd;              /*!< media control commands to acknowledge */
        esp_a2d_media_ctrl_ack_t status;       /*!< acknowledgement to media control commands */
    } media_ctrl_stat;                         /*!< status in acknowledgement to media control commands */
} esp_a2d_cb_param_t;

/**
 * @brief           A2DP profile callback function type
 *
 * @param           event : Event type
 *
 * @param           param : Pointer to callback parameter
 */
typedef void (* esp_a2d_cb_t)(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param);

/**
 * @brief           A2DP profile data callback function
 * @param[in]       buf : data received from A2DP source device and is PCM format decoder from SBC decoder;
 *                  buf references to a static memory block and can be overwritten by upcoming data
 * @param[in]       len : size(in bytes) in buf
 */
typedef void (* esp_a2d_sink_data_cb_t)(const uint8_t *buf, uint32_t len);

/**
 * @brief           A2DP source data read callback function
 *
 * @param[in]       buf : buffer to be filled with PCM data stream from higer layer
 *
 * @param[in]       len : size(in bytes) of data block to be copied to buf. -1 is an indication to user
 *                  that data buffer shall be flushed
 *
 * @return          size of bytes read successfully, if the argumetn len is -1, this value is ignored.
 *
 */
typedef int32_t (* esp_a2d_source_data_cb_t)(uint8_t *buf, int32_t len);

/**
 * @brief           Register application callback function to A2DP module. This function should be called
 *                  only after esp_bluedroid_enable() completes successfully, used by both A2DP source
 *                  and sink.
 *
 * @param[in]       callback: A2DP event callback function
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: if callback is a NULL function pointer
 *
 */
esp_err_t esp_a2d_register_callback(esp_a2d_cb_t callback);


/**
 * @brief           Register A2DP sink data output function; For now the output is PCM data stream decoded
 *                  from SBC format. This function should be called only after esp_bluedroid_enable()
 *                  completes successfully, used only by A2DP sink.
 *
 * @param[in]       callback: A2DP sink data callback function
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: if callback is a NULL function pointer
 *
 */
esp_err_t esp_a2d_sink_register_data_callback(esp_a2d_sink_data_cb_t callback);


/**
 *
 * @brief           Initialize the bluetooth A2DP sink module. This function should be called
 *                  after esp_bluedroid_enable() completes successfully
 *
 * @return
 *                  - ESP_OK: if the initialization request is sent successfully
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_a2d_sink_init(void);


/**
 *
 * @brief           De-initialize for A2DP sink module. This function
 *                  should be called only after esp_bluedroid_enable() completes successfully
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_a2d_sink_deinit(void);


/**
 *
 * @brief           Connect to remote bluetooth A2DP source device, must after esp_a2d_sink_init()
 *
 * @param[in]       remote_bda: remote bluetooth device address
 *
 * @return
 *                  - ESP_OK: connect request is sent to lower layer
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_a2d_sink_connect(esp_bd_addr_t remote_bda);


/**
 *
 * @brief           Disconnect from the remote A2DP source device
 *
 * @param[in]       remote_bda: remote bluetooth device address
 * @return
 *                  - ESP_OK: disconnect request is sent to lower layer
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_a2d_sink_disconnect(esp_bd_addr_t remote_bda);


/**
 *
 * @brief           media control commands; this API can be used for both A2DP sink and source
 *
 * @param[in]       ctrl: control commands for A2DP data channel
 * @return
 *                  - ESP_OK: control command is sent to lower layer
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_a2d_media_ctrl(esp_a2d_media_ctrl_t ctrl);


/**
 *
 * @brief           Initialize the bluetooth A2DP source module. This function should be called
 *                  after esp_bluedroid_enable() completes successfully
 *
 * @return
 *                  - ESP_OK: if the initialization request is sent successfully
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_a2d_source_init(void);


/**
 *
 * @brief           De-initialize for A2DP source module. This function
 *                  should be called only after esp_bluedroid_enable() completes successfully
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_a2d_source_deinit(void);


/**
 * @brief           Register A2DP source data input function; For now the input is PCM data stream.
 *                  This function should be called only after esp_bluedroid_enable() completes
 *                  successfully
 *
 * @param[in]       callback: A2DP source data callback function
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: if callback is a NULL function pointer
 *
 */
esp_err_t esp_a2d_source_register_data_callback(esp_a2d_source_data_cb_t callback);


/**
 *
 * @brief           Connect to remote A2DP sink device, must after esp_a2d_source_init()
 *
 * @param[in]       remote_bda: remote bluetooth device address
 *
 * @return
 *                  - ESP_OK: connect request is sent to lower layer
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_a2d_source_connect(esp_bd_addr_t remote_bda);


/**
 *
 * @brief           Disconnect from the remote A2DP sink device
 *
 * @param[in]       remote_bda: remote bluetooth device address
 * @return
 *                  - ESP_OK: disconnect request is sent to lower layer
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_a2d_source_disconnect(esp_bd_addr_t remote_bda);

#ifdef __cplusplus
}
#endif


#endif /* __ESP_A2DP_API_H__ */
