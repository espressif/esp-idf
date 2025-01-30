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
#include "mbedtls/aes.h"

int nvs_bootloader_aes_crypt_ecb(enum AES_TYPE mode,
                                const unsigned char *key,
                                enum AES_BITS key_bits,
                                const unsigned char input[16],
                                unsigned char output[16])
{
    int ret = -1;

    uint16_t keybits = key_bits == AES256 ? 256 : key_bits == AES192 ? 192 : 128;
    int mbedtls_aes_mode = mode == AES_ENC ? MBEDTLS_AES_ENCRYPT : MBEDTLS_AES_DECRYPT;

    mbedtls_aes_context ctx;
    mbedtls_aes_init(&ctx);

    if (mode == AES_ENC) {
        ret = mbedtls_aes_setkey_enc(&ctx, key, keybits);
    } else {
        ret = mbedtls_aes_setkey_dec(&ctx, key, keybits);
    }

    if (ret != 0) {
        mbedtls_aes_free(&ctx);
        return ret;
    }

    ret = mbedtls_aes_crypt_ecb(&ctx, mbedtls_aes_mode, input, output);

    if (ret != 0) {
        mbedtls_aes_free(&ctx);
        return ret;
    }

    mbedtls_aes_free(&ctx);
    return ret;
}
#endif /* !(BOOTLOADER_BUILD && !CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL_BOOTLOADER) */
#endif /* !SOC_AES_SUPPORTED */
