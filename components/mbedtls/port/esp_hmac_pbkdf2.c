/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include <string.h>
#include "esp_hmac.h"
#include "sdkconfig.h"

#include "esp_hmac_pbkdf2.h"

#define SHA256_DIGEST_SZ        32

esp_err_t esp_hmac_derive_pbkdf2_key(hmac_key_id_t key_id, const uint8_t *salt, size_t salt_len, int iterations, size_t key_len, uint8_t *out_key)
{
    if (!out_key || !salt || key_len == 0 || salt_len == 0 || iterations <= 0) {
        return ESP_ERR_INVALID_ARG;
    }

    if (key_id >= HMAC_KEY_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t U[SHA256_DIGEST_SZ] = {0};
    uint8_t T[SHA256_DIGEST_SZ] = {0};
    uint8_t counter[4] = {0, 0, 0, 1};

    uint8_t *hmac_input = calloc(1, salt_len + sizeof(counter));
    if (!hmac_input) {
        return ESP_ERR_NO_MEM;
    }

    esp_err_t err = ESP_FAIL;
    size_t remaining = key_len;
    uint8_t *out_p = out_key;

    while (remaining > 0) {
        memcpy(hmac_input, salt, salt_len);
        memcpy(hmac_input + salt_len, counter, sizeof(counter));

        err = esp_hmac_calculate(key_id, hmac_input, salt_len + sizeof(counter), U);
        if (err != ESP_OK) {
            goto cleanup;
        }

        memcpy(T, U, SHA256_DIGEST_SZ);
        for (int i = 1; i < iterations; i++) {
            err = esp_hmac_calculate(key_id, U, SHA256_DIGEST_SZ, U);
            if (err != ESP_OK) {
                goto cleanup;
            }

            for (int j = 0; j < SHA256_DIGEST_SZ; j++) {
                T[j] ^= U[j];
            }
        }

        size_t copy_len = (remaining < SHA256_DIGEST_SZ) ? remaining : SHA256_DIGEST_SZ;
        memcpy(out_p, T, copy_len);
        out_p += copy_len;
        remaining -= copy_len;

        for (int i = 3; i >= 0; i--) {
            if (++counter[i]) {
                break;
            }
        }
    }

    err = ESP_OK;

cleanup:
    memset(U, 0x00, sizeof(U));
    memset(T, 0x00, sizeof(T));
    if (hmac_input) {
        memset(hmac_input, 0x00, salt_len + sizeof(counter));
        free(hmac_input);
    }
    return err;
}
