/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"
#include "soc/soc_caps.h"

/*
 * SOC_CLK_RC_FAST_FREQ_APPROX is a nominal value and can be off by ~10% from the
 * clock the LP core actually runs at, e.g. 17.5 MHz nominal vs 16.0 MHz measured
 * on ESP32-C6. Hardcode a per-target estimate obtained from the "Test LP core
 * delay" test case instead. Values must be a multiple of 500 kHz, otherwise
 * LP_CORE_CYCLES_PER_US_NUM below silently rounds them down.
 */
#if CONFIG_IDF_TARGET_ESP32C5
#define LP_CORE_RC_FAST_FREQUENCY_HZ 16500000U
#elif CONFIG_IDF_TARGET_ESP32C6
#define LP_CORE_RC_FAST_FREQUENCY_HZ 16000000U
#elif CONFIG_IDF_TARGET_ESP32P4
#define LP_CORE_RC_FAST_FREQUENCY_HZ 16500000U
#elif CONFIG_IDF_TARGET_ESP32S31
#define LP_CORE_RC_FAST_FREQUENCY_HZ 17500000U
#else
#error "LP core RC_FAST frequency has not been measured for this target"
#endif

#if (LP_CORE_RC_FAST_FREQUENCY_HZ % 500000U) != 0U
#error "LP_CORE_RC_FAST_FREQUENCY_HZ must be a multiple of 500 kHz"
#endif

/* LP_FAST_CLK is not very accurate, for now use a target-specific rough estimate */
#if CONFIG_RTC_FAST_CLK_SRC_RC_FAST
#define LP_CORE_CPU_FREQUENCY_HZ LP_CORE_RC_FAST_FREQUENCY_HZ
#define LP_CORE_CYCLES_PER_US_NUM (LP_CORE_RC_FAST_FREQUENCY_HZ / 500000U)
#define LP_CORE_CYCLES_PER_US_DENOM 2U
#elif CONFIG_RTC_FAST_CLK_SRC_XTAL
#if SOC_XTAL_SUPPORT_48M
#define LP_CORE_CPU_FREQUENCY_HZ 48000000U
#define LP_CORE_CYCLES_PER_US_NUM 48U
#define LP_CORE_CYCLES_PER_US_DENOM 1U
#else
#define LP_CORE_CPU_FREQUENCY_HZ 40000000U
#define LP_CORE_CYCLES_PER_US_NUM 40U
#define LP_CORE_CYCLES_PER_US_DENOM 1U
#endif
#else  // Default value in chip without rtc fast clock sel option
#define LP_CORE_CPU_FREQUENCY_HZ LP_CORE_RC_FAST_FREQUENCY_HZ
#define LP_CORE_CYCLES_PER_US_NUM (LP_CORE_RC_FAST_FREQUENCY_HZ / 500000U)
#define LP_CORE_CYCLES_PER_US_DENOM 2U
#endif

/**
 * @brief Compensation for LP core delay function overhead in CPU cycles.
 *
 * @details Estimated cycles consumed by delay loop function call and measurement
 * overhead. Derived from empirical "LP core delay calibration" test measurements.
 * Used by delay functions to improve accuracy for short durations.
 *
 * @note Value is calibration-specific and may vary with compiler optimization.
 */
#define ULP_LP_CORE_DELAY_CALL_OVERHEAD_IN_CYCLES 14U
