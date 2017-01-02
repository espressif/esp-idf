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

#include <stdlib.h>
#include <string.h>
#include "btc_task.h"
#include "bt_trace.h"
#include "thread.h"
#include "gki.h"
#include "bt_defs.h"
#include "btc_main.h"
#include "btc_gatts.h"
#include "btc_gattc.h"
#include "btc_gap_ble.h"
#include "btc_blufi_prf.h"
#include "bta_gatt_api.h"


static xTaskHandle  xBtcTaskHandle = NULL;
static xQueueHandle xBtcQueue = 0;

static btc_func_t profile_tab[BTC_PID_NUM] = {
    [BTC_PID_MAIN_INIT] = {btc_main_call_handler,       NULL                    },
    [BTC_PID_GATTS]     = {btc_gatts_call_handler,      btc_gatts_cb_handler    },
    [BTC_PID_GATTC]     = {btc_gattc_call_handler,      btc_gattc_cb_handler    },
    [BTC_PID_GAP_BLE]   = {btc_gap_ble_call_handler,    btc_gap_ble_cb_handler  },
    [BTC_PID_GAP_BT]    = {NULL, NULL}, // {btc_gap_bt_call_handler,    btc_gap_bt_cb_handler   },
    [BTC_PID_SDP]       = {NULL, NULL},
    [BTC_PID_BLE_HID]   = {NULL, NULL},
    [BTC_PID_BT_HID]    = {NULL, NULL},
    [BTC_PID_SPP]       = {NULL, NULL},
    [BTC_PID_SPPLIKE]   = {NULL, NULL},
    [BTC_PID_BLUFI]     = {btc_blufi_call_handler,      btc_blufi_cb_handler    },
};

/*****************************************************************************
**
** Function         btc_task
**
** Description      Process profile Task Thread.
******************************************************************************/
static void btc_task(void *arg)
{
    btc_msg_t msg;

    for (;;) {
        if (pdTRUE == xQueueReceive(xBtcQueue, &msg, (portTickType)portMAX_DELAY)) {
            LOG_DEBUG("%s msg %u %u %u %p\n", __func__, msg.sig, msg.pid, msg.act, msg.arg);
            switch (msg.sig) {
            case BTC_SIG_API_CALL:
                profile_tab[msg.pid].btc_call(&msg);
                break;
            case BTC_SIG_API_CB:
                profile_tab[msg.pid].btc_cb(&msg);
                break;
            default:
                break;
            }
            if (msg.arg) {
                GKI_freebuf(msg.arg);
            }
        }
    }
}

static bt_status_t btc_task_post(btc_msg_t *msg)
{
    if (msg == NULL) {
        return BT_STATUS_PARM_INVALID;
    }

    if (xQueueSend(xBtcQueue, msg, 10 / portTICK_PERIOD_MS) != pdTRUE) {
        LOG_ERROR("Btc Post failed\n");
        return BT_STATUS_BUSY;
    }

    return BT_STATUS_SUCCESS;
}

bt_status_t btc_transfer_context(btc_msg_t *msg, void *arg, int arg_len, btc_arg_deep_copy_t copy_func)
{
    btc_msg_t lmsg;

    if (msg == NULL) {
        return BT_STATUS_PARM_INVALID;
    }

    LOG_DEBUG("%s msg %u %u %u %p\n", __func__, msg->sig, msg->pid, msg->act, arg);

    memcpy(&lmsg, msg, sizeof(btc_msg_t));
    if (arg) {
        lmsg.arg = (void *)GKI_getbuf(arg_len);
        memset(lmsg.arg, 0x00, arg_len);    //important, avoid arg which have no length
        if (lmsg.arg == NULL) {
            return BT_STATUS_NOMEM;
        }
        memcpy(lmsg.arg, arg, arg_len);
        if (copy_func) {
            copy_func(&lmsg, lmsg.arg, arg);
        }
    } else {
        lmsg.arg = NULL;
    }

    return btc_task_post(&lmsg);
}


int btc_init(void)
{
    xBtcQueue = xQueueCreate(BTC_TASK_QUEUE_NUM, sizeof(btc_msg_t));
    xTaskCreate(btc_task, "Btc_task", BTC_TASK_STACK_SIZE, NULL, BTC_TASK_PRIO, &xBtcTaskHandle);

    /* TODO: initial the profile_tab */

    return BT_STATUS_SUCCESS;
}

void btc_deinit(void)
{
    vTaskDelete(xBtcTaskHandle);
    vQueueDelete(xBtcQueue);

    xBtcTaskHandle = NULL;
    xBtcQueue = 0;
}
