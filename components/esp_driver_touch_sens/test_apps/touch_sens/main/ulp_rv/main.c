/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ulp_riscv_utils.h"
#include "test_helper.h"

int main(void)
{
    static uint8_t run_count = 0;
    if (run_count == 0) {
        run_count++;
    } else {
        test_touch_simulate_touch(TEST_TOUCH_WAKEUP_CHANNEL, true);
        ulp_riscv_delay_us(TEST_TOUCH_SIMULATE_HOLD_TIME_US);
        test_touch_simulate_touch(TEST_TOUCH_WAKEUP_CHANNEL, false);
        ulp_riscv_timer_stop();
    }
    return 0;
}
