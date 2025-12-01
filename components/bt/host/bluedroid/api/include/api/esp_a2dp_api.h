/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_A2DP_API_H__
#define __ESP_A2DP_API_H__

#include "esp_err.h"
#include "esp_bt_defs.h"
#include "esp_a2dp_legacy_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_A2D_MAX_SEPS        1               /*!< Maximum number of Stream Endpoint that supported */

typedef uint16_t esp_a2d_conn_hdl_t;            /*!< Connection handle, associate with specific device that connected */

/* Media codec types supported by A2DP. */
#define ESP_A2D_MCT_SBC         (0)             /*!< SBC */
#define ESP_A2D_MCT_M12         (0x01)          /*!< MPEG-1, 2 Audio */
#define ESP_A2D_MCT_M24         (0x02)          /*!< MPEG-2, 4 AAC */
#define ESP_A2D_MCT_ATRAC       (0x04)          /*!< ATRAC family */
#define ESP_A2D_MCT_NON_A2DP    (0xff)          /*!< NON-A2DP */
typedef uint8_t esp_a2d_mct_t;                  /*!< Media codec type of A2DP */

/* Protocol service capabilities. This value is a mask. */
#define ESP_A2D_PSC_DELAY_RPT          (1<<0)  /*!< Delay Report */
typedef uint16_t esp_a2d_psc_t;                /*!< Protocol service capabilities type */

/* A2DP SBC sampling frequency bit mask in CIE */
#define ESP_A2D_SBC_CIE_SF_16K                  (0x8)       /*!< SBC sampling frequency 16kHz  */
#define ESP_A2D_SBC_CIE_SF_32K                  (0x4)       /*!< SBC sampling frequency 32kHz */
#define ESP_A2D_SBC_CIE_SF_44K                  (0x2)       /*!< SBC sampling frequency 44.1kHz  */
#define ESP_A2D_SBC_CIE_SF_48K                  (0x1)       /*!< SBC sampling frequency 48kHz */

/* A2DP SBC channel mode bit mask in CIE */
#define ESP_A2D_SBC_CIE_CH_MODE_MONO            (0x8)       /*!< SBC channel mode Mono */
#define ESP_A2D_SBC_CIE_CH_MODE_DUAL_CHANNEL    (0x4)       /*!< SBC channel mode Dual Channel */
#define ESP_A2D_SBC_CIE_CH_MODE_STEREO          (0x2)       /*!< SBC channel mode Stereo */
#define ESP_A2D_SBC_CIE_CH_MODE_JOINT_STEREO    (0x1)       /*!< SBC channel mode Joint Stereo */

/* A2DP SBC block length bit mask in CIE */
#define ESP_A2D_SBC_CIE_BLOCK_LEN_4             (0x8)       /*!< SBC block length 4 */
#define ESP_A2D_SBC_CIE_BLOCK_LEN_8             (0x4)       /*!< SBC block length 8 */
#define ESP_A2D_SBC_CIE_BLOCK_LEN_12            (0x2)       /*!< SBC block length 12 */
#define ESP_A2D_SBC_CIE_BLOCK_LEN_16            (0x1)       /*!< SBC block length 16 */

/* A2DP SBC number of subbands bit mask in CIE */
#define ESP_A2D_SBC_CIE_NUM_SUBBANDS_4          (0x2)       /*!< SBC number of subbands 4 */
#define ESP_A2D_SBC_CIE_NUM_SUBBANDS_8          (0x1)       /*!< SBC number of subbands 8 */

/* A2DP SBC allocation method bit mask in CIE */
#define ESP_A2D_SBC_CIE_ALLOC_MTHD_SNR          (0x2)       /*!< SBC allocation method SNR */
/*!< @deprecated Renamed to ESP_A2D_SBC_CIE_ALLOC_MTHD_SNR */
#define ESP_A2D_SBC_CIE_ALLOC_MTHD_SRN          _Pragma("GCC warning \"'ESP_A2D_SBC_CIE_ALLOC_MTHD_SRN' macro is deprecated, use 'ESP_A2D_SBC_CIE_ALLOC_MTHD_SNR'\"") ESP_A2D_SBC_CIE_ALLOC_MTHD_SNR
#define ESP_A2D_SBC_CIE_ALLOC_MTHD_LOUDNESS     (0x1)       /*!< SBC allocation method Loudness */

