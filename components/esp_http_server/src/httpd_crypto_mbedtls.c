/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbedtls/sha1.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "httpd_crypto_mbedtls";

#define SHA1_LEN (20)

esp_err_t httpd_crypto_sha1(const uint8_t *data, size_t data_len, uint8_t *hash)
{
    if (data == NULL || data_len == 0 || hash == NULL) {
        ESP_LOGE(TAG, "Invalid input parameters");
        return ESP_FAIL;
    }

    esp_err_t err = ESP_FAIL;
    mbedtls_sha1_context ctx;
    mbedtls_sha1_init(&ctx);

    if (mbedtls_sha1_starts(&ctx) != 0) {
        ESP_LOGE(TAG, "Failed to start SHA-1 hash");
        goto exit;
    }

    if (mbedtls_sha1_update(&ctx, data, data_len) != 0) {
        ESP_LOGE(TAG, "Failed to update SHA-1 hash");
        goto exit;
    }
    if (mbedtls_sha1_finish(&ctx, hash) != 0) {
        ESP_LOGE(TAG, "Failed to finish SHA-1 hash");
        goto exit;
    }

    err = ESP_OK;

exit:
    mbedtls_sha1_free(&ctx);
    return err;
}
