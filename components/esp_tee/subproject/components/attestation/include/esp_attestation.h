/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
 * @param[in] nonce           Nonce value to include in the token
 * @param[in] client_id       Client identifier to include in the token
 * @param[in] psa_cert_ref    PSA certificate reference to include in the token
 * @param[in] token_buf       Buffer to store the generated token
 * @param[in] token_buf_size  Size of the token buffer
 * @param[out] token_len       Pointer to store the actual length of the generated token
 *
 * @return esp_err_t ESP_OK on success, or an error code on failure
 */
esp_err_t esp_att_generate_token(const uint32_t nonce, const uint32_t client_id, const char *psa_cert_ref,
                                 uint8_t *token_buf, const size_t token_buf_size, uint32_t *token_len);

#ifdef __cplusplus
}
#endif
