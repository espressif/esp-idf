/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include <string.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "psa/crypto.h"
#include "psa_crypto_driver_esp_hmac_opaque.h"
#include "esp_efuse.h"
#include "esp_efuse_chip.h"

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
    psa_status_t status;
    psa_key_id_t psa_key_id = 0;
    size_t remaining = key_len;
    uint8_t *out_p = out_key;

    // Setup key attributes for ESP-HMAC opaque driver
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_MESSAGE);
    psa_set_key_algorithm(&attributes, PSA_ALG_HMAC(PSA_ALG_SHA_256));
    psa_set_key_type(&attributes, PSA_KEY_TYPE_HMAC);
    psa_set_key_bits(&attributes, 256);
    psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_ESP_HMAC_VOLATILE);

    // Create opaque key reference
    esp_hmac_opaque_key_t opaque_key = {
        .use_km_key = false,
        .efuse_block = (uint8_t)(EFUSE_BLK_KEY0 + key_id),
    };

    // Import the opaque key
    status = psa_import_key(&attributes, (uint8_t *)&opaque_key,
                           sizeof(opaque_key), &psa_key_id);
    psa_reset_key_attributes(&attributes);

    if (status != PSA_SUCCESS) {
        err = ESP_ERR_INVALID_ARG;
        goto cleanup;
    }

    while (remaining > 0) {
        memcpy(hmac_input, salt, salt_len);
        memcpy(hmac_input + salt_len, counter, sizeof(counter));

        size_t mac_length = 0;
        status = psa_mac_compute(psa_key_id, PSA_ALG_HMAC(PSA_ALG_SHA_256),
                                hmac_input, salt_len + sizeof(counter),
                                U, SHA256_DIGEST_SZ, &mac_length);
        if (status != PSA_SUCCESS) {
            err = ESP_FAIL;
            goto cleanup;
        }

        memcpy(T, U, SHA256_DIGEST_SZ);
        for (int i = 1; i < iterations; i++) {
            status = psa_mac_compute(psa_key_id, PSA_ALG_HMAC(PSA_ALG_SHA_256),
                                    U, SHA256_DIGEST_SZ,
                                    U, SHA256_DIGEST_SZ, &mac_length);
            if (status != PSA_SUCCESS) {
                err = ESP_FAIL;
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
    if (psa_key_id != 0) {
        psa_destroy_key(psa_key_id);
    }
    memset(U, 0x00, sizeof(U));
    memset(T, 0x00, sizeof(T));
    if (hmac_input) {
        memset(hmac_input, 0x00, salt_len + sizeof(counter));
        free(hmac_input);
    }
    return err;
}
