/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "btc/btc_task.h"
#include "esp_pan_api.h"
#include "common/bt_target.h"
#include "stack/bt_types.h"
#include "bta/bta_pan_api.h"

#if (defined BTC_PAN_INCLUDED && BTC_PAN_INCLUDED == TRUE)

#define BTC_PAN_APP_ID_PANU     0
#define BTC_PAN_APP_ID_NAP      1
#define BTC_PAN_APP_ID_GN       2

typedef enum {
    BTC_PAN_ACT_INIT = 0,
    BTC_PAN_ACT_DEINIT,
    BTC_PAN_ACT_CONNECT,
    BTC_PAN_ACT_DISCONNECT,
    BTC_PAN_ACT_WRITE,
    BTC_PAN_ACT_SET_PFILTER,
    BTC_PAN_ACT_SET_MFILTER,
} btc_pan_act_t;

typedef union {
    struct init_arg {
        esp_pan_cfg_t cfg;
    } init;

    struct connect_arg {
        BD_ADDR bd_addr;
        esp_pan_role_t local_role;
        esp_pan_role_t peer_role;
    } connect;

    struct disconnect_arg {
        uint16_t handle;
    } disconnect;

    struct write_arg {
        uint16_t handle;
        BD_ADDR dst;
        BD_ADDR src;
        uint16_t protocol;
        uint16_t len;
        uint8_t *data;
        bool ext;
    } write;

    struct set_pfilter_arg {
        uint16_t handle;
        uint16_t num_filters;
        uint16_t *start_array;
        uint16_t *end_array;
    } set_pfilter;

    struct set_mfilter_arg {
        uint16_t handle;
        uint16_t num_filters;
        esp_bd_addr_t *start_array;
        esp_bd_addr_t *end_array;
    } set_mfilter;
} btc_pan_args_t;

void btc_pan_call_handler(btc_msg_t *msg);
void btc_pan_cb_handler(btc_msg_t *msg);
void btc_pan_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);
void btc_pan_arg_deep_free(btc_msg_t *msg);
void btc_pan_cb_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);
void btc_pan_cb_deep_free(btc_msg_t *msg);

void btc_pan_co_data_ind(UINT16 handle, BD_ADDR src, BD_ADDR dst, UINT16 protocol,
                         UINT8 *p_data, UINT16 len, BOOLEAN ext, BOOLEAN forward);
void btc_pan_co_rx_flow(UINT16 handle, BOOLEAN enable);
void btc_pan_co_pfilt_ind(UINT16 handle, BOOLEAN indication, tBTA_PAN_STATUS result,
                          UINT16 len, UINT8 *p_filters);
void btc_pan_co_mfilt_ind(UINT16 handle, BOOLEAN indication, tBTA_PAN_STATUS result,
                          UINT16 len, UINT8 *p_filters);

#endif /* BTC_PAN_INCLUDED */
