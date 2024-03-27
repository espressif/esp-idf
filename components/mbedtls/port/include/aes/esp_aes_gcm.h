/*
 * GCM block cipher, ESP DMA hardware accelerated version
 * Based on mbedTLS FIPS-197 compliant version.
 *
 * SPDX-FileCopyrightText: The Mbed TLS Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2016-2024 Espressif Systems (Shanghai) CO LTD
 */
#pragma once

#include "aes/esp_aes.h"
#include "mbedtls/cipher.h"

#ifdef __cplusplus
extern "C" {
#endif

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
    /* Software context needed for soft fallback for non-AES ciphers */
    void *ctx_soft;
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
 * \param ctx       The GCM context. This must be initialized.
 * \param mode      The operation to perform: #MBEDTLS_GCM_ENCRYPT or
 *                  #MBEDTLS_GCM_DECRYPT.
 * \param iv        The initialization vector. This must be a readable buffer of
 *                  at least \p iv_len Bytes.
 * \param iv_len    The length of the IV.
 *
 * \return          \c 0 on success.
 */
int esp_aes_gcm_starts( esp_gcm_context *ctx,
                        int mode,
                        const unsigned char *iv,
                        size_t iv_len );

/**
 * \brief           This function feeds an input buffer as associated data
 *                  (authenticated but not encrypted data) in a GCM
 *                  encryption or decryption operation.
 *
 *                  Call this function after mbedtls_gcm_starts() to pass
 *                  the associated data. If the associated data is empty,
 *                  you do not need to call this function. You may not
 *                  call this function after calling mbedtls_cipher_update().
 *
 * \param ctx       The GCM context. This must have been started with
 *                  mbedtls_gcm_starts() and must not have yet received
 *                  any input with mbedtls_gcm_update().
 * \param aad       The buffer holding the additional data, or \c NULL
 *                  if \p aad_len is \c 0.
 * \param aad_len   The length of the additional data. If \c 0,
 *                  \p add may be \c NULL.
 *
 * \return          \c 0 on success.
 */
int esp_aes_gcm_update_ad( esp_gcm_context *ctx,
                           const unsigned char *aad,
                           size_t aad_len );

/**
 * \brief           This function feeds an input buffer into an ongoing GCM
 *                  encryption or decryption operation.
 *
 *                  You may call this function zero, one or more times
 *                  to pass successive parts of the input: the plaintext to
 *                  encrypt, or the ciphertext (not including the tag) to
 *                  decrypt. After the last part of the input, call
 *                  mbedtls_gcm_finish().
 *
 *                  This function may produce output in one of the following
 *                  ways:
 *                  - Immediate output: the output length is always equal
 *                    to the input length.
 *                  - Buffered output: the output consists of a whole number
 *                    of 16-byte blocks. If the total input length so far
 *                    (not including associated data) is 16 \* *B* + *A*
 *                    with *A* < 16 then the total output length is 16 \* *B*.
 *
 *                  In particular:
 *                  - It is always correct to call this function with
 *                    \p output_size >= \p input_length + 15.
 *                  - If \p input_length is a multiple of 16 for all the calls
 *                    to this function during an operation, then it is
 *                    correct to use \p output_size = \p input_length.
 *
 * \note            For decryption, the output buffer cannot be the same as
 *                  input buffer. If the buffers overlap, the output buffer
 *                  must trail at least 8 Bytes behind the input buffer.
 *
 * \param ctx           The GCM context. This must be initialized.
 * \param input         The buffer holding the input data. If \p input_length
 *                      is greater than zero, this must be a readable buffer
 *                      of at least \p input_length bytes.
 * \param input_length  The length of the input data in bytes.
 * \param output        The buffer for the output data. If \p output_size
 *                      is greater than zero, this must be a writable buffer of
 *                      of at least \p output_size bytes.
 * \param output_size   The size of the output buffer in bytes.
 *                      See the function description regarding the output size.
 * \param output_length On success, \p *output_length contains the actual
 *                      length of the output written in \p output.
 *                      On failure, the content of \p *output_length is
 *                      unspecified.
 *
 * \return         \c 0 on success.
 * \return         #MBEDTLS_ERR_GCM_BAD_INPUT on failure:
 *                 total input length too long,
 *                 unsupported input/output buffer overlap detected,
 *                 or \p output_size too small.
 */
int esp_aes_gcm_update( esp_gcm_context *ctx,
                        const unsigned char *input, size_t input_length,
                        unsigned char *output, size_t output_size,
                        size_t *output_length );

/**
 * \brief           This function finishes the GCM operation and generates
 *                  the authentication tag.
 *
 *                  It wraps up the GCM stream, and generates the
 *                  tag. The tag can have a maximum length of 16 Bytes.
 *
 * \param ctx       The GCM context. This must be initialized.
 * \param tag       The buffer for holding the tag. This must be a writable
 *                  buffer of at least \p tag_len Bytes.
 * \param tag_len   The length of the tag to generate. This must be at least
 *                  four.
 * \param output    The buffer for the final output.
 *                  If \p output_size is nonzero, this must be a writable
 *                  buffer of at least \p output_size bytes.
 * \param output_size  The size of the \p output buffer in bytes.
 *                  This must be large enough for the output that
 *                  mbedtls_gcm_update() has not produced. In particular:
 *                  - If mbedtls_gcm_update() produces immediate output,
 *                    or if the total input size is a multiple of \c 16,
 *                    then mbedtls_gcm_finish() never produces any output,
 *                    so \p output_size can be \c 0.
 *                  - \p output_size never needs to be more than \c 15.
 * \param output_length On success, \p *output_length contains the actual
 *                      length of the output written in \p output.
 *                      On failure, the content of \p *output_length is
 *                      unspecified.
 *
 * \return          \c 0 on success.
 * \return          #MBEDTLS_ERR_GCM_BAD_INPUT on failure:
 *                  invalid value of \p tag_len,
 *                  or \p output_size too small.
 */
int esp_aes_gcm_finish( esp_gcm_context *ctx,
                        unsigned char *output, size_t output_size,
                        size_t *output_length,
                        unsigned char *tag, size_t tag_len );

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
 * \param aad       The buffer holding the additional data.
 * \param aad_len   The length of the additional data.
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
                               const unsigned char *aad,
                               size_t aad_len,
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
 * \param aad       The buffer holding the additional data.
 * \param aad_len   The length of the additional data.
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
                              const unsigned char *aad,
                              size_t aad_len,
                              const unsigned char *tag,
                              size_t tag_len,
                              const unsigned char *input,
                              unsigned char *output );

#ifdef __cplusplus
}
#endif
