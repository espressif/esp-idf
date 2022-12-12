/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: Blake Felt
 */

#include "btc/btc_manage.h"
#include "btc_hh.h"
#include "esp_bt_main.h"
#include "esp_err.h"
#include "esp_hidh_api.h"
#include <string.h>

#if (defined BTC_HH_INCLUDED && BTC_HH_INCLUDED == TRUE)

esp_err_t esp_bt_hid_host_register_callback(esp_hh_cb_t callback)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (callback == NULL) {
        return ESP_FAIL;
    }

    btc_profile_cb_set(BTC_PID_HH, callback);
    return ESP_OK;
}

esp_err_t esp_bt_hid_host_init(void)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HH;
    msg.act = BTC_HH_INIT_EVT;

    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_hid_host_deinit(void)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HH;
    msg.act = BTC_HH_DEINIT_EVT;

    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_hid_host_connect(esp_bd_addr_t bd_addr)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);
    btc_msg_t msg;
    btc_hidh_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HH;
    msg.act = BTC_HH_CONNECT_EVT;

    memcpy(arg.connect.bd_addr, bd_addr, sizeof(esp_bd_addr_t));

    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hidh_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_hid_host_disconnect(esp_bd_addr_t bd_addr)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);
    btc_msg_t msg;
    btc_hidh_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HH;
    msg.act = BTC_HH_DISCONNECT_EVT;

    memcpy(arg.disconnect.bd_addr, bd_addr, sizeof(esp_bd_addr_t));

    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hidh_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_hid_host_virtual_cable_unplug(esp_bd_addr_t bd_addr)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);
    btc_msg_t msg;
    btc_hidh_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HH;
    msg.act = BTC_HH_UNPLUG_EVT;

    memcpy(arg.unplug.bd_addr, bd_addr, sizeof(esp_bd_addr_t));

    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hidh_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_hid_host_set_info(esp_bd_addr_t bd_addr, esp_hidh_hid_info_t *hid_info)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);
    btc_msg_t msg;
    btc_hidh_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HH;
    msg.act = BTC_HH_SET_INFO_EVT;

    memcpy(arg.set_info.bd_addr, bd_addr, sizeof(esp_bd_addr_t));
    arg.set_info.hid_info = hid_info;

    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hidh_args_t),
                                                btc_hh_arg_deep_copy, btc_hh_cb_arg_deep_free);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_hid_host_get_protocol(esp_bd_addr_t bd_addr)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);
    btc_msg_t msg;
    btc_hidh_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HH;
    msg.act = BTC_HH_GET_PROTO_EVT;

    memcpy(arg.get_protocol.bd_addr, bd_addr, sizeof(esp_bd_addr_t));

    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hidh_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_hid_host_set_protocol(esp_bd_addr_t bd_addr, esp_hidh_protocol_mode_t protocol_mode)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);
    btc_msg_t msg;
    btc_hidh_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HH;
    msg.act = BTC_HH_SET_PROTO_EVT;

    memcpy(arg.set_protocol.bd_addr, bd_addr, sizeof(esp_bd_addr_t));
    arg.set_protocol.protocol_mode = protocol_mode;

    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hidh_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_hid_host_get_idle(esp_bd_addr_t bd_addr)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);
    btc_msg_t msg;
    btc_hidh_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HH;
    msg.act = BTC_HH_GET_IDLE_EVT;

    memcpy(arg.get_idle.bd_addr, bd_addr, sizeof(esp_bd_addr_t));

    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hidh_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_hid_host_set_idle(esp_bd_addr_t bd_addr, uint16_t idle_time)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);
    btc_msg_t msg;
    btc_hidh_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HH;
    msg.act = BTC_HH_SET_IDLE_EVT;

    memcpy(arg.set_idle.bd_addr, bd_addr, sizeof(esp_bd_addr_t));
    arg.set_idle.idle_time = idle_time;

    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hidh_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_hid_host_get_report(esp_bd_addr_t bd_addr, esp_hidh_report_type_t report_type, uint8_t report_id,
                                     int buffer_size)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);
    btc_msg_t msg;
    btc_hidh_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HH;
    msg.act = BTC_HH_GET_REPORT_EVT;

    memcpy(arg.get_report.bd_addr, bd_addr, sizeof(esp_bd_addr_t));
    arg.get_report.report_type = report_type;
    arg.get_report.report_id = report_id;
    arg.get_report.buffer_size = buffer_size;

    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hidh_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_hid_host_set_report(esp_bd_addr_t bd_addr, esp_hidh_report_type_t report_type, uint8_t *report,
                                     size_t len)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);
    btc_msg_t msg;
    btc_hidh_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HH;
    msg.act = BTC_HH_SET_REPORT_EVT;

    memcpy(arg.set_report.bd_addr, bd_addr, sizeof(esp_bd_addr_t));
    arg.set_report.report_type = report_type;
    arg.set_report.len = len;
    arg.set_report.report = report;

    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hidh_args_t),
                                                btc_hh_arg_deep_copy, btc_hh_cb_arg_deep_free);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_hid_host_send_data(esp_bd_addr_t bd_addr, uint8_t *data, size_t len)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);
    btc_msg_t msg;
    btc_hidh_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HH;
    msg.act = BTC_HH_SEND_DATA_EVT;

    memcpy(arg.send_data.bd_addr, bd_addr, sizeof(esp_bd_addr_t));
    arg.send_data.len = len;
    arg.send_data.data = data;

    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_hidh_args_t),
                                                btc_hh_arg_deep_copy, btc_hh_cb_arg_deep_free);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

#endif /* defined BTC_HH_INCLUDED && BTC_HH_INCLUDED == TRUE */
