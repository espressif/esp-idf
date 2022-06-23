/*
 *  SHA-256 implementation with hardware ESP32 support added.
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
#ifndef _SHA256_ALT_H_
#define _SHA256_ALT_H_

#if defined(MBEDTLS_SHA256_ALT)

#include "hal/sha_types.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_SHA_SUPPORT_PARALLEL_ENG
typedef enum {
    ESP_MBEDTLS_SHA256_UNUSED, /* first block hasn't been processed yet */
    ESP_MBEDTLS_SHA256_HARDWARE, /* using hardware SHA engine */
    ESP_MBEDTLS_SHA256_SOFTWARE, /* using software SHA */
} esp_mbedtls_sha256_mode;

/**
 * \brief          SHA-256 context structure
 */
typedef struct {
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[8];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
    int is224;                  /*!< 0 => SHA-256, else SHA-224 */
    esp_mbedtls_sha256_mode mode;
} mbedtls_sha256_context;

#elif SOC_SHA_SUPPORT_DMA || SOC_SHA_SUPPORT_RESUME
typedef enum {
    ESP_SHA256_STATE_INIT,
    ESP_SHA256_STATE_IN_PROCESS
} esp_sha256_state;

/**
 * \brief          SHA-256 context structure
 */
typedef struct {
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[8];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
    int first_block;           /*!< if first then true, else false */
    esp_sha_type mode;
    esp_sha256_state sha_state;
} mbedtls_sha256_context;

#endif

#endif

#ifdef __cplusplus
}
#endif

#endif
