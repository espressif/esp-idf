/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

/*
 * The time spend (T) to complete a PPA transaction is proportional to the amount of pixels (x) need to be processed.
 * T = k * x + b
 * k = (T - b) / x
 */

#if CONFIG_IDF_TARGET_ESP32P4
#if CONFIG_ESP32P4_SELECTS_REV_LESS_V3
#define PPA_SRM_MIN_PERFORMANCE_PX_PER_SEC      (21000 * 1000)  // k_min
#define PPA_SRM_TIME_OFFSET                     (-26000)        // b_approx
#else
#define PPA_SRM_MIN_PERFORMANCE_PX_PER_SEC      (35000 * 1000)  // k_min
#define PPA_SRM_TIME_OFFSET                     (-37000)        // b_approx
#endif

#define PPA_BLEND_MIN_PERFORMANCE_PX_PER_SEC    (31500 * 1000)  // k_min
#define PPA_BLEND_TIME_OFFSET                   (-37150)        // b_approx

#define PPA_FILL_MIN_PERFORMANCE_PX_PER_SEC     (150000 * 1000)  // k_min
#define PPA_FILL_TIME_OFFSET                    (-106000)        // b_approx
#endif
