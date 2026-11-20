/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#define TEST_TOUCH_WAKEUP_CHANNEL        4
#define TEST_TOUCH_SIMULATE_HOLD_TIME_US 10000

#ifndef __ASSEMBLER__
#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "hal/touch_sensor_ll.h"

static inline void test_touch_simulate_touch(int chan_id, bool active)
{
#if SOC_IS(ESP32S31)
    /* ESP32-S31 has no internal capacitor, emulate touch by changing charging cycles. */
    for (int i = 0; i < TOUCH_SAMPLE_CFG_NUM; i++) {
        uint16_t ct;
        touch_ll_get_charge_times(i, &ct);
        /**
         * assume original ct = 8q + r, 0 <= r < 8
         * activated ct = 8q + r + q = 9q + r
         * deactivated ct = (9q + r) - q = 8q + r = original ct
         */
        ct = active ? ct + (ct >> 3) : ct - (ct / 9);
        touch_ll_set_charge_times(i, ct);
    }
#elif SOC_TOUCH_SENSOR_VERSION <= 2
    touch_ll_set_charge_speed(chan_id, active ? TOUCH_CHARGE_SPEED_4 : TOUCH_CHARGE_SPEED_7);
#elif SOC_TOUCH_SENSOR_VERSION == 3
    touch_ll_set_internal_capacitor(active ? 0x7f : 0);
#endif
}
#endif  // __ASSEMBLER__
