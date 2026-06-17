/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "hal/lp_sys_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

#define lp_aon_hal_inform_wakeup_type(dslp)                     lp_sys_ll_inform_wakeup_type(dslp)
#define lp_aon_hal_store_wakeup_cause(wakeup_cause)             lp_sys_ll_store_wakeup_cause(wakeup_cause)
#define lp_aon_hal_load_wakeup_cause()                          lp_sys_ll_load_wakeup_cause()

#ifdef __cplusplus
}
#endif
