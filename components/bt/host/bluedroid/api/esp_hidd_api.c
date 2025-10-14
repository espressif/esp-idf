/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: Blake Felt
 */

#include <string.h>
#include "esp_err.h"
#include "esp_bt_main.h"
#include "btc/btc_manage.h"
#include "btc_hd.h"
#include "esp_hidd_api.h"

#if (defined BTC_HD_INCLUDED && BTC_HD_INCLUDED == TRUE)

esp_err_t esp_bt_hid_device_register_callback(esp_hd_cb_t callback)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (callback == NULL) {
        return ESP_FAIL;
    }

    btc_profile_cb_set(BTC_PID_HD, callback);
    return ESP_OK;
}

esp_err_t esp_bt_hid_device_init(void)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HD;
    msg.act = BTC_HD_INIT_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_hid_device_deinit(void)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HD;
    msg.act = BTC_HD_DEINIT_EVT;

    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_hid_device_register_app(esp_hidd_app_param_t* app_param, esp_hidd_qos_param_t* in_qos, esp_hidd_qos_param_t* out_qos)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    btc_msg_t msg;
    btc_hidd_args_t args;
    memset(&args, 0, sizeof(btc_hidd_args_t));
    args.register_app.app_param = app_param;
    args.register_app.in_qos = in_qos;
    args.register_app.out_qos = out_qos;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HD;
    msg.act = BTC_HD_REGISTER_APP_EVT;

    bt_status_t stat = btc_transfer_context(&msg, &args, sizeof(btc_hidd_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_hid_device_unregister_app(void)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HD;
    msg.act = BTC_HD_UNREGISTER_APP_EVT;

    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_hid_device_connect(esp_bd_addr_t bd_addr)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    btc_msg_t msg;
    btc_hidd_args_t args;
    memset(&args, 0, sizeof(btc_hidd_args_t));
    memcpy(args.connect.bd_addr, bd_addr, sizeof(esp_bd_addr_t));

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HD;
    msg.act = BTC_HD_CONNECT_EVT;

    bt_status_t stat = btc_transfer_context(&msg, &args, sizeof(btc_hidd_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_hid_device_disconnect(void)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HD;
    msg.act = BTC_HD_DISCONNECT_EVT;

    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_hid_device_send_report(esp_hidd_report_type_t type, uint8_t id, uint16_t len, uint8_t* data)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HD;
    msg.act = BTC_HD_SEND_REPORT_EVT;

    btc_hidd_args_t args;
    memset(&args, 0, sizeof(btc_hidd_args_t));
    args.send_report.type = type;
    args.send_report.id = id;
    args.send_report.len = len;
    args.send_report.data = data;

    bt_status_t stat = btc_transfer_context(&msg, &args, sizeof(btc_hidd_args_t),
                                                btc_hd_arg_deep_copy, btc_hd_cb_arg_deep_free);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_hid_device_report_error(esp_hidd_handshake_error_t error)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HD;
    msg.act = BTC_HD_REPORT_ERROR_EVT;

    btc_hidd_args_t args;
    memset(&args, 0, sizeof(btc_hidd_args_t));
    args.error = error;

    bt_status_t stat = btc_transfer_context(&msg, &args, sizeof(btc_hidd_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_hid_device_virtual_cable_unplug(void)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HD;
    msg.act = BTC_HD_UNPLUG_EVT;

    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_hid_device_get_profile_status(esp_hidd_profile_status_t *profile_status)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);
    if (profile_status == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(profile_status, 0, sizeof(esp_hidd_profile_status_t));
    btc_hd_get_profile_status(profile_status);

    return ESP_OK;
}

#endif /* defined BTC_HD_INCLUDED && BTC_HD_INCLUDED == TRUE */
