/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "test_utils.h"
#include "esp_private/periph_ctrl.h"
#include "soc/periph_defs.h"
#include "hal/timer_hal.h"
#include "hal/timer_ll.h"

#define TIMER_GROUP_ID (1)
#define TIMER_ID (0)
static timer_hal_context_t timer_hal;

void ref_clock_init(void)
{
    periph_module_enable(PERIPH_TIMG1_MODULE);
    timer_hal_init(&timer_hal, TIMER_GROUP_ID, TIMER_ID);
    timer_ll_set_clock_source(timer_hal.dev, TIMER_ID, GPTIMER_CLK_SRC_XTAL); // Select XTAL, so ref_clock is independent of the APL clock
    timer_ll_enable_counter(timer_hal.dev, TIMER_ID, false); // stop timer from running
    timer_ll_set_clock_prescale(timer_hal.dev, TIMER_ID, 40); // Resolution is configured to 1MHz
    timer_ll_set_count_direction(timer_hal.dev, timer_hal.timer_id, GPTIMER_COUNT_UP); // increase mode
    timer_hal_set_counter_value(&timer_hal, 0); // initial count value to zero
    timer_ll_enable_intr(timer_hal.dev, TIMER_LL_EVENT_ALARM(TIMER_ID), false); // disable interrupt
    timer_ll_enable_alarm(timer_hal.dev, TIMER_ID, false); // alarm event is not needed
    timer_ll_enable_auto_reload(timer_hal.dev, TIMER_ID, false);
    timer_ll_enable_counter(timer_hal.dev, TIMER_ID, true); // start counter
}

void ref_clock_deinit(void)
{
    timer_ll_enable_counter(timer_hal.dev, TIMER_ID, false); // stop timer from running
    periph_module_disable(PERIPH_TIMG1_MODULE);
}

uint64_t ref_clock_get(void)
{
    return timer_ll_get_counter_value(timer_hal.dev, timer_hal.timer_id);
}
