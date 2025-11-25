/* SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#if __has_include("freertos_performance_esp32p4.h")
#include "freertos_performance_esp32p4.h"
#endif

#ifndef IDF_PERFORMANCE_MAX_FREERTOS_SPINLOCK_CYCLES_PER_OP
#define IDF_PERFORMANCE_MAX_FREERTOS_SPINLOCK_CYCLES_PER_OP                     215
#endif
#ifndef IDF_PERFORMANCE_MAX_FREERTOS_SPINLOCK_CYCLES_PER_OP_PSRAM
#define IDF_PERFORMANCE_MAX_FREERTOS_SPINLOCK_CYCLES_PER_OP_PSRAM               300
#endif
#ifndef IDF_PERFORMANCE_MAX_FREERTOS_SPINLOCK_CYCLES_PER_OP_UNICORE
#define IDF_PERFORMANCE_MAX_FREERTOS_SPINLOCK_CYCLES_PER_OP_UNICORE             130
#endif

#ifndef IDF_PERFORMANCE_MAX_ISR_ENTER_CYCLES
#define IDF_PERFORMANCE_MAX_ISR_ENTER_CYCLES                                    290
#endif
#ifndef IDF_PERFORMANCE_MAX_ISR_EXIT_CYCLES
#define IDF_PERFORMANCE_MAX_ISR_EXIT_CYCLES                                     565
#endif

#ifndef IDF_PERFORMANCE_MAX_SCHEDULING_TIME
#define IDF_PERFORMANCE_MAX_SCHEDULING_TIME                                     2000
#endif
