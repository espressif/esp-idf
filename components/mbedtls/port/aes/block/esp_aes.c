/**
 * \brief AES block cipher, ESP block hardware accelerated version
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
#include <string.h>
#include "mbedtls/aes.h"
#include "mbedtls/platform_util.h"
#include "esp_log.h"
#include "aes/esp_aes.h"
#include "soc/hwcrypto_periph.h"
#include <sys/lock.h>
#include "hal/aes_hal.h"
#include "aes/esp_aes_internal.h"

#include <freertos/FreeRTOS.h>

#include <stdio.h>
#include "esp_private/periph_ctrl.h"


static const char *TAG = "esp-aes";
/* AES uses a spinlock mux not a lock as the underlying block operation
   only takes 208 cycles (to write key & compute block), +600 cycles
   for DPORT protection but +3400 cycles again if you use a full sized lock.

   For CBC, CFB, etc. this may mean that interrupts are disabled for a longer
   period of time for bigger lengths. However at the moment this has to happen
   anyway due to DPORT protection...
*/
static portMUX_TYPE aes_spinlock = portMUX_INITIALIZER_UNLOCKED;


void esp_aes_acquire_hardware( void )
{
    portENTER_CRITICAL(&aes_spinlock);

    /* Enable AES hardware */
    periph_module_enable(PERIPH_AES_MODULE);
}

void esp_aes_release_hardware( void )
{
    /* Disable AES hardware */
    periph_module_disable(PERIPH_AES_MODULE);

    portEXIT_CRITICAL(&aes_spinlock);
}



/* Run a single 16 byte block of AES, using the hardware engine.
 *
 * Call only while holding esp_aes_acquire_hardware().
 *
 * The function esp_aes_block zeroises the output buffer in the case of following conditions:
 * 1. If key is not written in the hardware
 * 2. If the fault injection check failed
 */
static int esp_aes_block(esp_aes_context *ctx, const void *input, void *output)
{
    uint32_t i0, i1, i2, i3;
    const uint32_t *input_words = (uint32_t *)input;
    uint32_t *output_words = (uint32_t *)output;

    /* If no key is written to hardware yet, either the user hasn't called
       mbedtls_aes_setkey_enc/mbedtls_aes_setkey_dec - meaning we also don't
       know which mode to use - or a fault skipped the
       key write to hardware. Treat this as a fatal error and zero the output block.
    */
    if (ctx->key_in_hardware != ctx->key_bytes) {
        mbedtls_platform_zeroize(output, 16);
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }
    i0 = input_words[0];
    i1 = input_words[1];
    i2 = input_words[2];
    i3 = input_words[3];

    aes_hal_transform_block(input, output);

    /* Physical security check: Verify the AES accelerator actually ran, and wasn't
       skipped due to external fault injection while starting the peripheral.

       Note that i0,i1,i2,i3 are copied from input buffer in case input==output.

       Bypassing this check requires at least one additional fault.
    */
    if (i0 == output_words[0] && i1 == output_words[1] && i2 == output_words[2] && i3 == output_words[3]) {
        // calling zeroing functions to narrow the
        // window for a double-fault of the abort step, here
        memset(output, 0, 16);
        mbedtls_platform_zeroize(output, 16);
        abort();
    }

    return 0;
}

static int esp_aes_validate_input(esp_aes_context *ctx, const unsigned char *input,
                                  const unsigned char *output )
{
    if (!ctx) {
        ESP_LOGD(TAG, "No AES context supplied");
        return -1;
    }
    if (!input) {
        ESP_LOGD(TAG, "No input supplied");
        return -1;
    }
    if (!output) {
        ESP_LOGD(TAG, "No output supplied");
        return -1;
    }

    return 0;
}


void esp_aes_encrypt(esp_aes_context *ctx,
                     const unsigned char input[16],
                     unsigned char output[16] )
{
    esp_internal_aes_encrypt(ctx, input, output);
}

/*
 * AES-ECB block encryption
 */
int esp_internal_aes_encrypt(esp_aes_context *ctx,
                             const unsigned char input[16],
                             unsigned char output[16] )
{
    int r = -1;

    if (esp_aes_validate_input(ctx, input, output)) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;
    ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, ESP_AES_ENCRYPT);
    r = esp_aes_block(ctx, input, output);
    esp_aes_release_hardware();
    return r;
}

