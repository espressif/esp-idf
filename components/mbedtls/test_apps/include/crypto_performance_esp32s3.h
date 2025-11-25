/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#define IDF_PERFORMANCE_MIN_AES_CBC_THROUGHPUT_MBSEC                            43

#define IDF_PERFORMANCE_MIN_SHA256_THROUGHPUT_MBSEC                             90
#define IDF_PERFORMANCE_MAX_TIME_SHA1_32KB                                      1000
#define IDF_PERFORMANCE_MAX_TIME_SHA512_32KB                                    900

#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PUBLIC_OP                               23000
#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PRIVATE_OP                              700000
#define IDF_PERFORMANCE_MAX_RSA_3072KEY_PUBLIC_OP                               45000
#define IDF_PERFORMANCE_MAX_RSA_3072KEY_PRIVATE_OP                              1300000
#define IDF_PERFORMANCE_MAX_RSA_4096KEY_PUBLIC_OP                               80000
#define IDF_PERFORMANCE_MAX_RSA_4096KEY_PRIVATE_OP                              3500000
