// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*******************************************************************************
 *
 *  Filename:      btc_profile_queue.h
 *
 *  Description:   Bluetooth remote device connection queuing
 *
 *******************************************************************************/

#ifndef __BTC_PROFILE_QUEUE_H__
#define __BTC_PROFILE_QUEUE_H__

#include "bt_defs.h"
#include "btc_task.h"

typedef enum {
    BTC_PRF_QUE_CONNECT = 0,
    BTC_PRF_QUE_ADVANCE
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
} btc_prf_que_args_t;

bt_status_t btc_queue_connect(uint16_t uuid, const bt_bdaddr_t *bda, btc_connect_cb_t connect_cb);
void btc_queue_advance(void);
bt_status_t btc_queue_connect_next(void);
void btc_queue_release(void);

void btc_profile_queue_handler(btc_msg_t *msg);
#endif /* __BTC_PROFILE_QUEUE_H__ */
