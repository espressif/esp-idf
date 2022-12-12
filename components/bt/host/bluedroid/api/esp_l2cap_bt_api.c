/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "esp_bt_main.h"
#include "btc/btc_manage.h"

#include "btc_l2cap.h"
#include "esp_l2cap_bt_api.h"
#include "common/bt_target.h"

#if (defined BTC_L2CAP_INCLUDED && BTC_L2CAP_INCLUDED == TRUE)

esp_err_t esp_bt_l2cap_register_callback(esp_bt_l2cap_cb_t callback)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (callback == NULL) {
        return ESP_FAIL;
    }

    btc_profile_cb_set(BTC_PID_L2CAP, callback);
    return ESP_OK;
}

esp_err_t esp_bt_l2cap_init(void)
{
    btc_msg_t msg;
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_L2CAP;
    msg.act = BTC_L2CAP_ACT_INIT;

    return (btc_transfer_context(&msg, NULL, 0, NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_bt_l2cap_deinit(void)
{
    btc_msg_t msg;
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_L2CAP;
    msg.act = BTC_L2CAP_ACT_UNINIT;

    return (btc_transfer_context(&msg, NULL, 0, NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_bt_l2cap_connect(esp_bt_l2cap_cntl_flags_t cntl_flag, uint16_t remote_psm, esp_bd_addr_t peer_bd_addr)
{
    btc_msg_t msg;
    btc_l2cap_args_t arg;
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_L2CAP;
    msg.act = BTC_L2CAP_ACT_CONNECT;

    arg.connect.sec_mask = (cntl_flag & 0xffff);
    arg.connect.remote_psm = remote_psm;
    memcpy(arg.connect.peer_bd_addr, peer_bd_addr, ESP_BD_ADDR_LEN);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_l2cap_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_bt_l2cap_start_srv(esp_bt_l2cap_cntl_flags_t cntl_flag, uint16_t local_psm)
{
    btc_msg_t msg;
    btc_l2cap_args_t arg;
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_L2CAP;
    msg.act = BTC_L2CAP_ACT_START_SRV;

    arg.start_srv.sec_mask = (cntl_flag & 0xffff);
    arg.start_srv.local_psm = local_psm;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_l2cap_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_bt_l2cap_stop_all_srv(void)
{
    btc_msg_t msg;
    btc_l2cap_args_t arg;
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_L2CAP;
    msg.act = BTC_L2CAP_ACT_STOP_SRV;

    arg.stop_srv.psm = BTC_L2CAP_INVALID_PSM;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_l2cap_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_bt_l2cap_stop_srv(uint16_t local_psm)
{
    btc_msg_t msg;
    btc_l2cap_args_t arg;
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_L2CAP;
    msg.act = BTC_L2CAP_ACT_STOP_SRV;

    arg.stop_srv.psm = local_psm;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_l2cap_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_bt_l2cap_vfs_register(void)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    return btc_l2cap_vfs_register();
}

esp_err_t esp_bt_l2cap_vfs_unregister(void)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    return btc_l2cap_vfs_unregister();
}

#endif ///defined BTC_L2CAP_INCLUDED && BTC_L2CAP_INCLUDED == TRUE