void esp_aes_decrypt(esp_aes_context *ctx,
                     const unsigned char input[16],
                     unsigned char output[16] )
{
    esp_internal_aes_decrypt(ctx, input, output);
}

/*
 * AES-ECB block decryption
 */

int esp_internal_aes_decrypt(esp_aes_context *ctx,
                             const unsigned char input[16],
                             unsigned char output[16] )
{
    int r = -1;

    if (esp_aes_validate_input(ctx, input, output)) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;
    ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, ESP_AES_DECRYPT);
    r = esp_aes_block(ctx, input, output);
    esp_aes_release_hardware();
    return r;
}

/*
 * AES-ECB block encryption/decryption
 */
int esp_aes_crypt_ecb(esp_aes_context *ctx,
                      int mode,
                      const unsigned char input[16],
                      unsigned char output[16] )
{
    int r = -1;

    if (esp_aes_validate_input(ctx, input, output)) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;
    ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, mode);
    r = esp_aes_block(ctx, input, output);
    esp_aes_release_hardware();
    return r;
}


/*
 * AES-CBC buffer encryption/decryption
 */
int esp_aes_crypt_cbc(esp_aes_context *ctx,
                      int mode,
                      size_t length,
                      unsigned char iv[16],
                      const unsigned char *input,
                      unsigned char *output )
{
    int ret = -1;
    if (esp_aes_validate_input(ctx, input, output)) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!iv) {
        ESP_LOGD(TAG, "No IV supplied");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    uint32_t *output_words = (uint32_t *)output;
    const uint32_t *input_words = (const uint32_t *)input;
    uint32_t *iv_words = (uint32_t *)iv;
    unsigned char temp[16];

    if ( length % 16 ) {
        return ( ERR_ESP_AES_INVALID_INPUT_LENGTH );
    }

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;
    ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, mode);


    if ( mode == ESP_AES_DECRYPT ) {
        while ( length > 0 ) {
            memcpy(temp, input_words, 16);
            ret = esp_aes_block(ctx, input_words, output_words);
            if (ret != 0) {
                goto cleanup;
            }

            output_words[0] = output_words[0] ^ iv_words[0];
            output_words[1] = output_words[1] ^ iv_words[1];
            output_words[2] = output_words[2] ^ iv_words[2];
            output_words[3] = output_words[3] ^ iv_words[3];

            memcpy( iv_words, temp, 16 );

            input_words += 4;
            output_words += 4;
            length -= 16;
        }
    } else { // ESP_AES_ENCRYPT
        while ( length > 0 ) {

            output_words[0] = input_words[0] ^ iv_words[0];
            output_words[1] = input_words[1] ^ iv_words[1];
            output_words[2] = input_words[2] ^ iv_words[2];
            output_words[3] = input_words[3] ^ iv_words[3];

            ret = esp_aes_block(ctx, output_words, output_words);
            if (ret != 0) {
                goto cleanup;
            }

            memcpy( iv_words, output_words, 16 );

            input_words  += 4;
            output_words += 4;
            length -= 16;
        }
    }
    ret = 0;

cleanup:
    esp_aes_release_hardware();
    return ret;
}

/*
 * AES-CFB128 buffer encryption/decryption
 */
int esp_aes_crypt_cfb128(esp_aes_context *ctx,
                         int mode,
                         size_t length,
                         size_t *iv_off,
                         unsigned char iv[16],
                         const unsigned char *input,
                         unsigned char *output )
{
    int ret = -1;
    if (esp_aes_validate_input(ctx, input, output)) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!iv) {
        ESP_LOGE(TAG, "No IV supplied");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!iv_off) {
        ESP_LOGE(TAG, "No IV offset supplied");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    int c;
    size_t n = *iv_off;
    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;
    ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, ESP_AES_ENCRYPT);

    if ( mode == ESP_AES_DECRYPT ) {
        while ( length-- ) {
            if ( n == 0 ) {
                ret = esp_aes_block(ctx, iv, iv);
                if (ret != 0) {
                    goto cleanup;
                }
            }

            c = *input++;
            *output++ = (unsigned char)( c ^ iv[n] );
            iv[n] = (unsigned char) c;

            n = ( n + 1 ) & 0x0F;
        }
    } else {
        while ( length-- ) {
            if ( n == 0 ) {
                ret = esp_aes_block(ctx, iv, iv);
                if (ret != 0) {
                    goto cleanup;
                }
            }

            iv[n] = *output++ = (unsigned char)( iv[n] ^ *input++ );

            n = ( n + 1 ) & 0x0F;
        }
    }

    *iv_off = n;
    ret = 0;

