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
 * @file esp_clk_internal.h
 *
 * Private clock-related functions
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
 * @brief Disables clock of some peripherals
 *
 * Called from cpu_start.c, not intended to be called from other places.
 * This function disables clock of useless peripherals when cpu starts.
 */
void esp_perip_clk_init(void);

/* Selects an external clock source (32 kHz) for RTC.
 * Only internal use in unit test.
 */
void rtc_clk_select_rtc_slow_clk(void);
