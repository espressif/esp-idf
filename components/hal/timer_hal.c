/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/timer_hal.h"
#include "hal/timer_ll.h"

void timer_hal_init(timer_hal_context_t *hal, uint32_t group_num, uint32_t timer_num)
{
    hal->dev = TIMER_LL_GET_HW(group_num);
    hal->timer_id = timer_num;
}

void timer_hal_set_counter_value(timer_hal_context_t *hal, uint64_t load_val)
{
    // save current reload value
    uint64_t old_reload = timer_ll_get_reload_value(hal->dev, hal->timer_id);
    timer_ll_set_reload_value(hal->dev, hal->timer_id, load_val);
    timer_ll_trigger_soft_reload(hal->dev, hal->timer_id);
    // restore the previous reload value
    timer_ll_set_reload_value(hal->dev, hal->timer_id, old_reload);
}
