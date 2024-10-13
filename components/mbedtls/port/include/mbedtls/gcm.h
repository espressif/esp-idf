/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include_next "mbedtls/gcm.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MBEDTLS_GCM_ALT) && defined(MBEDTLS_GCM_NON_AES_CIPHER_SOFT_FALLBACK)

/**
 * When the MBEDTLS_GCM_NON_AES_CIPHER_SOFT_FALLBACK is defined, for non-AES GCM
 * operations we need to fallback to the software function definitions of the
 * mbedtls GCM layer.
 * Thus in this case we need declarations for the software funtions.
 * Please refer mbedtls/include/mbedtls/gcm.h for function documentations
 */

void mbedtls_gcm_init_soft(mbedtls_gcm_context_soft *ctx);


int mbedtls_gcm_setkey_soft(mbedtls_gcm_context_soft *ctx,
                            mbedtls_cipher_id_t cipher,
                            const unsigned char *key,
                            unsigned int keybits);

int mbedtls_gcm_starts_soft(mbedtls_gcm_context_soft *ctx,
                            int mode,
                            const unsigned char *iv, size_t iv_len);

int mbedtls_gcm_update_ad_soft(mbedtls_gcm_context_soft *ctx,
                            const unsigned char *add, size_t add_len);

int mbedtls_gcm_update_soft(mbedtls_gcm_context_soft *ctx,
                            const unsigned char *input, size_t input_length,
                            unsigned char *output, size_t output_size,
                            size_t *output_length);

int mbedtls_gcm_finish_soft(mbedtls_gcm_context_soft *ctx,
                            unsigned char *output, size_t output_size,
                            size_t *output_length,
                            unsigned char *tag, size_t tag_len);


int mbedtls_gcm_crypt_and_tag_soft(mbedtls_gcm_context_soft *ctx,
                                int mode,
                                size_t length,
                                const unsigned char *iv,
                                size_t iv_len,
                                const unsigned char *add,
                                size_t add_len,
                                const unsigned char *input,
                                unsigned char *output,
                                size_t tag_len,
                                unsigned char *tag);


int mbedtls_gcm_auth_decrypt_soft(mbedtls_gcm_context_soft *ctx,
                                size_t length,
                                const unsigned char *iv,
                                size_t iv_len,
                                const unsigned char *add,
                                size_t add_len,
                                const unsigned char *tag,
                                size_t tag_len,
                                const unsigned char *input,
                                unsigned char *output);

void mbedtls_gcm_free_soft(mbedtls_gcm_context_soft *ctx);

#endif /* MBEDTLS_GCM_ALT && MBEDTLS_GCM_NON_AES_CIPHER_SOFT_FALLBACK*/

#ifdef __cplusplus
}
#endif
