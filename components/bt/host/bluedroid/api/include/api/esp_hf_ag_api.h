// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#ifndef __ESP_HF_AG_API_H__
#define __ESP_HF_AG_API_H__

#include "esp_err.h"
#include "esp_bt_defs.h"
#include "esp_hf_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* features masks of HF AG  */
#define ESP_HF_PEER_FEAT_3WAY       0x01        /* Three-way calling */
#define ESP_HF_PEER_FEAT_ECNR       0x02        /* Echo cancellation and/or noise reduction */
#define ESP_HF_PEER_FEAT_VREC       0x04        /* Voice recognition */
#define ESP_HF_PEER_FEAT_INBAND     0x08        /* In-band ring tone */
#define ESP_HF_PEER_FEAT_VTAG       0x10        /* Attach a phone number to a voice tag */
#define ESP_HF_PEER_FEAT_REJECT     0x20        /* Ability to reject incoming call */
#define ESP_HF_PEER_FEAT_ECS        0x40        /* Enhanced Call Status */
#define ESP_HF_PEER_FEAT_ECC        0x80        /* Enhanced Call Control */
#define ESP_HF_PEER_FEAT_EXTERR    0x100        /* Extended error codes */
#define ESP_HF_PEER_FEAT_CODEC     0x200        /* Codec Negotiation */

/* CHLD feature masks of HF AG */
#define ESP_HF_CHLD_FEAT_REL           0x01       /* 0  Release waiting call or held calls */
#define ESP_HF_CHLD_FEAT_REL_ACC       0x02       /* 1  Release active calls and accept other waiting or held call */
#define ESP_HF_CHLD_FEAT_REL_X         0x04       /* 1x Release specified active call only */
#define ESP_HF_CHLD_FEAT_HOLD_ACC      0x08       /* 2  Active calls on hold and accept other waiting or held call */
#define ESP_HF_CHLD_FEAT_PRIV_X        0x10       /* 2x Request private mode with specified call(put the rest on hold) */
#define ESP_HF_CHLD_FEAT_MERGE         0x20       /* 3  Add held call to multiparty */
#define ESP_HF_CHLD_FEAT_MERGE_DETACH  0x40       /* 4  Connect two calls and leave(disconnect from multiparty) */

/// HF callback events
typedef enum
{
    ESP_HF_CONNECTION_STATE_EVT = 0,          /*!< Connection state changed event */
    ESP_HF_AUDIO_STATE_EVT,                   /*!< Audio connection state change event */
    ESP_HF_BVRA_EVT,                          /*!< Voice recognition state change event */
    ESP_HF_VOLUME_CONTROL_EVT,                /*!< Audio volume control command from HF Client, provided by +VGM or +VGS message */

    ESP_HF_UNAT_RESPONSE_EVT,                 /*!< Unknown AT cmd Response*/
    ESP_HF_CIND_RESPONSE_EVT,                 /*!< Call And Device Indicator Response*/
    ESP_HF_COPS_RESPONSE_EVT,                 /*!< Current operator information */
    ESP_HF_CLCC_RESPONSE_EVT,                 /*!< List of current calls notification */
    ESP_HF_CNUM_RESPONSE_EVT,                 /*!< Subscriber information response from HF Client */
    ESP_HF_VTS_RESPONSE_EVT,                  /*!< Enable or not DTMF */
    ESP_HF_NREC_RESPONSE_EVT,                 /*!< Enable or not NREC */

    ESP_HF_ATA_RESPONSE_EVT,                  /*!< Answer an Incoming Call */
    ESP_HF_CHUP_RESPONSE_EVT,                 /*!< Reject an Incoming Call */
    ESP_HF_DIAL_EVT,                          /*!< Origin an outgoing call with specific number or the dial the last number */
    ESP_HF_BAC_RESPONSE_EVT,                  /*!< Codec Negotiation */
    ESP_HF_BCS_RESPONSE_EVT,                  /*!< Codec Negotiation */
} esp_hf_cb_event_t;

