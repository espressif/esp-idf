/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_HF_AG_API_H__
#define __ESP_HF_AG_API_H__

#include "esp_err.h"
#include "esp_bt_defs.h"
#include "esp_hf_defs.h"
#include "esp_hf_ag_legacy_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/* features masks of HF AG  */
#define ESP_HF_PEER_FEAT_3WAY       0x01        /*!< Three-way calling */
#define ESP_HF_PEER_FEAT_ECNR       0x02        /*!< Echo cancellation and/or noise reduction */
#define ESP_HF_PEER_FEAT_VREC       0x04        /*!< Voice recognition */
#define ESP_HF_PEER_FEAT_INBAND     0x08        /*!< In-band ring tone */
#define ESP_HF_PEER_FEAT_VTAG       0x10        /*!< Attach a phone number to a voice tag */
#define ESP_HF_PEER_FEAT_REJECT     0x20        /*!< Ability to reject incoming call */
#define ESP_HF_PEER_FEAT_ECS        0x40        /*!< Enhanced Call Status */
#define ESP_HF_PEER_FEAT_ECC        0x80        /*!< Enhanced Call Control */
#define ESP_HF_PEER_FEAT_EXTERR    0x100        /*!< Extended error codes */
#define ESP_HF_PEER_FEAT_CODEC     0x200        /*!< Codec Negotiation */
/* HFP 1.7+ */
#define ESP_HF_PEER_FEAT_HF_IND    0x400        /*!< HF Indicators */
#define ESP_HF_PEER_FEAT_ESCO_S4   0x800        /*!< eSCO S4 Setting Supported */


/* CHLD feature masks of HF AG */
#define ESP_HF_CHLD_FEAT_REL           0x01       /*!< 0  Release waiting call or held calls */
#define ESP_HF_CHLD_FEAT_REL_ACC       0x02       /*!< 1  Release active calls and accept other waiting or held call */
#define ESP_HF_CHLD_FEAT_REL_X         0x04       /*!< 1x Release specified active call only */
#define ESP_HF_CHLD_FEAT_HOLD_ACC      0x08       /*!< 2  Active calls on hold and accept other waiting or held call */
#define ESP_HF_CHLD_FEAT_PRIV_X        0x10       /*!< 2x Request private mode with specified call(put the rest on hold) */
#define ESP_HF_CHLD_FEAT_MERGE         0x20       /*!< 3  Add held call to multiparty */
#define ESP_HF_CHLD_FEAT_MERGE_DETACH  0x40       /*!< 4  Connect two calls and leave(disconnect from multiparty) */

/// HF callback events
typedef enum
{
    ESP_HF_CONNECTION_STATE_EVT = 0,          /*!< Connection state changed event */
    ESP_HF_AUDIO_STATE_EVT,                   /*!< Audio connection state change event */
    ESP_HF_BVRA_RESPONSE_EVT,                 /*!< Voice recognition state change event */
    ESP_HF_VOLUME_CONTROL_EVT,                /*!< Audio volume control command from HF Client, provided by +VGM or +VGS message */

    ESP_HF_UNAT_RESPONSE_EVT,                 /*!< Unknown AT cmd Response*/
    ESP_HF_IND_UPDATE_EVT,                    /*!< Indicator Update Event*/
    ESP_HF_CIND_RESPONSE_EVT,                 /*!< Call And Device Indicator Response*/
    ESP_HF_COPS_RESPONSE_EVT,                 /*!< Current operator information */
    ESP_HF_CLCC_RESPONSE_EVT,                 /*!< List of current calls notification */
    ESP_HF_CNUM_RESPONSE_EVT,                 /*!< Subscriber information response from HF Client */
    ESP_HF_VTS_RESPONSE_EVT,                  /*!< Enable or not DTMF */
    ESP_HF_NREC_RESPONSE_EVT,                 /*!< Enable or not NREC */

    ESP_HF_ATA_RESPONSE_EVT,                  /*!< Answer an Incoming Call */
    ESP_HF_CHUP_RESPONSE_EVT,                 /*!< Reject an Incoming Call */
    ESP_HF_DIAL_EVT,                          /*!< Originate an outgoing call with specific number or dial the last number */
    ESP_HF_WBS_RESPONSE_EVT,                  /*!< Codec Status */
    ESP_HF_BCS_RESPONSE_EVT,                  /*!< Final Codec Choice */
    ESP_HF_PKT_STAT_NUMS_GET_EVT,             /*!< Request number of packet different status */
    ESP_HF_PROF_STATE_EVT,                    /*!< Indicate HF init or deinit complete */
} esp_hf_cb_event_t;

