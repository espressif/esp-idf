/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __BTC_DEV_H__
#define __BTC_DEV_H__

#include "esp_bt_defs.h"
#include "esp_bt_device.h"
#include "btc/btc_task.h"

typedef enum {
    BTC_DEV_ACT_SET_DEVICE_NAME
} btc_dev_act_t;

/* btc_dev_args_t */
typedef union {
    // BTC_BT_GAP_ACT_SET_DEV_NAME
    struct set_bt_dev_name_args {
        char *device_name;
    } set_dev_name;
} btc_dev_args_t;

void btc_dev_call_handler(btc_msg_t *msg);

#endif /* __BTC_DEV_H__ */
