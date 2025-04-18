/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 *
 *  Filename:      btc_hf_ag.h
 *
 *  Description:   Main API header file for all BTC HF AG functions accessed
 *                 from internal stack.
 *
 *******************************************************************************/

#ifndef __BTC_HF_AG_H__
#define __BTC_HF_AG_H__

#include "common/bt_target.h"
#include "btc/btc_task.h"
#include "btc/btc_common.h"
#include "bta/bta_ag_api.h"
#include "esp_hf_ag_api.h"


#if (BTC_HF_INCLUDED == TRUE)

/*******************************************************************************
**  Type Defs
********************************************************************************/
/* btc_hf_act_t */
typedef enum
{
    //INIT
    BTC_HF_INIT_EVT,
    BTC_HF_DEINIT_EVT,
    BTC_HF_CONNECT_EVT,
    BTC_HF_DISCONNECT_EVT,
    BTC_HF_CONNECT_AUDIO_EVT,
    BTC_HF_DISCONNECT_AUDIO_EVT,
    BTC_HF_VRA_EVT,
    BTC_HF_VOLUME_CONTROL_EVT,
    //AT_RESPONSE
    BTC_HF_UNAT_RESPONSE_EVT,
    BTC_HF_CME_ERR_EVT,
    BTC_HF_IND_NOTIFICATION_EVT,
    BTC_HF_CIEV_REPORT_EVT,
    BTC_HF_CIND_RESPONSE_EVT,
    BTC_HF_COPS_RESPONSE_EVT,
    BTC_HF_CLCC_RESPONSE_EVT,
    BTC_HF_CNUM_RESPONSE_EVT,
    BTC_HF_INBAND_RING_EVT,
    //CALL_HANDLE
    BTC_HF_AC_INCALL_EVT,
    BTC_HF_RJ_INCALL_EVT,
    BTC_HF_OUT_CALL_EVT,
    BTC_HF_END_CALL_EVT,
    //REG
    BTC_HF_REGISTER_DATA_CALLBACK_EVT,
    BTC_HF_REQUEST_PKT_STAT_EVT
} btc_hf_act_t;

/* btc_hf_args_t */
typedef union
{
    // BTC_HF_CONNECT_EVT
    bt_bdaddr_t connect;

    // BTC_HF_DISCONNECT_EVT
    bt_bdaddr_t disconnect;

    // BTC_HF_CONNECT_AUDIO_EVT
    bt_bdaddr_t connect_audio;

    // BTC_HF_DISCONNECT_AUDIO_EVT
    bt_bdaddr_t disconnect_audio;

    //BTC_HF_VRA_EVT
    struct vra_param {
        bt_bdaddr_t       remote_addr;
        esp_hf_vr_state_t value;
    } vra_rep;

    // BTC_HF_VOLUME_CONTROL_EVT
    struct volcon_args {
        bt_bdaddr_t                     remote_addr;
        esp_hf_volume_control_target_t  target_type;
        int                             volume;
    } volcon;

    //BTC_HF_UNAT_RESPONSE_EVT
    struct unat_param {
        bt_bdaddr_t       remote_addr;
        char              *unat;
    } unat_rep;

    //BTC_HF_CME_ERR_EVT
    struct at_ok_err_args {
        bt_bdaddr_t               remote_addr;
        esp_hf_at_response_code_t response_code;
        esp_hf_cme_err_t          error_code;
    } ext_at;

    // BTC_HF_IND_NOTIFICATION_EVT
    struct indchange_status {
        bt_bdaddr_t                remote_addr;
        esp_hf_call_status_t       call_state;
        esp_hf_call_setup_status_t call_setup_state;
        esp_hf_network_state_t     ntk_state;
        int                        signal;
    } ind_change;

    //BTC_HF_CIEV_REPORT_EVT
    struct ciev_args {
        bt_bdaddr_t                remote_addr;
        tBTA_AG_IND                ind;
    } ciev_rep;

    //BTC_HF_CIND_RESPONSE_EVT
    struct cind_args {
        bt_bdaddr_t                              remote_addr;
        esp_hf_call_status_t                     call_state;
        esp_hf_call_setup_status_t               call_setup_state;
        esp_hf_network_state_t                   ntk_state;
        int                                      signal;
        esp_hf_roaming_status_t                  roam;
        int                                      batt_lev;
        esp_hf_call_held_status_t                call_held_state;
    } cind_rep;

    //BTC_HF_COPS_RESPONSE_EVT
    struct cops_args {
        bt_bdaddr_t remote_addr;
        char        *name;
    } cops_rep;

    // BTC_HF_CLCC_RESPONSE_EVT
    struct clcc_args {
        bt_bdaddr_t                     remote_addr;
        int                             index;
        esp_hf_current_call_direction_t dir;
        esp_hf_current_call_status_t    current_call_state;
        esp_hf_current_call_mode_t      mode;
        esp_hf_current_call_mpty_type_t mpty;
        char                            *number;
        esp_hf_call_addr_type_t         type;
    } clcc_rep;

    // BTC_HF_CNUM_RESPONSE_EVT
    struct cnum_args {
        bt_bdaddr_t                      remote_addr;
        char                             *number;
        int                              number_type;
        esp_hf_subscriber_service_type_t service_type;
    } cnum_rep;

    //BTC_HF_NREC_RESPONSE_EVT
    bt_bdaddr_t nrec_rep;

    //BTC_HF_VTC_RESPONSE_EVT
    struct bts_args {
        bt_bdaddr_t remote_addr;
        char        *code;
    } vts_rep;

    //BTC_HF_INBAND_RING_EVT
    struct bsir_args {
        bt_bdaddr_t remote_addr;
        esp_hf_in_band_ring_state_t state;
    } bsir;

    // BTC_HF_AC_INCALL_EVT
    // BTC_HF_RJ_INCALL_EVT
    // BTC_HF_OUT_CALL_EVT
    // BTC_HF_END_CALL_EVT
    struct phone_args {
        bt_bdaddr_t                remote_addr;
        int                        num_active;
        int                        num_held;
        esp_hf_call_status_t       call_state;
        esp_hf_call_setup_status_t call_setup_state;
        char                       *number;
        esp_hf_call_addr_type_t    call_addr_type;
    } phone;

    // BTC_HF_REGISTER_DATA_CALLBACK_EVT
    struct ag_reg_data_callback {
        esp_hf_incoming_data_cb_t recv;
        esp_hf_outgoing_data_cb_t send;
    } reg_data_cb;

    // BTC_HF_REQUEST_PKT_STAT_EVT
    struct ag_req_pkt_stat_sync_handle {
        UINT16            sync_conn_handle;
    } pkt_sync_hd;

} btc_hf_args_t;

