/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "bt_common.h"
#include "btc/btc_common.h"
#include "btc/btc_dm.h"
#include "btc_hf_ag.h"
#include "btc/btc_profile_queue.h"
#include "btc/btc_manage.h"
#include "btc/btc_util.h"
#include "bta/bta_ag_api.h"
#include "bta/bta_api.h"
#include "common/bt_target.h"
#include "common/bt_defs.h"
#include "device/bdaddr.h"
#include "esp_bt.h"
#include "esp_hf_ag_api.h"
#include "esp_err.h"
#include "esp_bt_main.h"
#include "osi/allocator.h"

#if (BTC_HF_INCLUDED == TRUE)
esp_err_t esp_hf_ag_register_callback(esp_hf_cb_t callback)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (callback == NULL) {
        return ESP_FAIL;
    }
    btc_profile_cb_set(BTC_PID_HF, callback);
    return ESP_OK;
}

esp_err_t esp_hf_ag_init(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_INIT_EVT;

    /* Switch to BTC context */
    bt_status_t status = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_deinit(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_DEINIT_EVT;

    /* Switch to BTC context */
    bt_status_t status = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_slc_connect(esp_bd_addr_t remote_addr)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_CONNECT_EVT;

    btc_hf_args_t arg;
    memset(&arg, 0, sizeof(btc_hf_args_t));
    memcpy(&(arg.connect), remote_addr, sizeof(esp_bd_addr_t));

    /* Switch to BTC context */
    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_slc_disconnect(esp_bd_addr_t remote_addr)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_DISCONNECT_EVT;

    btc_hf_args_t arg;
    memset(&arg, 0, sizeof(btc_hf_args_t));
    memcpy(&(arg.disconnect), remote_addr, sizeof(esp_bd_addr_t));

    /* Switch to BTC context */
    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_audio_connect(esp_bd_addr_t remote_addr)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_CONNECT_AUDIO_EVT;

    btc_hf_args_t arg;
    memset(&arg, 0, sizeof(btc_hf_args_t));
    memcpy(&(arg.connect_audio), remote_addr, sizeof(esp_bd_addr_t));

    /* Switch to BTC context */
    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_audio_disconnect(esp_bd_addr_t remote_addr)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_DISCONNECT_AUDIO_EVT;

    btc_hf_args_t arg;
    memset(&arg, 0, sizeof(btc_hf_args_t));
    memcpy(&(arg.disconnect_audio), remote_addr, sizeof(esp_bd_addr_t));

    /* Switch to BTC context */
    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_vra_control(esp_bd_addr_t remote_addr, esp_hf_vr_state_t value)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_VRA_EVT;

    btc_hf_args_t arg;
    memset(&arg, 0, sizeof(btc_hf_args_t));
    arg.vra_rep.value = value;
    memcpy(&(arg.volcon.remote_addr), remote_addr, sizeof(esp_bd_addr_t));

    /* Switch to BTC context */
    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_volume_control(esp_bd_addr_t remote_addr, esp_hf_volume_control_target_t type, int volume)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (volume < 0 || volume > 15) {
        return ESP_ERR_INVALID_ARG;
    }
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_VOLUME_CONTROL_EVT;

    btc_hf_args_t arg;
    memset(&arg, 0, sizeof(btc_hf_args_t));
    arg.volcon.target_type = type;
    arg.volcon.volume = volume;
    memcpy(&(arg.volcon.remote_addr), remote_addr, sizeof(esp_bd_addr_t));

    /* Switch to BTC context */
    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_unknown_at_send(esp_bd_addr_t remote_addr, char *unat)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_UNAT_RESPONSE_EVT;

    btc_hf_args_t arg;
    memset(&arg, 0, sizeof(btc_hf_args_t));
    arg.unat_rep.unat = unat;
    memcpy(&(arg.unat_rep.remote_addr), remote_addr, sizeof(esp_bd_addr_t));

    /* Switch to BTC context */
    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t),
                                                btc_hf_arg_deep_copy, btc_hf_arg_deep_free);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_cmee_send(esp_bd_addr_t remote_addr, esp_hf_at_response_code_t response_code, esp_hf_cme_err_t error_code)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_CME_ERR_EVT;

    btc_hf_args_t arg;
    memset(&arg, 0, sizeof(btc_hf_args_t));
    arg.ext_at.response_code = response_code;
    arg.ext_at.error_code = error_code;
    memcpy(&(arg.ext_at.remote_addr), remote_addr, sizeof(esp_bd_addr_t));

    /* Switch to BTC context */
    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_devices_status_indchange(esp_bd_addr_t remote_addr,
                                            esp_hf_call_status_t call_state,
                                            esp_hf_call_setup_status_t call_setup_state,
                                            esp_hf_network_state_t ntk_state, int signal)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (signal < 0 || signal > 5) {
        return ESP_ERR_INVALID_ARG;
    }
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_IND_NOTIFICATION_EVT;

    btc_hf_args_t arg;
    memset(&arg, 0, sizeof(btc_hf_args_t));
    memcpy(&(arg.ind_change.remote_addr), remote_addr, sizeof(esp_bd_addr_t));
    arg.ind_change.call_state = call_state;
    arg.ind_change.call_setup_state = call_setup_state;
    arg.ind_change.ntk_state = ntk_state;
    arg.ind_change.signal = signal;

    /* Switch to BTC context */
    bt_status_t state = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (state == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_ciev_report(esp_bd_addr_t remote_addr, esp_hf_ciev_report_type_t ind_type, int value)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_CIEV_REPORT_EVT;

    btc_hf_args_t arg;
    memset(&arg, 0, sizeof(btc_hf_args_t));
    memcpy(&(arg.ciev_rep.remote_addr), remote_addr, sizeof(esp_bd_addr_t));
    arg.ciev_rep.ind.type = ind_type;
    arg.ciev_rep.ind.value = value;

    /* Switch to BTC context */
    bt_status_t state = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (state == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_cind_response(esp_bd_addr_t remote_addr,
                                esp_hf_call_status_t call_state,
                                esp_hf_call_setup_status_t call_setup_state,
                                esp_hf_network_state_t ntk_state, int signal, esp_hf_roaming_status_t roam, int batt_lev,
                                esp_hf_call_held_status_t call_held_status)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (signal < 0 || signal > 5 || batt_lev < 0 || batt_lev > 5) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_CIND_RESPONSE_EVT;

    btc_hf_args_t arg;
    memset(&arg, 0, sizeof(btc_hf_args_t));
    memcpy(&(arg.cind_rep.remote_addr), remote_addr, sizeof(esp_bd_addr_t));
    arg.cind_rep.call_state = call_state;
    arg.cind_rep.call_setup_state = call_setup_state;
    arg.cind_rep.ntk_state = ntk_state;
    arg.cind_rep.signal = signal;
    arg.cind_rep.roam = roam;
    arg.cind_rep.batt_lev = batt_lev;
    arg.cind_rep.call_held_state = call_held_status;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_cops_response(esp_bd_addr_t remote_addr, char *name)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_COPS_RESPONSE_EVT;

    btc_hf_args_t arg;
    memset(&arg, 0, sizeof(btc_hf_args_t));
    memcpy(&(arg.cops_rep.remote_addr), remote_addr, sizeof(esp_bd_addr_t));
    arg.cops_rep.name = name; //deep_copy

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t),
                                                btc_hf_arg_deep_copy, btc_hf_arg_deep_free);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_clcc_response(esp_bd_addr_t remote_addr, int index, esp_hf_current_call_direction_t dir,
                                 esp_hf_current_call_status_t current_call_state, esp_hf_current_call_mode_t mode,
                                 esp_hf_current_call_mpty_type_t mpty, char *number, esp_hf_call_addr_type_t type)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_CLCC_RESPONSE_EVT;

    btc_hf_args_t arg;
    memset(&arg, 0, sizeof(btc_hf_args_t));
    memcpy(&(arg.clcc_rep.remote_addr), remote_addr, sizeof(esp_bd_addr_t));
    //mandatory args
    arg.clcc_rep.index = index;
    arg.clcc_rep.dir = dir;
    arg.clcc_rep.current_call_state = current_call_state;
    arg.clcc_rep.mode = mode;
    arg.clcc_rep.mpty = mpty;
    // option args
    arg.clcc_rep.number = number; //deep_copy
    arg.clcc_rep.type = type;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t),
                                                btc_hf_arg_deep_copy, btc_hf_arg_deep_free);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_cnum_response(esp_bd_addr_t remote_addr, char *number, int number_type, esp_hf_subscriber_service_type_t service_type)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (number == NULL || number_type < 128 || number_type > 175) {
        return ESP_ERR_INVALID_ARG;
    }
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_CNUM_RESPONSE_EVT;

    btc_hf_args_t arg;
    memset(&arg, 0, sizeof(btc_hf_args_t));
    memcpy(&(arg.cnum_rep), remote_addr, sizeof(esp_bd_addr_t));
    arg.cnum_rep.number = number; //deep_copy
    arg.cnum_rep.number_type = number_type;
    arg.cnum_rep.service_type = service_type;

    /* Switch to BTC context */
    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t),
                                                btc_hf_arg_deep_copy, btc_hf_arg_deep_free);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_bsir(esp_bd_addr_t remote_addr, esp_hf_in_band_ring_state_t state)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_INBAND_RING_EVT;

    btc_hf_args_t arg;
    memset(&arg, 0, sizeof(btc_hf_args_t));
    memcpy(&(arg.bsir.remote_addr), remote_addr, sizeof(esp_bd_addr_t));
    arg.bsir.state = state;

    /* Switch to BTC context */
    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_answer_call(esp_bd_addr_t remote_addr, int num_active, int num_held,
                                esp_hf_call_status_t call_state,  esp_hf_call_setup_status_t call_setup_state,
                                char *number, esp_hf_call_addr_type_t call_addr_type)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_AC_INCALL_EVT;

    btc_hf_args_t arg;
    memset(&arg, 0, sizeof(btc_hf_args_t));
    memcpy(&(arg.phone.remote_addr), remote_addr, sizeof(esp_bd_addr_t));
    arg.phone.num_active = num_active;
    arg.phone.num_held = num_held;
    arg.phone.call_state = call_state;
    arg.phone.call_setup_state = call_setup_state;
    arg.phone.number = number; //deep_copy
    arg.phone.call_addr_type = call_addr_type;

    /* Switch to BTC context */
    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t),
                                                btc_hf_arg_deep_copy, btc_hf_arg_deep_free);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_reject_call(esp_bd_addr_t remote_addr, int num_active, int num_held,
                                esp_hf_call_status_t call_state,  esp_hf_call_setup_status_t call_setup_state,
                                char *number, esp_hf_call_addr_type_t call_addr_type)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_RJ_INCALL_EVT;

    btc_hf_args_t arg;
    memset(&arg, 0, sizeof(btc_hf_args_t));
    memcpy(&(arg.phone.remote_addr), remote_addr, sizeof(esp_bd_addr_t));
    arg.phone.num_active = num_active;
    arg.phone.num_held = num_held;
    arg.phone.call_state = call_state;
    arg.phone.call_setup_state = call_setup_state;
    arg.phone.number = number; //deep_copy
    arg.phone.call_addr_type = call_addr_type;

    /* Switch to BTC context */
    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t),
                                                btc_hf_arg_deep_copy, btc_hf_arg_deep_free);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_end_call(esp_bd_addr_t remote_addr, int num_active, int num_held,
                            esp_hf_call_status_t call_state,  esp_hf_call_setup_status_t call_setup_state,
                            char *number, esp_hf_call_addr_type_t call_addr_type)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_END_CALL_EVT;

    btc_hf_args_t arg;
    memset(&arg, 0, sizeof(btc_hf_args_t));
    memcpy(&(arg.phone.remote_addr), remote_addr, sizeof(esp_bd_addr_t));
    arg.phone.num_active = num_active;
    arg.phone.num_held = num_held;
    arg.phone.call_state = call_state;
    arg.phone.call_setup_state = call_setup_state;
    arg.phone.number = number; //deep_copy
    arg.phone.call_addr_type = call_addr_type;

    /* Switch to BTC context */
    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t),
                                                btc_hf_arg_deep_copy, btc_hf_arg_deep_free);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_out_call(esp_bd_addr_t remote_addr, int num_active, int num_held,
                            esp_hf_call_status_t call_state,  esp_hf_call_setup_status_t call_setup_state,
                            char *number, esp_hf_call_addr_type_t call_addr_type)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_OUT_CALL_EVT;

    btc_hf_args_t arg;
    memset(&arg, 0, sizeof(btc_hf_args_t));
    memcpy(&(arg.phone.remote_addr), remote_addr, sizeof(esp_bd_addr_t));
    arg.phone.num_active = num_active;
    arg.phone.num_held = num_held;
    arg.phone.call_state = call_state;
    arg.phone.call_setup_state = call_setup_state;
    arg.phone.number = number; //deep_copy
    arg.phone.call_addr_type = call_addr_type;

    /* Switch to BTC context */
    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t),
                                                btc_hf_arg_deep_copy, btc_hf_arg_deep_free);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_register_data_callback(esp_hf_incoming_data_cb_t recv, esp_hf_outgoing_data_cb_t send)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_REGISTER_DATA_CALLBACK_EVT;

    btc_hf_args_t arg;
    memset(&arg, 0, sizeof(btc_hf_args_t));
    arg.reg_data_cb.recv = recv;
    arg.reg_data_cb.send = send;

    /* Switch to BTC context */
    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_get_profile_status(esp_hf_profile_status_t *profile_status)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (profile_status == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(profile_status, 0, sizeof(esp_hf_profile_status_t));
    btc_hf_get_profile_status(profile_status);

    return ESP_OK;
}

#if (BTM_SCO_HCI_INCLUDED == TRUE)
esp_err_t esp_hf_ag_pkt_stat_nums_get(uint16_t sync_conn_handle)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_REQUEST_PKT_STAT_EVT;

    btc_hf_args_t arg;
    memset(&arg, 0, sizeof(btc_hf_args_t));
    arg.pkt_sync_hd.sync_conn_handle = sync_conn_handle;

    /* Switch to BTC context */
    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

void esp_hf_ag_outgoing_data_ready(void)
{
    btc_hf_ci_sco_data();
}
#endif /* #if (BTM_SCO_HCI_INCLUDED == TRUE) */

#endif // BTC_HF_INCLUDED
