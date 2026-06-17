/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/crypto.h"

#define FACTOR_KEYLEN_IN_BYTES 4
#define SWAP_INT32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))
#define MIN_V15_PADDING_LEN 11

/**
 * @brief Encodes the hash using PKCS#1 v1.5 padding scheme
 *
 * @param alg   Algorithm identifier
 * @param hashlen Length of the hash
 * @param hash Pointer to the hash data
 * @param dst_len Length of the destination buffer
 * @param dst Pointer to the destination buffer
 * @return psa_status_t
 *          PSA_ERROR_INVALID_ARGUMENT if arguments are invalid
 *          PSA_SUCCESS on success
 */
psa_status_t esp_rsa_ds_pad_v15_encode(psa_algorithm_t alg, unsigned int hashlen,
                                            const unsigned char *hash,
                                            size_t dst_len,
                                            unsigned char *dst);

/**
 * @brief Unpads the input data using PKCS#1 v1.5 padding scheme
 *
 * @param input Pointer to the input data
 * @param ilen Length of the input data
 * @param output Pointer to the output buffer
 * @param output_max_len Maximum length of the output buffer
 * @param olen Pointer to the length of the output data
 * @return psa_status_t
 *          PSA_ERROR_INVALID_ARGUMENT if arguments are invalid
 *          PSA_SUCCESS on success
 */
psa_status_t esp_rsa_ds_pad_v15_unpad(unsigned char *input,
    size_t ilen,
    unsigned char *output,
    size_t output_max_len,
    size_t *olen);

#if CONFIG_MBEDTLS_SSL_PROTO_TLS1_3

/**
 * @brief Encodes the hash using PKCS#1 v2.2 (PSS) padding scheme
 *
 * @param hash_alg  Hash algorithm identifier
 * @param hashlen   Length of the hash
 * @param hash  Pointer to the hash data
 * @param saltlen   Length of the salt
 * @param sig   Pointer to the signature buffer
 * @param dst_len   Length of the destination buffer
 * @return psa_status_t
 *        PSA_ERROR_INVALID_ARGUMENT if arguments are invalid
 *        PSA_SUCCESS on success
 */
psa_status_t esp_rsa_ds_pad_v21_encode(psa_algorithm_t hash_alg,
                                            unsigned int hashlen,
                                            const unsigned char *hash,
                                            int saltlen,
                                            unsigned char *sig,
                                            size_t dst_len);

/**
 * @brief   Unpads the input data using PKCS#1 v2.2 (OAEP) padding scheme
 *
 * @param input     Pointer to the input data
 * @param ilen   Length of the input data
 * @param output    Pointer to the output buffer
 * @param output_max_len    Maximum length of the output buffer
 * @param olen  Pointer to the length of the output data
 * @param hash_alg  Hash algorithm identifier
 * @return psa_status_t
 *       PSA_ERROR_INVALID_ARGUMENT if arguments are invalid
 *        PSA_SUCCESS on success
 */
psa_status_t esp_rsa_ds_pad_oaep_unpad(unsigned char *input,
    size_t ilen,
    unsigned char *output,
    size_t output_max_len,
    size_t *olen,
    psa_algorithm_t hash_alg);
#endif /* CONFIG_MBEDTLS_SSL_PROTO_TLS1_3 */
#ifdef __cplusplus
}
#endif
