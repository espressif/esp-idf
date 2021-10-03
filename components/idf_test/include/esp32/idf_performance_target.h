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

// AES-CBC hardware throughput (accounts for worst-case performance with PSRAM workaround)
#define IDF_PERFORMANCE_MIN_AES_CBC_THROUGHPUT_MBSEC                            8.2

// SHA256 hardware throughput at 240MHz, threshold set lower than worst case
#define IDF_PERFORMANCE_MIN_SHA256_THROUGHPUT_MBSEC                             8.0
// esp_sha() time to process 32KB of input data from RAM
#define IDF_PERFORMANCE_MAX_TIME_SHA1_32KB                                      5000
#define IDF_PERFORMANCE_MAX_TIME_SHA512_32KB                                    4500

#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PUBLIC_OP                               19000
#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PRIVATE_OP                              190000
#define IDF_PERFORMANCE_MAX_RSA_3072KEY_PUBLIC_OP                               33000
#define IDF_PERFORMANCE_MAX_RSA_3072KEY_PRIVATE_OP                              360000
#define IDF_PERFORMANCE_MAX_RSA_4096KEY_PUBLIC_OP                               90000
#define IDF_PERFORMANCE_MAX_RSA_4096KEY_PRIVATE_OP                              870000

#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING                            30
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING_NO_DMA                     27

/*
 * Flash Performance value
 * 4 subsections: legacy, normal (new driver after v4.0), SPI1 (external but on SPI1), external (SPI2)
 * These thresholds are set to about 70% of the average test data, under certain condition.
 * Contact Espressif for details.
 */
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_WR_4B
//The single_core config is much faster than other configs. Use the value of other configs
//Collect data and correct it later
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_WR_4B               0
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_RD_4B
//The single_core config is much faster than other configs. Use the value of other configs
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_RD_4B               35300
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_WR_2KB
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_WR_2KB              (697*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_RD_2KB
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_RD_2KB              (6780*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_ERASE
//erase performance is highly depending on the chip vendor. Use 70% of the minimal value.
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_ERASE               11200
#endif

#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_WR_4B
//The single_core config is much faster than other configs. Use the value of other configs
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_WR_4B                      20100
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_RD_4B
//The single_core config is much faster than other configs. Use the value of other configs
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_RD_4B                      35200
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_WR_2KB
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_WR_2KB                     (754*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_RD_2KB
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_RD_2KB                     (6650*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_ERASE
//erase performance is highly depending on the chip vendor. Use 70% of the minimal value.
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_ERASE                      45300
#endif

#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_WR_4B
//The single_core config is much faster than other configs. Use the value of other configs
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_WR_4B                 16200
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_RD_4B
//The single_core config is much faster than other configs. Use the value of other configs
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_RD_4B                 33600
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_WR_2KB
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_WR_2KB                (484*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_RD_2KB
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_RD_2KB                (1512*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_ERASE
//erase performance is highly depending on the chip vendor. Use 70% of the minimal value.
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_ERASE                 49600
#endif

#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_WR_4B
//Collect data and correct it later
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_WR_4B                  0
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_RD_4B
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_RD_4B                  (261*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_WR_2KB
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_WR_2KB                 (470*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_RD_2KB
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_RD_2KB                 (261*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_ERASE
//erase performance is highly depending on the chip vendor. Use 70% of the minimal value.
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_ERASE                  30900
#endif

// floating point instructions per divide and per sqrt (configured for worst-case with PSRAM workaround)
#define IDF_PERFORMANCE_MAX_CYCLES_PER_DIV                                      70
#define IDF_PERFORMANCE_MAX_CYCLES_PER_SQRT                                     140
