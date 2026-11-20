/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_lp_timer_shared.h"
#include "test_helper.h"

int main(void)
{
    static uint8_t run_count = 0;
    if (run_count == 0) {
        run_count++;
    } else {
        test_touch_simulate_touch(TEST_TOUCH_WAKEUP_CHANNEL, true);
        ulp_lp_core_delay_us(TEST_TOUCH_SIMULATE_HOLD_TIME_US);
        test_touch_simulate_touch(TEST_TOUCH_WAKEUP_CHANNEL, false);
        ulp_lp_core_lp_timer_disable();
    }
    return 0;
}
