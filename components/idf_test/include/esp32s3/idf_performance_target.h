#pragma once

#define IDF_PERFORMANCE_MIN_AES_CBC_THROUGHPUT_MBSEC                            14.4

// SHA256 hardware throughput at 240MHz, threshold set lower than worst case
#define IDF_PERFORMANCE_MIN_SHA256_THROUGHPUT_MBSEC                             19.8
// esp_sha() time to process 32KB of input data from RAM
#define IDF_PERFORMANCE_MAX_TIME_SHA1_32KB                                      1000
#define IDF_PERFORMANCE_MAX_TIME_SHA512_32KB                                    900

#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PUBLIC_OP                               14000
#define IDF_PERFORMANCE_MAX_RSA_2048KEY_PRIVATE_OP                              100000
#define IDF_PERFORMANCE_MAX_RSA_4096KEY_PUBLIC_OP                               60000
#define IDF_PERFORMANCE_MAX_RSA_4096KEY_PRIVATE_OP                              600000

#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING                            32
#define IDF_PERFORMANCE_MAX_SPI_PER_TRANS_NO_POLLING_NO_DMA                     30
