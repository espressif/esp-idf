/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * PSA Attestation implementation
 */

#ifndef PSA_INITIAL_ATTESTATION_H
#define PSA_INITIAL_ATTESTATION_H

#include <stddef.h>
#include <stdint.h>

#include "psa/crypto_values.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PSA_INITIAL_ATTEST_API_VERSION_MAJOR  1 /*!< Major version of this implementation of the Attestation API */
#define PSA_INITIAL_ATTEST_API_VERSION_MINOR  0 /*!< Minor version of this implementation of the Attestation API */

#define PSA_INITIAL_ATTEST_MAX_TOKEN_SIZE     2048 /*!< Maximum size of an attestation token in bytes */

#define PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32  (32u)
#define PSA_INITIAL_ATTEST_CHALLENGE_SIZE_48  (48u)
#define PSA_INITIAL_ATTEST_CHALLENGE_SIZE_64  (64u)
#define PSA_INITIAL_ATTEST_CHALLENGE_SIZE_MAX (PSA_INITIAL_ATTEST_CHALLENGE_SIZE_64) /*!< Maximum supported challenge size */

/**
 * @brief  Generate an entity attestation token
 *
 * Generates an attestation token containing device identity and security claims, signed and
 * encoded in the JSON format.
 *
 * @param auth_challenge    Pointer to a buffer containing the challenge data from the verifier
 * @param challenge_size    Size of the challenge in bytes. Must be 32, 48, or 64 bytes
 * @param token_buf         Pointer to a buffer where the attestation token will be written
 * @param token_buf_size    Size of the token buffer in bytes
 * @param token_size        On success, will be set to the actual size of the generated token
 *
 * @return psa_status_t     PSA_SUCCESS on success,
 *                          PSA_ERROR_INVALID_ARGUMENT if parameters are invalid,
 *                          PSA_ERROR_BUFFER_TOO_SMALL if token_buf is too small,
 *                          PSA_ERROR_NOT_SUPPORTED if the requested challenge size is not supported,
 *                          or another error code on failure
 */
psa_status_t psa_initial_attest_get_token(const uint8_t *auth_challenge, size_t challenge_size,
                                          uint8_t *token_buf, size_t token_buf_size, size_t *token_size);

/**
 * @brief Get the size of the attestation token that would be generated
 *
 * This can be used to allocate an appropriately sized buffer before calling psa_initial_attest_get_token().
 *
 * @param challenge_size    Size of the challenge in bytes. Must be 32, 48, or 64 bytes
 * @param token_size        On success, will be set to the size of the token that would be generated
 *
 * @return psa_status_t     PSA_SUCCESS on success,
 *                          PSA_ERROR_INVALID_ARGUMENT if challenge_size is invalid,
 *                          PSA_ERROR_NOT_SUPPORTED if the requested challenge size is not supported,
 *                          or another error code on failure
 */
psa_status_t psa_initial_attest_get_token_size(size_t challenge_size, size_t *token_size);

#ifdef __cplusplus
}
#endif

#endif // PSA_INITIAL_ATTESTATION_H
