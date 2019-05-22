// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
// Copyright 2019      Blake Felt
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>
#include "esp_err.h"
#include "esp_bt_main.h"
#include "btc/btc_manage.h"
#include "btc_hd.h"
#include "esp_hidd_api.h"

esp_err_t esp_hid_device_init(esp_hidd_callbacks_t* callbacks)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    esp_hidd_args_t args;
    memset(&args, 0, sizeof(esp_hidd_args_t));
    args.callbacks = callbacks;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HD;
    msg.act = BTC_HD_INIT_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &args, sizeof(esp_hidd_args_t), NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hid_device_cleanup(void) {
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HD;
    msg.act = BTC_HD_CLEANUP_EVT;

    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hid_device_register_app(esp_hidd_app_param_t* app_param,
                                      esp_hidd_qos_param_t* in_qos,
                                      esp_hidd_qos_param_t* out_qos) {
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    esp_hidd_args_t args;
    memset(&args, 0, sizeof(esp_hidd_args_t));
    args.register_app.app_param = app_param;
    args.register_app.in_qos = in_qos;
    args.register_app.out_qos = out_qos;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HD;
    msg.act = BTC_HD_REGISTER_EVT;

    bt_status_t stat = btc_transfer_context(&msg, &args, sizeof(esp_hidd_args_t), NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hid_device_unregister_app(void) {
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HD;
    msg.act = BTC_HD_UNREGISTER_EVT;

    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hid_device_connect(void) {
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HD;
    msg.act = BTC_HD_CONNECT_EVT;

    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hid_device_disconnect(void) {
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HD;
    msg.act = BTC_HD_DISCONNECT_EVT;

    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hid_device_send_report(esp_hidd_report_type_t type, uint8_t id, uint16_t len, uint8_t* p_data) {
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HD;
    msg.act = BTC_HD_SEND_REPORT_EVT;

    esp_hidd_args_t args;
    memset(&args, 0, sizeof(esp_hidd_args_t));
    args.send_report.type = type;
    args.send_report.id = id;
    args.send_report.len = len;
    args.send_report.p_data = p_data;

    bt_status_t stat = btc_transfer_context(&msg, &args, sizeof(esp_hidd_args_t), NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hid_device_report_error(uint8_t error) {
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HD;
    msg.act = BTC_HD_DISCONNECT_EVT;

    esp_hidd_args_t args;
    memset(&args, 0, sizeof(esp_hidd_args_t));
    args.error = error;

    bt_status_t stat = btc_transfer_context(&msg, &args, sizeof(esp_hidd_args_t), NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_hid_device_virtual_cable_unplug(void) {
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_HD;
    msg.act = BTC_HD_UNPLUG_EVT;

    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}