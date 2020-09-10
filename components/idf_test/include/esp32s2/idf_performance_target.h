#pragma once

#define IDF_PERFORMANCE_MIN_AES_CBC_THROUGHPUT_MBSEC                            43.0
#define IDF_PERFORMANCE_MIN_AES_GCM_THROUGHPUT_MBSEC                            2.1

// SHA256 hardware throughput at 240MHz, threshold set lower than worst case
#define IDF_PERFORMANCE_MIN_SHA256_THROUGHPUT_MBSEC                             90.0
// esp_sha() time to process 32KB of input data from RAM
#define IDF_PERFORMANCE_MAX_TIME_SHA1_32KB                                      900
#define IDF_PERFORMANCE_MAX_TIME_SHA512_32KB                                    900

#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PUBLIC_OP                               13500
#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PRIVATE_OP                              130000
#define IDF_PERFORMANCE_MAX_RSA_4096KEY_PUBLIC_OP                               62000
#define IDF_PERFORMANCE_MAX_RSA_4096KEY_PRIVATE_OP                              800000

#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING                            32
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING_NO_DMA                     30

#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_RD_4B
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_RD_4B                  (309*1000)
#endif

#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_RD_2KB
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_RD_2KB                 (1504*1000)
#endif

#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_ERASE
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_ERASE                  40300
#endif
