/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define IDF_PERFORMANCE_MIN_AES_CBC_THROUGHPUT_MBSEC                            43

// SHA256 hardware throughput at 240MHz, threshold set lower than worst case
#define IDF_PERFORMANCE_MIN_SHA256_THROUGHPUT_MBSEC                             90
// esp_sha() time to process 32KB of input data from RAM
#define IDF_PERFORMANCE_MAX_TIME_SHA1_32KB                                      1000
#define IDF_PERFORMANCE_MAX_TIME_SHA512_32KB                                    900

#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PUBLIC_OP                               18000
#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PRIVATE_OP                              490000
#define IDF_PERFORMANCE_MAX_RSA_3072KEY_PUBLIC_OP                               45000
#define IDF_PERFORMANCE_MAX_RSA_3072KEY_PRIVATE_OP                              1300000
#define IDF_PERFORMANCE_MAX_RSA_4096KEY_PUBLIC_OP                               80000
#define IDF_PERFORMANCE_MAX_RSA_4096KEY_PRIVATE_OP                              2500000

// floating point instructions per divide and per sqrt (configured for worst-case with PSRAM workaround)
#define IDF_PERFORMANCE_MAX_CYCLES_PER_DIV                                      70
#define IDF_PERFORMANCE_MAX_CYCLES_PER_SQRT                                     140

#define IDF_PERFORMANCE_MAX_ADC_CONTINUOUS_STD_ATTEN3_NO_FILTER                 4
#define IDF_PERFORMANCE_MAX_ADC_CONTINUOUS_STD_ATTEN3_FILTER_2                  4
#define IDF_PERFORMANCE_MAX_ADC_CONTINUOUS_STD_ATTEN3_FILTER_4                  4
#define IDF_PERFORMANCE_MAX_ADC_CONTINUOUS_STD_ATTEN3_FILTER_8                  4
#define IDF_PERFORMANCE_MAX_ADC_CONTINUOUS_STD_ATTEN3_FILTER_16                 4
#define IDF_PERFORMANCE_MAX_ADC_CONTINUOUS_STD_ATTEN3_FILTER_64                 4
#define IDF_PERFORMANCE_MAX_ADC_ONESHOT_STD_ATTEN3                              4
