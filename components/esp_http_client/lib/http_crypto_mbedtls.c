/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "http_crypto.h"
#include "esp_rom_md5.h"
#include "esp_err.h"
#include "mbedtls/sha256.h"
#include "esp_log.h"

static const char *TAG = "http_crypto_mbedtls";


esp_err_t http_crypto_sha256(const uint8_t *data, size_t data_len, uint8_t *hash)
{
    if (data == NULL || data_len == 0 || hash == NULL) {
        ESP_LOGE(TAG, "Invalid input parameters");
        return ESP_FAIL;
    }

    esp_err_t err = ESP_FAIL;
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);

    if (mbedtls_sha256_starts(&ctx, false) != 0) {
        ESP_LOGE(TAG, "Failed to start SHA-256 hash");
        goto exit;
    }

    if (mbedtls_sha256_update(&ctx, data, data_len) != 0) {
        ESP_LOGE(TAG, "Failed to update SHA-256 hash");
        goto exit;
    }

    if (mbedtls_sha256_finish(&ctx, hash) != 0) {
        ESP_LOGE(TAG, "Failed to finish SHA-256 hash");
        goto exit;
    }

    err = ESP_OK;

exit:
    mbedtls_sha256_free(&ctx);
    return err;
}
