/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#ifndef __BTC_COMMON_H__
#define __BTC_COMMON_H__

#include <assert.h>
#include "common/bt_trace.h"
#include "stack/bt_types.h"
#include "osi/osi.h"

#define BTC_ASSERTC(cond, msg, val) assert(cond && msg)

#define BTC_HAL_CBACK(P_CB, P_CBACK, ...)\
    if (P_CB && P_CB->P_CBACK) {            \
        LOG_INFO("HAL %s->%s", #P_CB, #P_CBACK); \
        P_CB->P_CBACK(__VA_ARGS__);         \
    }                                       \
    else {                                  \
        BTC_ASSERTC(0, "Callback is NULL", 0);  \
    }

#endif /* __BTC_COMMON_H__ */
