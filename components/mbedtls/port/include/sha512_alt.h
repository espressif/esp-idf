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

#if defined(MBEDTLS_SHA512_ALT)

#include "hal/sha_types.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif


#if SOC_SHA_SUPPORT_PARALLEL_ENG

typedef enum {
    ESP_MBEDTLS_SHA512_UNUSED, /* first block hasn't been processed yet */
    ESP_MBEDTLS_SHA512_HARDWARE, /* using hardware SHA engine */
    ESP_MBEDTLS_SHA512_SOFTWARE, /* using software SHA */
} esp_mbedtls_sha512_mode;

/**
 * \brief          SHA-512 context structure
 */
typedef struct {
    uint64_t total[2];          /*!< number of bytes processed  */
    uint64_t state[8];          /*!< intermediate digest state  */
    unsigned char buffer[128];  /*!< data block being processed */
    int is384;                  /*!< 0 => SHA-512, else SHA-384 */
    esp_mbedtls_sha512_mode mode;
} mbedtls_sha512_context;

#elif SOC_SHA_SUPPORT_DMA || SOC_SHA_SUPPORT_RESUME

typedef enum {
    ESP_SHA512_STATE_INIT,
    ESP_SHA512_STATE_IN_PROCESS
} esp_sha512_state;

/**
 * \brief          SHA-512 context structure
 */
typedef struct {
    uint64_t total[2];          /*!< number of bytes processed  */
    uint64_t state[8];          /*!< intermediate digest state  */
    unsigned char buffer[128];  /*!< data block being processed */
    int first_block;
    esp_sha_type mode;
    uint32_t t_val;             /*!< t_val for 512/t mode */
    esp_sha512_state sha_state;
} mbedtls_sha512_context;

/**
 * @brief Sets the specific algorithm for SHA512
 *
 * @param ctx The mbedtls sha512 context
 *
 * @param type The mode, used for setting SHA2_512224 and SHA2_512256:
 *
 */
void esp_sha512_set_mode(mbedtls_sha512_context *ctx, esp_sha_type type);

/* For SHA512/t mode the initial hash value will depend on t */
void esp_sha512_set_t( mbedtls_sha512_context *ctx, uint16_t t_val);


#endif

#endif

#ifdef __cplusplus
}
#endif

#endif
