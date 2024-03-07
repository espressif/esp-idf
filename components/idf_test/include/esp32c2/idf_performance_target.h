/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#ifdef CONFIG_XTAL_FREQ_26
#define IDF_PERFORMANCE_MAX_ESP_TIMER_GET_TIME_PER_CALL                         1900
#else
#define IDF_PERFORMANCE_MAX_ESP_TIMER_GET_TIME_PER_CALL                         1200
#endif

// SHA256 hardware throughput at 160 MHz, threshold set lower than worst case
#define IDF_PERFORMANCE_MIN_SHA256_THROUGHPUT_MBSEC                             14
// esp_sha() time to process 32KB of input data from RAM
#define IDF_PERFORMANCE_MAX_TIME_SHA1_32KB                                      2500

#define IDF_PERFORMANCE_MAX_ECP_P192_POINT_MULTIPLY_OP                          9000
#define IDF_PERFORMANCE_MAX_ECP_P192_POINT_VERIFY_OP                            300
#define IDF_PERFORMANCE_MAX_ECP_P256_POINT_MULTIPLY_OP                          14000
#define IDF_PERFORMANCE_MAX_ECP_P256_POINT_VERIFY_OP                            300

#define IDF_PERFORMANCE_MAX_ECDSA_P192_VERIFY_OP                                32000
#define IDF_PERFORMANCE_MAX_ECDSA_P256_VERIFY_OP                                49000

// floating point instructions per divide and per sqrt (configured for worst-case with PSRAM workaround)
#define IDF_PERFORMANCE_MAX_CYCLES_PER_DIV                                      70
#define IDF_PERFORMANCE_MAX_CYCLES_PER_SQRT                                     140

#define IDF_PERFORMANCE_MAX_ADC_ONESHOT_STD_ATTEN3                              3
