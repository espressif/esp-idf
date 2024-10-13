/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __BTC_DEV_H__
#define __BTC_DEV_H__

#include "esp_bt_defs.h"
#include "esp_bt_device.h"
#include "btc/btc_task.h"

typedef enum {
    BTC_DEV_ACT_SET_DEVICE_NAME,
    BTC_DEV_ACT_GET_DEVICE_NAME,
#if (ESP_COEX_VSC_INCLUDED == TRUE)
    BTC_DEV_ACT_CFG_COEX_STATUS,
#endif
} btc_dev_act_t;

/* btc_dev_args_t */
typedef union {
    // BTC_BT_GAP_ACT_SET_DEV_NAME
    struct set_bt_dev_name_args {
        char *device_name;
    } set_dev_name;

#if (ESP_COEX_VSC_INCLUDED == TRUE)
    // BTC_DEV_ACT_CFG_COEX_STATUS
    struct cfg_bt_dev_coex_st_args {
        esp_bt_dev_coex_type_t type;
        esp_bt_dev_coex_op_t op;
        uint8_t status;
    } cfg_coex_status;
#endif
} btc_dev_args_t;

void btc_dev_call_handler(btc_msg_t *msg);
void btc_dev_cb_handler(btc_msg_t *msg);
void btc_dev_call_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);
void btc_dev_call_arg_deep_free(btc_msg_t *msg);
void btc_dev_cb_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);
void btc_dev_cb_arg_deep_free(btc_msg_t *msg);

#endif /* __BTC_DEV_H__ */
