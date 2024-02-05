/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __BTC_BT_MAIN_H__
#define __BTC_BT_MAIN_H__

#include "osi/future.h"
#include "stack/bt_types.h"
#include "bta/bta_api.h"
#include "btc/btc_main.h"
#include "btc/btc_task.h"

typedef enum {
    BTC_MAIN_ACT_INIT = 0,
    BTC_MAIN_ACT_DEINIT,
    BTC_MAIN_ACT_ENABLE,
    BTC_MAIN_ACT_DISABLE,
} btc_main_act_t;

typedef enum {
    BTC_MAIN_INIT_FUTURE = 0,
    BTC_MAIN_DEINIT_FUTURE,
    BTC_MAIN_ENABLE_FUTURE,
    BTC_MAIN_DISABLE_FUTURE,
    BTC_MAIN_FUTURE_NUM,
} btc_main_future_type_t;

#define BTC_BLE_STATUS_IDLE 0
typedef enum {
    BTC_BLE_STATUS_ADV = 0,     // Advertising exist
    BTC_BLE_STATUS_SCAN,        // Scanning exist
    BTC_BLE_STATUS_CONN,        // Connection exist
    BTC_BLE_STATUS_DEV,         // Device record exist
    BTC_BLE_STATUS_BOND,        // Bond info exist
    BTC_BLE_STATUS_GATTC_CACHE, // GATTC cache exist
    BTC_BLE_STATUS_GATTC_APP,   // GATTC application exist
    BTC_BLE_STATUS_GATTS_SRVC,  // GATTS service exist
} tBTC_BLE_STATUS;

future_t **btc_main_get_future_p(btc_main_future_type_t type);

#if 0
typedef union {
    struct btc_main_init_args {
        future_t *future;
    } init;
    struct btc_main_deinit_args {
        future_t *future;
    } deinit;
    struct btc_main_init_args {
        future_t *future;
    } enable;
    struct btc_main_init_args {
        future_t *future;
    } disable;
} btc_main_args_t;

bt_status_t btc_enable_bluetooth(future_t *future);
void btc_disable_bluetooth(future_t *future);
bt_status_t btc_init_bluetooth(future_t *future);
void btc_deinit_bluetooth(future_t *future);
#endif

void btc_main_call_handler(btc_msg_t *msg);
#endif /* __BTC_BT_MAIN_H__ */
