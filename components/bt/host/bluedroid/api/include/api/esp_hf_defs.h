/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_HF_DEFS_H__
#define __ESP_HF_DEFS_H__

#include "esp_bt_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/// profile states
typedef enum {
    ESP_HF_INIT_SUCCESS = 0,                /*!< Indicate init successful */
    ESP_HF_INIT_ALREADY,                    /*!< Indicate init repeated */
    ESP_HF_INIT_FAIL,                       /*!< Indicate init fail */
    ESP_HF_DEINIT_SUCCESS,                  /*!< Indicate deinit successful */
    ESP_HF_DEINIT_ALREADY,                  /*!< Indicate deinit repeated */
    ESP_HF_DEINIT_FAIL,                     /*!< Indicate deinit fail */
} esp_hf_prof_state_t;

/// in-band ring tone state
typedef enum {
    ESP_HF_IN_BAND_RINGTONE_NOT_PROVIDED = 0,
    ESP_HF_IN_BAND_RINGTONE_PROVIDED,
} esp_hf_in_band_ring_state_t;

/// voice recognition state
typedef enum {
    ESP_HF_VR_STATE_DISABLED = 0,           /*!< voice recognition disabled */
    ESP_HF_VR_STATE_ENABLED,                /*!< voice recognition enabled */
} esp_hf_vr_state_t;

/// Bluetooth HFP audio volume control target
typedef enum {
    ESP_HF_VOLUME_CONTROL_TARGET_SPK = 0,             /*!< speaker */
    ESP_HF_VOLUME_CONTROL_TARGET_MIC,                 /*!< microphone */
} esp_hf_volume_control_target_t;

/// Bluetooth HFP audio connection status
typedef enum {
    ESP_HF_AUDIO_STATE_DISCONNECTED = 0,          /*!< audio connection released */
    ESP_HF_AUDIO_STATE_CONNECTING,                /*!< audio connection has been initiated */
    ESP_HF_AUDIO_STATE_CONNECTED,                 /*!< audio connection is established */
    ESP_HF_AUDIO_STATE_CONNECTED_MSBC,            /*!< mSBC audio connection is established */
} esp_hf_audio_state_t;

typedef enum {
    ESP_HF_VOLUME_TYPE_SPK = 0,
    ESP_HF_VOLUME_TYPE_MIC
} esp_hf_volume_type_t;

/// +CIND network service availability status
typedef enum
{
    ESP_HF_NETWORK_STATE_NOT_AVAILABLE = 0,
    ESP_HF_NETWORK_STATE_AVAILABLE
} esp_hf_network_state_t;

/// +CIEV report type
typedef enum {
    ESP_HF_IND_TYPE_CALL = 1,       /*!< position of call indicator */
    ESP_HF_IND_TYPE_CALLSETUP,      /*!< position of callsetup indicator */
    ESP_HF_IND_TYPE_SERVICE,        /*!< position of service indicator */
    ESP_HF_IND_TYPE_SIGNAL,         /*!< position of signal strength indicator, range: 0-5  */
    ESP_HF_IND_TYPE_ROAM,           /*!< position of roaming indicator */
    ESP_HF_IND_TYPE_BATTCHG,        /*!< position of battery charge indicator, range: 0-5 */
    ESP_HF_IND_TYPE_CALLHELD        /*!< position of callheld indicator */
} esp_hf_ciev_report_type_t;

/** +CIEV Service type */
typedef enum
{
    ESP_HF_SERVICE_TYPE_HOME = 0,
    ESP_HF_SERVICE_TYPE_ROAMING
} esp_hf_service_type_t;

/// +CIND call status indicator values
typedef enum {
    ESP_HF_CALL_STATUS_NO_CALLS = 0,                  /*!< no call in progress */
    ESP_HF_CALL_STATUS_CALL_IN_PROGRESS = 1,          /*!< call is present(active or held) */
} esp_hf_call_status_t;

/// +CIND call setup status indicator values
typedef enum {
    ESP_HF_CALL_SETUP_STATUS_IDLE = 0,                /*!< no call setup in progress */
    ESP_HF_CALL_SETUP_STATUS_INCOMING = 1,            /*!< incoming call setup in progress */
    ESP_HF_CALL_SETUP_STATUS_OUTGOING_DIALING = 2,    /*!< outgoing call setup in dialing state */
    ESP_HF_CALL_SETUP_STATUS_OUTGOING_ALERTING = 3,   /*!< outgoing call setup in alerting state */
} esp_hf_call_setup_status_t;

/// +CIND roaming status indicator values
typedef enum {
    ESP_HF_ROAMING_STATUS_INACTIVE = 0,               /*!< roaming is not active */
    ESP_HF_ROAMING_STATUS_ACTIVE,                     /*!< a roaming is active */
} esp_hf_roaming_status_t;

