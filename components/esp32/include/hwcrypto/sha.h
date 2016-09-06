/*
 *  ESP32 hardware accelerated SHA1/256/512 implementation
 *  based on mbedTLS FIPS-197 compliant version.
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  Additions Copyright (C) 2016, Espressif Systems (Shanghai) PTE Ltd
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

#ifndef _ESP_SHA_H_
#define _ESP_SHA_H_

#include "rom/sha.h"

#include "esp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          SHA-1 context structure
 */
typedef struct {
    /* both types defined in rom/sha.h */
    SHA_CTX context;
    enum SHA_TYPE context_type;
} esp_sha_context;

/**
 * \brief Lock access to SHA hardware unit
 *
 * SHA hardware unit can only be used by one
 * consumer at a time.
 *
 * esp_sha_xxx API calls automatically manage locking & unlocking of
 * hardware, this function is only needed if you want to call
 * ets_sha_xxx functions directly.
 */
void esp_sha_acquire_hardware( void );

/**
 * \brief Unlock access to SHA hardware unit
 *
 * esp_sha_xxx API calls automatically manage locking & unlocking of
 * hardware, this function is only needed if you want to call
 * ets_sha_xxx functions directly.
 */
void esp_sha_release_hardware( void );

/**
 * \brief          Initialize SHA-1 context
 *
 * \param ctx      SHA-1 context to be initialized
 */
void esp_sha1_init( esp_sha_context *ctx );

/**
 * \brief          Clear SHA-1 context
 *
 * \param ctx      SHA-1 context to be cleared
 */
void esp_sha1_free( esp_sha_context *ctx );

/**
 * \brief          Clone (the state of) a SHA-1 context
 *
 * \param dst      The destination context
 * \param src      The context to be cloned
 */
void esp_sha1_clone( esp_sha_context *dst, const esp_sha_context *src );

/**
 * \brief          SHA-1 context setup
 *
 * \param ctx      context to be initialized
 */
void esp_sha1_start( esp_sha_context *ctx );

/**
 * \brief          SHA-1 process buffer
 *
 * \param ctx      SHA-1 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void esp_sha1_update( esp_sha_context *ctx, const unsigned char *input, size_t ilen );

/**
 * \brief          SHA-1 final digest
 *
 * \param ctx      SHA-1 context
 * \param output   SHA-1 checksum result
 */
void esp_sha1_finish( esp_sha_context *ctx, unsigned char output[20] );

/**
 * \brief          Calculate SHA-1 of input buffer
 *
 * \param input    buffer holding the data
 * \param ilen     length of the input data
 * \param output   SHA-1 checksum result
 */
void esp_sha1( const unsigned char *input, size_t ilen, unsigned char output[20] );

/**
 * \brief          SHA-256 context structure
 */

/**
 * \brief          Initialize SHA-256 context
 *
 * \param ctx      SHA-256 context to be initialized
 */
void esp_sha256_init( esp_sha_context *ctx );

/**
 * \brief          Clear SHA-256 context
 *
 * \param ctx      SHA-256 context to be cleared
 */
void esp_sha256_free( esp_sha_context *ctx );

/**
 * \brief          Clone (the state of) a SHA-256 context
 *
 * \param dst      The destination context
 * \param src      The context to be cloned
 */
void esp_sha256_clone( esp_sha_context *dst, const esp_sha_context *src );

/**
 * \brief          SHA-256 context setup
 *
 * \param ctx      context to be initialized
 * \param is224    0 = use SHA256, 1 = use SHA224
 */
void esp_sha256_start( esp_sha_context *ctx, int is224 );

/**
 * \brief          SHA-256 process buffer
 *
 * \param ctx      SHA-256 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void esp_sha256_update( esp_sha_context *ctx, const unsigned char *input, size_t ilen );

/**
 * \brief          SHA-256 final digest
 *
 * \param ctx      SHA-256 context
 * \param output   SHA-224/256 checksum result
 */
void esp_sha256_finish( esp_sha_context *ctx, unsigned char output[32] );

/**
 * \brief          Calculate SHA-256 of input buffer
 *
 * \param input    buffer holding the data
 * \param ilen     length of the input data
 * \param output   SHA-224/256 checksum result
 * \param is224    0 = use SHA256, 1 = use SHA224
 */
void esp_sha256( const unsigned char *input, size_t ilen, unsigned char output[32], int is224 );

//

/**
 * \brief          SHA-512 context structure
 */

/**
 * \brief          Initialize SHA-512 context
 *
 * \param ctx      SHA-512 context to be initialized
 */
void esp_sha512_init( esp_sha_context *ctx );

/**
 * \brief          Clear SHA-512 context
 *
 * \param ctx      SHA-512 context to be cleared
 */
void esp_sha512_free( esp_sha_context *ctx );

/**
 * \brief          Clone (the state of) a SHA-512 context
 *
 * \param dst      The destination context
 * \param src      The context to be cloned
 */
void esp_sha512_clone( esp_sha_context *dst, const esp_sha_context *src );

/**
 * \brief          SHA-512 context setup
 *
 * \param ctx      context to be initialized
 * \param is384    0 = use SHA512, 1 = use SHA384
 */
void esp_sha512_start( esp_sha_context *ctx, int is384 );

/**
 * \brief          SHA-512 process buffer
 *
 * \param ctx      SHA-512 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void esp_sha512_update( esp_sha_context *ctx, const unsigned char *input, size_t ilen );

/**
 * \brief          SHA-512 final digest
 *
 * \param ctx      SHA-512 context
 * \param output   SHA-384/512 checksum result
 */
void esp_sha512_finish( esp_sha_context *ctx, unsigned char output[64] );

/**
 * \brief          Calculate SHA-512 of input buffer.
 *
 * \param input    buffer holding the data
 * \param ilen     length of the input data
 * \param output   SHA-384/512 checksum result
 * \param is384    0 = use SHA512, 1 = use SHA384
 */
void esp_sha512( const unsigned char *input, size_t ilen, unsigned char output[64], int is384 );

//

#ifdef __cplusplus
}
#endif

#endif

