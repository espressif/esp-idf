/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "esp_err.h"
#include "http_crypto.h"
#include "psa/crypto.h"
#include "esp_log.h"

static const char *TAG = "http_crypto_psa";

esp_err_t http_crypto_sha256(const uint8_t *data, size_t data_len, uint8_t *hash)
{
    if (data == NULL || data_len == 0 || hash == NULL) {
        ESP_LOGE(TAG, "Invalid input parameters");
        return ESP_FAIL;
    }

    size_t hash_len = 0;
    psa_status_t status = psa_hash_compute(PSA_ALG_SHA_256, data, data_len, hash, SHA256_LEN, &hash_len);
    if (status != PSA_SUCCESS || hash_len != SHA256_LEN) {
        ESP_LOGE(TAG, "Failed to compute SHA-256 hash");
        return ESP_FAIL;
    }
    return ESP_OK;
}