/// +CIND call held indicator values
typedef enum {
    ESP_HF_CALL_HELD_STATUS_NONE = 0,                 /*!< no calls held */
    ESP_HF_CALL_HELD_STATUS_HELD_AND_ACTIVE = 1,      /*!< both active and held call */
    ESP_HF_CALL_HELD_STATUS_HELD = 2,                 /*!< call on hold, no active call*/
} esp_hf_call_held_status_t;

/// +CLCC status of the call
typedef enum {
    ESP_HF_CURRENT_CALL_STATUS_ACTIVE = 0,            /*!< active */
    ESP_HF_CURRENT_CALL_STATUS_HELD = 1,              /*!< held */
    ESP_HF_CURRENT_CALL_STATUS_DIALING = 2,           /*!< dialing (outgoing calls only) */
    ESP_HF_CURRENT_CALL_STATUS_ALERTING = 3,          /*!< alerting (outgoing calls only) */
    ESP_HF_CURRENT_CALL_STATUS_INCOMING = 4,          /*!< incoming (incoming calls only) */
    ESP_HF_CURRENT_CALL_STATUS_WAITING = 5,           /*!< waiting (incoming calls only) */
    ESP_HF_CURRENT_CALL_STATUS_HELD_BY_RESP_HOLD = 6, /*!< call held by response and hold */
} esp_hf_current_call_status_t;

/// +CLCC direction of the call
typedef enum {
    ESP_HF_CURRENT_CALL_DIRECTION_OUTGOING = 0,       /*!< outgoing */
    ESP_HF_CURRENT_CALL_DIRECTION_INCOMING = 1,       /*!< incoming */
} esp_hf_current_call_direction_t;

/// +CLCC multi-party call flag
typedef enum {
    ESP_HF_CURRENT_CALL_MPTY_TYPE_SINGLE = 0,         /*!< not a member of a multi-party call */
    ESP_HF_CURRENT_CALL_MPTY_TYPE_MULTI = 1,          /*!< member of a multi-party call */
} esp_hf_current_call_mpty_type_t;

/// +CLCC call mode
typedef enum {
    ESP_HF_CURRENT_CALL_MODE_VOICE = 0,
    ESP_HF_CURRENT_CALL_MODE_DATA = 1,
    ESP_HF_CURRENT_CALL_MODE_FAX = 2,
} esp_hf_current_call_mode_t;

/// +CLCC address type
typedef enum {
    ESP_HF_CALL_ADDR_TYPE_UNKNOWN = 0x81,            /*!< unknown address type */
    ESP_HF_CALL_ADDR_TYPE_INTERNATIONAL = 0x91,      /*!< international address */
} esp_hf_call_addr_type_t;

/// +CNUM service type of the phone number
typedef enum {
    ESP_HF_SUBSCRIBER_SERVICE_TYPE_UNKNOWN = 0,      /*!< unknown */
    ESP_HF_SUBSCRIBER_SERVICE_TYPE_VOICE = 4,        /*!< voice service */
    ESP_HF_SUBSCRIBER_SERVICE_TYPE_FAX,              /*!< fax service */
} esp_hf_subscriber_service_type_t;

/// +BTRH response and hold result code
typedef enum {
    ESP_HF_BTRH_STATUS_HELD = 0,       /*!< incoming call is put on held in AG */
    ESP_HF_BTRH_STATUS_ACCEPTED,       /*!< held incoming call is accepted in AG */
    ESP_HF_BTRH_STATUS_REJECTED,       /*!< held incoming call is rejected in AG */
} esp_hf_btrh_status_t;

/// AT+BTRH response and hold action code
typedef enum {
    ESP_HF_BTRH_CMD_HOLD = 0,          /*!< put the incoming call on hold */
    ESP_HF_BTRH_CMD_ACCEPT = 1,        /*!< accept a held incoming call */
    ESP_HF_BTRH_CMD_REJECT = 2,        /*!< reject a held incoming call */
} esp_hf_btrh_cmd_t;

/* +NREC */
typedef enum
{
    ESP_HF_NREC_STOP = 0,
    ESP_HF_NREC_START
} esp_hf_nrec_t;

///+CCWA response status
typedef enum {
    ESP_HF_CALL_WAITING_INACTIVE,
    ESP_HF_CALL_WAITING_ACTIVE,
} esp_hf_call_waiting_status_t;

/* WBS codec setting */
typedef enum
{
   ESP_HF_WBS_NONE,
   ESP_HF_WBS_NO,
   ESP_HF_WBS_YES
}esp_hf_wbs_config_t;

/// Bluetooth HFP RFCOMM connection and service level connection status
typedef enum {
    ESP_HF_CONNECTION_STATE_DISCONNECTED = 0,     /*!< RFCOMM data link channel released */
    ESP_HF_CONNECTION_STATE_CONNECTING,           /*!< connecting remote device on the RFCOMM data link*/
    ESP_HF_CONNECTION_STATE_CONNECTED,            /*!< RFCOMM connection established */
    ESP_HF_CONNECTION_STATE_SLC_CONNECTED,        /*!< service level connection established */
    ESP_HF_CONNECTION_STATE_DISCONNECTING,        /*!< disconnecting with remote device on the RFCOMM data link*/
} esp_hf_connection_state_t;

