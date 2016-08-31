// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _ESP_SHA_H_
#define _ESP_SHA_H_

#include "esp_types.h"
#include "rom/ets_sys.h"
#include "rom/sha.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          SHA-1 context structure
 */
typedef struct{
	SHA_CTX context;
	int context_type;
} sha_context;

typedef sha_context SHA1_CTX;

/**
 * \brief          Initialize SHA-1 context
 *
 * \param ctx      SHA-1 context to be initialized
 */
void esp_sha1_init( SHA1_CTX *ctx );

/**
 * \brief          Clear SHA-1 context
 *
 * \param ctx      SHA-1 context to be cleared
 */
void esp_sha1_free( SHA1_CTX *ctx );

/**
 * \brief          Clone (the state of) a SHA-1 context
 *
 * \param dst      The destination context
 * \param src      The context to be cloned
 */
void esp_sha1_clone( SHA1_CTX *dst, const SHA1_CTX *src );

void esp_sha1_process(SHA1_CTX *ctx, const unsigned char data[64]);

/**
 * \brief          SHA-1 context setup
 *
 * \param ctx      context to be initialized
 */
void esp_sha1_start( SHA1_CTX *ctx );

/**
 * \brief          SHA-1 process buffer
 *
 * \param ctx      SHA-1 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void esp_sha1_update( SHA1_CTX *ctx, const unsigned char *input, size_t ilen );

/**
 * \brief          SHA-1 final digest
 *
 * \param ctx      SHA-1 context
 * \param output   SHA-1 checksum result
 */
void esp_sha1_finish( SHA1_CTX *ctx, unsigned char output[20] );

/**
 * \brief          Output = SHA-1( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   SHA-1 checksum result
 */
void esp_sha1_output( const unsigned char *input, size_t ilen, unsigned char output[20] );

///
#define SHA256	SHA2_256
#define SHA224	4

/**
 * \brief          SHA-256 context structure
 */

typedef sha_context SHA256_CTX;

/**
 * \brief          Initialize SHA-256 context
 *
 * \param ctx      SHA-256 context to be initialized
 */
void esp_sha256_init( SHA256_CTX *ctx );

/**
 * \brief          Clear SHA-256 context
 *
 * \param ctx      SHA-256 context to be cleared
 */
void esp_sha256_free( SHA256_CTX *ctx );
void esp_sha256_process(SHA256_CTX *ctx, const unsigned char data[64]);

/**
 * \brief          Clone (the state of) a SHA-256 context
 *
 * \param dst      The destination context
 * \param src      The context to be cloned
 */
void esp_sha256_clone( SHA256_CTX *dst, const SHA256_CTX *src );

/**
 * \brief          SHA-256 context setup
 *
 * \param ctx      context to be initialized
 * \param is224    0 = use SHA256, 1 = use SHA224
 */
void esp_sha256_start( SHA256_CTX *ctx, int is224 );

/**
 * \brief          SHA-256 process buffer
 *
 * \param ctx      SHA-256 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void esp_sha256_update( SHA256_CTX *ctx, const unsigned char *input, size_t ilen );

/**
 * \brief          SHA-256 final digest
 *
 * \param ctx      SHA-256 context
 * \param output   SHA-224/256 checksum result
 */
void esp_sha256_finish( SHA256_CTX *ctx, unsigned char output[32] );

/**
 * \brief          Output = SHA-256( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   SHA-224/256 checksum result
 * \param is224    0 = use SHA256, 1 = use SHA224
 */
void esp_sha256_output( const unsigned char *input, size_t ilen, unsigned char output[32], int is224 );

//

/**
 * \brief          SHA-512 context structure
 */

typedef sha_context SHA512_CTX;

/**
 * \brief          Initialize SHA-512 context
 *
 * \param ctx      SHA-512 context to be initialized
 */
void esp_sha512_init( SHA512_CTX *ctx );

void esp_sha512_process( SHA512_CTX *ctx, const unsigned char data[128] );

/**
 * \brief          Clear SHA-512 context
 *
 * \param ctx      SHA-512 context to be cleared
 */
void esp_sha512_free( SHA512_CTX *ctx );

/**
 * \brief          Clone (the state of) a SHA-512 context
 *
 * \param dst      The destination context
 * \param src      The context to be cloned
 */
void esp_sha512_clone( SHA512_CTX *dst, const SHA512_CTX *src );

/**
 * \brief          SHA-512 context setup
 *
 * \param ctx      context to be initialized
 * \param is384    0 = use SHA512, 1 = use SHA384
 */
void esp_sha512_start( SHA512_CTX *ctx, int is384 );

/**
 * \brief          SHA-512 process buffer
 *
 * \param ctx      SHA-512 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 */
void esp_sha512_update( SHA512_CTX *ctx, const unsigned char *input, size_t ilen );

/**
 * \brief          SHA-512 final digest
 *
 * \param ctx      SHA-512 context
 * \param output   SHA-384/512 checksum result
 */
void esp_sha512_finish( SHA512_CTX *ctx, unsigned char output[64] );

/**
 * \brief          Output = SHA-512( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   SHA-384/512 checksum result
 * \param is384    0 = use SHA512, 1 = use SHA384
 */
void esp_sha512_output( const unsigned char *input, size_t ilen, unsigned char output[64], int is384 );

//

#ifdef __cplusplus
}
#endif

#endif

