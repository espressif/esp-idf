/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define IDF_PERFORMANCE_MIN_AES_CBC_THROUGHPUT_MBSEC                            43.0
#define IDF_PERFORMANCE_MIN_AES_GCM_CRYPT_TAG_THROUGHPUT_MBSEC                  30.0
#define IDF_PERFORMANCE_MIN_AES_GCM_UPDATE_THROUGHPUT_MBSEC                     2.1

// SHA256 hardware throughput at 240MHz, threshold set lower than worst case
#define IDF_PERFORMANCE_MIN_SHA256_THROUGHPUT_MBSEC                             90.0
// esp_sha() time to process 32KB of input data from RAM
#define IDF_PERFORMANCE_MAX_TIME_SHA1_32KB                                      900
#define IDF_PERFORMANCE_MAX_TIME_SHA512_32KB                                    900

#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PUBLIC_OP                               13500
#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PRIVATE_OP                              420000
#define IDF_PERFORMANCE_MAX_RSA_3072KEY_PUBLIC_OP                               36000
#define IDF_PERFORMANCE_MAX_RSA_3072KEY_PRIVATE_OP                              960000
#define IDF_PERFORMANCE_MAX_RSA_4096KEY_PUBLIC_OP                               62000
#define IDF_PERFORMANCE_MAX_RSA_4096KEY_PRIVATE_OP                              1850000

#define IDF_PERFORMANCE_MAX_SPI_CLK_FREQ                                        40*1000*1000
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_POLLING                               15
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_POLLING_NO_DMA                        15
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING                            32
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING_NO_DMA                     30

#define IDF_PERFORMANCE_MAX_ADC_CONTINUOUS_STD_ATTEN3_NO_FILTER                 3
#define IDF_PERFORMANCE_MAX_ADC_CONTINUOUS_STD_ATTEN3_FILTER_2                  3
#define IDF_PERFORMANCE_MAX_ADC_CONTINUOUS_STD_ATTEN3_FILTER_4                  3
#define IDF_PERFORMANCE_MAX_ADC_CONTINUOUS_STD_ATTEN3_FILTER_8                  3
#define IDF_PERFORMANCE_MAX_ADC_CONTINUOUS_STD_ATTEN3_FILTER_16                 3
#define IDF_PERFORMANCE_MAX_ADC_CONTINUOUS_STD_ATTEN3_FILTER_64                 3
#define IDF_PERFORMANCE_MAX_ADC_ONESHOT_STD_ATTEN3                              6
