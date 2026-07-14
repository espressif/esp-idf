/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "esp_bt_main.h"
#include "esp_bt_defs.h"
#include "esp_ble_l2cap_api.h"
#include "btc/btc_manage.h"
#include "btc/btc_task.h"

#if (BLE_L2CAP_COC_INCLUDED == TRUE)
#include "common/bt_target.h"
#include "btc_ble_l2cap.h"

/* LE PSM valid range per Core Spec: 0x0001..0x00FF */
#define ESP_BLE_L2CAP_IS_VALID_LE_PSM(psm) ((psm) > 0x0000 && (psm) < 0x0100)

/* Minimum MTU per Core Spec Vol 3 Part A: 23 for LE credit based (4.22),
 * 64 for enhanced credit based / ECFC (4.25). */
#define ESP_BLE_L2CAP_LE_MIN_MTU    23
#define ESP_BLE_L2CAP_ECFC_MIN_MTU  64
/* Minimum MPS for enhanced credit based / ECFC channels (Core Spec Vol 3
 * Part A 4.25). */
#define ESP_BLE_L2CAP_ECFC_MIN_MPS  64
/* Core Spec Vol 3 Part A 4.25/4.27: a single enhanced credit based connection
 * or reconfiguration request may target at most five channels, regardless of
 * the (pool-sized) BT_BLE_L2CAP_COC_MAX_CHAN Kconfig value. */
#define ESP_BLE_L2CAP_ECFC_MAX_REQ_CHANS  5

static esp_err_t btc_ble_l2cap_transfer(btc_ble_l2cap_act_t act, btc_ble_l2cap_args_t *arg)
{
    btc_msg_t msg = {0};

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_BLE_L2CAP;
    msg.act = act;

    return (btc_transfer_context(&msg, arg, sizeof(btc_ble_l2cap_args_t),
                                 btc_ble_l2cap_arg_deep_copy,
                                 btc_ble_l2cap_arg_deep_free) == BT_STATUS_SUCCESS)
           ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_ble_l2cap_register_callback(esp_ble_l2cap_cb_t callback)
{
    if (callback == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);
    return (btc_profile_cb_set(BTC_PID_BLE_L2CAP, callback) == 0) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_ble_l2cap_init(void)
{
    btc_ble_l2cap_args_t arg = {0};

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);
    return btc_ble_l2cap_transfer(BTC_BLE_L2CAP_ACT_INIT, &arg);
}

esp_err_t esp_ble_l2cap_deinit(void)
{
    btc_ble_l2cap_args_t arg = {0};

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);
    return btc_ble_l2cap_transfer(BTC_BLE_L2CAP_ACT_DEINIT, &arg);
}

#if (BLE_L2CAP_COC_SERVER_INCLUDED == TRUE)
esp_err_t esp_ble_l2cap_create_server(uint16_t psm, uint16_t mtu)
{
    btc_ble_l2cap_args_t arg = {0};

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);
    if (psm == 0 || mtu < ESP_BLE_L2CAP_LE_MIN_MTU || !ESP_BLE_L2CAP_IS_VALID_LE_PSM(psm)) {
        return ESP_ERR_INVALID_ARG;
    }
    arg.create_server.psm = psm;
    arg.create_server.mtu = mtu;
    return btc_ble_l2cap_transfer(BTC_BLE_L2CAP_ACT_CREATE_SERVER, &arg);
}

esp_err_t esp_ble_l2cap_delete_server(uint16_t psm)
{
    btc_ble_l2cap_args_t arg = {0};

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);
    if (psm == 0 || !ESP_BLE_L2CAP_IS_VALID_LE_PSM(psm)) {
        return ESP_ERR_INVALID_ARG;
    }
    arg.delete_server.psm = psm;
    return btc_ble_l2cap_transfer(BTC_BLE_L2CAP_ACT_DELETE_SERVER, &arg);
}

esp_err_t esp_ble_l2cap_accept(uint16_t conn_id, uint8_t l2cap_id,
                               uint16_t chan_handle, bool accept, uint16_t mtu)
{
    btc_ble_l2cap_args_t arg = {0};

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);
    if (chan_handle == 0 || (accept && mtu < ESP_BLE_L2CAP_LE_MIN_MTU)) {
        return ESP_ERR_INVALID_ARG;
    }
    arg.accept.conn_id = conn_id;
    arg.accept.l2cap_id = l2cap_id;
    arg.accept.chan_handle = chan_handle;
    arg.accept.accept = accept;
    arg.accept.mtu = mtu;
    return btc_ble_l2cap_transfer(BTC_BLE_L2CAP_ACT_ACCEPT, &arg);
}
#endif /* BLE_L2CAP_COC_SERVER_INCLUDED */