// HF callback parameters of corresponding esp event in esp_hf_cb_event_t
typedef union
{
    /**
     * @brief  ESP_HS_CONNECTION_STATE_EVT
     */
    struct hf_conn_stat_param {
        esp_bd_addr_t remote_bda;                 /*!< remote bluetooth device address */
        esp_hf_connection_state_t state;          /*!< HF connection state */
        uint32_t peer_feat;                       /*!< AG supported features */
        uint32_t chld_feat;                       /*!< AG supported features on call hold and multiparty services */
    } conn_stat;                                  /*!< AG callback param of ESP_AG_CONNECTION_STATE_EVT */

    /**
     * @brief ESP_HF_AUDIO_STATE_EVT
     */
    struct hf_audio_stat_param {
        esp_bd_addr_t remote_addr;                 /*!< remote bluetooth device address */
        esp_hf_audio_state_t state;               /*!< audio connection state */
    } audio_stat;                                 /*!< AG callback param of ESP_AG_AUDIO_STATE_EVT */

    /**
     * @brief ESP_HF_BVRA_EVT
     */
    struct hf_bvra_param {
        esp_bd_addr_t     remote_addr;
        esp_hf_vr_state_t value;                  /*!< voice recognition state */
    } vra_rep;                                       /*!< AG callback param of ESP_AG_BVRA_EVT */

    /**
     * @brief ESP_HF_VOLUME_CONTROL_EVT
     */
    struct hf_volume_control_param {
        esp_hf_volume_type_t type;                    /*!< volume control target, speaker or microphone */
        int volume;                              /*!< gain, ranges from 0 to 15 */
    } volume_control;                            /*!< AG callback param of ESP_AG_VOLUME_CONTROL_EVT */

    /**
     * @brief ESP_HF_UNAT_RESPOSNE_EVT
     */
    struct hf_unat_param {
        char *unat;
    }unat_rep;

    /**
     * @brief ESP_HF_AT_RESPONSE_EVT
     */
    struct hf_at_code_param {
        esp_hf_at_response_code_t code;          /*!< AT response code */
        esp_hf_cme_err_t cme;                    /*!< Extended Audio Gateway Error Result Code */
    } at;                               /*!< AG callback param of ESP_HF_EXT_AT_EVT */

   /**
     * @brief ESP_HF_CIND_CALL_EVT
     */
    struct hf_cind_param {
        esp_hf_call_status_t       call_status;         /*!< call status indicator */
        esp_hf_call_setup_status_t call_setup_status;   /*!< call setup status indicator */
        esp_hf_network_state_t svc;       /*!< bluetooth proprietary call hold status indicator */
        int signal_strength;                            /*!< bluetooth proprietary call hold status indicator */
        esp_hf_roaming_status_t roam;                   /*!< bluetooth proprietary call hold status indicator */
        int battery_level;                              /*!< battery charge value, ranges from 0 to 5 */
        esp_hf_call_held_status_t  call_held_status;    /*!< bluetooth proprietary call hold status indicator */
    } cind;

    /**
     * @brief ESP_HF_VTS_RESPOSNE_EVT
     */
    struct hf_vts_param {
        char *code;
    }vts_rep;

    /**
     * @brief ESP_HF_NREC_RESPOSNE_EVT
     */
    struct hf_nrec_param {
       esp_hf_nrec_t state;
    } nrec;

    struct hf_outcall_param {
        esp_bd_addr_t remote_addr;
        char *num_or_loc;
    } out_call;

    /**
     * @brief ESP_HF_BSIR_EVT
     */
    struct hf_bsir_param {
        esp_bd_addr_t remote_addr;
        esp_hf_in_band_ring_state_t state;         /*!< setting state of in-band ring tone */
    } bsir;                                        

    /**
     * @brief ESP_HF_BCS_RESPONSE_EVT
     */
    struct hf_codec_param {
        esp_hf_wbs_config_t mode;
    } codec;

} esp_hf_cb_param_t;

