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
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file esp_clk.h
 *
 * This file contains declarations of clock related functions.
 */

/**
 * @brief Get the calibration value of RTC slow clock
 *
 * The value is in the same format as returned by rtc_clk_cal (microseconds,
 * in Q13.19 fixed-point format).
 *
 * @return the calibration value obtained using rtc_clk_cal, at startup time
 */
uint32_t esp_clk_slowclk_cal_get(void);

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

/**
 * @brief Return current CPU clock frequency
 * When frequency switching is performed, this frequency may change.
 * However it is guaranteed that the frequency never changes with a critical
 * section.
 *
 * @return CPU clock frequency, in Hz
 */
int esp_clk_cpu_freq(void);

/**
 * @brief Return current APB clock frequency
 *
 * When frequency switching is performed, this frequency may change.
 * However it is guaranteed that the frequency never changes with a critical
 * section.
 *
 * @return APB clock frequency, in Hz
 */
int esp_clk_apb_freq(void);

/**
 * @brief Return frequency of the main XTAL
 *
 * Frequency of the main XTAL can be either auto-detected or set at compile
 * time (see CONFIG_ESP32_XTAL_FREQ_SEL sdkconfig option). In both cases, this
 * function returns the actual value at run time.
 *
 * @return XTAL frequency, in Hz
 */
int esp_clk_xtal_freq(void);


/**
 * @brief Read value of RTC counter, converting it to microseconds
 * @attention The value returned by this function may change abruptly when
 * calibration value of RTC counter is updated via esp_clk_slowclk_cal_set
 * function. This should not happen unless application calls esp_clk_slowclk_cal_set.
 * In ESP-IDF, esp_clk_slowclk_cal_set is only called in startup code.
 *
 * @return Value or RTC counter, expressed in microseconds
 */
uint64_t esp_clk_rtc_time(void);

#ifdef __cplusplus
}
#endif
