/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common/bt_target.h"
#include <string.h>
#include "esp_err.h"
#include "esp_a2dp_api.h"
#include "esp_bt_main.h"
#include "btc/btc_manage.h"
#include "btc_av.h"

#if BTC_AV_INCLUDED

esp_a2d_audio_buff_t *esp_a2d_audio_buff_alloc(uint16_t size)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return NULL;
    }

    if (size == 0) {
        return NULL;
    }

    uint8_t *p_buf = NULL, *p_data;
    btc_av_audio_buff_alloc(size, &p_buf, &p_data);
    if (p_buf == NULL) {
        return NULL;
    }

    esp_a2d_audio_buff_t *audio_buf = (esp_a2d_audio_buff_t *)p_buf;
    audio_buf->buff_size = size;
    audio_buf->data_len = 0;
    audio_buf->data = p_data;
    return audio_buf;
}

void esp_a2d_audio_buff_free(esp_a2d_audio_buff_t *audio_buf)
{
    if (audio_buf == NULL) {
        return;
    }
    btc_av_audio_buff_free((uint8_t *)audio_buf);
}

#if BTC_AV_SINK_INCLUDED
esp_err_t esp_a2d_sink_init(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (g_a2dp_on_init || g_a2dp_sink_ongoing_deinit) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_SINK_API_INIT_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_a2d_sink_register_stream_endpoint(uint8_t seid, const esp_a2d_mcc_t *mcc)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (g_a2dp_on_deinit || g_a2dp_sink_ongoing_deinit) {
        return ESP_ERR_INVALID_STATE;
    }

    if (seid >= ESP_A2D_MAX_SEPS) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_SINK_API_REG_SEP_EVT;

    btc_av_args_t arg;
    memset(&arg, 0, sizeof(btc_av_args_t));
    arg.reg_sep.seid = seid;
    memcpy(&arg.reg_sep.mcc, mcc, sizeof(esp_a2d_mcc_t));

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_av_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_a2d_sink_deinit(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (g_a2dp_on_deinit || g_a2dp_sink_ongoing_deinit) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_SINK_API_DEINIT_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_a2d_sink_register_data_callback(esp_a2d_sink_data_cb_t callback)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (g_a2dp_sink_ongoing_deinit) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_SINK_API_REG_DATA_CB_EVT;

    btc_av_args_t arg;
    memset(&arg, 0, sizeof(btc_av_args_t));
    arg.data_cb = callback;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_av_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_a2d_sink_register_audio_data_callback(esp_a2d_sink_audio_data_cb_t callback)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (g_a2dp_sink_ongoing_deinit) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_SINK_API_REG_AUDIO_DATA_CB_EVT;

    btc_av_args_t arg;
    memset(&arg, 0, sizeof(btc_av_args_t));
    arg.audio_data_cb = callback;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_av_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_a2d_sink_connect(esp_bd_addr_t remote_bda)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (g_a2dp_on_deinit || g_a2dp_sink_ongoing_deinit) {
        return ESP_ERR_INVALID_STATE;
    }

    bt_status_t stat;
    btc_av_args_t arg;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_SINK_API_CONNECT_EVT;

    memset(&arg, 0, sizeof(btc_av_args_t));

    /* Switch to BTC context */
    memcpy(&(arg.connect), remote_bda, sizeof(bt_bdaddr_t));
    stat = btc_transfer_context(&msg, &arg, sizeof(btc_av_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_a2d_sink_disconnect(esp_bd_addr_t remote_bda)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (g_a2dp_on_deinit || g_a2dp_sink_ongoing_deinit) {
        return ESP_ERR_INVALID_STATE;
    }

    bt_status_t stat;
    btc_av_args_t arg;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_SINK_API_DISCONNECT_EVT;

    /* Switch to BTC context */
    memcpy(&(arg.disconn), remote_bda, sizeof(bt_bdaddr_t));
    stat = btc_transfer_context(&msg, &arg, sizeof(btc_av_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_a2d_sink_set_delay_value(uint16_t delay_value)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (g_a2dp_on_deinit || g_a2dp_sink_ongoing_deinit) {
        return ESP_ERR_INVALID_STATE;
    }

    bt_status_t stat;
    btc_av_args_t arg;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_SINK_API_SET_DELAY_VALUE_EVT;

    memset(&arg, 0, sizeof(btc_av_args_t));
    arg.delay_value = delay_value;

    /* Switch to BTC context */
    stat = btc_transfer_context(&msg, &arg, sizeof(btc_av_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_a2d_sink_get_delay_value(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (g_a2dp_on_deinit || g_a2dp_sink_ongoing_deinit) {
        return ESP_ERR_INVALID_STATE;
    }

    bt_status_t stat;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_SINK_API_GET_DELAY_VALUE_EVT;

    /* Switch to BTC context */
    stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}
#endif /* BTC_AV_SINK_INCLUDED */

esp_err_t esp_a2d_register_callback(esp_a2d_cb_t callback)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (g_a2dp_sink_ongoing_deinit || g_a2dp_source_ongoing_deinit) {
        return ESP_ERR_INVALID_STATE;
    }

    if (callback == NULL) {
        return ESP_FAIL;
    }

    btc_profile_cb_set(BTC_PID_A2DP, callback);
    return ESP_OK;
}

esp_err_t esp_a2d_media_ctrl(esp_a2d_media_ctrl_t ctrl)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (g_a2dp_on_deinit || g_a2dp_sink_ongoing_deinit || g_a2dp_source_ongoing_deinit) {
        return ESP_ERR_INVALID_STATE;
    }

    if (ctrl > ESP_A2D_MEDIA_CTRL_SUSPEND) {
        return ESP_ERR_INVALID_ARG;
    }

    bt_status_t stat;
    btc_av_args_t arg;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_API_MEDIA_CTRL_EVT;

    memset(&arg, 0, sizeof(btc_av_args_t));

    /* Switch to BTC context */
    arg.ctrl = ctrl;
    stat = btc_transfer_context(&msg, &arg, sizeof(btc_av_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_a2d_get_profile_status(esp_a2d_profile_status_t *profile_status)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (profile_status == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(profile_status, 0, sizeof(esp_a2d_profile_status_t));
    btc_a2dp_get_profile_status(profile_status);

    return ESP_OK;
}

#if BTC_AV_SRC_INCLUDED
esp_err_t esp_a2d_source_init(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (g_a2dp_on_init || g_a2dp_source_ongoing_deinit) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_SRC_API_INIT_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_a2d_source_set_pref_mcc(esp_a2d_conn_hdl_t conn_hdl, const esp_a2d_mcc_t *pref_mcc)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (g_a2dp_on_deinit || g_a2dp_source_ongoing_deinit) {
        return ESP_ERR_INVALID_STATE;
    }

    if (conn_hdl == 0 || pref_mcc == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_SRC_API_SET_PREF_MCC_EVT;

    btc_av_args_t arg;
    memset(&arg, 0, sizeof(btc_av_args_t));
    arg.set_pref_mcc.conn_hdl = conn_hdl;
    memcpy(&arg.set_pref_mcc.pref_mcc, pref_mcc, sizeof(esp_a2d_mcc_t));

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_av_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_a2d_source_register_stream_endpoint(uint8_t seid, const esp_a2d_mcc_t *mcc)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (g_a2dp_on_deinit || g_a2dp_source_ongoing_deinit) {
        return ESP_ERR_INVALID_STATE;
    }

    if (seid >= ESP_A2D_MAX_SEPS) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_SRC_API_REG_SEP_EVT;

    btc_av_args_t arg;
    memset(&arg, 0, sizeof(btc_av_args_t));
    arg.reg_sep.seid = seid;
    memcpy(&arg.reg_sep.mcc, mcc, sizeof(esp_a2d_mcc_t));

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_av_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_a2d_source_deinit(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (g_a2dp_on_deinit || g_a2dp_source_ongoing_deinit) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_SRC_API_DEINIT_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_a2d_source_connect(esp_bd_addr_t remote_bda)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (g_a2dp_on_deinit || g_a2dp_source_ongoing_deinit) {
        return ESP_ERR_INVALID_STATE;
    }

    bt_status_t stat;
    btc_av_args_t arg;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_SRC_API_CONNECT_EVT;

    memset(&arg, 0, sizeof(btc_av_args_t));

    /* Switch to BTC context */
    memcpy(&(arg.src_connect), remote_bda, sizeof(bt_bdaddr_t));
    stat = btc_transfer_context(&msg, &arg, sizeof(btc_av_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_a2d_source_disconnect(esp_bd_addr_t remote_bda)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (g_a2dp_on_deinit || g_a2dp_source_ongoing_deinit) {
        return ESP_ERR_INVALID_STATE;
    }

    bt_status_t stat;
    btc_av_args_t arg;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_SRC_API_DISCONNECT_EVT;

    memset(&arg, 0, sizeof(btc_av_args_t));

    /* Switch to BTC context */
    memcpy(&(arg.src_disconn), remote_bda, sizeof(bt_bdaddr_t));
    stat = btc_transfer_context(&msg, &arg, sizeof(btc_av_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_a2d_source_register_data_callback(esp_a2d_source_data_cb_t callback)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (g_a2dp_source_ongoing_deinit) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_A2DP;
    msg.act = BTC_AV_SRC_API_REG_DATA_CB_EVT;

    btc_av_args_t arg;
    memset(&arg, 0, sizeof(btc_av_args_t));
    arg.src_data_cb = callback;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_av_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_a2d_source_audio_data_send(esp_a2d_conn_hdl_t conn_hdl, esp_a2d_audio_buff_t *audio_buf)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED || !btc_av_is_started()) {
        return ESP_ERR_INVALID_STATE;
    }

    if (audio_buf == NULL || audio_buf->data_len == 0 || conn_hdl == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!btc_a2d_src_audio_mtu_check(audio_buf->data_len)) {
        return ESP_ERR_INVALID_SIZE;
    }

    if (btc_a2d_src_audio_data_send(conn_hdl, audio_buf) != BT_STATUS_SUCCESS) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

#endif /* BTC_AV_SRC_INCLUDED */

#endif /* #if BTC_AV_INCLUDED */
