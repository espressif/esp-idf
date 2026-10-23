/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 *
 *  Filename:      btc/btc_profile_queue.h
 *
 *  Description:   Bluetooth remote device connection queuing
 *
 *******************************************************************************/

#ifndef __BTC_PROFILE_QUEUE_H__
#define __BTC_PROFILE_QUEUE_H__

#include "common/bt_defs.h"
#include "btc/btc_task.h"

typedef enum {
    BTC_PRF_QUE_CONNECT = 0,
    BTC_PRF_QUE_ADVANCE,
    BTC_PRF_QUE_REMOVE_UUID,
    BTC_PRF_QUE_ADVANCE_BUSY_CONNECT
} btc_prf_que_act_t;

typedef bt_status_t (*btc_connect_cb_t) (bt_bdaddr_t *bda, uint16_t uuid);

typedef struct connect_node_t {
    bt_bdaddr_t bda;
    uint16_t uuid;
    bool busy;
    btc_connect_cb_t connect_cb;
} connect_node_t;

/* btc_prf_que_args_t */
typedef union {
    // BTC_PRF_QUE_CONNECT
    connect_node_t connect_node;
    // BTC_PRF_QUE_REMOVE_UUID
    uint16_t uuid;
} btc_prf_que_args_t;

bt_status_t btc_queue_connect(uint16_t uuid, const bt_bdaddr_t *bda, btc_connect_cb_t connect_cb);
void btc_queue_advance(void);
void btc_queue_remove_uuid(uint16_t uuid);
void btc_queue_advance_busy_connect(uint16_t uuid, const bt_bdaddr_t *bda);
bt_status_t btc_queue_connect_next(void);
void btc_queue_release(void);

void btc_profile_queue_handler(btc_msg_t *msg);
#endif /* __BTC_PROFILE_QUEUE_H__ */
