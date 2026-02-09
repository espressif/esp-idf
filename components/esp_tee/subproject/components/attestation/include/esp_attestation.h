/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Generate an entity attestation token
 *
 * @param[in] auth_challenge  Authentication challenge buffer
 * @param[in] challenge_size  Size of the authentication challenge
 * @param[in] token_buf       Buffer to store the generated token
 * @param[in] token_buf_size  Size of the token buffer
 * @param[out] token_size     Pointer to store the actual length of the generated token
 *
 * @return esp_err_t ESP_OK on success, or an error code on failure
 */
esp_err_t esp_att_generate_token(const uint8_t *auth_challenge, size_t challenge_size,
                                 uint8_t *token_buf, size_t token_buf_size, size_t *token_size);

/**
 * @brief Get the required buffer size for an attestation token
 *
 * @param[in]  challenge_size  Size of the authentication challenge in bytes
 *                             Must be one of: PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32 (32),
 *                             PSA_INITIAL_ATTEST_CHALLENGE_SIZE_48 (48), or
 *                             PSA_INITIAL_ATTEST_CHALLENGE_SIZE_64 (64)
 * @param[out] token_size      Pointer to store the required token buffer size
 *
 * @return
 *          - `ESP_OK` on success
 *          - `ESP_ERR_INVALID_ARG` if token_size is NULL or challenge_size is invalid
 */
esp_err_t esp_att_get_token_size(size_t challenge_size, size_t *token_size);

#ifdef __cplusplus
}
#endif
