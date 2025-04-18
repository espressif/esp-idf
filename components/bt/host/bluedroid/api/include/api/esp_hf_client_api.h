/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_HF_CLIENT_API_H__
#define __ESP_HF_CLIENT_API_H__

#include "esp_err.h"
#include "esp_bt_defs.h"
#include "esp_hf_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_BT_HF_CLIENT_NUMBER_LEN           (32)
#define ESP_BT_HF_CLIENT_OPERATOR_NAME_LEN    (16)
#define ESP_BT_HF_AT_SEND_XAPL_LEN            (14)

/// Bluetooth HFP RFCOMM connection and service level connection status
typedef enum {
    ESP_HF_CLIENT_CONNECTION_STATE_DISCONNECTED = 0,     /*!< RFCOMM data link channel released */
    ESP_HF_CLIENT_CONNECTION_STATE_CONNECTING,           /*!< connecting remote device on the RFCOMM data link*/
    ESP_HF_CLIENT_CONNECTION_STATE_CONNECTED,            /*!< RFCOMM connection established */
    ESP_HF_CLIENT_CONNECTION_STATE_SLC_CONNECTED,        /*!< service level connection established */
    ESP_HF_CLIENT_CONNECTION_STATE_DISCONNECTING,        /*!< disconnecting with remote device on the RFCOMM dat link*/
} esp_hf_client_connection_state_t;

/// Bluetooth HFP audio connection status
typedef enum {
    ESP_HF_CLIENT_AUDIO_STATE_DISCONNECTED = 0,          /*!< audio connection released */
    ESP_HF_CLIENT_AUDIO_STATE_CONNECTING,                /*!< audio connection has been initiated */
    ESP_HF_CLIENT_AUDIO_STATE_CONNECTED,                 /*!< audio connection is established */
    ESP_HF_CLIENT_AUDIO_STATE_CONNECTED_MSBC,            /*!< mSBC audio connection is established */
} esp_hf_client_audio_state_t;

/// in-band ring tone state
typedef enum {
    ESP_HF_CLIENT_IN_BAND_RINGTONE_NOT_PROVIDED = 0,
    ESP_HF_CLIENT_IN_BAND_RINGTONE_PROVIDED,
} esp_hf_client_in_band_ring_state_t;

/**
 * @brief HF client profile status parameters
 */
typedef struct {
    bool hf_client_inited;                             /*!< hf client initialization */
    uint8_t slc_conn_num;                              /*!< Number of Service Level Connections */
    uint8_t sync_conn_num;                             /*!< Number of (e)SCO Connections */
} esp_hf_client_profile_status_t;

/* features masks of AG */
#define ESP_HF_CLIENT_PEER_FEAT_3WAY       0x01        /* Three-way calling */
#define ESP_HF_CLIENT_PEER_FEAT_ECNR       0x02        /* Echo cancellation and/or noise reduction */
#define ESP_HF_CLIENT_PEER_FEAT_VREC       0x04        /* Voice recognition */
#define ESP_HF_CLIENT_PEER_FEAT_INBAND     0x08        /* In-band ring tone */
#define ESP_HF_CLIENT_PEER_FEAT_VTAG       0x10        /* Attach a phone number to a voice tag */
#define ESP_HF_CLIENT_PEER_FEAT_REJECT     0x20        /* Ability to reject incoming call */
#define ESP_HF_CLIENT_PEER_FEAT_ECS        0x40        /* Enhanced Call Status */
#define ESP_HF_CLIENT_PEER_FEAT_ECC        0x80        /* Enhanced Call Control */
#define ESP_HF_CLIENT_PEER_FEAT_EXTERR    0x100        /* Extended error codes */
#define ESP_HF_CLIENT_PEER_FEAT_CODEC     0x200        /* Codec Negotiation */
/* HFP 1.7+ */
#define ESP_HF_CLIENT_PEER_FEAT_HF_IND    0x400        /* HF Indicators */
#define ESP_HF_CLIENT_PEER_FEAT_ESCO_S4   0x800        /* eSCO S4 Setting Supported */