/// Dial type of ESP_HF_DIAL_EVT
typedef enum
{
    ESP_HF_DIAL_NUM = 0,                      /*!< Dial with a phone number */
    ESP_HF_DIAL_VOIP,                         /*!< Dial with VoIP */
    ESP_HF_DIAL_MEM,                          /*!< Dial with a memory position */
} esp_hf_dial_type_t;


/// HFP AG profile status parameters
typedef struct {
    bool hfp_ag_inited;                        /*!< hfp ag initialization */
    uint8_t slc_conn_num;                      /*!< Number of Service Level Connections */
    uint8_t sync_conn_num;                     /*!< Number of (e)SCO Connections */
} esp_hf_profile_status_t;

/// HFP AG callback parameters
typedef union
{
    /**
     * @brief  ESP_HF_CONNECTION_STATE_EVT
     */
    struct hf_conn_stat_param {
        esp_bd_addr_t remote_bda;                 /*!< Remote Bluetooth device address */
        esp_hf_connection_state_t state;          /*!< Connection state */
        uint32_t peer_feat;                       /*!< HF supported features */
        uint32_t chld_feat;                       /*!< AG supported features on call hold and multiparty services */
    } conn_stat;                                  /*!< AG callback param of ESP_HF_CONNECTION_STATE_EVT */

    /**
     * @brief ESP_HF_AUDIO_STATE_EVT
     */
    struct hf_audio_stat_param {
        esp_bd_addr_t remote_addr;                /*!< Remote bluetooth device address */
        esp_hf_audio_state_t state;               /*!< Audio connection state */
        esp_hf_sync_conn_hdl_t sync_conn_handle;    /*!< (e)SCO connection handle */
        uint16_t preferred_frame_size;            /*!< Valid only when Voice Over HCI is enabled, recommended frame size to send */
    } audio_stat;                                 /*!< AG callback param of ESP_HF_AUDIO_STATE_EVT */

    /**
     * @brief ESP_HF_BVRA_RESPONSE_EVT
     */
    struct hf_vra_rep_param {
        esp_bd_addr_t     remote_addr;            /*!< Remote bluetooth device address */
        esp_hf_vr_state_t value;                  /*!< Voice recognition state */
    } vra_rep;                                    /*!< AG callback param of ESP_HF_BVRA_RESPONSE_EVT */

    /**
     * @brief ESP_HF_VOLUME_CONTROL_EVT
     */
    struct hf_volume_control_param {
        esp_bd_addr_t remote_addr;                /*!< Remote bluetooth device address */
        esp_hf_volume_type_t type;                /*!< Volume control target, speaker or microphone */
        int volume;                               /*!< Gain, ranges from 0 to 15 */
    } volume_control;                             /*!< AG callback param of ESP_HF_VOLUME_CONTROL_EVT */

    /**
     * @brief ESP_HF_UNAT_RESPONSE_EVT
     */
    struct hf_unat_rep_param {
        esp_bd_addr_t remote_addr;                /*!< Remote bluetooth device address */
        char *unat;                               /*!< Unknown AT command string */
    } unat_rep;                                   /*!< AG callback param of ESP_HF_UNAT_RESPONSE_EVT */

    /**
     * @brief ESP_HF_DIAL_EVT
     */
    struct hf_out_call_param {
        esp_bd_addr_t remote_addr;                /*!< remote bluetooth device address */
        esp_hf_dial_type_t type;                  /*!< dial type */
        char *num_or_loc;                         /*!< location in phone memory */
    } out_call;                                   /*!< AG callback param of ESP_HF_DIAL_EVT */

    /**
     * @brief ESP_HF_IND_UPDATE_EVT
     */
    struct hf_ind_upd_param {
        esp_bd_addr_t remote_addr;                /*!< remote bluetooth device address */
    } ind_upd;                                    /*!< AG callback param of ESP_HF_IND_UPDATE_EVT */

    /**
     * @brief ESP_HF_CIND_RESPONSE_EVT
     */
    struct hf_cind_rep_param {
        esp_bd_addr_t remote_addr;                /*!< remote bluetooth device address */
    } cind_rep;                                   /*!< AG callback param of ESP_HF_CIND_RESPONSE_EVT */

    /**
     * @brief ESP_HF_COPS_RESPONSE_EVT
     */
    struct hf_cops_rep_param {
        esp_bd_addr_t remote_addr;                /*!< remote bluetooth device address */
    } cops_rep;                                   /*!< AG callback param of ESP_HF_COPS_RESPONSE_EVT */

    /**
     * @brief ESP_HF_CLCC_RESPONSE_EVT
     */
    struct hf_clcc_rep_param {
        esp_bd_addr_t remote_addr;                /*!< remote bluetooth device address */
    } clcc_rep;                                   /*!< AG callback param of ESP_HF_CLCC_RESPONSE_EVT */

    /**
     * @brief ESP_HF_CNUM_RESPONSE_EVT
     */
    struct hf_cnum_rep_param {
        esp_bd_addr_t remote_addr;                /*!< remote bluetooth device address */
    } cnum_rep;                                   /*!< AG callback param of ESP_HF_CNUM_RESPONSE_EVT */

    /**
     * @brief ESP_HF_VTS_RESPONSE_EVT
     */
    struct hf_vts_rep_param {
        esp_bd_addr_t remote_addr;                /*!< Remote bluetooth device address */
        char *code;                               /*!< MTF code from HF Client */
    } vts_rep;                                    /*!< AG callback param of ESP_HF_VTS_RESPONSE_EVT */

    /**
     * @brief ESP_HF_NREC_RESPONSE_EVT
     */
    struct hf_nrec_param {
        esp_bd_addr_t remote_addr;                /*!< Remote bluetooth device address */
        esp_hf_nrec_t state;                      /*!< NREC enabled or disabled */
    } nrec;                                       /*!< AG callback param of ESP_HF_NREC_RESPONSE_EVT */

    /**
     * @brief ESP_HF_ATA_RESPONSE_EVT
     */
    struct hf_ata_rep_param {
        esp_bd_addr_t remote_addr;                /*!< remote bluetooth device address */
    } ata_rep;                                    /*!< AG callback param of ESP_HF_ATA_RESPONSE_EVT */

    /**
     * @brief ESP_HF_CHUP_RESPONSE_EVT
     */
    struct hf_chup_rep_param {
        esp_bd_addr_t remote_addr;                /*!< remote bluetooth device address */
    } chup_rep;                                   /*!< AG callback param of ESP_HF_CHUP_RESPONSE_EVT */

    /**
     * @brief ESP_HF_WBS_RESPONSE_EVT
     */
    struct hf_wbs_rep_param {
        esp_bd_addr_t remote_addr;                /*!< Remote bluetooth device address */
        esp_hf_wbs_config_t codec;                /*!< codec mode CVSD or mSBC */
    } wbs_rep;                                    /*!< AG callback param of ESP_HF_WBS_RESPONSE_EVT */

    /**
     * @brief ESP_HF_BCS_RESPONSE_EVT
     */
    struct hf_bcs_rep_param {
        esp_bd_addr_t remote_addr;                /*!< Remote bluetooth device address */
        esp_hf_wbs_config_t mode;                 /*!< codec mode CVSD or mSBC */
    } bcs_rep;                                    /*!< AG callback param of ESP_HF_BCS_RESPONSE_EVT */

    /**
     * @brief ESP_HF_PKT_STAT_NUMS_GET_EVT
     */
    struct ag_pkt_status_nums {
        uint32_t rx_total;        /*!< the total number of packets received */
        uint32_t rx_correct;      /*!< the total number of packets data correctly received */
        uint32_t rx_err;          /*!< the total number of packets data with possible invalid */
        uint32_t rx_none;         /*!< the total number of packets data not received */
        uint32_t rx_lost;         /*!< the total number of packets data partially lost */
        uint32_t tx_total;        /*!< the total number of packets sent */
        uint32_t tx_discarded;    /*!< the total number of packets sent and lost */
    } pkt_nums;                   /*!< AG callback param of ESP_HF_PKT_STAT_NUMS_GET_EVT */

    /**
     * @brief ESP_HF_PROF_STATE_EVT
     */
    struct ag_prof_stat_param {
        esp_hf_prof_state_t state;                /*!< hf profile state param */
    } prof_stat;                                  /*!< status to indicate hf prof init or deinit */

} esp_hf_cb_param_t;                              /*!< HFP AG callback param compound*/

