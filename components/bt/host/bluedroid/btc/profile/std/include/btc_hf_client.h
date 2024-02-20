/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


/*******************************************************************************
 *
 *  Filename:      btc_hf_client.h
 *
 *  Description:   Main API header file for all BTC HF client functions accessed
 *                 from internal stack.
 *
 *******************************************************************************/

#ifndef __BTC_HF_CLIENT_H__
#define __BTC_HF_CLIENT_H__

#include "common/bt_target.h"
#include "esp_hf_client_api.h"
#include "btc/btc_task.h"
#include "btc/btc_common.h"
#include "bta/bta_hf_client_api.h"

#if (BTC_HF_CLIENT_INCLUDED == TRUE)
/*******************************************************************************
**  Type definitions for callback functions
********************************************************************************/
typedef enum {
    BTC_HF_CLIENT_INIT_EVT,
    BTC_HF_CLIENT_DEINIT_EVT,
    BTC_HF_CLIENT_CONNECT_EVT,
    BTC_HF_CLIENT_DISCONNECT_EVT,
    BTC_HF_CLIENT_CONNECT_AUDIO_EVT,
    BTC_HF_CLIENT_DISCONNECT_AUDIO_EVT,
    BTC_HF_CLIENT_START_VOICE_RECOGNITION_EVT,
    BTC_HF_CLIENT_STOP_VOICE_RECOGNITION_EVT,
    BTC_HF_CLIENT_VOLUME_UPDATE_EVT,
    BTC_HF_CLIENT_DIAL_EVT,
    BTC_HF_CLIENT_DIAL_MEMORY_EVT,
    BTC_HF_CLIENT_SEND_CHLD_CMD_EVT,
    BTC_HF_CLIENT_SEND_BTRH_CMD_EVT,
    BTC_HF_CLIENT_ANSWER_CALL_EVT,
    BTC_HF_CLIENT_REJECT_CALL_EVT,
    BTC_HF_CLIENT_QUERY_CURRENT_CALLS_EVT,
    BTC_HF_CLIENT_QUERY_CURRENT_OPERATOR_NAME_EVT,
    BTC_HF_CLIENT_RETRIEVE_SUBSCRIBER_INFO_EVT,
    BTC_HF_CLIENT_SEND_DTMF_EVT,
    BTC_HF_CLIENT_REQUEST_LAST_VOICE_TAG_NUMBER_EVT,
    BTC_HF_CLIENT_REGISTER_DATA_CALLBACK_EVT,
    BTC_HF_CLIENT_SEND_NREC_EVT,
    BTC_HF_CLIENT_SEND_XAPL_EVT,
    BTC_HF_CLIENT_SEND_IPHONEACCEV_EVT,
    BTC_HF_CLIENT_REQUEST_PKT_STAT_EVT,
} btc_hf_client_act_t;

/* btc_hf_client_args_t */
typedef union {
    // BTC_HF_CLIENT_CONNECT_EVT
    bt_bdaddr_t connect;

    // BTC_HF_CLIENT_DISCONNECT_EVT
    bt_bdaddr_t disconnect;

    // BTC_HF_CLIENT_CONNECT_AUDIO_EVT
    bt_bdaddr_t connect_audio;

    // BTC_HF_CLIENT_DISCONNECT_AUDIO_EVT
    bt_bdaddr_t disconnect_audio;

    // BTC_HF_CLIENT_VOLUME_UPDATE_EVT,
    struct volume_update_args {
        esp_hf_volume_control_target_t type;
        int volume;
    } volume_update;

    // BTC_HF_CLIENT_DIAL_EVT
    struct dial_args {
        char number[ESP_BT_HF_CLIENT_NUMBER_LEN + 1];
    } dial;

    // BTC_HF_CLIENT_DIAL_MEMORY_EVT
    struct dial_memory_args {
        int location;
    } dial_memory;

    // BTC_HF_CLIENT_SEND_CHLD_CMD_EVT
    struct send_chld_cmd_args {
        esp_hf_chld_type_t type;
        int idx;
    } chld;

    // BTC_HF_CLIENT_SEND_BTRH_CMD_EVT
    struct send_btrh_cmd_args {
        esp_hf_btrh_cmd_t cmd;
    } btrh;

    // BTC_HF_CLIENT_SEND_DTMF_EVT
    struct send_dtmf {
        char code;
    } send_dtmf;

    // BTC_HF_CLIENT_REGISTER_DATA_CALLBACK_EVT
    struct hf_client_reg_data_callback {
        esp_hf_client_incoming_data_cb_t recv;
        esp_hf_client_outgoing_data_cb_t send;
    } reg_data_cb;

    //BTC_HF_CLIENT_SEND_XAPL_EVT
    struct send_xapl_args {
        char information[ESP_BT_HF_AT_SEND_XAPL_LEN + 1];
        uint32_t features;
    } send_xapl;

    // BTC_HF_CLIENT_SEND_IPHONEACCEV_EVT
    struct send_iphoneaccev_args {
        uint32_t bat_level;
        bool docked;
    } send_iphoneaccev;

    // BTC_HF_CLIENT_REQUEST_PKT_STAT_EVT
    struct hf_client_req_pkt_stat_sync_handle {
        UINT16            sync_conn_handle;
    } pkt_sync_hd;

} btc_hf_client_args_t;

/************************************************************************************
**  Local type definitions
************************************************************************************/
/* BTC-HF control block to map bdaddr to BTA handle */
typedef struct
{
    bool                               initialized;
    UINT16                             handle;
    bt_bdaddr_t                        connected_bda;
    esp_hf_client_connection_state_t   state;
    esp_hf_vr_state_t                  vr_state;
    tBTA_HF_CLIENT_PEER_FEAT           peer_feat;
    tBTA_HF_CLIENT_CHLD_FEAT           chld_feat;
} btc_hf_client_cb_t;

typedef struct
{
    UINT32                              btc_hf_client_features;
    btc_hf_client_cb_t                  btc_hf_client_cb;
    esp_hf_client_incoming_data_cb_t    btc_hf_client_incoming_data_cb;
    esp_hf_client_outgoing_data_cb_t    btc_hf_client_outgoing_data_cb;
}hf_client_local_param_t;

#if HFP_DYNAMIC_MEMORY == TRUE
extern hf_client_local_param_t *hf_client_local_param_ptr;
#define hf_client_local_param (*hf_client_local_param_ptr)
#endif

/*******************************************************************************
**  BTC HF AG API
********************************************************************************/

void btc_hf_client_call_handler(btc_msg_t *msg);

void btc_hf_client_cb_handler(btc_msg_t *msg);

void btc_hf_client_incoming_data_cb_to_app(const uint8_t *data, uint32_t len);

uint32_t btc_hf_client_outgoing_data_cb_to_app(uint8_t *data, uint32_t len);
#endif  ///BTC_HF_CLIENT_INCLUDED == TRUE

#endif /* __BTC_HF_CLIENT_H__ */