/**
 * @brief A2DP SBC media codec capabilities information struct
 */
typedef struct {
    uint8_t ch_mode         : 4;        /*!< Channel mode */
    uint8_t samp_freq       : 4;        /*!< Sampling frequency */
    uint8_t alloc_mthd      : 2;        /*!< Allocation method */
    uint8_t num_subbands    : 2;        /*!< Number of subbands */
    uint8_t block_len       : 4;        /*!< Block length */
    uint8_t min_bitpool;                /*!< Minimum bitpool */
    uint8_t max_bitpool;                /*!< Maximum bitpool */
} __attribute__((packed)) esp_a2d_cie_sbc_t;

/**
 * @brief A2DP MPEG-1, 2 media codec capabilities information struct (Not supported yet)
 */
typedef struct {
    uint8_t     ch_mode         : 4;        /*!< Channel mode */
    uint8_t     crc             : 1;        /*!< CRC protection */
    uint8_t     layer           : 3;        /*!< Layers of MPEG-1,2 Audio */
    uint8_t     samp_freq       : 6;        /*!< Sampling frequency */
    uint8_t     mpf             : 1;        /*!< Media payload format */
    uint8_t     rfu             : 1;        /*!< Reserved */
    uint8_t     bri1            : 7;        /*!< Bit rate index part 1 */
    uint8_t     vbr             : 1;        /*!< Support of VBR */
    uint8_t     bri2;                       /*!< Bit rate index part 2 */
} __attribute__((packed)) esp_a2d_cie_m12_t;

/**
 * @brief A2DP MPEG-2, 4 media codec capabilities information struct (Not supported yet)
 */
typedef struct {
    uint8_t     drc             : 1;        /*!< Support of MPEG-D DRC */
    uint8_t     obj_type        : 7;        /*!< Object type */
    uint8_t     samp_freq1      : 8;        /*!< Sampling frequency part 1 */
    uint8_t     ch              : 4;        /*!< Channels */
    uint8_t     samp_freq2      : 4;        /*!< Sampling frequency part 2 */
    uint8_t     br1             : 7;        /*!< Bit rate part 1 */
    uint8_t     vbr             : 1;        /*!< Support of VBR */
    uint8_t     br2;                        /*!< Bit rate part 2 */
    uint8_t     br3;                        /*!< Bit rate part 3 */
} __attribute__((packed)) esp_a2d_cie_m24_t;

/**
 * @brief A2DP ATRAC media codec capabilities information struct (Not supported yet)
 */
typedef struct {
    uint8_t     rfu1            : 2;        /*!< Reserved */
    uint8_t     ch_mode         : 3;        /*!< Channel mode */
    uint8_t     version         : 3;        /*!< Version */
    uint8_t     bri1            : 3;        /*!< Bit rate index part 1 */
    uint8_t     vbr             : 1;        /*!< Support of VBR */
    uint8_t     samp_freq       : 2;        /*!< Sampling frequency */
    uint8_t     rfu2            : 2;        /*!< Reserved */
    uint8_t     bri2;                       /*!< Bit rate index part 2 */
    uint8_t     bri3;                       /*!< Bit rate index part 3 */
    uint16_t    max_sul;                    /*!< Maximum SUL */
    uint8_t     rfu3;                       /*!< Reserved */
} __attribute__((packed)) esp_a2d_cie_atrac_t;

/**
 * @brief A2DP media codec capabilities union
 */
