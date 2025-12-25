/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"

#if SOC_IS(ESP32) || SOC_IS(ESP32C2) || SOC_IS(ESP32C3) || SOC_IS(ESP32S2) || SOC_IS(ESP32S3)
#include "hal/rtc_timer_ll.h"
#endif

#if SOC_LP_TIMER_SUPPORTED
#include "hal/lp_timer_hal.h"
#include "hal/lp_timer_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set wakeup timer value (for legacy chips with RTC_CNTL)
 *
 * @param ticks Timer value in RTC slow clock ticks
 */
#if SOC_IS(ESP32) || SOC_IS(ESP32C2) || SOC_IS(ESP32C3) || SOC_IS(ESP32S2) || SOC_IS(ESP32S3)
#define rtc_timer_hal_set_wakeup_timer(ticks)                   rtc_timer_ll_set_wakeup_timer(ticks)
#elif SOC_LP_TIMER_SUPPORTED
#define rtc_timer_hal_set_wakeup_timer(ticks)                   lp_timer_hal_set_alarm_target(0, ticks)
#else
#error "RTC timer not supported on this target"
#endif

/**
 * @brief Get current RTC time (for legacy chips with RTC_CNTL)
 *
 * @return Current RTC time in RTC slow clock ticks
 */
#if SOC_IS(ESP32) || SOC_IS(ESP32C2) || SOC_IS(ESP32C3) || SOC_IS(ESP32S2) || SOC_IS(ESP32S3)
#define rtc_timer_hal_get_rtc_time()                            rtc_timer_ll_get_rtc_time()
#elif SOC_LP_TIMER_SUPPORTED
#define rtc_timer_hal_get_rtc_time()                            lp_timer_hal_get_cycle_count()
#else
#error "RTC timer not supported on this target"
#endif

#ifdef __cplusplus
}
#endif
