/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"

// Chip-Specific Data
#if CONFIG_IDF_TARGET_ESP32
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

#elif CONFIG_IDF_TARGET_ESP32S2
#define IDF_PERFORMANCE_MIN_AES_CBC_THROUGHPUT_MBSEC                            43.0
#define IDF_PERFORMANCE_MIN_AES_GCM_CRYPT_TAG_THROUGHPUT_MBSEC                  30.0
#define IDF_PERFORMANCE_MIN_AES_GCM_UPDATE_THROUGHPUT_MBSEC                     2.1

// SHA256 hardware throughput at 240MHz, threshold set lower than worst case
#define IDF_PERFORMANCE_MIN_SHA256_THROUGHPUT_MBSEC                             90.0
// esp_sha() time to process 32KB of input data from RAM
#define IDF_PERFORMANCE_MAX_TIME_SHA1_32KB                                      900
#define IDF_PERFORMANCE_MAX_TIME_SHA512_32KB                                    900

#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PUBLIC_OP                               15500
#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PRIVATE_OP                              650000
#define IDF_PERFORMANCE_MAX_RSA_3072KEY_PUBLIC_OP                               36000
#define IDF_PERFORMANCE_MAX_RSA_3072KEY_PRIVATE_OP                              960000
#define IDF_PERFORMANCE_MAX_RSA_4096KEY_PUBLIC_OP                               62000
#define IDF_PERFORMANCE_MAX_RSA_4096KEY_PRIVATE_OP                              2850000

#elif CONFIG_IDF_TARGET_ESP32S3
#define IDF_PERFORMANCE_MIN_AES_CBC_THROUGHPUT_MBSEC                            43

#define IDF_PERFORMANCE_MIN_SHA256_THROUGHPUT_MBSEC                             90
#define IDF_PERFORMANCE_MAX_TIME_SHA1_32KB                                      1000
#define IDF_PERFORMANCE_MAX_TIME_SHA512_32KB                                    900

#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PUBLIC_OP                               24000
#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PRIVATE_OP                              700000
#define IDF_PERFORMANCE_MAX_RSA_3072KEY_PUBLIC_OP                               45000
#define IDF_PERFORMANCE_MAX_RSA_3072KEY_PRIVATE_OP                              1300000
#define IDF_PERFORMANCE_MAX_RSA_4096KEY_PUBLIC_OP                               80000
#define IDF_PERFORMANCE_MAX_RSA_4096KEY_PRIVATE_OP                              3500000

#elif CONFIG_IDF_TARGET_ESP32C2
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

#elif CONFIG_IDF_TARGET_ESP32C3
#define IDF_PERFORMANCE_MIN_AES_CBC_THROUGHPUT_MBSEC                            43

// SHA256 hardware throughput at 160 MHz, threshold set lower than worst case
#define IDF_PERFORMANCE_MIN_SHA256_THROUGHPUT_MBSEC                             90
// esp_sha() time to process 32KB of input data from RAM
#define IDF_PERFORMANCE_MAX_TIME_SHA1_32KB                                      560

#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PUBLIC_OP                               19000
#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PRIVATE_OP                              210000
#define IDF_PERFORMANCE_MAX_RSA_3072KEY_PUBLIC_OP                               45000
#define IDF_PERFORMANCE_MAX_RSA_3072KEY_PRIVATE_OP                              670000

#elif CONFIG_IDF_TARGET_ESP32C6
#define IDF_PERFORMANCE_MAX_ECP_P192_POINT_MULTIPLY_OP                          5000
#define IDF_PERFORMANCE_MAX_ECP_P192_POINT_VERIFY_OP                            60
#define IDF_PERFORMANCE_MAX_ECP_P256_POINT_MULTIPLY_OP                          8400
#define IDF_PERFORMANCE_MAX_ECP_P256_POINT_VERIFY_OP                            70

#define IDF_PERFORMANCE_MAX_ECDSA_P192_VERIFY_OP                                18000
#define IDF_PERFORMANCE_MAX_ECDSA_P256_VERIFY_OP                                27000

#elif CONFIG_IDF_TARGET_ESP32H2
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
#endif
