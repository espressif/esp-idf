/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/timer_hal.h"
#include "hal/timer_ll.h"

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
