/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
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
#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PRIVATE_OP                              130000
#define IDF_PERFORMANCE_MAX_RSA_3072KEY_PUBLIC_OP                               36000
#define IDF_PERFORMANCE_MAX_RSA_3072KEY_PRIVATE_OP                              400000
#define IDF_PERFORMANCE_MAX_RSA_4096KEY_PUBLIC_OP                               62000
#define IDF_PERFORMANCE_MAX_RSA_4096KEY_PRIVATE_OP                              800000

#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING                            32
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING_NO_DMA                     30

/*
 * Flash Performance value
 * 4 subsections: legacy, normal (new driver after v4.0), SPI1 (external but on SPI1), external (SPI2)
 * These thresholds are set to about 70% of the average test data, under certain condition.
 * Contact Espressif for details.
 */
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_WR_4B
//Great variation, use the 70% of min value
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_WR_4B               33300
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_RD_4B
// legacy config is much faster. use the 70% of slower configs
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_RD_4B               (239*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_WR_2KB
// Weird data. Use 70% average of slower configs.
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_WR_2KB              (546*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_RD_2KB
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_RD_2KB              (1191*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_ERASE
//erase performance is highly depending on the chip vendor. Use 70% of the minimal value.
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_LEGACY_ERASE               19500
#endif

#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_WR_4B
//release config is much faster than other configs. Use 70% average of other configs
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_WR_4B                      46300
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_RD_4B
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_RD_4B                      (249*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_WR_2KB
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_WR_2KB                     (851*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_RD_2KB
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_RD_2KB                     (11480*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_ERASE
//erase performance is highly depending on the chip vendor. Use 70% of the minimal value.
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_ERASE                      40100
#endif

#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_WR_4B
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_WR_4B                 30500
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_RD_4B
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_RD_4B                 (183*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_WR_2KB
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_WR_2KB                (474*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_RD_2KB
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_RD_2KB                (1283*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_ERASE
//erase performance is highly depending on the chip vendor. Use 70% of the minimal value.
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_SPI1_ERASE                 44100
#endif

#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_WR_4B
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_WR_4B                  47800
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_RD_4B
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_RD_4B                  (252*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_WR_2KB
// write with large RAM buffer tests has lower performance value than normal performance tests
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_WR_2KB                 (398*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_RD_2KB
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_RD_2KB                 (1204*1000)
#endif
#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_ERASE
//erase performance is highly depending on the chip vendor. Use 70% of the minimal value.
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_ERASE                  22100
#endif
