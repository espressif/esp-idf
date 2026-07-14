/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __BTC_BLE_L2CAP_H__
#define __BTC_BLE_L2CAP_H__

#if (BLE_L2CAP_COC_INCLUDED == TRUE)

#include "btc/btc_manage.h"
#include "common/bt_target.h"
#include "esp_ble_l2cap_api.h"

typedef enum {
    BTC_BLE_L2CAP_ACT_INIT = 0,
    BTC_BLE_L2CAP_ACT_DEINIT,
    BTC_BLE_L2CAP_ACT_CREATE_SERVER,
    BTC_BLE_L2CAP_ACT_DELETE_SERVER,
    BTC_BLE_L2CAP_ACT_CONNECT,
    BTC_BLE_L2CAP_ACT_ACCEPT,
    BTC_BLE_L2CAP_ACT_DISCONNECT,
    BTC_BLE_L2CAP_ACT_SEND,
    BTC_BLE_L2CAP_ACT_RECV_READY,
    BTC_BLE_L2CAP_ACT_CONNECT_ECOC,
    BTC_BLE_L2CAP_ACT_RECONFIG,
    BTC_BLE_L2CAP_ACT_SET_AUTO_CREDIT,
} btc_ble_l2cap_act_t;

typedef union {
    struct {
        uint16_t psm;
        uint16_t mtu;
    } create_server;
    struct {
        uint16_t psm;
    } delete_server;
    struct {
        uint16_t conn_id;
        uint16_t psm;
        uint16_t mtu;
    } connect;
    struct {
        uint16_t conn_id;
        uint8_t l2cap_id;
        uint16_t chan_handle;
        bool accept;
        uint16_t mtu;
    } accept;
    struct {
        uint16_t chan_handle;
    } disconnect;
    struct {
        uint16_t chan_handle;
        uint16_t len;
        uint8_t *data;
    } send;
    struct {
        uint16_t chan_handle;
    } recv_ready;
    struct {
        uint16_t conn_id;
        uint16_t psm;
        uint16_t mtu;
        uint8_t num_chan;
    } connect_ecoc;
    struct {
        uint16_t num_chan;
        uint16_t mtu;
        uint16_t mps;
        uint16_t chan_handles[BLE_MAX_L2CAP_CLIENTS];
    } reconfig;
    struct {
        uint16_t chan_handle;
        bool enable;
    } set_auto_credit;
} btc_ble_l2cap_args_t;

void btc_ble_l2cap_call_handler(btc_msg_t *msg);
void btc_ble_l2cap_cb_handler(btc_msg_t *msg);
void btc_ble_l2cap_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);
void btc_ble_l2cap_arg_deep_free(btc_msg_t *msg);
void btc_ble_l2cap_cb_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);
void btc_ble_l2cap_cb_deep_free(btc_msg_t *msg);

#endif /* BLE_L2CAP_COC_INCLUDED == TRUE */

#endif /* __BTC_BLE_L2CAP_H__ */
