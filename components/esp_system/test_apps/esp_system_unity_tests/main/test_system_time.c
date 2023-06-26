/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include "unity.h"

#include "esp_private/system_internal.h"
#include "esp_private/esp_clk.h"

TEST_CASE("Test effect of rtc clk calibration compensation on system time", "[esp_system]")
{
    uint32_t prev_cal = esp_clk_slowclk_cal_get();
    int64_t t1 = esp_system_get_time();

    // Modify calibration value
    esp_clk_slowclk_cal_set(prev_cal/2);

    // Internally, the origin point of rtc clk has been adjusted
    // so that t2 > t1 remains true
    int64_t t2 = esp_system_get_time();

    TEST_ASSERT_GREATER_THAN(t1, t2);

    // Restore calibration value
    esp_clk_slowclk_cal_set(prev_cal);

    t2 = esp_system_get_time();

    TEST_ASSERT_GREATER_THAN(t1, t2);
}
