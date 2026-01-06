/* SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"

// events dispatched per second by event loop library
#if !CONFIG_FREERTOS_SMP // IDF-5826
#if CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ < 100 // Allowing lesser event dispatch performance for slower CPU frequency chips
#ifndef IDF_PERFORMANCE_MIN_EVENT_DISPATCH
#define IDF_PERFORMANCE_MIN_EVENT_DISPATCH                                      11000
#endif
#ifndef IDF_PERFORMANCE_MIN_EVENT_DISPATCH_PSRAM
#define IDF_PERFORMANCE_MIN_EVENT_DISPATCH_PSRAM                                9000
#endif
#else
#ifndef IDF_PERFORMANCE_MIN_EVENT_DISPATCH
#define IDF_PERFORMANCE_MIN_EVENT_DISPATCH                                      25000
#endif
#ifndef IDF_PERFORMANCE_MIN_EVENT_DISPATCH_PSRAM
#define IDF_PERFORMANCE_MIN_EVENT_DISPATCH_PSRAM                                21000
#endif
#endif
#else
#ifndef IDF_PERFORMANCE_MIN_EVENT_DISPATCH
#define IDF_PERFORMANCE_MIN_EVENT_DISPATCH                                      18000
#endif
#ifndef IDF_PERFORMANCE_MIN_EVENT_DISPATCH_PSRAM
#define IDF_PERFORMANCE_MIN_EVENT_DISPATCH_PSRAM                                14000
#endif
#endif
