/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for ESP32-C6 LP_SYS register operations

#pragma once

#include <stdlib.h>
#include "soc/soc.h"
#include "soc/lp_system_struct.h"
#include "hal/misc.h"
#include "esp32p4/rom/rtc.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ROM obtains the wake-up type through LP_SYS_STORE9_REG[0].
 *         Set the flag to inform
 * @param true: deepsleep      false: lightsleep
 */
static inline  void lp_sys_ll_inform_wakeup_type(bool dslp)
{
    if (dslp) {
        REG_SET_BIT(RTC_SLEEP_MODE_REG, BIT(0));    /* Tell rom to run deep sleep wake stub */

    } else {
        REG_CLR_BIT(RTC_SLEEP_MODE_REG, BIT(0));    /* Tell rom to run light sleep wake stub */
    }
}

#ifdef __cplusplus
}
#endif
