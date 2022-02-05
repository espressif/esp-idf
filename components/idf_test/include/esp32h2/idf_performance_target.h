/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
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

#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_POLLING                               15
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_POLLING_NO_DMA                        15
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING                            32
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING_NO_DMA                     30

#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_RD_4B
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_RD_4B               53400
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_WR_2KB
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_WR_2KB              (701*1000)
#endif

#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_WR_4B
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_WR_4B                      0
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_RD_4B
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_RD_4B                      53600
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_ERASE
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_ERASE                      44300
#endif

#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_WR_4B
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_WR_4B                 24400
#endif

#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_WR_4B
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_WR_4B                  64900
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_RD_4B
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_RD_4B                  (309*1000)
#endif

#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_RD_2KB
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_RD_2KB                 (1697*1000)
#endif

#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_ERASE
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_ERASE                  76600
#endif

// floating point instructions per divide and per sqrt (configured for worst-case with PSRAM workaround)
#define IDF_PERFORMANCE_MAX_CYCLES_PER_DIV                                      70
#define IDF_PERFORMANCE_MAX_CYCLES_PER_SQRT                                     140
