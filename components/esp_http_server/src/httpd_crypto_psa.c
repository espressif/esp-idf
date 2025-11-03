/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "psa/crypto.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "httpd_crypto_psa";

#define SHA1_LEN (20)

esp_err_t httpd_crypto_sha1(const uint8_t *data, size_t data_len, uint8_t *hash)
{
    if (data == NULL || data_len == 0 || hash == NULL) {
        ESP_LOGE(TAG, "Invalid input parameters");
        return ESP_FAIL;
    }

    size_t hash_len = 0;
    psa_status_t status = psa_hash_compute(PSA_ALG_SHA_1, data, data_len, hash, SHA1_LEN, &hash_len);
    if (status != PSA_SUCCESS || hash_len != SHA1_LEN) {
        ESP_LOGE(TAG, "Failed to compute SHA-1 hash");
        return ESP_FAIL;
    }
    return ESP_OK;
}