typedef struct {
    esp_a2d_mct_t type;                        /*!< A2DP media codec type */
#define ESP_A2D_CIE_LEN_SBC          (4)       /*!< SBC cie length */
#define ESP_A2D_CIE_LEN_M12          (4)       /*!< MPEG-1,2 cie length */
#define ESP_A2D_CIE_LEN_M24          (6)       /*!< MPEG-2,4 AAC cie length */
#define ESP_A2D_CIE_LEN_ATRAC        (7)       /*!< ATRAC family cie length */
    union {
        esp_a2d_cie_sbc_t   sbc_info;          /*!< SBC codec capabilities */
        esp_a2d_cie_m12_t   m12_info;          /*!< MPEG-1,2 audio codec capabilities */
        esp_a2d_cie_m24_t   m24_info;          /*!< MPEG-2, 4 AAC audio codec capabilities */
        esp_a2d_cie_atrac_t atrac_info;        /*!< ATRAC family codec capabilities */
    } cie;                                     /*!< A2DP codec information element */
} __attribute__((packed)) esp_a2d_mcc_t;

/**
 * @brief Bluetooth A2DP connection states
 */
typedef enum {
    ESP_A2D_CONNECTION_STATE_DISCONNECTED = 0, /*!< connection released  */
    ESP_A2D_CONNECTION_STATE_CONNECTING,       /*!< connecting remote device */
    ESP_A2D_CONNECTION_STATE_CONNECTED,        /*!< connection established */
    ESP_A2D_CONNECTION_STATE_DISCONNECTING     /*!< disconnecting remote device */
} esp_a2d_connection_state_t;

/**
 * @brief Bluetooth A2DP disconnection reason
 */
typedef enum {
    ESP_A2D_DISC_RSN_NORMAL = 0,               /*!< Finished disconnection that is initiated by local or remote device */
    ESP_A2D_DISC_RSN_ABNORMAL                  /*!< Abnormal disconnection caused by signal loss */
} esp_a2d_disc_rsn_t;

/**
 * @brief Bluetooth A2DP datapath states
 */
typedef enum {
    ESP_A2D_AUDIO_STATE_SUSPEND = 0,           /*!< audio stream datapath suspended by remote device */
    ESP_A2D_AUDIO_STATE_STARTED,               /*!< audio stream datapath started */
} esp_a2d_audio_state_t;

/**
 * @brief A2DP media control command acknowledgement code
 */
typedef enum {
    ESP_A2D_MEDIA_CTRL_ACK_SUCCESS = 0,        /*!< media control command is acknowledged with success */
    ESP_A2D_MEDIA_CTRL_ACK_FAILURE,            /*!< media control command is acknowledged with failure */
    ESP_A2D_MEDIA_CTRL_ACK_BUSY,               /*!< media control command is rejected, as previous command is not yet acknowledged */
} esp_a2d_media_ctrl_ack_t;

/**
 * @brief A2DP media control commands
 */
typedef enum {
    ESP_A2D_MEDIA_CTRL_NONE = 0,               /*!< Not for application use, use inside stack only. */
    ESP_A2D_MEDIA_CTRL_CHECK_SRC_RDY,          /*!< check whether AVDTP is connected, only used in A2DP source */
    ESP_A2D_MEDIA_CTRL_START,                  /*!< command to set up media transmission channel */
    ESP_A2D_MEDIA_CTRL_SUSPEND,                /*!< command to suspend media transmission  */
} esp_a2d_media_ctrl_t;

/**
 * @brief Bluetooth A2DP Initiation states
 */
typedef enum {
    ESP_A2D_DEINIT_SUCCESS = 0,                /*!< A2DP profile deinit successful event */
    ESP_A2D_INIT_SUCCESS                       /*!< A2DP profile init successful event */
} esp_a2d_init_state_t;

/**
 * @brief Bluetooth A2DP SEP register states
 */
typedef enum {
    ESP_A2D_SEP_REG_SUCCESS = 0,               /*!< A2DP stream endpoint register success */
    ESP_A2D_SEP_REG_FAIL,                      /*!< A2DP stream endpoint register generic fail */
    ESP_A2D_SEP_REG_UNSUPPORTED,               /*!< A2DP stream endpoint register fail, unsupported codec type or param */
    ESP_A2D_SEP_REG_INVALID_STATE,             /*!< A2DP stream endpoint register fail, invalid state */
} esp_a2d_sep_reg_state_t;

