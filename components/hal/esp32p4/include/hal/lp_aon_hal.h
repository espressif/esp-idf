/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "hal/lp_sys_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

#define lp_aon_hal_inform_wakeup_type(dslp)                     lp_sys_ll_inform_wakeup_type(dslp)

#ifdef __cplusplus
}
#endif
