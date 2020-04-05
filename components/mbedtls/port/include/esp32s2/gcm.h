/**
 * \brief AES block cipher, ESP32C hardware accelerated version
 * Based on mbedTLS FIPS-197 compliant version.
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  Additions Copyright (C) 2019-2020, Espressif Systems (Shanghai) PTE Ltd
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
 *
 */

#ifndef ESP_GCM_H
#define ESP_GCM_H

#include "aes.h"
#include "mbedtls/cipher.h"
#ifdef __cplusplus
extern "C" {
#endif


#define MBEDTLS_ERR_GCM_AUTH_FAILED      -0x0012  /**< Authenticated decryption failed. */
#define MBEDTLS_ERR_GCM_BAD_INPUT        -0x0014  /**< Bad input parameters to function.*/

typedef enum {
    ESP_AES_GCM_STATE_INIT,
    ESP_AES_GCM_STATE_START,
    ESP_AES_GCM_STATE_UPDATE,
    ESP_AES_GCM_STATE_FINISH
} esp_aes_gcm_state;
/**
 * \brief          The GCM context structure.
 */
typedef struct {
    uint8_t H[16];                        /*!< Initial hash value */
    uint8_t ghash[16];                    /*!< GHASH value. */
    uint8_t J0[16];
    uint64_t HL[16];                      /*!< Precalculated HTable low. */
    uint64_t HH[16];                      /*!< Precalculated HTable high. */
    uint8_t ori_j0[16];                   /*!< J0 from first iteration. */
    const uint8_t *iv;
    size_t iv_len;                       /*!< The length of IV. */
    uint64_t aad_len;                     /*!< The total length of the additional data. */
    size_t data_len;
    int mode;
    const unsigned char *aad;             /*!< The additional data. */
    esp_aes_context aes_ctx;
    esp_aes_gcm_state gcm_state;
} esp_gcm_context;

/**
 * \brief           This function initializes the specified GCM context
 *
 * \param ctx       The GCM context to initialize.
 */
void esp_aes_gcm_init( esp_gcm_context *ctx);

/**
 * \brief           This function associates a GCM context with a
 *                  key.
 *
 * \param ctx       The GCM context to initialize.
 * \param cipher    The 128-bit block cipher to use.
 * \param key       The encryption key.
 * \param keybits   The key size in bits. Valid options are:
 *                  <ul><li>128 bits</li>
 *                  <li>192 bits</li>
 *                  <li>256 bits</li></ul>
 *
 * \return          \c 0 on success.
 * \return          A cipher-specific error code on failure.
 */
int esp_aes_gcm_setkey( esp_gcm_context *ctx,
                        mbedtls_cipher_id_t cipher,
                        const unsigned char *key,
                        unsigned int keybits );

/**
 * \brief           This function starts a GCM encryption or decryption
 *                  operation.
 *
 * \param ctx       The GCM context.
 * \param mode      The operation to perform: #MBEDTLS_GCM_ENCRYPT or
 *                  #MBEDTLS_GCM_DECRYPT.
 * \param iv        The initialization vector.
 * \param iv_len    The length of the IV.
 * \param add       The buffer holding the additional data, or NULL
 *                  if \p add_len is 0.
 * \param add_len   The length of the additional data. If 0,
 *                  \p add is NULL.
 *
 * \return          \c 0 on success.
 */
int esp_aes_gcm_starts( esp_gcm_context *ctx,
                        int mode,
                        const unsigned char *iv,
                        size_t iv_len,
                        const unsigned char *aad,
                        size_t aad_len );

/**
 * \brief           This function feeds an input buffer into an ongoing GCM
 *                  encryption or decryption operation.
 *
 *    `             The function expects input to be a multiple of 16
 *                  Bytes. Only the last call before calling
 *                  mbedtls_gcm_finish() can be less than 16 Bytes.
 *
 * \note            For decryption, the output buffer cannot be the same as
 *                  input buffer. If the buffers overlap, the output buffer
 *                  must trail at least 8 Bytes behind the input buffer.
 *
 * \param ctx       The GCM context.
 * \param length    The length of the input data. This must be a multiple of
 *                  16 except in the last call before mbedtls_gcm_finish().
 * \param input     The buffer holding the input data.
 * \param output    The buffer for holding the output data.
 *
 * \return         \c 0 on success.
 * \return         #MBEDTLS_ERR_GCM_BAD_INPUT on failure.
 */
int esp_aes_gcm_update( esp_gcm_context *ctx,
                        size_t length,
                        const unsigned char *input,
                        unsigned char *output );

/**
 * \brief           This function finishes the GCM operation and generates
 *                  the authentication tag.
 *
 *                  It wraps up the GCM stream, and generates the
 *                  tag. The tag can have a maximum length of 16 Bytes.
 *
 * \param ctx       The GCM context.
 * \param tag       The buffer for holding the tag.
 * \param tag_len   The length of the tag to generate. Must be at least four.
 *
 * \return          \c 0 on success.
 * \return          #MBEDTLS_ERR_GCM_BAD_INPUT on failure.
 */
int esp_aes_gcm_finish( esp_gcm_context *ctx,
                        unsigned char *tag,
                        size_t tag_len );

/**
 * \brief           This function clears a GCM context
 *
 * \param ctx       The GCM context to clear.
 */
void esp_aes_gcm_free( esp_gcm_context *ctx);

/**
 * \brief           This function performs GCM encryption or decryption of a buffer.
 *
 * \note            For encryption, the output buffer can be the same as the
 *                  input buffer. For decryption, the output buffer cannot be
 *                  the same as input buffer. If the buffers overlap, the output
 *                  buffer must trail at least 8 Bytes behind the input buffer.
 *
 * \param ctx       The GCM context to use for encryption or decryption.
 * \param mode      The operation to perform: #MBEDTLS_GCM_ENCRYPT or
 *                  #MBEDTLS_GCM_DECRYPT.
 * \param length    The length of the input data. This must be a multiple of
 *                  16 except in the last call before mbedtls_gcm_finish().
 * \param iv        The initialization vector.
 * \param iv_len    The length of the IV.
 * \param add       The buffer holding the additional data.
 * \param add_len   The length of the additional data.
 * \param input     The buffer holding the input data.
 * \param output    The buffer for holding the output data.
 * \param tag_len   The length of the tag to generate.
 * \param tag       The buffer for holding the tag.
 *
 * \return         \c 0 on success.
 */
int esp_aes_gcm_crypt_and_tag( esp_gcm_context *ctx,
                               int mode,
                               size_t length,
                               const unsigned char *iv,
                               size_t iv_len,
                               const unsigned char *add,
                               size_t add_len,
                               const unsigned char *input,
                               unsigned char *output,
                               size_t tag_len,
                               unsigned char *tag );


/**
 * \brief           This function performs a GCM authenticated decryption of a
 *                  buffer.
 *
 * \note            For decryption, the output buffer cannot be the same as
 *                  input buffer. If the buffers overlap, the output buffer
 *                  must trail at least 8 Bytes behind the input buffer.
 *
 * \param ctx       The GCM context.
 * \param length    The length of the input data. This must be a multiple
 *                  of 16 except in the last call before mbedtls_gcm_finish().
 * \param iv        The initialization vector.
 * \param iv_len    The length of the IV.
 * \param add       The buffer holding the additional data.
 * \param add_len   The length of the additional data.
 * \param tag       The buffer holding the tag.
 * \param tag_len   The length of the tag.
 * \param input     The buffer holding the input data.
 * \param output    The buffer for holding the output data.
 *
 * \return         0 if successful and authenticated.
 * \return         #MBEDTLS_ERR_GCM_AUTH_FAILED if the tag does not match.
 */
int esp_aes_gcm_auth_decrypt( esp_gcm_context *ctx,
                              size_t length,
                              const unsigned char *iv,
                              size_t iv_len,
                              const unsigned char *add,
                              size_t add_len,
                              const unsigned char *tag,
                              size_t tag_len,
                              const unsigned char *input,
                              unsigned char *output );

#ifdef __cplusplus
}
#endif

#endif /* gcm.h */
