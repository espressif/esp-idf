/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <stdbool.h>
#include "esp_log.h"
#include "soc/soc_caps.h"
#include "nvs_bootloader_aes.h"
#include "sdkconfig.h"

#if SOC_AES_SUPPORTED
int nvs_bootloader_aes_crypt_ecb(enum AES_TYPE mode,
                                const unsigned char *key,
                                enum AES_BITS key_bits,
                                const unsigned char input[16],
                                unsigned char output[16])
{
    int ret = -1;
    ets_aes_enable();

#if CONFIG_IDF_TARGET_ESP32
    if (mode == AES_ENC) {
        ret = ets_aes_setkey_enc(key, key_bits);
    } else {
        ret = ets_aes_setkey_dec(key, key_bits);
    }

    if (ret) {
        ets_aes_crypt(input, output);
        // In case of esp32, ets_aes_setkey_dec returns 1 on success,
        // whereas for other targets ets_aes_setkey return 0 on success
        ret = 0;
    }
#else /* CONFIG_IDF_TARGET_ESP32 */
    ret = ets_aes_setkey(mode, key, key_bits);

    if (ret == 0) {
        ets_aes_block(input, output);
    }
#endif /* !CONFIG_IDF_TARGET_ESP32 */

    ets_aes_disable();
    return ret;
}

#else /* SOC_AES_SUPPORTED */
#if BOOTLOADER_BUILD && !CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL_BOOTLOADER
#if CONFIG_ESP_ROM_HAS_MBEDTLS_CRYPTO_LIB
#error "Enable `CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL_BOOTLOADER` for non SOC_AES_SUPPORTED targets for supporting NVS encryption in bootloader build"
#else /* !CONFIG_ESP_ROM_HAS_MBEDTLS_CRYPTO_LIB */
// TODO: IDF-11673
// Due to unavailability of an software AES layer for bootloader build,
// we cannot support the below NVS bootloader's AES operations
// Thus we are adding stub APIs to indicate that the following operation fail.

static const char *TAG = "nvs_bootloader_aes";
static const char *op_unsupported_error = "AES operation in bootloader unsupported for this target";

int nvs_bootloader_aes_crypt_ecb(enum AES_TYPE mode,
                                const unsigned char *key,
                                enum AES_BITS key_bits,
                                const unsigned char input[16],
                                unsigned char output[16])
{
    ESP_EARLY_LOGE(TAG, "%s", op_unsupported_error);
    abort();
    return -1;
}
#endif /* CONFIG_ESP_ROM_HAS_MBEDTLS_CRYPTO_LIB */
#else /* BOOTLOADER_BUILD && !CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL_BOOTLOADER */
#include "psa/crypto.h"

static const char *TAG = "nvs_bootloader_aes";

int nvs_bootloader_aes_crypt_ecb(enum AES_TYPE mode,
                                const unsigned char *key,
                                enum AES_BITS key_bits,
                                const unsigned char input[16],
                                unsigned char output[16])
{
    psa_status_t status;
    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
    psa_key_id_t key_id = 0;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&key_attributes, PSA_ALG_ECB_NO_PADDING);
    psa_set_key_type(&key_attributes, PSA_KEY_TYPE_AES);
    uint16_t keybits = key_bits == AES256 ? 256 : key_bits == AES192 ? 192 : 128;
    psa_set_key_bits(&key_attributes, keybits);
    status = psa_import_key(&key_attributes, key, keybits / 8, &key_id);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to import key: %d", status);
        return -1;
    }
    psa_reset_key_attributes(&key_attributes);

    if (mode == AES_ENC) {
        status = psa_cipher_encrypt_setup(&operation, key_id, PSA_ALG_ECB_NO_PADDING);
    } else {
        status = psa_cipher_decrypt_setup(&operation, key_id, PSA_ALG_ECB_NO_PADDING);
    }
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to setup cipher operation: %d", status);
        psa_destroy_key(key_id);
        return -1;
    }

    size_t output_len = 0;
    status = psa_cipher_update(&operation, input, 16, output, 16, &output_len);
    if (status != PSA_SUCCESS || output_len != 16) {
        ESP_LOGE(TAG, "Failed to update cipher operation: %d", status);
        psa_cipher_abort(&operation);
        psa_destroy_key(key_id);
        return -1;
    }

    status = psa_cipher_finish(&operation, output + output_len, 16 - output_len, &output_len);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to finish cipher operation: %d", status);
        psa_cipher_abort(&operation);
        psa_destroy_key(key_id);
        return -1;
    }

    if (output_len != 0) {
        ESP_LOGE(TAG, "Output length mismatch: expected 0, got %zu", output_len);
        psa_cipher_abort(&operation);
        psa_destroy_key(key_id);
        return -1;
    }

    status = psa_cipher_finish(&operation, output, 16, &output_len);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to finish cipher operation: %d", status);
        psa_destroy_key(key_id);
        return -1;
    }

    psa_destroy_key(key_id);
    return 0;
}
#endif /* !(BOOTLOADER_BUILD && !CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL_BOOTLOADER) */
#endif /* !SOC_AES_SUPPORTED */