/* CHLD feature masks of AG */
#define ESP_HF_CLIENT_CHLD_FEAT_REL           0x01       /* 0  Release waiting call or held calls */
#define ESP_HF_CLIENT_CHLD_FEAT_REL_ACC       0x02       /* 1  Release active calls and accept other waiting or held call */
#define ESP_HF_CLIENT_CHLD_FEAT_REL_X         0x04       /* 1x Release specified active call only */
#define ESP_HF_CLIENT_CHLD_FEAT_HOLD_ACC      0x08       /* 2  Active calls on hold and accept other waiting or held call */
#define ESP_HF_CLIENT_CHLD_FEAT_PRIV_X        0x10       /* 2x Request private mode with specified call(put the rest on hold) */
#define ESP_HF_CLIENT_CHLD_FEAT_MERGE         0x20       /* 3  Add held call to multiparty */
#define ESP_HF_CLIENT_CHLD_FEAT_MERGE_DETACH  0x40       /* 4  Connect two calls and leave(disconnect from multiparty) */

/* XAPL feature masks*/
#define ESP_HF_CLIENT_XAPL_FEAT_RESERVED            0x01    /* reserved */
#define ESP_HF_CLIENT_XAPL_FEAT_BATTERY_REPORT      0x02    /* The accessory supports battery reporting (reserved only for battery operated accessories) */
#define ESP_HF_CLIENT_XAPL_FEAT_DOCKED              0x04    /* The accessory is docked or powered (reserved only for battery operated accessories). */
#define ESP_HF_CLIENT_XAPL_FEAT_SIRI_STATUS_REPORT  0x08    /* The accessory supports Siri status reporting */
#define ESP_HF_CLIENT_XAPL_NR_STATUS_REPORT         0x10    /* the accessory supports noise reduction (NR) status reporting */

/// HF CLIENT callback events
typedef enum {
    ESP_HF_CLIENT_CONNECTION_STATE_EVT = 0,          /*!< connection state changed event */
    ESP_HF_CLIENT_AUDIO_STATE_EVT,                   /*!< audio connection state change event */
    ESP_HF_CLIENT_BVRA_EVT,                          /*!< voice recognition state change event */
    ESP_HF_CLIENT_CIND_CALL_EVT,                     /*!< call indication */
    ESP_HF_CLIENT_CIND_CALL_SETUP_EVT,               /*!< call setup indication */
    ESP_HF_CLIENT_CIND_CALL_HELD_EVT,                /*!< call held indication */
    ESP_HF_CLIENT_CIND_SERVICE_AVAILABILITY_EVT,     /*!< network service availability indication */
    ESP_HF_CLIENT_CIND_SIGNAL_STRENGTH_EVT,          /*!< signal strength indication */
    ESP_HF_CLIENT_CIND_ROAMING_STATUS_EVT,           /*!< roaming status indication */
    ESP_HF_CLIENT_CIND_BATTERY_LEVEL_EVT,            /*!< battery level indication */
    ESP_HF_CLIENT_COPS_CURRENT_OPERATOR_EVT,         /*!< current operator information */
    ESP_HF_CLIENT_BTRH_EVT,                          /*!< call response and hold event */
    ESP_HF_CLIENT_CLIP_EVT,                          /*!< Calling Line Identification notification */
    ESP_HF_CLIENT_CCWA_EVT,                          /*!< call waiting notification */
    ESP_HF_CLIENT_CLCC_EVT,                          /*!< list of current calls notification */
    ESP_HF_CLIENT_VOLUME_CONTROL_EVT,                /*!< audio volume control command from AG, provided by +VGM or +VGS message */
    ESP_HF_CLIENT_AT_RESPONSE_EVT,                   /*!< AT command response event */
    ESP_HF_CLIENT_CNUM_EVT,                          /*!< subscriber information response from AG */
    ESP_HF_CLIENT_BSIR_EVT,                          /*!< setting of in-band ring tone */
    ESP_HF_CLIENT_BINP_EVT,                          /*!< requested number of last voice tag from AG */
    ESP_HF_CLIENT_RING_IND_EVT,                      /*!< ring indication event */
    ESP_HF_CLIENT_PKT_STAT_NUMS_GET_EVT,             /*!< requested number of packet different status */
    ESP_HF_CLIENT_PROF_STATE_EVT,                    /*!< Indicate HF CLIENT init or deinit complete */
} esp_hf_client_cb_event_t;

