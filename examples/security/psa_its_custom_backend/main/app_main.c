/*
 * PSA ITS custom storage backend example
 *
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 *
 * Demonstrates the PSA ITS custom storage backend feature:
 *
 *   Key IDs 0x00000001 .. 0x2FFFFFFF  → default NVS backend (framework)
 *   Key IDs 0x30000000 .. 0x3FFFFFFF  → user-registered custom backend
 *                                       (this example: a separate NVS
 *                                       namespace "psa_its_ext")
 *
 * The custom UID range is configured via Kconfig
 * (CONFIG_MBEDTLS_PSA_ITS_CUSTOM_BACKEND_UID_{MIN,MAX}).
 */

#include <stdio.h>
#include <string.h>

#include "esp_err.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "psa/crypto.h"

#include "esp_psa_its.h"
#include "custom_nvs_its_backend.h"

static const char *TAG = "example";

/* ---- Key IDs ---- */

/* Default NVS range (handled by the framework) */
#define NVS_KEY_ID      ((psa_key_id_t) 0x00000001)

/* Custom range routed to the registered backend */
#define CUSTOM_KEY_ID   ((psa_key_id_t) 0x30000001)

/* ---- Helpers ---- */

static psa_status_t provision_aes_key(psa_key_id_t key_id, const char *label)
{
    psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t handle;
    psa_status_t status;

    status = psa_get_key_attributes(key_id, &attr);
    if (status == PSA_SUCCESS) {
        ESP_LOGI(TAG, "[%s] Key 0x%lx already exists", label, (unsigned long)key_id);
        psa_reset_key_attributes(&attr);
        return PSA_SUCCESS;
    }

    psa_reset_key_attributes(&attr);
    psa_set_key_id(&attr, key_id);
    psa_set_key_lifetime(&attr, PSA_KEY_LIFETIME_PERSISTENT);
    psa_set_key_type(&attr, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attr, 256);
    psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attr, PSA_ALG_CBC_NO_PADDING);

    status = psa_generate_key(&attr, &handle);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "[%s] Failed to generate key 0x%lx: %d",
                 label, (unsigned long)key_id, (int)status);
    } else {
        ESP_LOGI(TAG, "[%s] Generated persistent key 0x%lx",
                 label, (unsigned long)key_id);
    }

    psa_reset_key_attributes(&attr);
    return status;
}

static psa_status_t test_encrypt_decrypt(psa_key_id_t key_id, const char *label)
{
    const uint8_t plaintext[32] = "Hello PSA ITS custom backend!!";
    uint8_t ciphertext[16 + sizeof(plaintext)];
    uint8_t decrypted[sizeof(plaintext)];
    size_t ciphertext_len = 0;
    size_t decrypted_len = 0;

    psa_status_t status = psa_cipher_encrypt(key_id, PSA_ALG_CBC_NO_PADDING,
                                             plaintext, sizeof(plaintext),
                                             ciphertext, sizeof(ciphertext),
                                             &ciphertext_len);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "[%s] Encrypt failed: %d", label, (int)status);
        return status;
    }

    status = psa_cipher_decrypt(key_id, PSA_ALG_CBC_NO_PADDING,
                                ciphertext, ciphertext_len,
                                decrypted, sizeof(decrypted),
                                &decrypted_len);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "[%s] Decrypt failed: %d", label, (int)status);
        return status;
    }

    if (decrypted_len != sizeof(plaintext) ||
        memcmp(decrypted, plaintext, sizeof(plaintext)) != 0) {
        ESP_LOGE(TAG, "[%s] Decrypted data mismatch!", label);
        return PSA_ERROR_CORRUPTION_DETECTED;
    }

    ESP_LOGI(TAG, "[%s] Encrypt/decrypt OK with key 0x%lx",
             label, (unsigned long)key_id);
    return PSA_SUCCESS;
}

/* ---- Application entry ---- */

void app_main(void)
{
    psa_status_t status;

    ESP_LOGI(TAG, "=== PSA ITS Custom Storage Backend ===");

    /* Initialize NVS (used by both the default backend and our custom backend,
     * which keep their entries in distinct namespaces). */
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    /* Register the custom backend. The signatures of custom_nvs_its_*
     * match esp_psa_its_custom_ops_t exactly, so they can be bound directly. */
    static const esp_psa_its_custom_ops_t custom_ops = {
        .set      = custom_nvs_its_set,
        .get      = custom_nvs_its_get,
        .get_info = custom_nvs_its_get_info,
        .remove   = custom_nvs_its_remove,
        .ctx      = NULL,
    };

    status = esp_psa_its_register_custom_backend(&custom_ops);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to register custom backend: %d", (int)status);
        return;
    }
    ESP_LOGI(TAG, "Custom ITS backend registered");

    /* Provision and exercise one key per storage path. */
    ESP_LOGI(TAG, "--- Provisioning keys ---");
    if (provision_aes_key(NVS_KEY_ID, "NVS") != PSA_SUCCESS) {
        return;
    }
    if (provision_aes_key(CUSTOM_KEY_ID, "CUSTOM") != PSA_SUCCESS) {
        return;
    }

    ESP_LOGI(TAG, "--- Testing encryption/decryption ---");
    if (test_encrypt_decrypt(NVS_KEY_ID, "NVS") != PSA_SUCCESS) {
        return;
    }
    if (test_encrypt_decrypt(CUSTOM_KEY_ID, "CUSTOM") != PSA_SUCCESS) {
        return;
    }

    ESP_LOGI(TAG, "=== All tests passed ===");
}
