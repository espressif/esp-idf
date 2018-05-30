/*
 *  SHA-512 implementation with hardware ESP32 support added.
 *  Uses mbedTLS software implementation for failover when concurrent
 *  SHA operations are in use.
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  Additions Copyright (C) 2016, Espressif Systems (Shanghai) PTE LTD
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */
#ifndef _SHA512_ALT_H_
#define _SHA512_ALT_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MBEDTLS_SHA512_ALT)

typedef enum {
    ESP_MBEDTLS_SHA512_UNUSED, /* first block hasn't been processed yet */
    ESP_MBEDTLS_SHA512_HARDWARE, /* using hardware SHA engine */
    ESP_MBEDTLS_SHA512_SOFTWARE, /* using software SHA */
} esp_mbedtls_sha512_mode;

/**
 * \brief          SHA-512 context structure
 */
typedef struct
{
    uint64_t total[2];          /*!< number of bytes processed  */
    uint64_t state[8];          /*!< intermediate digest state  */
    unsigned char buffer[128];  /*!< data block being processed */
    int is384;                  /*!< 0 => SHA-512, else SHA-384 */
    esp_mbedtls_sha512_mode mode;
}
mbedtls_sha512_context;

#endif

#ifdef __cplusplus
}
#endif

#endif