/// HFP client callback parameters
typedef union {
    /**
     * @brief  ESP_HF_CLIENT_CONNECTION_STATE_EVT
     */
    struct hf_client_conn_stat_param {
        esp_hf_client_connection_state_t state;  /*!< HF connection state */
        uint32_t peer_feat;                      /*!< AG supported features */
        uint32_t chld_feat;                      /*!< AG supported features on call hold and multiparty services */
        esp_bd_addr_t remote_bda;                /*!< remote bluetooth device address */
    } conn_stat;                                 /*!< HF callback param of ESP_HF_CLIENT_CONNECTION_STATE_EVT */

    /**
     * @brief ESP_HF_CLIENT_AUDIO_STATE_EVT
     */
    struct hf_client_audio_stat_param {
        esp_hf_client_audio_state_t state;       /*!< audio connection state */
        esp_bd_addr_t remote_bda;                /*!< remote bluetooth device address */
        uint16_t  sync_conn_handle;              /*!< (e)SCO connection handle */
    } audio_stat;                                /*!< HF callback param of ESP_HF_CLIENT_AUDIO_STATE_EVT */

    /**
     * @brief ESP_HF_CLIENT_BVRA_EVT
     */
    struct hf_client_bvra_param {
        esp_hf_vr_state_t value;                 /*!< voice recognition state */
    } bvra;                                      /*!< HF callback param of ESP_HF_CLIENT_BVRA_EVT */

    /**
     * @brief ESP_HF_CLIENT_CIND_SERVICE_AVAILABILITY_EVT
     */
    struct hf_client_service_availability_param {
        esp_hf_network_state_t status;           /*!< service availability status */
    } service_availability;                      /*!< HF callback param of ESP_HF_CLIENT_CIND_SERVICE_AVAILABILITY_EVT */

    /**
     * @brief ESP_HF_CLIENT_CIND_ROAMING_STATUS_EVT
     */
    struct hf_client_network_roaming_param {
        esp_hf_roaming_status_t status;          /*!< roaming status */
    } roaming;                                   /*!< HF callback param of ESP_HF_CLIENT_CIND_ROAMING_STATUS_EVT */

    /**
     * @brief ESP_HF_CLIENT_CIND_SIGNAL_STRENGTH_EVT
     */
    struct hf_client_signal_strength_ind_param {
        int value;                               /*!< signal strength value, ranges from 0 to 5 */
    } signal_strength;                           /*!< HF callback param of ESP_HF_CLIENT_CIND_SIGNAL_STRENGTH_EVT */

    /**
     * @brief ESP_HF_CLIENT_CIND_BATTERY_LEVEL_EVT
     */
    struct hf_client_battery_level_ind_param {
        int value;                               /*!< battery charge value, ranges from 0 to 5 */
    } battery_level;                             /*!< HF callback param of ESP_HF_CLIENT_CIND_BATTERY_LEVEL_EVT */

    /**
     * @brief ESP_HF_CLIENT_COPS_CURRENT_OPERATOR_EVT
     */
    struct hf_client_current_operator_param {
        const char *name;                        /*!< name of the network operator */
    } cops;                                      /*!< HF callback param of ESP_HF_CLIENT_COPS_CURRENT_OPERATOR_EVT */

    /**
     * @brief ESP_HF_CLIENT_CIND_CALL_EVT
     */
    struct hf_client_call_ind_param {
        esp_hf_call_status_t status;             /*!< call status indicator */
    } call;                                      /*!< HF callback param of ESP_HF_CLIENT_CIND_CALL_EVT */

    /**
     * @brief ESP_HF_CLIENT_CIND_CALL_SETUP_EVT
     */
    struct hf_client_call_setup_ind_param {
        esp_hf_call_setup_status_t status;       /*!< call setup status indicator */
    } call_setup;                                /*!< HF callback param of ESP_HF_CLIENT_BVRA_EVT */

    /**
     * @brief ESP_HF_CLIENT_CIND_CALL_HELD_EVT
     */
    struct hf_client_call_held_ind_param {
        esp_hf_call_held_status_t status;        /*!< bluetooth proprietary call hold status indicator */
    } call_held;                                 /*!< HF callback param of ESP_HF_CLIENT_CIND_CALL_HELD_EVT */

    /**
     * @brief ESP_HF_CLIENT_BTRH_EVT
     */
    struct hf_client_btrh_param {
        esp_hf_btrh_status_t status;             /*!< call hold and response status result code */
    } btrh;                                      /*!< HF callback param of ESP_HF_CLIENT_BRTH_EVT */

    /**
     * @brief ESP_HF_CLIENT_CLIP_EVT
     */
    struct hf_client_clip_param {
        const char *number;                      /*!< phone number string of call */
    } clip;                                      /*!< HF callback param of ESP_HF_CLIENT_CLIP_EVT */

    /**
     * @brief ESP_HF_CLIENT_CCWA_EVT
     */
    struct hf_client_ccwa_param {
        const char *number;                      /*!< phone number string of waiting call */
    } ccwa;                                      /*!< HF callback param of ESP_HF_CLIENT_BVRA_EVT */

    /**
     * @brief ESP_HF_CLIENT_CLCC_EVT
     */
    struct hf_client_clcc_param {
        int idx;                                 /*!< numbering(starting with 1) of the call */
        esp_hf_current_call_direction_t dir;     /*!< direction of the call */
        esp_hf_current_call_status_t status;     /*!< status of the call */
        esp_hf_current_call_mpty_type_t mpty;    /*!< multi-party flag */
        char *number;                            /*!< phone number(optional) */
    } clcc;                                      /*!< HF callback param of ESP_HF_CLIENT_CLCC_EVT */

    /**
     * @brief ESP_HF_CLIENT_VOLUME_CONTROL_EVT
     */
    struct hf_client_volume_control_param {
        esp_hf_volume_control_target_t type;     /*!< volume control target, speaker or microphone */
        int volume;                              /*!< gain, ranges from 0 to 15 */
    } volume_control;                            /*!< HF callback param of ESP_HF_CLIENT_VOLUME_CONTROL_EVT */

    /**
     * @brief ESP_HF_CLIENT_AT_RESPONSE_EVT
     */
    struct hf_client_at_response_param {
        esp_hf_at_response_code_t code;          /*!< AT response code */
        esp_hf_cme_err_t cme;                    /*!< Extended Audio Gateway Error Result Code */
    } at_response;                               /*!< HF callback param of ESP_HF_CLIENT_AT_RESPONSE_EVT */

    /**
     * @brief ESP_HF_CLIENT_CNUM_EVT
     */
    struct hf_client_cnum_param {
        const char *number;                      /*!< phone number string */
        esp_hf_subscriber_service_type_t type;   /*!< service type that the phone number relates to */
    } cnum;                                      /*!< HF callback param of ESP_HF_CLIENT_CNUM_EVT */

    /**
     * @brief ESP_HF_CLIENT_BSIR_EVT
     */
    struct hf_client_bsirparam {
        esp_hf_client_in_band_ring_state_t state;  /*!< setting state of in-band ring tone */
    } bsir;                                        /*!< HF callback param of ESP_HF_CLIENT_BSIR_EVT */

    /**
     * @brief ESP_HF_CLIENT_BINP_EVT
     */
    struct hf_client_binp_param {
        const char *number;                      /*!< phone number corresponding to the last voice tag in the HF */
    } binp;                                      /*!< HF callback param of ESP_HF_CLIENT_BINP_EVT */

    /**
     * @brief ESP_HF_CLIENT_PKT_STAT_NUMS_GET_EVT
     */
    struct hf_client_pkt_status_nums {
        uint32_t rx_total;        /*!< the total number of packets received */
        uint32_t rx_correct;      /*!< the total number of packets data correctly received */
        uint32_t rx_err;          /*!< the total number of packets data with possible invalid */
        uint32_t rx_none;         /*!< the total number of packets data no received */
        uint32_t rx_lost;         /*!< the total number of packets data partially lost */
        uint32_t tx_total;        /*!< the total number of packets send */
        uint32_t tx_discarded;    /*!< the total number of packets send lost */
    } pkt_nums;                   /*!< HF callback param of ESP_HF_CLIENT_PKT_STAT_NUMS_GET_EVT */

    /**
     * @brief ESP_HF_CLIENT_PROF_STATE_EVT
     */
    struct hf_client_prof_stat_param {
        esp_hf_prof_state_t state;               /*!< hf client profile state param */
    } prof_stat;                                 /*!< status to indicate hf client prof init or deinit */

} esp_hf_client_cb_param_t;                      /*!< HFP client callback parameters */