/**
 * @brief Bluetooth A2DP set delay report value states
 */
typedef enum {
    ESP_A2D_SET_SUCCESS = 0,                /*!< A2DP profile set delay report value successful */
    ESP_A2D_SET_INVALID_PARAMS              /*!< A2DP profile set delay report value is invalid parameter */
} esp_a2d_set_delay_value_state_t;

/**
 * @brief A2DP profile status parameters
 */
typedef struct {
    bool a2d_snk_inited;                   /*!< A2DP sink initialization */
    bool a2d_src_inited;                   /*!< A2DP source initialization */
    uint8_t conn_num;                      /*!< Number of connections */
} esp_a2d_profile_status_t;

/**
 * @brief A2DP callback events
 */
typedef enum {
    ESP_A2D_CONNECTION_STATE_EVT = 0,          /*!< connection state changed event */
    ESP_A2D_AUDIO_STATE_EVT,                   /*!< audio stream transmission state changed event */
    ESP_A2D_AUDIO_CFG_EVT,                     /*!< audio codec is configured */
    ESP_A2D_MEDIA_CTRL_ACK_EVT,                /*!< acknowledge event in response to media control commands */
    ESP_A2D_PROF_STATE_EVT,                    /*!< indicate a2dp init&deinit complete */
    ESP_A2D_SEP_REG_STATE_EVT,                 /*!< indicate a2dp stream endpoint register status */
    ESP_A2D_SNK_PSC_CFG_EVT,                   /*!< protocol service capabilities configured，only used for A2DP SINK */
    ESP_A2D_SNK_SET_DELAY_VALUE_EVT,           /*!< indicate a2dp sink set delay report value complete,  only used for A2DP SINK */
    ESP_A2D_SNK_GET_DELAY_VALUE_EVT,           /*!< indicate a2dp sink get delay report value complete,  only used for A2DP SINK */
    ESP_A2D_REPORT_SNK_DELAY_VALUE_EVT,        /*!< report delay value,  only used for A2DP SRC */
} esp_a2d_cb_event_t;

/**
 * @brief A2DP audio buffer
 */
typedef struct {
    uint16_t    buff_size;                      /*!< audio buff size */
    uint16_t    number_frame;                   /*!< number of encoded frame in this buff */
    uint32_t    timestamp;                      /*!< timestamp of the first frame */
    uint16_t    data_len;                       /*!< audio data len */
    uint8_t    *data;                           /*!< pointer to audio data start */
} esp_a2d_audio_buff_t;                         /*!< struct to store audio data */

/**
 * @brief A2DP state callback parameters
 */
