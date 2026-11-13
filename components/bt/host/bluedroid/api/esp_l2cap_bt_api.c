/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <string.h>

#include "esp_bt_main.h"
#include "btc/btc_manage.h"

#include "btc_l2cap.h"
#include "esp_l2cap_bt_api.h"
#include "common/bt_target.h"

#if (defined BTC_L2CAP_INCLUDED && BTC_L2CAP_INCLUDED == TRUE)

/* BR/EDR PSM: odd, and the least significant bit of the most significant
   octet is zero. */
static bool l2cap_psm_is_valid(uint16_t psm)
{
    return ((psm & 0x0101) == 0x0001);
}

esp_err_t esp_bt_l2cap_register_callback(esp_bt_l2cap_cb_t callback)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (callback == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    int ret = btc_profile_cb_set(BTC_PID_L2CAP, callback);
    return (ret == 0) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_l2cap_init(void)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_L2CAP;
    msg.act = BTC_L2CAP_ACT_INIT;

    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_l2cap_deinit(void)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_L2CAP;
    msg.act = BTC_L2CAP_ACT_UNINIT;

    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_l2cap_connect(esp_bt_l2cap_cntl_flags_t cntl_flag, uint16_t remote_psm, esp_bd_addr_t peer_bd_addr)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if ((peer_bd_addr == NULL) || !l2cap_psm_is_valid(remote_psm)) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_L2CAP;
    msg.act = BTC_L2CAP_ACT_CONNECT;

    btc_l2cap_args_t arg = {0};
    arg.connect.sec_mask = (cntl_flag & 0xffff);
    arg.connect.remote_psm = remote_psm;
    memcpy(arg.connect.peer_bd_addr, peer_bd_addr, ESP_BD_ADDR_LEN);

    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_l2cap_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_l2cap_start_srv(esp_bt_l2cap_cntl_flags_t cntl_flag, uint16_t local_psm)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (!l2cap_psm_is_valid(local_psm)) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_L2CAP;
    msg.act = BTC_L2CAP_ACT_START_SRV;

    btc_l2cap_args_t arg = {0};
    arg.start_srv.sec_mask = (cntl_flag & 0xffff);
    arg.start_srv.local_psm = local_psm;

    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_l2cap_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_l2cap_stop_all_srv(void)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_L2CAP;
    msg.act = BTC_L2CAP_ACT_STOP_SRV;

    btc_l2cap_args_t arg = {0};
    arg.stop_srv.psm = BTC_L2CAP_INVALID_PSM;

    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_l2cap_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_l2cap_stop_srv(uint16_t local_psm)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_L2CAP;
    msg.act = BTC_L2CAP_ACT_STOP_SRV;

    btc_l2cap_args_t arg = {0};
    arg.stop_srv.psm = local_psm;

    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_l2cap_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_l2cap_vfs_register(void)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_L2CAP;
    msg.act = BTC_L2CAP_ACT_VFS_REGISTER;

    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_l2cap_vfs_unregister(void)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_L2CAP;
    msg.act = BTC_L2CAP_ACT_VFS_UNREGISTER;

    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_l2cap_get_protocol_status(esp_bt_l2cap_protocol_status_t *status)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (status == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(status, 0, sizeof(esp_bt_l2cap_protocol_status_t));
    btc_l2cap_get_protocol_status(status);

    return ESP_OK;
}

#endif ///defined BTC_L2CAP_INCLUDED && BTC_L2CAP_INCLUDED == TRUE
