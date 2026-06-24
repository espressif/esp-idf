/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"
#include "soc/soc_caps.h"

/* LP_FAST_CLK is not very accurate, for now use a rough estimate */
#if CONFIG_RTC_FAST_CLK_SRC_RC_FAST
#define LP_CORE_CPU_FREQUENCY_HZ 16000000U /* For P4 TRM says 20 MHz by default, but we tune it closer to 16 MHz */
#define LP_CORE_CYCLES_PER_US_NUM 16U
#define LP_CORE_CYCLES_PER_US_DENOM 1U
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
#define LP_CORE_CPU_FREQUENCY_HZ 16000000U
#define LP_CORE_CYCLES_PER_US_NUM 16U
#define LP_CORE_CYCLES_PER_US_DENOM 1U
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
#define ULP_LP_CORE_DELAY_CALL_OVERHEAD_IN_CYCLES 11U