typedef union {
    /**
     * @brief  ESP_A2D_CONNECTION_STATE_EVT
     */
    struct a2d_conn_stat_param {
        esp_a2d_connection_state_t state;      /*!< one of values from esp_a2d_connection_state_t */
        esp_bd_addr_t remote_bda;              /*!< remote bluetooth device address */
        esp_a2d_conn_hdl_t conn_hdl;           /*!< connection handle */
        uint16_t audio_mtu;                    /*!< MTU of audio connection */
        esp_a2d_disc_rsn_t disc_rsn;           /*!< reason of disconnection for "DISCONNECTED" */
    } conn_stat;                               /*!< A2DP connection status */

    /**
     * @brief ESP_A2D_AUDIO_STATE_EVT
     */
    struct a2d_audio_stat_param {
        esp_a2d_audio_state_t state;           /*!< one of the values from esp_a2d_audio_state_t */
        esp_bd_addr_t remote_bda;              /*!< remote bluetooth device address*/
        esp_a2d_conn_hdl_t conn_hdl;           /*!< connection handle */
    } audio_stat;                              /*!< audio stream playing state */

    /**
     * @brief ESP_A2D_AUDIO_CFG_EVT
     */
    struct a2d_audio_cfg_param {
        esp_bd_addr_t remote_bda;              /*!< remote bluetooth device address */
        esp_a2d_conn_hdl_t conn_hdl;           /*!< connection handle */
        esp_a2d_mcc_t mcc;                     /*!< A2DP media codec capability information */
    } audio_cfg;                               /*!< media codec configuration information */

    /**
     * @brief ESP_A2D_MEDIA_CTRL_ACK_EVT
     */
    struct media_ctrl_stat_param {
        esp_a2d_media_ctrl_t cmd;              /*!< media control commands to acknowledge */
        esp_a2d_media_ctrl_ack_t status;       /*!< acknowledgement to media control commands */
    } media_ctrl_stat;                         /*!< status in acknowledgement to media control commands */

    /**
     * @brief ESP_A2D_PROF_STATE_EVT
     */
    struct a2d_prof_stat_param {
        esp_a2d_init_state_t init_state;       /*!< a2dp profile state param */
    } a2d_prof_stat;                           /*!< status to indicate a2d prof init or deinit */

    /**
     * @brief ESP_A2D_SEP_REG_STATE_EVT
     */
    struct a2d_sep_reg_stat_param {
        uint8_t seid;                          /*!< the stream endpoint to register */
        esp_a2d_sep_reg_state_t reg_state;     /*!< stream endpoint register state */
    } a2d_sep_reg_stat;                        /*!< status to indicate a2d sep register success or not */

    /**
     * @brief ESP_A2D_SNK_PSC_CFG_EVT
     */
    struct a2d_psc_cfg_param {
        esp_a2d_psc_t psc_mask;                /*!< protocol service capabilities configured */
    } a2d_psc_cfg_stat;                        /*!< status to indicate protocol service capabilities configured */

    /**
     * @brief ESP_A2D_SNK_SET_DELAY_VALUE_EVT
     */
    struct a2d_set_stat_param {
        esp_a2d_set_delay_value_state_t set_state;       /*!< a2dp profile state param */
        uint16_t delay_value;                            /*!< delay report value */
    } a2d_set_delay_value_stat;                          /*!< A2DP sink set delay report value status */

    /**
     * @brief ESP_A2D_SNK_GET_DELAY_VALUE_EVT
     */
    struct a2d_get_stat_param {
        uint16_t delay_value;                  /*!< delay report value */
    } a2d_get_delay_value_stat;                /*!< A2DP sink get delay report value status */

    /**
     * @brief ESP_A2D_REPORT_SNK_DELAY_VALUE_EVT
     */
    struct a2d_report_delay_stat_param {
        uint16_t delay_value;                  /*!< delay report value */
    } a2d_report_delay_value_stat;             /*!< A2DP source received sink report value status */

} esp_a2d_cb_param_t;

/**
 * @brief           A2DP profile callback function type
 *
 * @param[in]       event : Event type
 *
 * @param[in]       param : Pointer to callback parameter
 */
typedef void (* esp_a2d_cb_t)(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param);

/**
 * @brief           A2DP sink data callback function
 *
 * @param[in]       conn_hdl: connection handle
 *
 * @param[in]       audio_buf: pointer to the data received from A2DP source device, should be freed by
 *                  calling esp_a2d_audio_buff_free
 */
typedef void (* esp_a2d_sink_audio_data_cb_t)(esp_a2d_conn_hdl_t conn_hdl, esp_a2d_audio_buff_t *audio_buf);

/**
 * @brief           Allocate a audio buffer to store and send audio data, can be used in both sink and source.
 *
 * @param[in]       size: buffer size to allocate
 *
 * @return          allocated audio buffer, if Bluedroid is not enabled, no memory, or size is
 *                  zeros, will return NULL
 *
 */
esp_a2d_audio_buff_t *esp_a2d_audio_buff_alloc(uint16_t size);

/**
 * @brief           Free a audio buffer allocated by esp_a2d_audio_buff_alloc.
 *
 * @param[in]       audio_buf: audio buffer to free
 *
 */