/**
 * @brief           HFP client incoming data callback function, the callback is useful in case of
 *                  Voice Over HCI.
 * @param[in]       buf : pointer to incoming data(payload of HCI synchronous data packet), the
 *                  buffer is allocated inside bluetooth protocol stack and will be released after
 *                  invoke of the callback is finished.
 * @param[in]       len : size(in bytes) in buf
 */
typedef void (* esp_hf_client_incoming_data_cb_t)(const uint8_t *buf, uint32_t len);

/**
 * @brief           HFP client outgoing data callback function, the callback is useful in case of
 *                  Voice Over HCI. Once audio connection is set up and the application layer has
 *                  prepared data to send, the lower layer will call this function to read data
 *                  and then send. This callback is supposed to be implemented as non-blocking,
 *                  and if data is not enough, return value 0 is supposed.
 *
 * @param[in]       buf : pointer to incoming data(payload of HCI synchronous data packet), the
 *                  buffer is allocated inside bluetooth protocol stack and will be released after
 *                  invoke of the callback is finished.
 *
 * @param[in]       len : size(in bytes) in buf
 *
 * @return          length of data successfully read
 *
 */
typedef uint32_t (* esp_hf_client_outgoing_data_cb_t)(uint8_t *buf, uint32_t len);

/**
 * @brief           HFP client callback function type
 *
 * @param           event : Event type
 *
 * @param           param : Pointer to callback parameter
 */
