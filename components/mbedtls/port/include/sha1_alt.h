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

#if defined(MBEDTLS_SHA1_ALT)

#include "hal/sha_types.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_SHA_SUPPORT_PARALLEL_ENG

typedef enum {
    ESP_MBEDTLS_SHA1_UNUSED, /* first block hasn't been processed yet */
    ESP_MBEDTLS_SHA1_HARDWARE, /* using hardware SHA engine */
    ESP_MBEDTLS_SHA1_SOFTWARE, /* using software SHA */
} esp_mbedtls_sha1_mode;

/**
 * \brief          SHA-1 context structure
 */
typedef struct {
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[5];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
    esp_mbedtls_sha1_mode mode;
} mbedtls_sha1_context;

/**
 * \brief          Set the SHA-1 mode for a mbedtls_sha1_context.
 *
 * \param ctx      The SHA-1 context structure.
 * \param mode     The SHA-1 mode to be set. It can be one of the following:
 *                  - ESP_MBEDTLS_SHA1_UNUSED: Indicates that the first block hasn't been processed yet.
 *                  - ESP_MBEDTLS_SHA1_HARDWARE: Specifies the use of hardware SHA engine for SHA-1 calculations.
 *                  - ESP_MBEDTLS_SHA1_SOFTWARE: Specifies the use of software-based SHA-1 calculations.
 *
 * \return         None.
 */
static inline void esp_mbedtls_set_sha1_mode(mbedtls_sha1_context *ctx, esp_mbedtls_sha1_mode mode)
{
    if (ctx) {
        ctx->mode = mode;
    }
}

#elif SOC_SHA_SUPPORT_DMA || SOC_SHA_SUPPORT_RESUME

typedef enum {
    ESP_SHA1_STATE_INIT,
    ESP_SHA1_STATE_IN_PROCESS
} esp_sha1_state;

/**
 * \brief          SHA-1 context structure
 */
typedef struct {
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[5];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
    int first_block;            /*!< if first then true else false */
    esp_sha_type mode;
    esp_sha1_state sha_state;
} mbedtls_sha1_context;

#endif

#endif

#ifdef __cplusplus
}
#endif

#endif