/************************************************************************************
**  Local definitions
************************************************************************************/
/* Number of BTC-HF-AG control blocks */
#define BTC_HF_NUM_CB    1

/* Handsfree AG app ids for service registration */
/* APP ID definition*/
#define BTC_HF_ID_1    0

/* BTC-AG control block to map bdaddr to BTA handle */
typedef struct
{
    bool                               initialized;
    UINT16                             handle;
    UINT16                             sync_conn_hdl;
    bt_bdaddr_t                        connected_bda;
    tBTA_AG_PEER_FEAT                  peer_feat;
    tBTA_AG_CHLD_FEAT                  chld_feat;
    struct timespec                    call_end_timestamp;
    struct timespec                    connected_timestamp;
    esp_hf_connection_state_t          connection_state;
    esp_hf_vr_state_t                  vr_state;
    int                                num_active;
    int                                num_held;
    esp_hf_call_status_t               call_state;
    esp_hf_call_setup_status_t         call_setup_state;
} btc_hf_cb_t;

typedef struct
{
    int                                hf_idx;
    UINT32                             btc_hf_features;
    btc_hf_cb_t                        btc_hf_cb;
    esp_hf_incoming_data_cb_t          btc_hf_incoming_data_cb;
    esp_hf_outgoing_data_cb_t          btc_hf_outgoing_data_cb;
} hf_local_param_t;

#if HFP_DYNAMIC_MEMORY == TRUE
extern hf_local_param_t *hf_local_param_ptr;
#define hf_local_param (hf_local_param_ptr)
#endif

/*******************************************************************************
**  BTC HF AG Handle Hub
********************************************************************************/
void btc_hf_call_handler(btc_msg_t *msg); // act the cmd from esp-application

void btc_hf_cb_handler(btc_msg_t *msg); //handle the event from bta

void btc_hf_incoming_data_cb_to_app(const uint8_t *data, uint32_t len);

uint32_t btc_hf_outgoing_data_cb_to_app(uint8_t *data, uint32_t len);

void btc_hf_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);

void btc_hf_arg_deep_free(btc_msg_t *msg);

bt_status_t btc_hf_ci_sco_data(void);

void btc_hf_get_profile_status(esp_hf_profile_status_t *param);

#endif  // BTC_HF_INCLUDED == TRUE

#endif /* __BTC_HF_AG_H__ */