typedef void (* esp_hf_client_cb_t)(esp_hf_client_cb_event_t event, esp_hf_client_cb_param_t *param);

/**
 * @brief           Register application callback function to HFP client module.
 *                  This function should be called only after esp_bluedroid_enable() completes successfully.
 *
 * @param[in]       callback: HFP client event callback function
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: if callback is a NULL function pointer
 *
 */
esp_err_t esp_hf_client_register_callback(esp_hf_client_cb_t callback);

/**
 *
 * @brief           Initialize the bluetooth HFP client module.
 *                  This function should be called after esp_bluedroid_enable() completes successfully.
 *                  ESP_HF_CLIENT_PROF_STATE_EVT with ESP_HF_INIT_SUCCESS will reported to the APP layer.
 *
 * @return
 *                  - ESP_OK: if the initialization request is sent successfully
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_client_init(void);

/**
 *
 * @brief           De-initialize for HFP client module.
 *                  This function should be called only after esp_bluedroid_enable() completes successfully.
 *                  ESP_HF_CLIENT_PROF_STATE_EVT with ESP_HF_DEINIT_SUCCESS will reported to the APP layer.
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_client_deinit(void);

/**
 *
 * @brief           Establish a Service Level Connection to remote bluetooth HFP audio gateway(AG) device.
 *                  This function must be called after esp_hf_client_init() and before esp_hf_client_deinit().
 *
 * @param[in]       remote_bda: remote bluetooth device address
 *
 * @return
 *                  - ESP_OK: connect request is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_client_connect(esp_bd_addr_t remote_bda);

/**
 *
 * @brief           Disconnect from the remote HFP audio gateway.
 *                  This function must be called after esp_hf_client_init() and before esp_hf_client_deinit().
 *
 * @param[in]       remote_bda: remote bluetooth device address
 *
 * @return
 *                  - ESP_OK: disconnect request is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_client_disconnect(esp_bd_addr_t remote_bda);

/**
 *
 * @brief           Create audio connection with remote HFP AG.
 *                  As a precondition to use this API, Service Level Connection shall exist with AG.
 *
 * @param[in]       remote_bda: remote bluetooth device address
 * @return
 *                  - ESP_OK: connect audio request is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_client_connect_audio(esp_bd_addr_t remote_bda);

/**
 *
 * @brief           Release the established audio connection with remote HFP AG.
 *                  As a precondition to use this API, Service Level Connection shall exist with AG.
 *
 * @param[in]       remote_bda: remote bluetooth device address
 * @return
 *                  - ESP_OK: disconnect audio request is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_client_disconnect_audio(esp_bd_addr_t remote_bda);

/**
 *
 * @brief           Enable voice recognition in the AG.
 *                  As a precondition to use this API, Service Level Connection shall exist with AG.
 *
 * @return
 *                  - ESP_OK: starting voice recognition is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_client_start_voice_recognition(void);

/**
 *
 * @brief           Disable voice recognition in the AG.
 *                  As a precondition to use this API, Service Level Connection shall exist with AG.
 *
 * @return
 *                  - ESP_OK: stopping voice recognition is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_client_stop_voice_recognition(void);

/**
 *
 * @brief           Volume synchronization with AG.
 *                  As a precondition to use this API, Service Level Connection shall exist with AG.
 *
 * @param[in]       type: volume control target, speaker or microphone
 * @param[in]       volume: gain of the speaker of microphone, ranges 0 to 15
 *
 * @return
 *                  - ESP_OK: volume update is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_client_volume_update(esp_hf_volume_control_target_t type, int volume);

/**
 *
 * @brief           Place a call with a specified number, if number is NULL, last called number is called.
 *                  As a precondition to use this API, Service Level Connection shall exist with AG.
 *
 * @param[in]       number: number string of the call. If NULL, the last number is called(aka re-dial)
 *
 * @return
 *                  - ESP_OK: a call placing is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_client_dial(const char *number);

/**
 *
 * @brief           Place a call with number specified by location(speed dial).
 *                  As a precondition to use this API, Service Level Connection shall exist with AG.
 *
 * @param[in]       location: location of the number in the memory
 *
 * @return
 *                  - ESP_OK: a memory call placing is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */

