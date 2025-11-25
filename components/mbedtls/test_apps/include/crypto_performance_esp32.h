/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"


// AES-CBC hardware throughput (accounts for worst-case performance with PSRAM workaround)
#define IDF_PERFORMANCE_MIN_AES_CBC_THROUGHPUT_MBSEC                            8.2

// SHA256 hardware throughput at 240MHz, threshold set lower than worst case
#if CONFIG_FREERTOS_SMP // IDF-5826
#define IDF_PERFORMANCE_MIN_SHA256_THROUGHPUT_MBSEC                             6.0
#else
#define IDF_PERFORMANCE_MIN_SHA256_THROUGHPUT_MBSEC                             8.0
#endif

#define IDF_PERFORMANCE_MAX_TIME_SHA1_32KB                                      5000
#define IDF_PERFORMANCE_MAX_TIME_SHA512_32KB                                    4500

#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PUBLIC_OP                               19000
#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PRIVATE_OP                              750000
#define IDF_PERFORMANCE_MAX_RSA_3072KEY_PUBLIC_OP                               33000
#define IDF_PERFORMANCE_MAX_RSA_3072KEY_PRIVATE_OP                              950000
#define IDF_PERFORMANCE_MAX_RSA_4096KEY_PUBLIC_OP                               90000
#define IDF_PERFORMANCE_MAX_RSA_4096KEY_PRIVATE_OP                              3000000
