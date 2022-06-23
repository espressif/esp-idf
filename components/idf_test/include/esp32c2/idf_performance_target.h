/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

// SHA256 hardware throughput at 160 MHz, threshold set lower than worst case
#define IDF_PERFORMANCE_MIN_SHA256_THROUGHPUT_MBSEC                             14
// esp_sha() time to process 32KB of input data from RAM
#define IDF_PERFORMANCE_MAX_TIME_SHA1_32KB                                      2500

#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_POLLING                               20
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_POLLING_NO_DMA                        15
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING                            45
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING_NO_DMA                     40

// floating point instructions per divide and per sqrt (configured for worst-case with PSRAM workaround)
#define IDF_PERFORMANCE_MAX_CYCLES_PER_DIV                                      70
#define IDF_PERFORMANCE_MAX_CYCLES_PER_SQRT                                     140