/**
 * @brief           HF AG callback function type
 *
 * @param[in]       event : Event type
 *
 * @param[in]       param : Pointer to callback parameter
 */
typedef void (* esp_hf_cb_t) (esp_hf_cb_event_t event, esp_hf_cb_param_t *param);

/**
 * @brief           HFP AG incoming audio data callback function, user should copy audio_buf struct
 *                  to other place before return. This callback is used in case of Voice Over HCI.
 *
 * @param[in]       sync_conn_hdl: (e)SCO connection handle
 *
 * @param[in]       audio_buf: pointer to incoming data(payload of HCI synchronous data packet), user
 *                  should free audio buffer by calling esp_hf_ag_audio_buff_free
 *
 * @param[in]       is_bad_frame: whether this packet is marked as bad frame by baseband
 *
 */
typedef void (* esp_hf_ag_audio_data_cb_t)(esp_hf_sync_conn_hdl_t sync_conn_hdl, esp_hf_audio_buff_t *audio_buf, bool is_bad_frame);

/************************************************************************************
**  ESP HF API
************************************************************************************/
/**
 * @brief           Register application callback function to HFP AG module.
 *                  This function should be called only after esp_bluedroid_enable() completes successfully.
 *
 * @param[in]       callback: HFP AG event callback function
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: if callback is a NULL function pointer
 *
 */