/**
 * @brief           AG incoming data callback function, the callback is useful in case of
 *                  Voice Over HCI.
 * @param[in]       buf : pointer to incoming data(payload of HCI synchronous data packet), the
 *                  buffer is allocated inside bluetooth protocol stack and will be released after
 *                  invoke of the callback is finished.
 * @param[in]       len : size(in bytes) in buf
 */
typedef void (* esp_hf_incoming_data_cb_t)(const uint8_t *buf, uint32_t len);

/**
 * @brief           AG outgoing data callback function, the callback is useful in case of
 *                  Voice Over HCI. Once audio connection is set up and the application layer has
 *                  prepared data to send, the lower layer will call this function to read data
 *                  and then send. This callback is supposed to be implemented as non-blocking,
 *                  and if data is not enough, return value 0 is supposed.
 *
 * @param[in]       buf : pointer to incoming data(payload of HCI synchronous data packet), the
 *                  buffer is allocated inside bluetooth protocol stack and will be released after
 *                  invoke of the callback is finished.
 * @param[in]       len : size(in bytes) in buf
 * @param[out]      length of data successfully read
 */
typedef uint32_t (* esp_hf_outgoing_data_cb_t) (uint8_t *buf, uint32_t len);

/**
 * @brief           HF AG callback function type
 *
 * @param           event : Event type
 *
 * @param           param : Pointer to callback parameter
 */
typedef void (* esp_hf_cb_t) (esp_hf_cb_event_t event, esp_hf_cb_param_t *param);

/************************************************************************************
**  ESP HF API
************************************************************************************/
/**
 * @brief           Register application callback function to HFP client module. This function should be called
 *                  only after esp_bluedroid_enable() completes successfully, used by HFP client
 *
 * @param[in]       callback: HFP AG event callback function
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: if callback is a NULL function pointer
 *
 */
esp_err_t esp_bt_hf_register_callback(esp_hf_cb_t callback);

