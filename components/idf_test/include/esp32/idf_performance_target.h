/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

// AES-CBC hardware throughput (accounts for worst-case performance with PSRAM workaround)
#define IDF_PERFORMANCE_MIN_AES_CBC_THROUGHPUT_MBSEC                            8.2

// SHA256 hardware throughput at 240MHz, threshold set lower than worst case
#if CONFIG_FREERTOS_SMP // IDF-5222
#define IDF_PERFORMANCE_MIN_SHA256_THROUGHPUT_MBSEC                             6.0
#else
#define IDF_PERFORMANCE_MIN_SHA256_THROUGHPUT_MBSEC                             8.0
#endif
// esp_sha() time to process 32KB of input data from RAM
#define IDF_PERFORMANCE_MAX_TIME_SHA1_32KB                                      5000
#define IDF_PERFORMANCE_MAX_TIME_SHA512_32KB                                    4500

#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PUBLIC_OP                               19000
#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PRIVATE_OP                              450000
#define IDF_PERFORMANCE_MAX_RSA_3072KEY_PUBLIC_OP                               33000
#define IDF_PERFORMANCE_MAX_RSA_3072KEY_PRIVATE_OP                              950000
#define IDF_PERFORMANCE_MAX_RSA_4096KEY_PUBLIC_OP                               90000
#define IDF_PERFORMANCE_MAX_RSA_4096KEY_PRIVATE_OP                              1900000

#define IDF_PERFORMANCE_MAX_SPI_CLK_FREQ                                        16*1000*1000
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_POLLING                               15
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_POLLING_NO_DMA                        15
#if !CONFIG_FREERTOS_SMP // IDF-5223
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING                            34      // TODO: IDF-5180
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING_NO_DMA                     30      // TODO: IDF-5180
#else
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING                            50
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING_NO_DMA                     50
#endif

// floating point instructions per divide and per sqrt (configured for worst-case with PSRAM workaround)
#define IDF_PERFORMANCE_MAX_CYCLES_PER_DIV                                      70
#define IDF_PERFORMANCE_MAX_CYCLES_PER_SQRT                                     140

#define IDF_PERFORMANCE_MAX_ADC_CONTINUOUS_STD_ATTEN3_NO_FILTER                 3
#define IDF_PERFORMANCE_MAX_ADC_ONESHOT_STD_ATTEN3                              3