esp_err_t esp_hf_client_dial_memory(int location);

/**
 *
 * @brief           Send call hold and multiparty commands, or enhanced call control commands(Use AT+CHLD).
 *                  As a precondition to use this API, Service Level Connection shall exist with AG.
 *
 * @param[in]       chld: AT+CHLD call hold and multiparty handling AT command.
 * @param[in]       idx: used in Enhanced Call Control Mechanisms, used if chld is
 *                       ESP_HF_CHLD_TYPE_REL_X or ESP_HF_CHLD_TYPE_PRIV_X
 *
 * @return
 *                  - ESP_OK: command AT+CHLD is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_client_send_chld_cmd(esp_hf_chld_type_t chld, int idx);

/**
 *
 * @brief           Send response and hold action command(Send AT+BTRH command)
 *                  As a precondition to use this API, Service Level Connection shall exist with AG.
 *
 * @param[in]       btrh: response and hold action to send
 *
 * @return
 *                  - ESP_OK: command AT+BTRH is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_client_send_btrh_cmd(esp_hf_btrh_cmd_t btrh);

/**
 *
 * @brief           Answer an incoming call(send ATA command).
 *                  As a precondition to use this API, Service Level Connection shall exist with AG.
 *
 * @return
 *                  - ESP_OK: a call answering is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_client_answer_call(void);

/**
 *
 * @brief           Reject an incoming call or terminate an ongoing call(send AT+CHUP command).
 *                  As a precondition to use this API, Service Level Connection shall exist with AG.
 *
 * @return
 *                  - ESP_OK: the call rejecting is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_client_reject_call(void);

/**
 *
 * @brief           Query list of current calls in AG(send AT+CLCC command).
 *                  As a precondition to use this API, Service Level Connection shall exist with AG.
 *
 * @return
 *                  - ESP_OK: query of current calls is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_client_query_current_calls(void);

/**
 *
 * @brief           Query the name of currently selected network operator in AG(use AT+COPS commands).
 *                  As a precondition to use this API, Service Level Connection shall exist with AG.
 *
 * @return
 *                  - ESP_OK: query of current operator name is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_client_query_current_operator_name(void);

/**
 *
 * @brief           Get subscriber information number from AG(send AT+CNUM command)
 *                  As a precondition to use this API, Service Level Connection shall exist with AG
 *
 * @return
 *                  - ESP_OK: the retrieving of subscriber information is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_client_retrieve_subscriber_info(void);

/**
 *
 * @brief           Transmit DTMF codes during an ongoing call(use AT+VTS commands)
 *                  As a precondition to use this API, Service Level Connection shall exist with AG.
 *
 * @param[in]       code: dtmf code, single ascii character in the set 0-9, #, *, A-D
 *
 * @return
 *                  - ESP_OK: the DTMF codes are sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_client_send_dtmf(char code);

/**
 *
 * @brief           Send command to enable Vendor specific feature to indicate battery level
 *                  and docker status
 *                  This is Apple-specific commands, but used by most device, including Android and Windows
 *
 * @param[in]       information: XAPL vendorID-productID-version, such as "0505-1995-0610"
 *                               vendorID: A string representation of the hex value of the vendor ID from the manufacturer, without the 0x prefix.
 *                               productID: A string representation of the hex value of the product ID from the manufacturer, without the 0x prefix.
 *                               version: The revision of the software
 * @param[in]       features: A base-10 representation of a bit field. such as ESP_HF_CLIENT_XAPL_FEAT_BATTERY_REPORT
 *
 * @return
 *                  - ESP_OK: Feature enable request is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_client_send_xapl(char *information, uint32_t features);

/**
 *
 * @brief           Send Battery level and docker status
 *                  Enable this feature using XAPL command first
 *                  This is Apple-specific commands, but used by most device, including Android and Windows
 *
 *
 * @param[in]       bat_level: Battery Level: value between 0 and 9
 * @param[in]       docked: Dock State: false = undocked, true = docked
 *
 * @return
 *                  - ESP_OK: battery level is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_client_send_iphoneaccev(uint32_t bat_level, bool docked);

/**
 *
 * @brief           Request a phone number from AG corresponding to last voice tag recorded (send AT+BINP command).
 *                  As a precondition to use this API, Service Level Connection shall exist with AG.
 *
 * @return
 *                  - ESP_OK: the phone number request corresponding to last voice tag recorded is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_client_request_last_voice_tag_number(void);

/**
 *
 * @brief           Disable echo cancellation and noise reduction in the AG (use AT+NREC=0 command).
 *                  As a precondition to use this API, Service Level Connection shall exist with AG
 *
 * @return
 *                  - ESP_OK: NREC=0 request is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_client_send_nrec(void);


/**
 * @brief           Register HFP client data output function; the callback is only used in
 *                  the case that Voice Over HCI is enabled.
 *
 * @param[in]       recv: HFP client incoming data callback function
 *
 * @param[in]       send: HFP client outgoing data callback function
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: if callback is a NULL function pointer
 *
 */
