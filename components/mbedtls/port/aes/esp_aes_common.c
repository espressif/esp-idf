/*
 * AES block cipher, ESP hardware accelerated version, common
 * Based on mbedTLS FIPS-197 compliant version.
 *
 * SPDX-FileCopyrightText: The Mbed TLS Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2016-2025 Espressif Systems (Shanghai) CO LTD
 */
/*
 *  The AES block cipher was designed by Vincent Rijmen and Joan Daemen.
 *
 *  http://csrc.nist.gov/encryption/aes/rijndael/Rijndael.pdf
 *  http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
 */
#include "sdkconfig.h"
#include "aes/esp_aes_internal.h"
#include "mbedtls/aes.h"
#include "hal/aes_hal.h"
#include "hal/aes_types.h"
#include "soc/soc_caps.h"
#include "mbedtls/error.h"

#include <string.h>
#include "mbedtls/platform.h"

#if SOC_AES_SUPPORT_DMA
#include "esp_aes_dma_priv.h"
#endif

bool valid_key_length(const esp_aes_context *ctx)
{
    bool valid_len = (ctx->key_bytes == AES_128_KEY_BYTES) || (ctx->key_bytes == AES_256_KEY_BYTES);

#if SOC_AES_SUPPORT_AES_192
    valid_len |= ctx->key_bytes == AES_192_KEY_BYTES;
#endif

    return valid_len;
}

void esp_aes_init(esp_aes_context *ctx)
{
    bzero(ctx, sizeof(esp_aes_context));
#if SOC_AES_SUPPORT_DMA && CONFIG_MBEDTLS_AES_USE_INTERRUPT
    esp_aes_intr_alloc();
#endif
}

void esp_aes_free( esp_aes_context *ctx )
{
    if ( ctx == NULL ) {
        return;
    }

    bzero( ctx, sizeof( esp_aes_context ) );
}

/*
 * AES key schedule (same for encryption or decryption, as hardware handles schedule)
 *
 */
int esp_aes_setkey( esp_aes_context *ctx, const unsigned char *key,
                    unsigned int keybits )
{
#if !SOC_AES_SUPPORT_AES_192
    if (keybits == 192) {
        return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
    }
#endif
    if (keybits != 128 && keybits != 192 && keybits != 256) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }
    ctx->key_bytes = keybits / 8;
    memcpy(ctx->key, key, ctx->key_bytes);
    ctx->key_in_hardware = 0;
    return 0;
}

#ifdef CONFIG_MBEDTLS_AES_USE_PSEUDO_ROUND_FUNC
/* The total number of pseudo-rounds randomly inserted in an AES operation are controlled by
 * configuring the AES_PSEUDO_BASE, AES_PSEUDO_INC parameters.
 * Users can also set the frequency of random key updates by configuring the AES_PSEUDO_RNG_CNT.
 * Here, we would be using some pre-decided values for these parameters corresponding to the security needed.
 * For more information regarding these parameters please refer the TRM.
 */
#define AES_PSEUDO_ROUNDS_BASE_LOW      4
#define AES_PSEUDO_ROUNDS_BASE_MEDIUM   7
#define AES_PSEUDO_ROUNDS_BASE_HIGH     15
#define AES_PSEUDO_ROUNDS_INC           3
#define AES_PSEUDO_ROUNDS_RNG_CNT       7

void esp_aes_enable_pseudo_rounds(esp_aes_psuedo_rounds_state_t state)
{
    if (state == ESP_AES_PSEUDO_ROUNDS_DISABLE) {
        aes_hal_enable_pseudo_rounds(false, 0, 0, 0);
    } else if (state == ESP_AES_PSEUDO_ROUNDS_LOW) {
        aes_hal_enable_pseudo_rounds(true, AES_PSEUDO_ROUNDS_BASE_LOW, AES_PSEUDO_ROUNDS_INC, AES_PSEUDO_ROUNDS_RNG_CNT);
    } else if (state == ESP_AES_PSEUDO_ROUNDS_MEDIUM) {
        aes_hal_enable_pseudo_rounds(true, AES_PSEUDO_ROUNDS_BASE_MEDIUM, AES_PSEUDO_ROUNDS_INC, AES_PSEUDO_ROUNDS_RNG_CNT);
    } else {
        aes_hal_enable_pseudo_rounds(true, AES_PSEUDO_ROUNDS_BASE_HIGH, AES_PSEUDO_ROUNDS_INC, AES_PSEUDO_ROUNDS_RNG_CNT);
    }
}
#endif /* CONFIG_MBEDTLS_AES_USE_PSEUDO_ROUND_FUNC */
