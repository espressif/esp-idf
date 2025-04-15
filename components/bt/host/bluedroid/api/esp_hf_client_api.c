/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common/bt_target.h"
#include <string.h>
#include "esp_err.h"
#include "esp_hf_client_api.h"
#include "esp_bt_main.h"
#include "btc/btc_manage.h"
#include "btc_hf_client.h"
#include "bta/bta_api.h"
#include "bta/bta_hf_client_api.h"

#if BTC_HF_CLIENT_INCLUDED
esp_err_t esp_hf_client_register_callback(esp_hf_client_cb_t callback)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (callback == NULL) {
        return ESP_FAIL;
    }

    btc_profile_cb_set(BTC_PID_HF_CLIENT, callback);
    return ESP_OK;
}

esp_err_t esp_hf_client_init(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_INIT_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_client_deinit(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_DEINIT_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_client_connect(esp_bd_addr_t remote_bda)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    bt_status_t stat;
    btc_hf_client_args_t arg;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_CONNECT_EVT;

    memset(&arg, 0, sizeof(btc_hf_client_args_t));

    /* Switch to BTC context */
    memcpy(&(arg.connect), remote_bda, sizeof(bt_bdaddr_t));
    stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_client_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_client_disconnect(esp_bd_addr_t remote_bda)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    bt_status_t stat;
    btc_hf_client_args_t arg;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_DISCONNECT_EVT;

    memset(&arg, 0, sizeof(btc_hf_client_args_t));

    /* Switch to BTC context */
    memcpy(&(arg.disconnect), remote_bda, sizeof(bt_bdaddr_t));
    stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_client_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_client_connect_audio(esp_bd_addr_t remote_bda)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    bt_status_t stat;
    btc_hf_client_args_t arg;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_CONNECT_AUDIO_EVT;

    memset(&arg, 0, sizeof(btc_hf_client_args_t));

    /* Switch to BTC context */
    memcpy(&(arg.connect_audio), remote_bda, sizeof(bt_bdaddr_t));
    stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_client_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_client_disconnect_audio(esp_bd_addr_t remote_bda)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    bt_status_t stat;
    btc_hf_client_args_t arg;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_DISCONNECT_AUDIO_EVT;

    memset(&arg, 0, sizeof(btc_hf_client_args_t));

    /* Switch to BTC context */
    memcpy(&(arg.disconnect_audio), remote_bda, sizeof(bt_bdaddr_t));
    stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_client_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}


esp_err_t esp_hf_client_start_voice_recognition(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_START_VOICE_RECOGNITION_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_client_stop_voice_recognition(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_STOP_VOICE_RECOGNITION_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_client_volume_update(esp_hf_volume_control_target_t type, int volume)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    btc_hf_client_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_VOLUME_UPDATE_EVT;

    memset(&arg, 0, sizeof(btc_hf_client_args_t));
    arg.volume_update.type = type;
    arg.volume_update.volume = volume;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_client_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_client_dial(const char *number)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    btc_hf_client_args_t arg;

    if (number != NULL && strlen(number) > ESP_BT_HF_CLIENT_NUMBER_LEN) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_DIAL_EVT;

    memset(&arg, 0, sizeof(btc_hf_client_args_t));
    if (number != NULL) {
        strcpy(arg.dial.number, number);
    } else {
        arg.dial.number[0] = '\0';
    }

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_client_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_client_dial_memory(int location)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    btc_hf_client_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_DIAL_MEMORY_EVT;

    memset(&arg, 0, sizeof(btc_hf_client_args_t));
    arg.dial_memory.location = location;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_client_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_client_send_chld_cmd(esp_hf_chld_type_t chld, int idx)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    btc_hf_client_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_SEND_CHLD_CMD_EVT;

    memset(&arg, 0, sizeof(btc_hf_client_args_t));
    arg.chld.type = chld;
    arg.chld.idx = idx;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_client_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_client_send_btrh_cmd(esp_hf_btrh_cmd_t btrh)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    btc_hf_client_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_SEND_BTRH_CMD_EVT;

    memset(&arg, 0, sizeof(btc_hf_client_args_t));
    arg.btrh.cmd = btrh;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_client_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_client_answer_call(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_ANSWER_CALL_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_client_reject_call(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_REJECT_CALL_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_client_query_current_calls(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_QUERY_CURRENT_CALLS_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_client_query_current_operator_name(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_QUERY_CURRENT_OPERATOR_NAME_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_client_retrieve_subscriber_info(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_RETRIEVE_SUBSCRIBER_INFO_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_client_send_dtmf(char code)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    btc_hf_client_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_SEND_DTMF_EVT;

    memset(&arg, 0, sizeof(btc_hf_client_args_t));
    arg.send_dtmf.code = code;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_client_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_client_send_xapl(char *information, uint32_t features)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (information == NULL || strlen(information) != ESP_BT_HF_AT_SEND_XAPL_LEN) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg;
    btc_hf_client_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_SEND_XAPL_EVT;

    memset(&arg, 0, sizeof(btc_hf_client_args_t));
    strcpy(arg.send_xapl.information, information);
    arg.send_xapl.features = features;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_client_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_client_send_iphoneaccev(uint32_t bat_level, bool docked)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (bat_level > 9) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg;
    btc_hf_client_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_SEND_IPHONEACCEV_EVT;

    memset(&arg, 0, sizeof(btc_hf_client_args_t));
    arg.send_iphoneaccev.bat_level = bat_level;
    arg.send_iphoneaccev.docked = docked;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_client_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}


esp_err_t esp_hf_client_request_last_voice_tag_number(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_REQUEST_LAST_VOICE_TAG_NUMBER_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_client_send_nrec(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_SEND_NREC_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_client_register_data_callback(esp_hf_client_incoming_data_cb_t recv,
                                               esp_hf_client_outgoing_data_cb_t send)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_REGISTER_DATA_CALLBACK_EVT;

    btc_hf_client_args_t arg;
    memset(&arg, 0, sizeof(btc_hf_client_args_t));
    arg.reg_data_cb.recv = recv;
    arg.reg_data_cb.send = send;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_client_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hf_client_get_profile_status(esp_hf_client_profile_status_t *profile_status)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (profile_status == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(profile_status, 0, sizeof(esp_hf_client_profile_status_t));
    btc_hf_client_get_profile_status(profile_status);

    return ESP_OK;
}

#if (BTM_SCO_HCI_INCLUDED == TRUE)
esp_err_t esp_hf_client_pkt_stat_nums_get(uint16_t sync_conn_handle)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HF_CLIENT;
    msg.act = BTC_HF_CLIENT_REQUEST_PKT_STAT_EVT;

    btc_hf_client_args_t arg;
    memset(&arg, 0, sizeof(btc_hf_client_args_t));
    arg.pkt_sync_hd.sync_conn_handle = sync_conn_handle;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hf_client_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

void esp_hf_client_outgoing_data_ready(void)
{
    BTA_HfClientCiData();
}

void esp_hf_client_pcm_resample_init(uint32_t src_sps, uint32_t bits, uint32_t channels)
{
    BTA_DmPcmInitSamples(src_sps, bits, channels);
}

void esp_hf_client_pcm_resample_deinit(void)
{
    BTA_DmPcmDeinitSamples();
}

int32_t esp_hf_client_pcm_resample(void *src, uint32_t in_bytes, void *dst)
{
    return BTA_DmPcmResample(src, in_bytes, dst);
}

#endif /* #if (BTM_SCO_HCI_INCLUDED == TRUE) */

#endif /* BTC_HF_CLIENT_INCLUDED */
