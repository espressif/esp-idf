/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
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
#if (BT_CONTROLLER_INCLUDED == TRUE)
#include "esp_bt.h"
#endif
#include "esp_hf_ag_api.h"
#include "esp_err.h"
#include "esp_bt_main.h"
#include "osi/allocator.h"

#if (BTC_HF_INCLUDED == TRUE)
esp_err_t esp_hf_ag_register_callback(esp_hf_cb_t callback)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (callback == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    int ret = btc_profile_cb_set(BTC_PID_HF, callback);
    return (ret == 0) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_init(void)
{
    esp_hf_ag_cfg_t cfg = ESP_HF_AG_INIT_CONFIG_DEFAULT();
    return esp_hf_ag_init_with_cfg(&cfg);
}

esp_err_t esp_hf_ag_init_with_cfg(const esp_hf_ag_cfg_t *cfg)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (cfg == NULL || cfg->service_mask == 0 || (cfg->service_mask & ~ESP_HF_AG_SERVICE_MASK_ALL) != 0) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_INIT_EVT;

    btc_hf_args_t arg = {0};
    arg.init_cfg.service_mask = cfg->service_mask;

    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_deinit(void)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_DEINIT_EVT;

    bt_status_t status = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_slc_connect(esp_bd_addr_t remote_addr)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (remote_addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_CONNECT_EVT;

    btc_hf_args_t arg = {0};
    memcpy(&(arg.connect), remote_addr, sizeof(esp_bd_addr_t));

    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_slc_disconnect(esp_bd_addr_t remote_addr)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (remote_addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_DISCONNECT_EVT;

    btc_hf_args_t arg = {0};
    memcpy(&(arg.disconnect), remote_addr, sizeof(esp_bd_addr_t));

    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_audio_connect(esp_bd_addr_t remote_addr)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (remote_addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_CONNECT_AUDIO_EVT;

    btc_hf_args_t arg = {0};
    memcpy(&(arg.connect_audio), remote_addr, sizeof(esp_bd_addr_t));

    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_audio_disconnect(esp_bd_addr_t remote_addr)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (remote_addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_DISCONNECT_AUDIO_EVT;

    btc_hf_args_t arg = {0};
    memcpy(&(arg.disconnect_audio), remote_addr, sizeof(esp_bd_addr_t));

    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_set_codec(esp_bd_addr_t remote_bda, esp_hf_codec_mode_t mode)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (remote_bda == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (mode > ESP_HF_CODEC_LC3) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!btc_hf_ag_is_connected((bt_bdaddr_t *)remote_bda)) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_SET_CODEC_EVT;

    btc_hf_args_t arg = {0};
    memcpy(&arg.set_codec.remote_addr, remote_bda, sizeof(esp_bd_addr_t));
    arg.set_codec.mode = mode;

    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_vra_control(esp_bd_addr_t remote_addr, esp_hf_vr_state_t value)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (remote_addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (value != ESP_HF_VR_STATE_ENABLED && value != ESP_HF_VR_STATE_DISABLED) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_VRA_EVT;

    btc_hf_args_t arg = {0};
    arg.vra_rep.value = value;
    memcpy(&(arg.vra_rep.remote_addr), remote_addr, sizeof(esp_bd_addr_t));

    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_volume_control(esp_bd_addr_t remote_addr, esp_hf_volume_control_target_t type, int volume)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (remote_addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (type != ESP_HF_VOLUME_CONTROL_TARGET_SPK && type != ESP_HF_VOLUME_CONTROL_TARGET_MIC) {
        return ESP_ERR_INVALID_ARG;
    }
    if (volume < 0 || volume > 15) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_VOLUME_CONTROL_EVT;

    btc_hf_args_t arg = {0};
    arg.volcon.target_type = type;
    arg.volcon.volume = volume;
    memcpy(&(arg.volcon.remote_addr), remote_addr, sizeof(esp_bd_addr_t));

    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_unknown_at_send(esp_bd_addr_t remote_addr, char *unat)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (remote_addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_UNAT_RESPONSE_EVT;

    btc_hf_args_t arg = {0};
    arg.unat_rep.unat = unat;
    memcpy(&(arg.unat_rep.remote_addr), remote_addr, sizeof(esp_bd_addr_t));

    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t),
                                              btc_hf_arg_deep_copy, btc_hf_arg_deep_free);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_cmee_send(esp_bd_addr_t remote_addr, esp_hf_at_response_code_t response_code, esp_hf_cme_err_t error_code)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (remote_addr == NULL ||
        response_code < ESP_HF_AT_RESPONSE_CODE_OK ||
        response_code >= ESP_HF_AT_RESPONSE_CODE_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_CME_ERR_EVT;

    btc_hf_args_t arg = {0};
    arg.ext_at.response_code = response_code;
    arg.ext_at.error_code = error_code;
    memcpy(&(arg.ext_at.remote_addr), remote_addr, sizeof(esp_bd_addr_t));

    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_ciev_report(esp_bd_addr_t remote_addr, esp_hf_ciev_report_type_t ind_type, int value)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (remote_addr == NULL || ind_type < ESP_HF_IND_TYPE_CALL || ind_type >= ESP_HF_IND_TYPE_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_CIEV_REPORT_EVT;

    btc_hf_args_t arg = {0};
    memcpy(&(arg.ciev_rep.remote_addr), remote_addr, sizeof(esp_bd_addr_t));
    arg.ciev_rep.ind.type = ind_type;
    arg.ciev_rep.ind.value = value;

    bt_status_t state = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (state == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_cind_response(esp_bd_addr_t remote_addr,
                                esp_hf_call_status_t call_state,
                                esp_hf_call_setup_status_t call_setup_state,
                                esp_hf_network_state_t ntk_state, int signal, esp_hf_roaming_status_t roam, int batt_lev,
                                esp_hf_call_held_status_t call_held_status)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (remote_addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (signal < 0 || signal > 5 || batt_lev < 0 || batt_lev > 5 ||
        call_state < ESP_HF_CALL_STATUS_NO_CALLS || call_state > ESP_HF_CALL_STATUS_CALL_IN_PROGRESS ||
        call_setup_state < ESP_HF_CALL_SETUP_STATUS_IDLE ||
        call_setup_state > ESP_HF_CALL_SETUP_STATUS_OUTGOING_ALERTING ||
        ntk_state < ESP_HF_NETWORK_STATE_NOT_AVAILABLE || ntk_state > ESP_HF_NETWORK_STATE_AVAILABLE ||
        roam < ESP_HF_ROAMING_STATUS_INACTIVE || roam > ESP_HF_ROAMING_STATUS_ACTIVE ||
        call_held_status < ESP_HF_CALL_HELD_STATUS_NONE || call_held_status > ESP_HF_CALL_HELD_STATUS_HELD) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_CIND_RESPONSE_EVT;

    btc_hf_args_t arg = {0};
    memcpy(&(arg.cind_rep.remote_addr), remote_addr, sizeof(esp_bd_addr_t));
    arg.cind_rep.call_state = call_state;
    arg.cind_rep.call_setup_state = call_setup_state;
    arg.cind_rep.ntk_state = ntk_state;
    arg.cind_rep.signal = signal;
    arg.cind_rep.roam = roam;
    arg.cind_rep.batt_lev = batt_lev;
    arg.cind_rep.call_held_state = call_held_status;

    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_cops_response(esp_bd_addr_t remote_addr, char *name)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (remote_addr == NULL || name == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_COPS_RESPONSE_EVT;

    btc_hf_args_t arg = {0};
    memcpy(&(arg.cops_rep.remote_addr), remote_addr, sizeof(esp_bd_addr_t));
    arg.cops_rep.name = name; //deep_copy

    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t),
                                            btc_hf_arg_deep_copy, btc_hf_arg_deep_free);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_clcc_response(esp_bd_addr_t remote_addr, int index, esp_hf_current_call_direction_t dir,
                                 esp_hf_current_call_status_t current_call_state, esp_hf_current_call_mode_t mode,
                                 esp_hf_current_call_mpty_type_t mpty, char *number, esp_hf_call_addr_type_t type)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (remote_addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_CLCC_RESPONSE_EVT;

    btc_hf_args_t arg = {0};
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

    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t),
                                            btc_hf_arg_deep_copy, btc_hf_arg_deep_free);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_cnum_response(esp_bd_addr_t remote_addr, char *number, int number_type, esp_hf_subscriber_service_type_t service_type)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (remote_addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (number == NULL || number_type < 128 || number_type > 175) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_CNUM_RESPONSE_EVT;

    btc_hf_args_t arg = {0};
    memcpy(&(arg.cnum_rep.remote_addr), remote_addr, sizeof(esp_bd_addr_t));
    arg.cnum_rep.number = number; //deep_copy
    arg.cnum_rep.number_type = number_type;
    arg.cnum_rep.service_type = service_type;

    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t),
                                              btc_hf_arg_deep_copy, btc_hf_arg_deep_free);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_bsir(esp_bd_addr_t remote_addr, esp_hf_in_band_ring_state_t state)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (remote_addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_INBAND_RING_EVT;

    btc_hf_args_t arg = {0};
    memcpy(&(arg.bsir.remote_addr), remote_addr, sizeof(esp_bd_addr_t));
    arg.bsir.state = state;

    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_answer_call(esp_bd_addr_t remote_addr, int num_active, int num_held,
                                esp_hf_call_status_t call_state,  esp_hf_call_setup_status_t call_setup_state,
                                char *number, esp_hf_call_addr_type_t call_addr_type)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (remote_addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_AC_INCALL_EVT;

    btc_hf_args_t arg = {0};
    memcpy(&(arg.phone.remote_addr), remote_addr, sizeof(esp_bd_addr_t));
    arg.phone.num_active = num_active;
    arg.phone.num_held = num_held;
    arg.phone.call_state = call_state;
    arg.phone.call_setup_state = call_setup_state;
    arg.phone.number = number; //deep_copy
    arg.phone.call_addr_type = call_addr_type;

    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t),
                                              btc_hf_arg_deep_copy, btc_hf_arg_deep_free);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_reject_call(esp_bd_addr_t remote_addr, int num_active, int num_held,
                                esp_hf_call_status_t call_state,  esp_hf_call_setup_status_t call_setup_state,
                                char *number, esp_hf_call_addr_type_t call_addr_type)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (remote_addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_RJ_INCALL_EVT;

    btc_hf_args_t arg = {0};
    memcpy(&(arg.phone.remote_addr), remote_addr, sizeof(esp_bd_addr_t));
    arg.phone.num_active = num_active;
    arg.phone.num_held = num_held;
    arg.phone.call_state = call_state;
    arg.phone.call_setup_state = call_setup_state;
    arg.phone.number = number; //deep_copy
    arg.phone.call_addr_type = call_addr_type;

    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t),
                                              btc_hf_arg_deep_copy, btc_hf_arg_deep_free);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_end_call(esp_bd_addr_t remote_addr, int num_active, int num_held,
                            esp_hf_call_status_t call_state,  esp_hf_call_setup_status_t call_setup_state,
                            char *number, esp_hf_call_addr_type_t call_addr_type)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (remote_addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_END_CALL_EVT;

    btc_hf_args_t arg = {0};
    memcpy(&(arg.phone.remote_addr), remote_addr, sizeof(esp_bd_addr_t));
    arg.phone.num_active = num_active;
    arg.phone.num_held = num_held;
    arg.phone.call_state = call_state;
    arg.phone.call_setup_state = call_setup_state;
    arg.phone.number = number; //deep_copy
    arg.phone.call_addr_type = call_addr_type;

    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t),
                                              btc_hf_arg_deep_copy, btc_hf_arg_deep_free);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_out_call(esp_bd_addr_t remote_addr, int num_active, int num_held,
                            esp_hf_call_status_t call_state,  esp_hf_call_setup_status_t call_setup_state,
                            char *number, esp_hf_call_addr_type_t call_addr_type)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (remote_addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_OUT_CALL_EVT;

    btc_hf_args_t arg = {0};
    memcpy(&(arg.phone.remote_addr), remote_addr, sizeof(esp_bd_addr_t));
    arg.phone.num_active = num_active;
    arg.phone.num_held = num_held;
    arg.phone.call_state = call_state;
    arg.phone.call_setup_state = call_setup_state;
    arg.phone.number = number; //deep_copy
    arg.phone.call_addr_type = call_addr_type;

    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t),
                                              btc_hf_arg_deep_copy, btc_hf_arg_deep_free);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_get_profile_status(esp_hf_profile_status_t *profile_status)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (profile_status == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(profile_status, 0, sizeof(esp_hf_profile_status_t));
    btc_hf_get_profile_status(profile_status);

    return ESP_OK;
}

#if (BTM_SCO_HCI_INCLUDED == TRUE)

esp_err_t esp_hf_ag_register_data_callback(esp_hf_incoming_data_cb_t recv, esp_hf_outgoing_data_cb_t send)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_REGISTER_DATA_CALLBACK_EVT;

    btc_hf_args_t arg = {0};
    arg.reg_data_cb.recv = recv;
    arg.reg_data_cb.send = send;

    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_ag_register_audio_data_callback(esp_hf_ag_audio_data_cb_t callback)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_REGISTER_AUDIO_DATA_CALLBACK_EVT;

    btc_hf_args_t arg = {0};
    arg.reg_audio_data_cb.callback = callback;

    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_hf_audio_buff_t *esp_hf_ag_audio_buff_alloc(uint16_t size)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return NULL;
    }

    if (size == 0) {
        return NULL;
    }

    uint8_t *p_buf = NULL, *p_data = NULL;
    BTA_AgAudioBuffAlloc(size, &p_buf, &p_data);
    if (p_buf == NULL) {
        return NULL;
    }

    esp_hf_audio_buff_t *audio_buf = (esp_hf_audio_buff_t *)p_buf;
    audio_buf->buff_size = size;
    audio_buf->data_len = 0;
    audio_buf->data = p_data;
    return audio_buf;
}

void esp_hf_ag_audio_buff_free(esp_hf_audio_buff_t *audio_buf)
{
    if (audio_buf == NULL) {
        return;
    }
    BTA_AgAudioBuffFree((UINT8 *)audio_buf);
}

esp_err_t esp_hf_ag_audio_data_send(esp_hf_sync_conn_hdl_t sync_conn_hdl, esp_hf_audio_buff_t *audio_buf)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (audio_buf == NULL || audio_buf->data_len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    if (btc_hf_ag_audio_data_send(sync_conn_hdl, (uint8_t *)audio_buf, audio_buf->data, audio_buf->data_len) == BT_STATUS_SUCCESS) {
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t esp_hf_ag_pkt_stat_nums_get(uint16_t sync_conn_handle)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF;
    msg.act = BTC_HF_REQUEST_PKT_STAT_EVT;

    btc_hf_args_t arg = {0};
    arg.pkt_sync_hd.sync_conn_handle = sync_conn_handle;

    bt_status_t status = btc_transfer_context(&msg, &arg, sizeof(btc_hf_args_t), NULL, NULL);
    return (status == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

void esp_hf_ag_outgoing_data_ready(void)
{
    btc_hf_ci_sco_data();
}
#endif /* #if (BTM_SCO_HCI_INCLUDED == TRUE) */

#endif // BTC_HF_INCLUDED
