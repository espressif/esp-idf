/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
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
    // stop counter, alarm, auto-reload at first place
    timer_ll_enable_counter(hal->dev, timer_num, false);
    timer_ll_enable_auto_reload(hal->dev, timer_num, false);
    timer_ll_enable_alarm(hal->dev, timer_num, false);
    // enable ETM subsystem if available
#if SOC_TIMER_SUPPORT_ETM
    timer_ll_enable_etm(hal->dev, true);
#endif
}

void timer_hal_deinit(timer_hal_context_t *hal)
{
    // ensure counter, alarm, auto-reload are disabled
    timer_ll_enable_counter(hal->dev, hal->timer_id, false);
    timer_ll_enable_auto_reload(hal->dev, hal->timer_id, false);
    timer_ll_enable_alarm(hal->dev, hal->timer_id, false);
#if SOC_TIMER_SUPPORT_ETM
    timer_ll_enable_etm(hal->dev, false);
#endif
    hal->dev = NULL;
}

void timer_hal_set_counter_value(timer_hal_context_t *hal, uint64_t load_val)
{
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // - `timer_ll_set_reload_value()` will only indicate the `reload_value`
    // - `timer_ll_set_reload_value()` + ``timer_ll_trigger_soft_reload()` can update the HW counter value by software
    // Therefore, after updating the HW counter value, we need to restore the previous `reload_value`.
    // Attention: The following process should be protected by a lock in the driver layer.
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // save current reload value
    uint64_t old_reload = timer_ll_get_reload_value(hal->dev, hal->timer_id);
    timer_ll_set_reload_value(hal->dev, hal->timer_id, load_val);
    timer_ll_trigger_soft_reload(hal->dev, hal->timer_id);
    // restore the previous reload value
    timer_ll_set_reload_value(hal->dev, hal->timer_id, old_reload);
}

uint64_t timer_hal_capture_and_get_counter_value(timer_hal_context_t *hal)
{
    timer_ll_trigger_soft_capture(hal->dev, hal->timer_id);
    return timer_ll_get_counter_value(hal->dev, hal->timer_id);
}
