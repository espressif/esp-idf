/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#define IDF_PERFORMANCE_MIN_AES_CBC_THROUGHPUT_MBSEC                            43

// SHA256 hardware throughput at 160 MHz, threshold set lower than worst case
#define IDF_PERFORMANCE_MIN_SHA256_THROUGHPUT_MBSEC                             90
// esp_sha() time to process 32KB of input data from RAM
#define IDF_PERFORMANCE_MAX_TIME_SHA1_32KB                                      560

#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PUBLIC_OP                               19000
#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PRIVATE_OP                              210000
#define IDF_PERFORMANCE_MAX_RSA_3072KEY_PUBLIC_OP                               45000
#define IDF_PERFORMANCE_MAX_RSA_3072KEY_PRIVATE_OP                              670000

#define IDF_PERFORMANCE_MAX_SPI_CLK_FREQ                                        40*1000*1000
#if !CONFIG_FREERTOS_SMP // IDF-5223
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_POLLING                               15
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_POLLING_NO_DMA                        15
#else
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_POLLING                               17
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_POLLING_NO_DMA                        17
#endif

#if !CONFIG_FREERTOS_SMP // IDF-5223
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING                            32
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING_NO_DMA                     30
#else
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING                            60
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING_NO_DMA                     60
#endif

// floating point instructions per divide and per sqrt (configured for worst-case with PSRAM workaround)
#define IDF_PERFORMANCE_MAX_CYCLES_PER_DIV                                      70
#define IDF_PERFORMANCE_MAX_CYCLES_PER_SQRT                                     140

#define IDF_PERFORMANCE_MAX_ADC_CONTINUOUS_STD_ATTEN3_NO_FILTER                 5
#define IDF_PERFORMANCE_MAX_ADC_CONTINUOUS_STD_ATTEN3_FILTER_2                  5
#define IDF_PERFORMANCE_MAX_ADC_CONTINUOUS_STD_ATTEN3_FILTER_4                  5
#define IDF_PERFORMANCE_MAX_ADC_CONTINUOUS_STD_ATTEN3_FILTER_8                  5
#define IDF_PERFORMANCE_MAX_ADC_CONTINUOUS_STD_ATTEN3_FILTER_16                 5
#define IDF_PERFORMANCE_MAX_ADC_CONTINUOUS_STD_ATTEN3_FILTER_64                 5
#define IDF_PERFORMANCE_MAX_ADC_ONESHOT_STD_ATTEN3                              5
