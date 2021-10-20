/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "osi/allocator.h"
#include "bta/bta_api.h"
#include "btc/btc_task.h"
#include "btc/btc_manage.h"
#include "btc/btc_dev.h"

void btc_dev_arg_deep_free(btc_msg_t *msg)
{
    BTC_TRACE_DEBUG("%s \n", __func__);

    switch (msg->act) {
    case BTC_DEV_ACT_SET_DEVICE_NAME:{
        char *device_name = ((btc_dev_args_t *)msg->arg)->set_dev_name.device_name;
        if (device_name) {
            osi_free(device_name);
        }
        break;
    }
    default:
        BTC_TRACE_DEBUG("Unhandled deep free %d\n", msg->act);
        break;
    }
}

void btc_dev_call_handler(btc_msg_t *msg)
{
    btc_dev_args_t *arg = (btc_dev_args_t *)msg->arg;

    BTC_TRACE_DEBUG("%s act %d\n", __FUNCTION__, msg->act);

    switch (msg->act) {
    case BTC_DEV_ACT_SET_DEVICE_NAME:
        BTA_DmSetDeviceName(arg->set_dev_name.device_name);
        break;
    default:
        break;
    }

    btc_dev_arg_deep_free(msg);
}
