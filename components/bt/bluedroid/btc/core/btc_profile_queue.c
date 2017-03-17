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

#include "bt_target.h"
#include <string.h>
#include "esp_bt_main.h"
#include "bt_trace.h"
#include "bt_defs.h"
#include "btc_profile_queue.h"
#include "gki.h"
#include "list.h"
#include "allocator.h"

#if BTC_PRF_QUEUE_INCLUDED
/*******************************************************************************
**  Local type definitions
*******************************************************************************/
/*******************************************************************************
**  Static variables
*******************************************************************************/

static list_t *connect_queue;

static const size_t MAX_REASONABLE_REQUESTS = 10;

/*******************************************************************************
**  Queue helper functions
*******************************************************************************/

static void queue_int_add(connect_node_t *p_param)
{
    if (!connect_queue) {
        connect_queue = list_new(osi_free_func);
        assert(connect_queue != NULL);
    }

    // Sanity check to make sure we're not leaking connection requests
    assert(list_length(connect_queue) < MAX_REASONABLE_REQUESTS);

    for (const list_node_t *node = list_begin(connect_queue); node != list_end(connect_queue); node = list_next(node)) {
        if (((connect_node_t *)list_node(node))->uuid == p_param->uuid) {
            LOG_DEBUG("%s dropping duplicate connect request for uuid: %04x", __func__, p_param->uuid);
            return;
        }
    }

    connect_node_t *p_node = osi_malloc(sizeof(connect_node_t));
    assert(p_node != NULL);
    memcpy(p_node, p_param, sizeof(connect_node_t));
    list_append(connect_queue, p_node);
}

static void queue_int_advance()
{
    if (connect_queue && !list_is_empty(connect_queue)) {
        list_remove(connect_queue, list_front(connect_queue));
    }
}

void btc_profile_queue_handler(btc_msg_t *msg)
{
    btc_prf_que_args_t *arg = (btc_prf_que_args_t *)(msg->arg);
    switch (msg->act) {
    case BTC_PRF_QUE_CONNECT:
        queue_int_add(&(arg->connect_node));
        break;

    case BTC_PRF_QUE_ADVANCE:
        queue_int_advance();
        break;
    }

    if (esp_bluedroid_get_status() == ESP_BLUEDROID_STATUS_ENABLED) {
        btc_queue_connect_next();
    }
}

/*******************************************************************************
**
** Function         btc_queue_connect
**
** Description      Add a new connection to the queue and trigger the next
**                  scheduled connection.
**
** Returns          BT_STATUS_SUCCESS if successful
**
*******************************************************************************/
bt_status_t btc_queue_connect(uint16_t uuid, const bt_bdaddr_t *bda, btc_connect_cb_t connect_cb)
{
    btc_msg_t msg;
    btc_prf_que_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PRF_QUE;
    msg.act = BTC_PRF_QUE_CONNECT;

    memset(&arg, 0, sizeof(btc_prf_que_args_t));
    memcpy(&arg.connect_node.bda, bda, sizeof(bt_bdaddr_t));
    arg.connect_node.uuid = uuid;
    arg.connect_node.connect_cb = connect_cb;

    return btc_transfer_context(&msg, &arg, sizeof(btc_prf_que_args_t), NULL);
}
/*******************************************************************************
**
** Function         btc_queue_advance
**
** Description      Clear the queue's busy status and advance to the next
**                  scheduled connection.
**
** Returns          void
**
*******************************************************************************/
void btc_queue_advance(void)
{
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PRF_QUE;
    msg.act = BTC_PRF_QUE_ADVANCE;

    btc_transfer_context(&msg, NULL, 0, NULL);
}

// This function dispatches the next pending connect request. It is called from
// stack_manager when the stack comes up.
bt_status_t btc_queue_connect_next(void)
{
    if (!connect_queue || list_is_empty(connect_queue)) {
        return BT_STATUS_FAIL;
    }

    connect_node_t *p_head = list_front(connect_queue);

    // If the queue is currently busy, we return success anyway,
    // since the connection has been queued...
    if (p_head->busy) {
        return BT_STATUS_SUCCESS;
    }

    p_head->busy = true;
    return p_head->connect_cb(&p_head->bda, p_head->uuid);
}


/*******************************************************************************
**
** Function         btc_queue_release
**
** Description      Free up all the queue nodes and set the queue head to NULL
**
** Returns          void
**
*******************************************************************************/
void btc_queue_release(void)
{
    list_free(connect_queue);
    connect_queue = NULL;
}

#endif /* if BTC_PRF_QUEUE_INCLUDED */