esp_err_t esp_hf_ag_register_callback(esp_hf_cb_t callback);

/**
 *
 * @brief           Initialize the bluetooth HF AG module.
 *                  This function should be called after esp_bluedroid_enable() completes successfully.
 *                  ESP_HF_PROF_STATE_EVT with ESP_HF_INIT_SUCCESS will reported to the APP layer.
 *
 * @return
 *                  - ESP_OK: if the initialization request is sent successfully
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_ag_init(void);

/**
 *
 * @brief           De-initialize for HF AG module.
 *                  This function should be called only after esp_bluedroid_enable() completes successfully.
 *                  ESP_HF_PROF_STATE_EVT with ESP_HF_DEINIT_SUCCESS will reported to the APP layer.
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_ag_deinit(void);

/**
 *
 * @brief           To establish a Service Level Connection to remote bluetooth HFP client device.
 *                  This function must be called after esp_hf_ag_init() and before esp_hf_ag_deinit().
 *
 * @param[in]       remote_bda: remote bluetooth HFP client device address
 *
 * @return
 *                  - ESP_OK: connect request is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_ag_slc_connect(esp_bd_addr_t remote_bda);

/**
 *
 * @brief           Disconnect from the remote HFP client. This function must be called
 *                  after esp_hf_ag_init() and before esp_hf_ag_deinit().
 *
 * @param[in]       remote_bda: remote bluetooth device address
 *
 * @return
 *                  - ESP_OK: disconnect request is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_ag_slc_disconnect(esp_bd_addr_t remote_bda);

/**
 *
 * @brief           Create audio connection with remote HFP client.
 *                  As a precondition to use this API, Service Level Connection shall exist with HFP client.
 *
 * @param[in]       remote_bda: remote bluetooth device address
 *
 * @return
 *                  - ESP_OK: audio connect request is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_ag_audio_connect(esp_bd_addr_t remote_bda);

/**
 *
 * @brief           Release the established audio connection with remote HFP client.
 *                  As a precondition to use this API, Service Level Connection shall exist with HFP client.
 *
 * @param[in]       remote_bda: remote bluetooth device address
 *
 * @return
 *                  - ESP_OK: audio disconnect request is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_ag_audio_disconnect(esp_bd_addr_t remote_bda);

/**
 *
 * @brief           Response of Volume Recognition Command(AT+VRA) from HFP client.
 *                  As a precondition to use this API, Service Level Connection shall exist with HFP client.
 *
 * @param[in]       remote_bda: the device address of voice recognition initiator
 *
 * @param[in]       value: 0 - voice recognition disabled, 1- voice recognition enabled
 *
 * @return
 *                  - ESP_OK: response of volume recognition is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_ag_vra_control(esp_bd_addr_t remote_bda, esp_hf_vr_state_t value);

/**
 *
 * @brief           Volume synchronization with HFP client.
 *                  As a precondition to use this API, Service Level Connection shall exist with HFP client.
 *
 * @param[in]       remote_bda: remote bluetooth device address
 *
 * @param[in]       type: volume control target, speaker or microphone
 *
 * @param[in]       volume: gain of the speaker of microphone, ranges 0 to 15
 *
 * @return
 *                  - ESP_OK: volume synchronization control is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_ERR_INVALID_ARG: if arguments are invalid
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_ag_volume_control(esp_bd_addr_t remote_bda, esp_hf_volume_control_target_t type, int volume);

/**
 *
 * @brief           Handle Unknown AT command from HFP Client.
 *                  As a precondition to use this API, Service Level Connection shall exist with HFP client.
 *
 * @param[in]       remote_addr: remote bluetooth device address
 *
 * @param[in]       unat: User AT command response to HF Client.
 *                        It will response "ERROR" by default if unat is NULL.
 * @return
 *                  - ESP_OK: response of unknown AT command is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_ag_unknown_at_send(esp_bd_addr_t remote_addr, char *unat);

/**
 *
 * @brief           Unsolicited send extend AT error code to HFP Client.
 *                  As a precondition to use this API, Service Level Connection shall exist with HFP client.
 *
 * @param[in]       remote_bda: remote bluetooth device address
 * @param[in]       response_code: AT command response code
 * @param[in]       error_code: CME error code
 * @return
 *                  - ESP_OK: extend error code is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_ag_cmee_send(esp_bd_addr_t remote_bda, esp_hf_at_response_code_t response_code, esp_hf_cme_err_t error_code);

/**
 *
 * @brief           Send indicator report "+CIEV: <ind> <value>" to HFP Client. "CIEV" means “indicator events reporting",
 *                  and all indicator types can be sent one type at a time.
 *                  As a precondition to use this API, Service Level Connection shall exist with HFP client.
 *
 * @param[in]       remote_addr: remote bluetooth device address
 * @param[in]       ind_type: indicator type
 * @param[in]       value: indicator value
 * @return
 *                  - ESP_OK: indicator report is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_ag_ciev_report(esp_bd_addr_t remote_addr, esp_hf_ciev_report_type_t ind_type, int value);

/**
 *
 * @brief           Response to device individual indicators to HFP Client.
 *                  As a precondition to use this API, Service Level Connection shall exist with HFP client.
 *
 * @param[in]       remote_addr: remote bluetooth device address
 * @param[in]       call_state: call state
 * @param[in]       call_setup_state: call setup state
 * @param[in]       ntk_state: network service state
 * @param[in]       signal: signal strength from 0 to 5
 * @param[in]       roam: roam state
 * @param[in]       batt_lev: battery level from 0 to 5
 * @param[in]       call_held_status: call held status
 * @return
 *                  - ESP_OK: response to device individual indicators is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_ERR_INVALID_ARG: if the arguments are invalid
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_ag_cind_response(esp_bd_addr_t remote_addr,
                                esp_hf_call_status_t call_state,
                                esp_hf_call_setup_status_t call_setup_state,
                                esp_hf_network_state_t ntk_state, int signal, esp_hf_roaming_status_t roam, int batt_lev,
                                esp_hf_call_held_status_t call_held_status);

/**
 *
 * @brief           Response for AT+COPS command from HF Client.
 *                  As a precondition to use this API, Service Level Connection shall exist with HFP client.
 *
 * @param[in]       remote_addr: remote bluetooth device address
 * @param[in]       name: current operator name
 * @return
 *                  - ESP_OK: response for AT+COPS command is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_ag_cops_response(esp_bd_addr_t remote_addr, char *name);

/**
 *
 * @brief           Response to AT+CLCC command from HFP Client.
 *                  As a precondition to use this API, Service Level Connection shall exist with HFP client.
 *
 * @param[in]       remote_addr: remote bluetooth device address
 * @param[in]       index: the index of current call, starting with 1, finishing response with 0 (send OK)
 * @param[in]       dir: call direction (incoming/outgoing)
 * @param[in]       current_call_state: current call state
 * @param[in]       mode: current call mode (voice/data/fax)
 * @param[in]       mpty: single or multi type
 * @param[in]       number: current call number
 * @param[in]       type: international type or unknown
 * @return
 *                  - ESP_OK: response to AT+CLCC command is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_ag_clcc_response(esp_bd_addr_t remote_addr, int index, esp_hf_current_call_direction_t dir,
                                 esp_hf_current_call_status_t current_call_state, esp_hf_current_call_mode_t mode,
                                 esp_hf_current_call_mpty_type_t mpty, char *number, esp_hf_call_addr_type_t type);

/**
 *
 * @brief           Response for AT+CNUM command from HF Client.
 *                  As a precondition to use this API, Service Level Connection shall exist with HFP client.
 *
 * @param[in]       remote_addr: remote bluetooth device address
 * @param[in]       number: registration number
 * @param[in]       number_type: value of number type from
 *                               128-143: national or international, may contain prefix and/or escape digits
 *                               144-159: international, includes country code prefix, add "+" if needed
 *                               160-175: national, but no prefix nor escape digits
 * @param[in]       service_type: service type (unknown/voice/fax)
 * @return
 *                  - ESP_OK: response for AT+CNUM command is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_ag_cnum_response(esp_bd_addr_t remote_addr, char *number, int number_type, esp_hf_subscriber_service_type_t service_type);

/**
 *
 * @brief           Inform HF Client that AG Provided in-band ring tone or not.
 *                  As a precondition to use this API, Service Level Connection shall exist with HFP client.
 *
 * @param[in]       remote_addr: remote bluetooth device address
 * @param[in]       state: in-band ring tone state
 * @return
 *                  - ESP_OK: information of in-band ring tone is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_ERR_INVALID_ARG: if arguments are invalid
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_ag_bsir(esp_bd_addr_t remote_addr, esp_hf_in_band_ring_state_t state);

/**
 *
 * @brief           Answer Incoming Call from AG.
 *                  As a precondition to use this API, Service Level Connection shall exist with HFP client.
 *
 * @param[in]       remote_addr: remote bluetooth device address
 * @param[in]       num_active: the number of active call
 * @param[in]       num_held: the number of held call
 * @param[in]       call_state: call state
 * @param[in]       call_setup_state: call setup state
 * @param[in]       number: number of the incoming call
 * @param[in]       call_addr_type: call address type
 * @return
 *                  - ESP_OK: answer incoming call is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_ag_answer_call(esp_bd_addr_t remote_addr, int num_active, int num_held,
                                esp_hf_call_status_t call_state,  esp_hf_call_setup_status_t call_setup_state,
                                char *number, esp_hf_call_addr_type_t call_addr_type);

/**
 *
 * @brief           Reject Incoming Call from AG.
 *                  As a precondition to use this API, Service Level Connection shall exist with HFP client.
 *
 * @param[in]       remote_addr: remote bluetooth device address
 * @param[in]       num_active: the number of active call
 * @param[in]       num_held: the number of held call
 * @param[in]       call_state: call state
 * @param[in]       call_setup_state: call setup state
 * @param[in]       number: number of the incoming call
 * @param[in]       call_addr_type: call address type
 * @return
 *                  - ESP_OK: reject incoming call is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_ag_reject_call(esp_bd_addr_t remote_addr, int num_active, int num_held,
                                esp_hf_call_status_t call_state,  esp_hf_call_setup_status_t call_setup_state,
                                char *number, esp_hf_call_addr_type_t call_addr_type);

/**
 *
 * @brief           Initiate a call from AG.
 *                  As a precondition to use this API, Service Level Connection shall exist with HFP client.
 *                  If the AG is driven by the HF to call esp_hf_ag_out_call, it needs to response an OK or ERROR
 *                  to HF. But if the AG is actively calling esp_hf_ag_out_call, it does not need to take a response
 *                  to HF.
 *
 * @param[in]       remote_addr: remote bluetooth device address
 * @param[in]       num_active: the number of active call
 * @param[in]       num_held: the number of held call
 * @param[in]       call_state: call state
 * @param[in]       call_setup_state: call setup state
 * @param[in]       number: number of the outgoing call
 * @param[in]       call_addr_type: call address type
 * @return
 *                  - ESP_OK: a call initiation is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_ag_out_call(esp_bd_addr_t remote_addr, int num_active, int num_held,
                            esp_hf_call_status_t call_state,  esp_hf_call_setup_status_t call_setup_state,
                            char *number, esp_hf_call_addr_type_t call_addr_type);

/**
 *
 * @brief           End an ongoing call.
 *                  As a precondition to use this API, Service Level Connection shall exist with HFP client.
 *
 * @param[in]       remote_addr: remote bluetooth device address
 * @param[in]       num_active: the number of active call
 * @param[in]       num_held: the number of held call
 * @param[in]       call_state: call state
 * @param[in]       call_setup_state: call setup state
 * @param[in]       number: number of the call
 * @param[in]       call_addr_type: call address type
 * @return
 *                  - ESP_OK: end an ongoing call is sent to lower layer
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_ag_end_call(esp_bd_addr_t remote_addr, int num_active, int num_held,
                            esp_hf_call_status_t call_state,  esp_hf_call_setup_status_t call_setup_state,
                            char *number, esp_hf_call_addr_type_t call_addr_type);

/**
 * @brief           Register HFP AG audio data output function; the callback is only used in
 *                  the case that Voice Over HCI is enabled.
 *
 * @param[in]       callback: HFP AG incoming audio data callback function
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_ag_register_audio_data_callback(esp_hf_ag_audio_data_cb_t callback);

/**
 * @brief           Allocate a audio buffer to store and send audio data. This function is only
 *                  used in the case that Voice Over HCI is enabled.
 *
 * @param[in]       size: buffer size to allocate
 *
 * @return          allocated audio buffer, if Bluedroid is not enabled, no memory, or size is
 *                  zeros, will return NULL
 *
 */