/**
 *
 * @brief           Initialize the bluetooth HF AG module. This function should be called
 *                  after esp_bluedroid_enable() completes successfully
 *
 * @return
 *                  - ESP_OK: if the initialization request is sent successfully
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_bt_hf_init(esp_bd_addr_t remote_addr);

/**
 *
 * @brief           De-initialize for HF AG module. This function
 *                  should be called only after esp_bluedroid_enable() completes successfully
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_bt_hf_deinit(esp_bd_addr_t remote_addr);

/**
 *
 * @brief           Connect to remote bluetooth HFP client device, must after esp_bt_hf_init()
 *
 * @param[in]       remote_bda: remote bluetooth HFP client device address
 *
 * @return
 *                  - ESP_OK: connect request is sent to lower layer
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_bt_hf_connect(esp_bd_addr_t remote_bda);

/**
 *
 * @brief           Disconnect from the remote HFP client
 *
 * @param[in]       remote_bda: remote bluetooth device address
 * @return
 *                  - ESP_OK: disconnect request is sent to lower layer
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_bt_hf_disconnect(esp_bd_addr_t remote_bda);

/**
 *
 * @brief           Create audio connection with remote HFP client. As a precondition to use this API,
 *                  Service Level Connection shall exist between HF client and AG.
 *
 * @param[in]       remote_bda: remote bluetooth device address
 * @return
 *                  - ESP_OK: disconnect request is sent to lower layer
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_bt_hf_connect_audio(esp_bd_addr_t remote_bda);

/**
 *
 * @brief           Release the established audio connection with remote HFP client.
 *
 * @param[in]       remote_bda: remote bluetooth device address
 * @return
 *                  - ESP_OK: disconnect request is sent to lower layer
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_bt_hf_disconnect_audio(esp_bd_addr_t remote_bda);

/**
 *
 * @brief           Response of Volume Recognition Command(AT+VRA) from HFP client. As a precondition to use this API,
 *                  Service Level Connection shall exist with HFP client.
 *
 * @param[in]       remote: volume control target, speaker or microphone
 * @param[in]       volume: gain of the speaker of microphone, ranges 0 to 15
 *
 * @return
 *                  - ESP_OK: disconnect request is sent to lower layer
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_bt_hf_vra(esp_bd_addr_t remote_bda, esp_hf_vr_state_t value);

/**
 *
 * @brief           Volume synchronization with HFP client. As a precondition to use this API,
 *                  Service Level Connection shall exist with HFP client
 *
 * @param[in]       type: volume control target, speaker or microphone
 * @param[in]       volume: gain of the speaker of microphone, ranges 0 to 15
 *
 * @return
 *                  - ESP_OK: disconnect request is sent to lower layer
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_bt_hf_volume_control(esp_bd_addr_t remote_bda, esp_hf_volume_control_target_t type, int volume);

 /**
 *
 * @brief           Handle Unknown AT command from HFP Client.
 *                  As a precondition to use this API, Service Level Connection shall exist between AG and HF Client
 *
 * @return
 *                  - ESP_OK: disconnect request is sent to lower layer
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_unat_response(esp_bd_addr_t remote_addr, char *unat);

 /**
 *
 * @brief           Unsolicited send extend AT error code to HFP Client.
 *                  As a precondition to use this API, Service Level Connection shall exist between AG and HF Client
 *
 * @return
 *                  - ESP_OK: disconnect request is sent to lower layer
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_bt_hf_cmee_response(esp_bd_addr_t remote_bda, esp_hf_at_response_code_t response_code, esp_hf_cme_err_t error_code);

 /**
 *
 * @brief           Usolicited send device status notificationto HFP Client.
 *                  As a precondition to use this API, Service Level Connection shall exist between AG and HF Client
 *
 * @return
 *                  - ESP_OK: disconnect request is sent to lower layer
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_bt_hf_indchange_notification(esp_bd_addr_t remote_addr, esp_hf_call_status_t call_state,
                                            esp_hf_call_setup_status_t call_setup_state,
                                            esp_hf_network_state_t ntk_state, int signal);

 /**
 *
 * @brief           Response to device individual indicatiors to HFP Client.
 *                  As a precondition to use this API, Service Level Connection shall exist between AG and HF Client.
 *
 * @return
 *                  - ESP_OK: disconnect request is sent to lower layer
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_bt_hf_cind_response(esp_bd_addr_t remote_addr,
                                esp_hf_call_status_t call_state,
                                esp_hf_call_setup_status_t call_setup_state, 
                                esp_hf_network_state_t ntk_state, int signal, esp_hf_roaming_status_t roam, int batt_lev,
                                esp_hf_call_held_status_t call_held_status);

/**
 *
 * @brief           Query the name of currently selected network operator in AG,
 *                  As a precondition to use this API, Service Level Connection shall exist with HFP Client
 *
 * @return
 *                  - ESP_OK: disconnect request is sent to lower layer
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_bt_hf_cops_response(esp_bd_addr_t remote_addr, char *name);

/**
 *
 * @brief           Response to Query list of current calls from HFP Client (use AT+CLCC command), 
 *                  As a precondition to use this API, Service Level Connection shall exist between AG and HF Client
 *
 * @return
 *                  - ESP_OK: disconnect request is sent to lower layer
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_bt_hf_clcc_response(esp_bd_addr_t remote_addr, int index, esp_hf_current_call_direction_t dir,
                                 esp_hf_current_call_status_t current_call_state, esp_hf_current_call_mode_t mode,
                                 esp_hf_current_call_mpty_type_t mpty, char *number, esp_hf_call_addr_type_t type);

/**
 *
 * @brief           Get subscriber information number from HFP client(send AT+CNUM command),
 *                  As a precondition to use this API, Service Level Connection shall exist with AG
 *
 * @return
 *                  - ESP_OK: disconnect request is sent to lower layer
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_bt_hf_cnum_response(esp_bd_addr_t remote_addr, char *number, esp_hf_subscriber_service_type_t type);

/**
 *
 * @brief           Inform HF Client of Provided or not Inband Ring Tone.
 *                  As a precondition to use this API, Service Level Connection shall exist with AG
 *
 * @return
 *                  - ESP_OK: disconnect request is sent to lower layer
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_bt_hf_bsir(esp_bd_addr_t remote_addr, esp_hf_in_band_ring_state_t state);

/**
 *
 * @brief           Answer Incoming Call by AG or response to the AT+A command from Hands-Free Unit.
 *                  As a precondition to use this API, Service Level Connection shall exist with AG.
 *
 * @return
 *                  - ESP_OK: disconnect request is sent to lower layer
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_bt_hf_answer_call(esp_bd_addr_t remote_addr, int num_active, int num_held,
                                esp_hf_call_status_t call_state,  esp_hf_call_setup_status_t call_setup_state,
                                char *number, esp_hf_call_addr_type_t call_addr_type);

/**
 *
 * @brief           Reject Incoming Call by AG or response to the AT+CHUP command from Hands-Free Unit.
 *                  As a precondition to use this API, Service Level Connection shall exist with AG.
 *
 * @return
 *                  - ESP_OK: disconnect request is sent to lower layer
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_bt_hf_reject_call(esp_bd_addr_t remote_addr, int num_active, int num_held,
                                esp_hf_call_status_t call_state,  esp_hf_call_setup_status_t call_setup_state,
                                char *number, esp_hf_call_addr_type_t call_addr_type);

/**
 *
 * @brief           Reject Incoming Call by AG or response to the AT+CHUP command from Hands-Free Unit.
 *                  As a precondition to use this API, Service Level Connection shall exist with AG.
 *
 * @return
 *                  - ESP_OK: disconnect request is sent to lower layer
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_bt_hf_out_call(esp_bd_addr_t remote_addr, int num_active, int num_held,
                            esp_hf_call_status_t call_state,  esp_hf_call_setup_status_t call_setup_state,
                            char *number, esp_hf_call_addr_type_t call_addr_type);

/**
 *
 * @brief           Reject Incoming Call by AG or response to the AT+CHUP command from Hands-Free Unit.
 *                  As a precondition to use this API, Service Level Connection shall exist with AG.
 *
 * @return
 *                  - ESP_OK: disconnect request is sent to lower layer
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_bt_hf_end_call(esp_bd_addr_t remote_addr, int num_active, int num_held,
                            esp_hf_call_status_t call_state,  esp_hf_call_setup_status_t call_setup_state,
                            char *number, esp_hf_call_addr_type_t call_addr_type);

/**
 * @brief           Register AG data output function; the callback is only used in
 *                  the case that Voice Over HCI is enabled.
 *
 * @param[in]       recv: HFP client incoming data callback function
 * @param[in]       send: HFP client outgoing data callback function
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: if callback is a NULL function pointer
 *
 */
esp_err_t esp_bt_hf_register_data_callback(esp_hf_incoming_data_cb_t recv, esp_hf_outgoing_data_cb_t send);


/**
 * @brief           Trigger the lower-layer to fetch and send audio data. This function is only
 *                  only used in the case that Voice Over HCI is enabled. Precondition is that
 *                  the HFP audio connection is connected. After this function is called, lower
 *                  layer will invoke esp_hf_client_outgoing_data_cb_t to fetch data
 *
 */
void esp_hf_outgoing_data_ready(void);

#ifdef __cplusplus
}
#endif

#endif //__ESP_HF_AG_API_H__