void esp_a2d_audio_buff_free(esp_a2d_audio_buff_t *audio_buf);

/**
 * @brief           Register application callback function to A2DP module. This function should be called
 *                  only after esp_bluedroid_enable() completes successfully, used by both A2DP source
 *                  and sink.
 *
 * @param[in]       callback: A2DP event callback function
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: if callback is a NULL function pointer
 *
 */
esp_err_t esp_a2d_register_callback(esp_a2d_cb_t callback);

/**
 * @brief           Register A2DP sink audio data output function, the output format is undecoded audio data
 *                  frame in esp_a2d_audio_buff_t, user shall call esp_a2d_audio_buff_free to free the buff
 *                  when the data is consumed.
 *
 * @param[in]       callback: A2DP sink audio data callback function
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_a2d_sink_register_audio_data_callback(esp_a2d_sink_audio_data_cb_t callback);

/**
 *
 * @brief           Initialize the bluetooth A2DP sink module. This function should be called
 *                  after esp_bluedroid_enable() completes successfully, and ESP_A2D_PROF_STATE_EVT
 *                  with ESP_A2D_INIT_SUCCESS will reported to the APP layer. Note: A2DP can work independently.
 *                  If you want to use AVRC together, you should initiate AVRC first. This
 *                  function should be called after esp_bluedroid_enable() completes successfully.
 *
 * @return
 *                  - ESP_OK: if the initialization request is sent successfully
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_a2d_sink_init(void);

/**
 * @brief           Register a a2dp sink Stream Endpoint (SEP) with specific codec capability, shall register
 *                  SEP after a2dp sink initializing and before a2dp connection establishing. Register the same
 *                  SEP index repeatedly will overwrite the old one.
 *
 * @param[in]       seid: local SEP identifier, start from 0, less than ESP_A2D_MAX_SEPS
 *
 * @param[in]       mcc: codec capability, currently only supports SBC
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_ERR_INVALID_ARG: invalid parameter
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_a2d_sink_register_stream_endpoint(uint8_t seid, const esp_a2d_mcc_t *mcc);

/**
 *
 * @brief           De-initialize for A2DP sink module. This function
 *                  should be called only after esp_bluedroid_enable() completes successfully,
 *                  and ESP_A2D_PROF_STATE_EVT with ESP_A2D_DEINIT_SUCCESS will reported to APP layer.
 *
 * @return
 *                  - ESP_OK: if the deinitialization request is sent successfully
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_a2d_sink_deinit(void);

/**
 *
 * @brief           Connect to remote bluetooth A2DP source device. This API must be called after
 *                  esp_a2d_sink_init() and before esp_a2d_sink_deinit().
 *
 * @param[in]       remote_bda: remote bluetooth device address
 *
 * @return
 *                  - ESP_OK: connect request is sent to lower layer successfully
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_a2d_sink_connect(esp_bd_addr_t remote_bda);

/**
 *
 * @brief           Disconnect from the remote A2DP source device. This API must be called after
 *                  esp_a2d_sink_init() and before esp_a2d_sink_deinit().
 *
 * @param[in]       remote_bda: remote bluetooth device address
 *
 * @return
 *                  - ESP_OK: disconnect request is sent to lower layer successfully
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_a2d_sink_disconnect(esp_bd_addr_t remote_bda);

/**
 *
 * @brief           Set delay reporting value. The delay value of sink is caused by buffering (including
 *                  protocol stack and application layer), decoding and rendering. The default delay
 *                  value is 120ms, if the set value is less than 120ms, the setting will fail. This API
 *                  must be called after esp_a2d_sink_init() and before esp_a2d_sink_deinit().
 *
 * @param[in]       delay_value: reporting value is in 1/10 millisecond
 *
 * @return
 *                  - ESP_OK: delay value is sent to lower layer successfully
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_a2d_sink_set_delay_value(uint16_t delay_value);

/**
 *
 * @brief           Get delay reporting value. This API must be called after
 *                  esp_a2d_sink_init() and before esp_a2d_sink_deinit().
 *
 * @return
 *                  - ESP_OK: if the request is sent successfully
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_a2d_sink_get_delay_value(void);

/**
 *
 * @brief           Media control commands. This API can be used for both A2DP sink and source
 *                  and must be called after esp_a2d_sink_init() and before esp_a2d_sink_deinit().
 *
 * @param[in]       ctrl: control commands for A2DP data channel
 *
 * @return
 *                  - ESP_OK: control command is sent to lower layer successfully
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_a2d_media_ctrl(esp_a2d_media_ctrl_t ctrl);

/**
 * @brief       This function is used to get the status of A2DP
 *
 * @param[out]  profile_status - A2DP status
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_a2d_get_profile_status(esp_a2d_profile_status_t *profile_status);


/**
 *
 * @brief           Initialize the bluetooth A2DP source module. A2DP can work independently.
 *                  If you want to use AVRC together, you should initiate AVRC first. This function should be called
 *                  after esp_bluedroid_enable() completes successfully, and ESP_A2D_PROF_STATE_EVT
 *                  with ESP_A2D_INIT_SUCCESS will reported to the APP layer. Note: A2DP can work independently.
 *                  If you want to use AVRC together, you should initiate AVRC first.
 *
 * @return
 *                  - ESP_OK: if the initialization request is sent to lower layer successfully
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_a2d_source_init(void);

/**
 * @brief           Register a a2dp source Stream Endpoint (SEP) with specific codec capability, shall register
 *                  SEP after a2dp source initializing and before a2dp connection establishing. Register the same
 *                  SEP index repeatedly will overwrite the old one.
 *
 * @param[in]       seid: local SEP identifier, start from 0, less than ESP_A2D_MAX_SEPS
 *
 * @param[in]       mcc: codec capability, currently, only SBC supported
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_ERR_INVALID_ARG: invalid parameter
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_a2d_source_register_stream_endpoint(uint8_t seid, const esp_a2d_mcc_t *mcc);

/**
 *
 * @brief           De-initialize for A2DP source module. This function
 *                  should be called only after esp_bluedroid_enable() completes successfully,
 *                  and ESP_A2D_PROF_STATE_EVT with ESP_A2D_DEINIT_SUCCESS will reported to APP layer.
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_a2d_source_deinit(void);

/**
 * @brief           Send an audio buffer with encoded audio data to sink. The audio data length shall not be bigger than
 *                  audio connection mtu (retrieved from ESP_A2D_CONNECTION_STATE_EVT). if the return value is
 *                  ESP_OK, then the audio buff is consumed, otherwise, audio buff can be reused by user.
 *
 * @param[in]       conn_hdl: connection handle
 *
 * @param[in]       audio_buf: encoded audio data
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_ERR_INVALID_ARG: invalid parameter
 *                  - ESP_ERR_INVALID_SIZE: data len bigger than mtu
 *                  - ESP_FAIL: buffer queue is full, try again later
 *
 */
esp_err_t esp_a2d_source_audio_data_send(esp_a2d_conn_hdl_t conn_hdl, esp_a2d_audio_buff_t *audio_buf);

/**
 *
 * @brief           Connect to remote A2DP sink device. This API must be called
 *                  after esp_a2d_source_init() and before esp_a2d_source_deinit().
 *
 * @param[in]       remote_bda: remote bluetooth device address
 *
 * @return
 *                  - ESP_OK: connect request is sent to lower layer successfully
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_a2d_source_connect(esp_bd_addr_t remote_bda);

/**
 *
 * @brief           Disconnect from the remote A2DP sink device. This API must be called
 *                  after esp_a2d_source_init() and before esp_a2d_source_deinit().
 *
 * @param[in]       remote_bda: remote bluetooth device address
 * @return
 *                  - ESP_OK: disconnect request is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_a2d_source_disconnect(esp_bd_addr_t remote_bda);

#ifdef __cplusplus
}
#endif


#endif /* __ESP_A2DP_API_H__ */
