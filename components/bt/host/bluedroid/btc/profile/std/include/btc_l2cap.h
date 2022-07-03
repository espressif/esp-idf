/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __BTC_L2CAP_H__
#define __BTC_L2CAP_H__

#include "btc/btc_task.h"
#include "esp_bt_defs.h"
#include "esp_l2cap_bt_api.h"
#include "common/bt_target.h"
#include "bta/bta_jv_api.h"

#if (defined BTC_L2CAP_INCLUDED && BTC_L2CAP_INCLUDED == TRUE)

#define BTC_L2CAP_INVALID_PSM 0x00

typedef enum {
    BTC_L2CAP_ACT_INIT = 0,
    BTC_L2CAP_ACT_UNINIT,
    BTC_L2CAP_ACT_START_DISCOVERY,
    BTC_L2CAP_ACT_CONNECT,
    BTC_L2CAP_ACT_START_SRV,
    BTC_L2CAP_ACT_STOP_SRV,
} btc_l2cap_act_t;

/* btc_l2cap_args_t */
typedef union {
    //BTC_L2CAP_ACT_INIT
    struct l2cap_init_arg {
    } init;

    //BTC_L2CAP_ACT_UNINIT
    struct l2cap_uninit_arg {
    } uninit;

    //BTC_L2CAP_ACT_START_DISCOVERY
    struct l2cap_start_discovery_arg {
        BD_ADDR bd_addr;
        UINT16 num_uuid;
        tSDP_UUID *p_uuid_list;
    } start_discovery;

    //BTC_L2CAP_ACT_CONNECT
    struct l2cap_connect_arg {
        UINT16 sec_mask;
        UINT16 remote_psm;
        esp_bd_addr_t peer_bd_addr;
    } connect;

    //BTC_L2CAP_ACT_START_SRV
    struct l2cap_start_srv_arg {
        UINT16 sec_mask;
        UINT16 local_psm;
    } start_srv;

    //BTC_L2CAP_ACT_STOP_SRV
    struct l2cap_stop_srv_arg {
        UINT16 psm;
    } stop_srv;

} btc_l2cap_args_t;

void btc_l2cap_call_handler(btc_msg_t *msg);
void btc_l2cap_cb_handler(btc_msg_t *msg);
esp_err_t btc_l2cap_vfs_register(void);
esp_err_t btc_l2cap_vfs_unregister(void);

#endif ///defined BTC_L2CAP_INCLUDED && BTC_L2CAP_INCLUDED == TRUE
#endif ///__BTC_L2CAP_H__