esp_hf_audio_buff_t *esp_hf_ag_audio_buff_alloc(uint16_t size);

/**
 * @brief           Free a audio buffer allocated by esp_hf_ag_audio_buff_alloc. This function
 *                  is only used in the case that Voice Over HCI is enabled.
 *
 * @param[in]       audio_buf: audio buffer to free
 *
 */
void esp_hf_ag_audio_buff_free(esp_hf_audio_buff_t *audio_buf);

/**
 * @brief           Send audio data, the audio buffer should by allocated by esp_hf_ag_audio_buff_alloc.
 *                  If the length of the audio data is equal to preferred_frame_size indicated by
 *                  ESP_HF_AUDIO_STATE_EVT, then we can reduce one memory copy inside the Bluedroid stack.
 *                  This function is only used in the case that Voice Over HCI is enabled.
 *
 * @param[in]       sync_conn_hdl: (e)SCO connection handle
 *
 * @param[in]       audio_buf: audio buffer that audio data stored
 *
 * @return
 *                  - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_ERR_INVALID_ARG: invalid parameter
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_ag_audio_data_send(esp_hf_sync_conn_hdl_t sync_conn_hdl, esp_hf_audio_buff_t *audio_buf);

/**
 *
 * @brief           Get the number of packets received and sent
 *
 *                  This function is only used in the case that Voice Over HCI is enabled and the audio state is connected.
 *                  When the operation is completed, the callback function will be called with ESP_HF_PKT_STAT_NUMS_GET_EVT.
 *
 * @param[in]       sync_conn_handle: the (e)SCO connection handle
 *
 * @return
 *                  - ESP_OK: if the request is sent successfully
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_hf_ag_pkt_stat_nums_get(uint16_t sync_conn_handle);

/**
 * @brief       This function is used to get the status of hfp ag
 *
 * @param[out]  profile_status - hfp ag status
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_hf_ag_get_profile_status(esp_hf_profile_status_t *profile_status);

#ifdef __cplusplus
}
#endif

#endif //__ESP_HF_AG_API_H__
