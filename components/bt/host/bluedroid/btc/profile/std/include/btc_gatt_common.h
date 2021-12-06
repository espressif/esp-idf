/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __BTC_GATT_COMMON_H__
#define __BTC_GATT_COMMON_H__

#include "osi/future.h"
#include "stack/bt_types.h"
#include "bta/bta_api.h"
#include "btc/btc_main.h"
#include "btc/btc_task.h"

typedef enum {
    BTC_GATT_ACT_SET_LOCAL_MTU = 0,
} btc_gatt_com_act_t;

/* btc_ble_gattc_args_t */
typedef union {
    //BTC_GATT_ACT_SET_LOCAL_MTU,
    struct set_mtu_arg {
        uint16_t mtu;
    } set_mtu;
} btc_ble_gatt_com_args_t;

void btc_gatt_com_call_handler(btc_msg_t *msg);
#endif /* __BTC_GATT_COMMON_H__ */
