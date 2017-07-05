// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

/**
 * @file esp_clk.h
 *
 * This file contains declarations of clock related functions.
 * These functions are used in ESP-IDF components, but should not be considered
 * to be part of public API.
 */

/**
 * @brief Initialize clock-related settings
 *
 * Called from cpu_start.c, not intended to be called from other places.
 * This function configures the CPU clock, RTC slow and fast clocks, and
 * performs RTC slow clock calibration.
 */
void esp_clk_init(void);


/**
 * @brief Get the calibration value of RTC slow clock
 *
 * The value is in the same format as returned by rtc_clk_cal (microseconds,
 * in Q13.19 fixed-point format).
 *
 * @return the calibration value obtained using rtc_clk_cal, at startup time
 */
uint32_t esp_clk_slowclk_cal_get();


/**
 * @brief Update the calibration value of RTC slow clock
 *
 * The value has to be in the same format as returned by rtc_clk_cal (microseconds,
 * in Q13.19 fixed-point format).
 * This value is used by timekeeping functions (such as gettimeofday) to
 * calculate current time based on RTC counter value.
 * @param value calibration value obtained using rtc_clk_cal
 */
void esp_clk_slowclk_cal_set(uint32_t value);

