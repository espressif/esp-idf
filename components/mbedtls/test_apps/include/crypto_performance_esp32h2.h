/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
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
