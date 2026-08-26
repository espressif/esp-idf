/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * PSA Attestation implementation
 */

#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "psa/initial_attestation.h"

#include <limits.h>
#include <stdint.h>
#include <string.h>

#include "esp_log.h"

static const char *TAG = "esp_psa_initial_attest";

/**
 * @brief Dummy implementation of PSA attestation APIs
 *
 */
psa_status_t psa_initial_attest_get_token(const uint8_t *auth_challenge, size_t challenge_size,
                                          uint8_t *token_buf, size_t token_buf_size, size_t *token_size)
{
    ESP_LOGE(TAG, "Attestation service is not supported");
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_initial_attest_get_token_size(size_t challenge_size, size_t *token_size)
{
    ESP_LOGE(TAG, "Attestation service is not supported");
    return PSA_ERROR_NOT_SUPPORTED;
}
