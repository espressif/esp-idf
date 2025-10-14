/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_bt_device.h"
#include "esp_bt_main.h"
#if (BLE_FEAT_CTE_EN == TRUE)
#include "esp_ble_cte_api.h"
#include "btc_ble_cte.h"
#include "btc/btc_manage.h"


esp_err_t esp_ble_cte_register_callback(esp_ble_cte_cb_t callback)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    return (btc_profile_cb_set(BTC_PID_BLE_CTE, callback) == 0 ? ESP_OK : ESP_FAIL);
}

esp_ble_cte_cb_t esp_ble_cte_get_callback(void)
{
    return (esp_ble_cte_cb_t) btc_profile_cb_get(BTC_PID_BLE_CTE);
}

#if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
esp_err_t esp_ble_cte_set_connectionless_trans_params(esp_ble_cte_connless_trans_params_t *cte_trans_params)
{
    btc_msg_t msg;
    btc_ble_cte_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if ((cte_trans_params == NULL) || (cte_trans_params->antenna_ids == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_BLE_CTE;
    msg.act = BTC_CTE_ACT_SET_TRANS_PARAMS;

    arg.cte_trans_params.adv_handle = cte_trans_params->adv_handle;
    arg.cte_trans_params.cte_len = cte_trans_params->cte_len;
    arg.cte_trans_params.cte_type = cte_trans_params->cte_type;
    arg.cte_trans_params.cte_count = cte_trans_params->cte_count;
    arg.cte_trans_params.switching_pattern_len = cte_trans_params->switching_pattern_len;
    arg.cte_trans_params.antenna_ids = cte_trans_params->antenna_ids;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_cte_args_t), btc_ble_cte_arg_deep_copy, btc_ble_cte_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_cte_set_connectionless_trans_enable(esp_ble_cte_trans_enable_params_t *cte_trans_enable)
{
    btc_msg_t msg;
    btc_ble_cte_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (cte_trans_enable == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_BLE_CTE;
    msg.act = BTC_CTE_ACT_SET_TRANS_ENABLE;

    arg.cte_trans_enable.adv_handle = cte_trans_enable->adv_handle;
    arg.cte_trans_enable.cte_enable = cte_trans_enable->cte_enable;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_cte_args_t), NULL , NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_cte_set_connectionless_iq_sampling_enable(esp_ble_cte_iq_sampling_params_t *iq_sampling_en)
{
    btc_msg_t msg;
    btc_ble_cte_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if ((iq_sampling_en == NULL) || (iq_sampling_en->antenna_ids == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_BLE_CTE;
    msg.act = BTC_CTE_ACT_SET_IQ_SAMPLING_EN;

    arg.cte_iq_sampling_en.sync_handle = iq_sampling_en->sync_handle;
    arg.cte_iq_sampling_en.sampling_en = iq_sampling_en->sampling_en;
    arg.cte_iq_sampling_en.slot_dur = iq_sampling_en->slot_dur;
    arg.cte_iq_sampling_en.max_sampled_ctes = iq_sampling_en->max_sampled_ctes;
    arg.cte_iq_sampling_en.switching_pattern_len = iq_sampling_en->switching_pattern_len;
    arg.cte_iq_sampling_en.antenna_ids = iq_sampling_en->antenna_ids;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_cte_args_t), btc_ble_cte_arg_deep_copy, btc_ble_cte_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}
#endif // #if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)

#if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
esp_err_t esp_ble_cte_set_connection_receive_params(esp_ble_cte_recv_params_params_t *cte_recv_params)
{
    btc_msg_t msg;
    btc_ble_cte_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if ((cte_recv_params == NULL) || (cte_recv_params->antenna_ids == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_BLE_CTE;
    msg.act = BTC_CTE_ACT_SET_CONN_CTE_RECV_PARAMS;

    arg.cte_recv_params.conn_handle = cte_recv_params->conn_handle;
    arg.cte_recv_params.sampling_en = cte_recv_params->sampling_en;
    arg.cte_recv_params.slot_dur = cte_recv_params->slot_dur;
    arg.cte_recv_params.switching_pattern_len = cte_recv_params->switching_pattern_len;
    arg.cte_recv_params.antenna_ids = cte_recv_params->antenna_ids;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_cte_args_t), btc_ble_cte_arg_deep_copy, btc_ble_cte_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_cte_set_connection_transmit_params(esp_ble_cte_conn_trans_params_t *cte_conn_trans_params)
{
    btc_msg_t msg;
    btc_ble_cte_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if ((cte_conn_trans_params == NULL) || (cte_conn_trans_params->antenna_ids == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_BLE_CTE;
    msg.act = BTC_CTE_ACT_SET_CONN_CTE_TRANS_PARAMS;

    arg.cte_conn_trans_params.conn_handle = cte_conn_trans_params->conn_handle;
    arg.cte_conn_trans_params.cte_types = cte_conn_trans_params->cte_types;
    arg.cte_conn_trans_params.switching_pattern_len = cte_conn_trans_params->switching_pattern_len;
    arg.cte_conn_trans_params.antenna_ids = cte_conn_trans_params->antenna_ids;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_cte_args_t), btc_ble_cte_arg_deep_copy, btc_ble_cte_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_cte_connection_cte_request_enable(esp_ble_cte_req_en_params_t *cte_conn_req_en)
{
    btc_msg_t msg;
    btc_ble_cte_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (cte_conn_req_en == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_BLE_CTE;
    msg.act = BTC_CTE_ACT_SET_CONN_CTE_REQUEST_EN;

    arg.cte_req_en.conn_handle = cte_conn_req_en->conn_handle;
    arg.cte_req_en.enable = cte_conn_req_en->enable;
    arg.cte_req_en.cte_req_interval = cte_conn_req_en->cte_req_interval;
    arg.cte_req_en.req_cte_len = cte_conn_req_en->req_cte_len;
    arg.cte_req_en.req_cte_Type = cte_conn_req_en->req_cte_Type;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_cte_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_cte_connection_cte_response_enable(esp_ble_cte_rsp_en_params_t *cte_conn_rsp_en)
{
    btc_msg_t msg;
    btc_ble_cte_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (cte_conn_rsp_en == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_BLE_CTE;
    msg.act = BTC_CTE_ACT_SET_CONN_CTE_RESPONSE_EN;

    arg.cte_rsp_en.conn_handle = cte_conn_rsp_en->conn_handle;
    arg.cte_rsp_en.enable = cte_conn_rsp_en->enable;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_cte_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}
#endif // #if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)

esp_err_t esp_ble_cte_read_antenna_information(void)
{
    btc_msg_t msg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_BLE_CTE;
    msg.act = BTC_CTE_ACT_READ_ANTENNA_INFOR;

    return (btc_transfer_context(&msg, NULL, 0, NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

#endif // #if (BLE_FEAT_CTE_EN == TRUE)
