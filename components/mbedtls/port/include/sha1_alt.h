/*
 *  SHA-1 implementation with hardware ESP32 support added.
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
#ifndef _SHA1_ALT_H_
#define _SHA1_ALT_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MBEDTLS_SHA1_ALT)

typedef enum {
    ESP_MBEDTLS_SHA1_UNUSED, /* first block hasn't been processed yet */
    ESP_MBEDTLS_SHA1_HARDWARE, /* using hardware SHA engine */
    ESP_MBEDTLS_SHA1_SOFTWARE, /* using software SHA */
} esp_mbedtls_sha1_mode;

/**
 * \brief          SHA-1 context structure
 */
typedef struct
{
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[5];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
    esp_mbedtls_sha1_mode mode;
}
mbedtls_sha1_context;

#endif

#ifdef __cplusplus
}
#endif

#endif

