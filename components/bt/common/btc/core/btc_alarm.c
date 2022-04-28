/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "btc/btc_task.h"
#include "btc/btc_alarm.h"
#include "esp_log.h"

void btc_alarm_handler(btc_msg_t *msg)
{
    btc_alarm_args_t *arg = (btc_alarm_args_t *)msg->arg;

    BTC_TRACE_DEBUG("%s act %d\n", __FUNCTION__, msg->act);

    if (arg->cb) {
        arg->cb(arg->cb_data);
    }
}
