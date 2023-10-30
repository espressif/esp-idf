/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define IDF_PERFORMANCE_MIN_AES_CBC_THROUGHPUT_MBSEC                            43

// SHA256 hardware throughput at 160 MHz, threshold set lower than worst case
#define IDF_PERFORMANCE_MIN_SHA256_THROUGHPUT_MBSEC                             90
// esp_sha() time to process 32KB of input data from RAM
#define IDF_PERFORMANCE_MAX_TIME_SHA1_32KB                                      560

#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PUBLIC_OP                               19000
#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PRIVATE_OP                              210000
#define IDF_PERFORMANCE_MAX_RSA_3072KEY_PUBLIC_OP                               45000
#define IDF_PERFORMANCE_MAX_RSA_3072KEY_PRIVATE_OP                              670000

#define IDF_PERFORMANCE_MAX_ECP_P192_POINT_MULTIPLY_OP                          11000
#define IDF_PERFORMANCE_MAX_ECP_P192_POINT_VERIFY_OP                            300
#define IDF_PERFORMANCE_MAX_ECP_P256_POINT_MULTIPLY_OP                          19000
#define IDF_PERFORMANCE_MAX_ECP_P256_POINT_VERIFY_OP                            300

#define IDF_PERFORMANCE_MAX_ECDSA_P192_VERIFY_OP                                44000
#define IDF_PERFORMANCE_MAX_ECDSA_P256_VERIFY_OP                                67000

#define IDF_PERFORMANCE_MAX_SPI_CLK_FREQ                                        26*1000*1000
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_POLLING                               28
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_POLLING_NO_DMA                        24
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING                            58
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING_NO_DMA                     54

// floating point instructions per divide and per sqrt (configured for worst-case with PSRAM workaround)
#define IDF_PERFORMANCE_MAX_CYCLES_PER_DIV                                      70
#define IDF_PERFORMANCE_MAX_CYCLES_PER_SQRT                                     140

#define IDF_PERFORMANCE_MAX_ADC_CONTINUOUS_STD_ATTEN3_NO_FILTER                 10
#define IDF_PERFORMANCE_MAX_ADC_CONTINUOUS_STD_ATTEN3_FILTER_2                  10
#define IDF_PERFORMANCE_MAX_ADC_CONTINUOUS_STD_ATTEN3_FILTER_4                  10
#define IDF_PERFORMANCE_MAX_ADC_CONTINUOUS_STD_ATTEN3_FILTER_8                  10
#define IDF_PERFORMANCE_MAX_ADC_CONTINUOUS_STD_ATTEN3_FILTER_16                 10
#define IDF_PERFORMANCE_MAX_ADC_CONTINUOUS_STD_ATTEN3_FILTER_64                 10
#define IDF_PERFORMANCE_MAX_ADC_ONESHOT_STD_ATTEN3                              10