#if (BLE_L2CAP_COC_CLIENT_INCLUDED == TRUE)
esp_err_t esp_ble_l2cap_connect(uint16_t conn_id, uint16_t psm, uint16_t mtu)
{
    btc_ble_l2cap_args_t arg = {0};

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);
    if (psm == 0 || mtu < ESP_BLE_L2CAP_LE_MIN_MTU || !ESP_BLE_L2CAP_IS_VALID_LE_PSM(psm)) {
        return ESP_ERR_INVALID_ARG;
    }
    arg.connect.conn_id = conn_id;
    arg.connect.psm = psm;
    arg.connect.mtu = mtu;
    return btc_ble_l2cap_transfer(BTC_BLE_L2CAP_ACT_CONNECT, &arg);
}
#endif /* BLE_L2CAP_COC_CLIENT_INCLUDED */

esp_err_t esp_ble_l2cap_disconnect(uint16_t chan_handle)
{
    btc_ble_l2cap_args_t arg = {0};

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);
    if (chan_handle == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    arg.disconnect.chan_handle = chan_handle;
    return btc_ble_l2cap_transfer(BTC_BLE_L2CAP_ACT_DISCONNECT, &arg);
}

esp_err_t esp_ble_l2cap_send(uint16_t chan_handle, uint8_t *data, uint16_t len)
{
    btc_ble_l2cap_args_t arg = {0};

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);
    if (chan_handle == 0 || data == NULL || len == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    arg.send.chan_handle = chan_handle;
    arg.send.len = len;
    arg.send.data = data;
    return btc_ble_l2cap_transfer(BTC_BLE_L2CAP_ACT_SEND, &arg);
}

esp_err_t esp_ble_l2cap_recv_ready(uint16_t chan_handle)
{
    btc_ble_l2cap_args_t arg = {0};

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);
    if (chan_handle == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    arg.recv_ready.chan_handle = chan_handle;
    return btc_ble_l2cap_transfer(BTC_BLE_L2CAP_ACT_RECV_READY, &arg);
}

esp_err_t esp_ble_l2cap_set_auto_credit(uint16_t chan_handle, bool enable)
{
    btc_ble_l2cap_args_t arg = {0};

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);
    if (chan_handle == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    arg.set_auto_credit.chan_handle = chan_handle;
    arg.set_auto_credit.enable = enable;
    return btc_ble_l2cap_transfer(BTC_BLE_L2CAP_ACT_SET_AUTO_CREDIT, &arg);
}

#if (BLE_L2CAP_ENHANCED_COC_INCLUDED == TRUE)
#if (BLE_L2CAP_COC_CLIENT_INCLUDED == TRUE)
esp_err_t esp_ble_l2cap_connect_ecoc(uint16_t conn_id, uint16_t psm, uint16_t mtu, uint8_t num_chan)
{
    btc_ble_l2cap_args_t arg = {0};

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);
    if (psm == 0 || mtu < ESP_BLE_L2CAP_ECFC_MIN_MTU || num_chan == 0 ||
        num_chan > BLE_MAX_L2CAP_CLIENTS ||
        num_chan > ESP_BLE_L2CAP_ECFC_MAX_REQ_CHANS || !ESP_BLE_L2CAP_IS_VALID_LE_PSM(psm)) {
        return ESP_ERR_INVALID_ARG;
    }
    arg.connect_ecoc.conn_id = conn_id;
    arg.connect_ecoc.psm = psm;
    arg.connect_ecoc.mtu = mtu;
    arg.connect_ecoc.num_chan = num_chan;
    return btc_ble_l2cap_transfer(BTC_BLE_L2CAP_ACT_CONNECT_ECOC, &arg);
}
#endif /* BLE_L2CAP_COC_CLIENT_INCLUDED */

esp_err_t esp_ble_l2cap_reconfig(uint16_t *chan_handles, uint8_t num_chan, uint16_t mtu, uint16_t mps)
{
    btc_ble_l2cap_args_t arg = {0};

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);
    if (chan_handles == NULL || num_chan == 0 || num_chan > BLE_MAX_L2CAP_CLIENTS ||
        num_chan > ESP_BLE_L2CAP_ECFC_MAX_REQ_CHANS ||
        mtu < ESP_BLE_L2CAP_ECFC_MIN_MTU || mps < ESP_BLE_L2CAP_ECFC_MIN_MPS) {
        return ESP_ERR_INVALID_ARG;
    }
    arg.reconfig.num_chan = num_chan;
    arg.reconfig.mtu = mtu;
    arg.reconfig.mps = mps;
    memcpy(arg.reconfig.chan_handles, chan_handles, num_chan * sizeof(uint16_t));
    return btc_ble_l2cap_transfer(BTC_BLE_L2CAP_ACT_RECONFIG, &arg);
}
#endif /* BLE_L2CAP_ENHANCED_COC_INCLUDED */

#endif /* BLE_L2CAP_COC_INCLUDED == TRUE */
