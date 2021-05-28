/**
 * \brief AES block cipher, ESP hardware accelerated version, common
 * Based on mbedTLS FIPS-197 compliant version.
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  Additions Copyright (C) 2016-2017, Espressif Systems (Shanghai) PTE Ltd
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */
/*
 *  The AES block cipher was designed by Vincent Rijmen and Joan Daemen.
 *
 *  http://csrc.nist.gov/encryption/aes/rijndael/Rijndael.pdf
 *  http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
 */

#include "aes/esp_aes_internal.h"
#include "mbedtls/aes.h"
#include "hal/aes_hal.h"
#include "hal/aes_types.h"
#include "soc/soc_caps.h"
#include "mbedtls/error.h"

#include <string.h>
#include "mbedtls/platform.h"

#if SOC_AES_GDMA
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


void esp_aes_init( esp_aes_context *ctx )
{
    bzero( ctx, sizeof( esp_aes_context ) );
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