/// AT+CHLD command values
typedef enum {
    ESP_HF_CHLD_TYPE_REL = 0,               /*!< <0>, Terminate all held or set UDUB("busy") to a waiting call */
    ESP_HF_CHLD_TYPE_REL_ACC,               /*!< <1>, Terminate all active calls and accepts a waiting/held call */
    ESP_HF_CHLD_TYPE_HOLD_ACC,              /*!< <2>, Hold all active calls and accepts a waiting/held call */
    ESP_HF_CHLD_TYPE_MERGE,                 /*!< <3>, Add all held calls to a conference */
    ESP_HF_CHLD_TYPE_MERGE_DETACH,          /*!< <4>, connect the two calls and disconnects the subscriber from both calls */
    ESP_HF_CHLD_TYPE_REL_X,                 /*!< <1x>, releases specified calls only */
    ESP_HF_CHLD_TYPE_PRIV_X,                /*!< <2x>, request private consultation mode with specified call */
} esp_hf_chld_type_t;

/* AT response code - OK/Error */
typedef enum {
    ESP_HF_AT_RESPONSE_CODE_OK = 0,         /*!< acknowledges execution of a command line */
    ESP_HF_AT_RESPONSE_CODE_ERR,            /*!< command not accepted */
    ESP_HF_AT_RESPONSE_CODE_NO_CARRIER,     /*!< connection terminated */
    ESP_HF_AT_RESPONSE_CODE_BUSY,           /*!< busy signal detected */
    ESP_HF_AT_RESPONSE_CODE_NO_ANSWER,      /*!< connection completion timeout */
    ESP_HF_AT_RESPONSE_CODE_DELAYED,        /*!< delayed */
    ESP_HF_AT_RESPONSE_CODE_BLACKLISTED,    /*!< blacklisted */
    ESP_HF_AT_RESPONSE_CODE_CME,            /*!< CME error */
} esp_hf_at_response_code_t;

/* AT response code - OK/Error */
typedef enum {
    ESP_HF_AT_RESPONSE_ERROR = 0,
    ESP_HF_AT_RESPONSE_OK
} esp_hf_at_response_t;

/// Extended Audio Gateway Error Result Code Response
typedef enum {
    ESP_HF_CME_AG_FAILURE = 0,                    /*!< ag failure */
    ESP_HF_CME_NO_CONNECTION_TO_PHONE = 1,        /*!< no connection to phone */
    ESP_HF_CME_OPERATION_NOT_ALLOWED = 3,         /*!< operation not allowed */
    ESP_HF_CME_OPERATION_NOT_SUPPORTED = 4,       /*!< operation not supported */
    ESP_HF_CME_PH_SIM_PIN_REQUIRED = 5,           /*!< PH-SIM PIN Required */
    ESP_HF_CME_SIM_NOT_INSERTED = 10,             /*!< SIM not inserted */
    ESP_HF_CME_SIM_PIN_REQUIRED = 11,             /*!< SIM PIN required */
    ESP_HF_CME_SIM_PUK_REQUIRED = 12,             /*!< SIM PUK required */
    ESP_HF_CME_SIM_FAILURE = 13,                  /*!< SIM failure */
    ESP_HF_CME_SIM_BUSY = 14,                     /*!< SIM busy */
    ESP_HF_CME_INCORRECT_PASSWORD = 16,           /*!< incorrect password */
    ESP_HF_CME_SIM_PIN2_REQUIRED = 17,            /*!< SIM PIN2 required */
    ESP_HF_CME_SIM_PUK2_REQUIRED = 18,            /*!< SIM PUK2 required */
    ESP_HF_CME_MEMORY_FULL = 20,                  /*!< memory full */
    ESP_HF_CME_INVALID_INDEX = 21,                /*!< invalid index */
    ESP_HF_CME_MEMORY_FAILURE = 23,              /*!< memory failure */
    ESP_HF_CME_TEXT_STRING_TOO_LONG = 24,         /*!< test string too long */
    ESP_HF_CME_INVALID_CHARACTERS_IN_TEXT_STRING = 25,  /*!< invalid characters in text string */
    ESP_HF_CME_DIAL_STRING_TOO_LONG = 26,         /*!< dial string too long*/
    ESP_HF_CME_INVALID_CHARACTERS_IN_DIAL_STRING = 27,  /*!< invalid characters in dial string */
    ESP_HF_CME_NO_NETWORK_SERVICE = 30,           /*!< no network service */
    ESP_HF_CME_NETWORK_TIMEOUT = 31,              /*!< network timeout */
    ESP_HF_CME_NETWORK_NOT_ALLOWED = 32,          /*!< network not allowed --emergency calls only */
} esp_hf_cme_err_t;

#ifdef __cplusplus
}
#endif

#endif /* __ESP_HF_DEFS_H__ */
