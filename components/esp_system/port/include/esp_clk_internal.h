/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file esp_clk_internal.h
 *
 * Private clock-related functions
 */

/**
 * @brief Initialize rtc-related settings
 *
 * Called from cpu_start.c, not intended to be called from other places.
 * This function configures the power related stuff.
 * After this the MSPI timing tuning can be done.
 */
void esp_rtc_init(void);

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

#if !CONFIG_IDF_TARGET_ESP32C2
/* Selects an external clock source (32 kHz) for RTC.
 * Only internal use in unit test.
 */
void rtc_clk_select_rtc_slow_clk(void);
#endif

#ifdef __cplusplus
}
#endif
