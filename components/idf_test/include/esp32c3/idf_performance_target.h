// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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

#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING                            32
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING_NO_DMA                     30

/*
 * Flash Performance value
 * 4 subsections: legacy, normal (new driver after v4.0), SPI1 (external but on SPI1), external (SPI2)
 * These thresholds are set to about 70% of the average test data, under certain condition.
 * Contact Espressif for details.
 */
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_WR_4B
// rom options is much slower. use its 70%
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_WR_4B               42200
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_RD_4B
// legacy & suspend config are much faster. use the 70% of slower configs
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_RD_4B               (179*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_WR_2KB
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_WR_2KB              (622*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_RD_2KB
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_RD_2KB              (6536*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_ERASE
//erase performance is highly depending on the chip vendor. Use 70% of the minimal value.
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_ERASE               23700
#endif

#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_WR_4B
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_WR_4B                      46400
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_RD_4B
// legacy & suspend config are much faster. use the 70% of slower configs
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_RD_4B                      (183*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_WR_2KB
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_WR_2KB                     (605*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_RD_2KB
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_RD_2KB                     (6676*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_ERASE
//erase performance is highly depending on the chip vendor. Use 70% of the minimal value.
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_ERASE                      22900
#endif

// No SPI1 tests for C3
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_WR_4B
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_WR_4B                 0
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_RD_4B
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_RD_4B                 0
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_WR_2KB
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_WR_2KB                0
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_RD_2KB
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_RD_2KB                0
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_ERASE
//erase performance is highly depending on the chip vendor. Use 70% of the minimal value.
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_ERASE                 0
#endif

#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_WR_4B
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_WR_4B                  43300
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_RD_4B
// rom options is much slower. use its 70%
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_RD_4B                  99500
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_WR_2KB
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_WR_2KB                 (300*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_RD_2KB
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_RD_2KB                 (754*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_ERASE
//erase performance is highly depending on the chip vendor. Use 70% of the minimal value.
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_ERASE                  33900
#endif

// floating point instructions per divide and per sqrt (configured for worst-case with PSRAM workaround)
#define IDF_PERFORMANCE_MAX_CYCLES_PER_DIV                                      70
#define IDF_PERFORMANCE_MAX_CYCLES_PER_SQRT                                     140