cleanup:
    esp_aes_release_hardware();
    return ret;
}

/*
 * AES-CFB8 buffer encryption/decryption
 */
int esp_aes_crypt_cfb8(esp_aes_context *ctx,
                       int mode,
                       size_t length,
                       unsigned char iv[16],
                       const unsigned char *input,
                       unsigned char *output )
{
    int ret = -1;
    unsigned char c;
    unsigned char ov[17];

    if (esp_aes_validate_input(ctx, input, output)) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!iv) {
        ESP_LOGE(TAG, "No IV supplied");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;
    ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, ESP_AES_ENCRYPT);


    while ( length-- ) {
        memcpy( ov, iv, 16 );
        ret = esp_aes_block(ctx, iv, iv);
        if (ret != 0) {
            goto cleanup;
        }

        if ( mode == ESP_AES_DECRYPT ) {
            ov[16] = *input;
        }

        c = *output++ = (unsigned char)( iv[0] ^ *input++ );

        if ( mode == ESP_AES_ENCRYPT ) {
            ov[16] = c;
        }

        memcpy( iv, ov + 1, 16 );
    }
    ret = 0;

cleanup:
    esp_aes_release_hardware();
    return ret;
}

/*
 * AES-CTR buffer encryption/decryption
 */
int esp_aes_crypt_ctr(esp_aes_context *ctx,
                      size_t length,
                      size_t *nc_off,
                      unsigned char nonce_counter[16],
                      unsigned char stream_block[16],
                      const unsigned char *input,
                      unsigned char *output )
{
    int c, i, ret = -1;

    if (esp_aes_validate_input(ctx, input, output)) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!stream_block) {
        ESP_LOGE(TAG, "No stream supplied");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!nonce_counter) {
        ESP_LOGE(TAG, "No nonce supplied");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!nc_off) {
        ESP_LOGE(TAG, "No nonce offset supplied");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    size_t n = *nc_off;
    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;
    ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, ESP_AES_ENCRYPT);


    while ( length-- ) {
        if ( n == 0 ) {
            ret = esp_aes_block(ctx, nonce_counter, stream_block);
            if (ret != 0) {
                goto cleanup;
            }

            for ( i = 16; i > 0; i-- ) {
                if ( ++nonce_counter[i - 1] != 0 ) {
                    break;
                }
            }
        }
        c = *input++;
        *output++ = (unsigned char)( c ^ stream_block[n] );

        n = ( n + 1 ) & 0x0F;
    }

    *nc_off = n;
    ret = 0;

cleanup:
    esp_aes_release_hardware();
    return ret;
}

/*
 * AES-OFB (Output Feedback Mode) buffer encryption/decryption
 */
int esp_aes_crypt_ofb(esp_aes_context *ctx,
                      size_t length,
                      size_t *iv_off,
                      unsigned char iv[16],
                      const unsigned char *input,
                      unsigned char *output )
{
    int ret = -1;
    size_t n;

    if (esp_aes_validate_input(ctx, input, output)) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!iv) {
        ESP_LOGE(TAG, "No IV supplied");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!iv_off) {
        ESP_LOGE(TAG, "No IV offset supplied");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    n = *iv_off;

    if (n > 15) {
        return (MBEDTLS_ERR_AES_BAD_INPUT_DATA);
    }

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;
    ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, ESP_AES_ENCRYPT);


    while (length--) {
        if ( n == 0 ) {
            ret = esp_aes_block(ctx, iv, iv);
            if (ret != 0) {
                goto cleanup;
            }
        }
        *output++ =  *input++ ^ iv[n];

        n = ( n + 1 ) & 0x0F;
    }

    *iv_off = n;
    ret = 0;

cleanup:
    esp_aes_release_hardware();

    return ( ret );
}
