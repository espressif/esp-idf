/*
 * AES block cipher, ESP hardware accelerated version, common
 * Based on mbedTLS FIPS-197 compliant version.
 *
 * SPDX-FileCopyrightText: The Mbed TLS Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2016-2023 Espressif Systems (Shanghai) CO LTD
 */
/*
 *  The AES block cipher was designed by Vincent Rijmen and Joan Daemen.
 *
 *  http://csrc.nist.gov/encryption/aes/rijndael/Rijndael.pdf
 *  http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
 */
#include "sdkconfig.h"
#include "esp_aes_internal.h"
#include "mbedtls/aes.h"
#include "hal/aes_hal.h"
#include "hal/aes_types.h"
#include "soc/soc_caps.h"
#include "mbedtls/error.h"

#include <string.h>

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