esp_err_t esp_hf_client_register_data_callback(esp_hf_client_incoming_data_cb_t recv,
                                               esp_hf_client_outgoing_data_cb_t send);

/**
 *
 * @brief           Get the number of packets received and sent
 *                  This function is only used in the case that Voice Over HCI is enabled and the audio state is connected.
 *                  When the operation is completed, the callback function will be called with ESP_HF_CLIENT_PKT_STAT_NUMS_GET_EVT.
 *
 * @param[in]       sync_conn_handle: the (e)SCO connection handle
 *
 * @return
 *                  - ESP_OK: if the request is sent successfully
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_client_pkt_stat_nums_get(uint16_t sync_conn_handle);

/**
 * @brief           Trigger the lower-layer to fetch and send audio data.
 *                  This function is only only used in the case that Voice Over HCI is enabled. After this
 *                  function is called, lower layer will invoke esp_hf_client_outgoing_data_cb_t to fetch data.
 *
 *                  As a precondition to use this API, Service Level Connection shall exist with AG.
 *
 */
void esp_hf_client_outgoing_data_ready(void);


/**
 * @brief           Initialize the down sampling converter. This is a utility function that can
 *                  only be used in the case that Voice Over HCI is enabled.
 *
 * @param[in]       src_sps: original samples per second(source audio data, i.e. 48000, 32000,
 *                  16000, 44100, 22050, 11025)
 * @param[in]       bits: number of bits per pcm sample (16)
 *
 * @param[in]       channels: number of channels (i.e. mono(1), stereo(2)...)
 */
void esp_hf_client_pcm_resample_init(uint32_t src_sps, uint32_t bits, uint32_t channels);

/**
 * @brief           Deinitialize the down sampling converter.
 */
void esp_hf_client_pcm_resample_deinit(void);

/**
 * @brief           Down sampling utility to convert high sampling rate into 8K/16bits 1-channel mode PCM
 *                  samples. This can only be used in the case that Voice Over HCI is enabled.
 *
 * @param[in]       src: pointer to the buffer where the original sampling PCM are stored
 *
 * @param[in]       in_bytes: length of the input PCM sample buffer in byte
 *
 * @param[in]       dst: pointer to the buffer which is to be used to store the converted PCM samples
 *
 * @return          number of samples converted
 */
int32_t esp_hf_client_pcm_resample(void *src, uint32_t in_bytes, void *dst);

/**
 * @brief       This function is used to get the status of hf client
 *
 * @param[out]  profile_status - hf client status
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_hf_client_get_profile_status(esp_hf_client_profile_status_t *profile_status);

#ifdef __cplusplus
}
#endif


#endif /* __ESP_HF_CLIENT_API_H__ */
