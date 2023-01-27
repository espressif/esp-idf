/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

// ECC peripheral output at 160MHz
#define IDF_PERFORMANCE_MAX_ECP_P192_POINT_MULTIPLY_OP                          5000
#define IDF_PERFORMANCE_MAX_ECP_P192_POINT_VERIFY_OP                            60
#define IDF_PERFORMANCE_MAX_ECP_P256_POINT_MULTIPLY_OP                          8400
#define IDF_PERFORMANCE_MAX_ECP_P256_POINT_VERIFY_OP                            70

#define IDF_PERFORMANCE_MAX_ECDSA_P192_VERIFY_OP                                18000
#define IDF_PERFORMANCE_MAX_ECDSA_P256_VERIFY_OP                                27000

#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING                            34
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_POLLING                               17
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING_NO_DMA                     32
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_POLLING_NO_DMA                        15
