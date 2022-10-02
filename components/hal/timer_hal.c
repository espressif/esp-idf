/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include "hal/timer_hal.h"
#include "hal/timer_ll.h"
#include "soc/soc_caps.h"

void timer_hal_init(timer_hal_context_t *hal, uint32_t group_num, uint32_t timer_num)
{
    hal->dev = TIMER_LL_GET_HW(group_num);
    hal->timer_id = timer_num;
    // enable peripheral clock
    timer_ll_enable_clock(hal->dev, timer_num, true);
    // stop counter, alarm, auto-reload at first place
    timer_ll_enable_counter(hal->dev, timer_num, false);
    timer_ll_enable_auto_reload(hal->dev, timer_num, false);
    timer_ll_enable_alarm(hal->dev, timer_num, false);
    // enable RTM subsystem if available
#if SOC_TIMER_SUPPORT_ETM
    timer_ll_enable_etm(hal->dev, true);
#endif
}

void timer_hal_deinit(timer_hal_context_t *hal)
{
    // disable peripheral clock
    timer_ll_enable_clock(hal->dev, hal->timer_id, false);
    // ensure counter, alarm, auto-reload are disabled
    timer_ll_enable_counter(hal->dev, hal->timer_id, false);
    timer_ll_enable_auto_reload(hal->dev, hal->timer_id, false);
    timer_ll_enable_alarm(hal->dev, hal->timer_id, false);
#if SOC_TIMER_SUPPORT_ETM
    timer_ll_enable_etm(hal->dev, false);
#endif
    hal->dev = NULL;
}
