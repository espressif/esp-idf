/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdlib.h>
#include <esp_types.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "soc/soc.h"
#include "hal/lp_timer_ll.h"

static DRAM_ATTR struct {
    lp_timer_dev_t *dev;
} lp_timer_context = { .dev = &LP_TIMER };

void IRAM_ATTR lp_timer_hal_set_alarm_target(uint8_t timer_id, uint64_t value)
{
    lp_timer_ll_clear_alarm_intr_status(lp_timer_context.dev);
    lp_timer_ll_set_alarm_target(lp_timer_context.dev, timer_id, value);
    lp_timer_ll_set_target_enable(lp_timer_context.dev, timer_id, true);
}

uint64_t IRAM_ATTR lp_timer_hal_get_cycle_count(void)
{
    /* Shifts current count to buffer 0, and the value in buffer 0 to buffer 1 */
    lp_timer_ll_counter_snapshot(lp_timer_context.dev);

    uint32_t lo = lp_timer_ll_get_counter_value_low(lp_timer_context.dev, 0);
    uint32_t hi = lp_timer_ll_get_counter_value_high(lp_timer_context.dev, 0);

    lp_timer_counter_value_t result = {
        .lo = lo,
        .hi = hi
    };
    return result.val;
}

void IRAM_ATTR lp_timer_hal_clear_alarm_intr_status(void)
{
    lp_timer_ll_clear_alarm_intr_status(lp_timer_context.dev);
}

void IRAM_ATTR lp_timer_hal_clear_overflow_intr_status(void)
{
    lp_timer_ll_clear_overflow_intr_status(lp_timer_context.dev);
}
