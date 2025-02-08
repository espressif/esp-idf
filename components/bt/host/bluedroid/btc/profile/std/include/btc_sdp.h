/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __BTC_SDP_H__
#define __BTC_SDP_H__

#include "btc/btc_task.h"
#include "esp_bt_defs.h"
#include "common/bt_target.h"
#include "bta/bta_sdp_api.h"
#include "bt_sdp.h"
#include "esp_sdp_api.h"

#if (defined BTC_SDP_INCLUDED && BTC_SDP_INCLUDED == TRUE)

typedef enum {
    BTC_SDP_ACT_INIT = 0,
    BTC_SDP_ACT_DEINIT,
    BTC_SDP_ACT_SEARCH,
    BTC_SDP_ACT_CREATE_RECORD,
    BTC_SDP_ACT_REMOVE_RECORD,
} btc_sdp_act_t;

/* btc_sdp_args_t */
typedef union {
    //BTC_SDP_ACT_SEARCH
    struct search_record_arg {
        BD_ADDR bd_addr;
        tSDP_UUID sdp_uuid;
    } search;

    //BTC_SDP_ACT_CREATE_RECORD
    struct create_record_arg {
        bluetooth_sdp_record *record;
    } create_record;

    //BTC_SDP_ACT_REMOVE_RECORD
    struct remove_record_arg {
        int record_handle;
    } remove_record;

} btc_sdp_args_t;

void btc_sdp_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);
void btc_sdp_arg_deep_free(btc_msg_t *msg);

void btc_sdp_call_handler(btc_msg_t *msg);
void btc_sdp_cb_handler(btc_msg_t *msg);

void btc_sdp_get_protocol_status(esp_sdp_protocol_status_t *param);

#endif ///defined BTC_SDP_INCLUDED && BTC_SDP_INCLUDED == TRUE
#endif ///__BTC_SDP_H__
