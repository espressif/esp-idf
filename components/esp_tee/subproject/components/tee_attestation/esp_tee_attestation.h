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
 * @brief Generate and return an entity attestation token (EAT) from the TEE
 *
 * The EAT consists of the below details:
 * - For all firmware images (Bootloader, active TEE and non-secure app)
 *   - Project and ESP-IDF version
 *   - Digest (SHA256)
 * - Public key corresponding to the private key used for signing (in compressed format)
 * - Signature generated using the ECDSA key stored in the configured slot of the TEE's Secure Storage (`r` and `s` components)
 *
 * @param[in]   nonce           Attestation request identification
 * @param[in]   client_id       Relying Party identification
 * @param[in]   psa_cert_ref    PSA certification ID
 * @param[in]   token_buf       Output buffer which will hold the resultant EAT in JSON format
 * @param[in]   token_buf_size  Size of the output buffer
 * @param[out]  token_len       Actual length of the output EAT JSON
 *
 * @return
 *          - `ESP_OK` on success
 *          - `ESP_ERR_INVALID_ARG` in case token_buf or token_len are NULL or token_buf_size is 0
 *          - `ESP_ERR_NO_MEM` in case memory could not be allocated for the internal structures
 *          - `ESP_FAIL` other errors
 */
esp_err_t esp_tee_att_generate_token(const uint32_t nonce, const uint32_t client_id, const char *psa_cert_ref,
                                     uint8_t *token_buf, const size_t token_buf_size, uint32_t *token_len);

#ifdef __cplusplus
}
#endif
