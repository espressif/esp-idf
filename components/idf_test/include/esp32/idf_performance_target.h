#pragma once

// AES-CBC hardware throughput (accounts for worst-case performance with PSRAM workaround)
#define IDF_PERFORMANCE_MIN_AES_CBC_THROUGHPUT_MBSEC                            8.2
#define IDF_PERFORMANCE_MIN_AES_GCM_THROUGHPUT_MBSEC                            0.5

// SHA256 hardware throughput at 240MHz, threshold set lower than worst case
#define IDF_PERFORMANCE_MIN_SHA256_THROUGHPUT_MBSEC                             8.0
// esp_sha() time to process 32KB of input data from RAM
#define IDF_PERFORMANCE_MAX_TIME_SHA1_32KB                                      5000
#define IDF_PERFORMANCE_MAX_TIME_SHA512_32KB                                    4500

#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PUBLIC_OP                               19000
#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PRIVATE_OP                              190000
#define IDF_PERFORMANCE_MAX_RSA_4096KEY_PUBLIC_OP                               90000
#define IDF_PERFORMANCE_MAX_RSA_4096KEY_PRIVATE_OP                              870000

#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING                            30
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING_NO_DMA                     27

#ifndef IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_RD_4B
#define IDF_PERFORMANCE_MIN_FLASH_SPEED_BYTE_PER_SEC_EXT_RD_4B                  (359*1000)
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
