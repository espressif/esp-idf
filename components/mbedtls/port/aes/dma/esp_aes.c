/**
 * \brief AES block cipher, ESP DMA hardware accelerated version
 * Based on mbedTLS FIPS-197 compliant version.
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  Additions Copyright (C) 2016-2020, Espressif Systems (Shanghai) PTE Ltd
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
#include "esp_log.h"
#include "esp_crypto_lock.h"
#include "hal/aes_hal.h"
#include "hal/aes_ll.h"
#include "esp_aes_internal.h"
#include "esp_private/esp_crypto_lock_internal.h"

#if SOC_AES_GDMA
#define AES_LOCK() esp_crypto_sha_aes_lock_acquire()
#define AES_RELEASE() esp_crypto_sha_aes_lock_release()
#elif SOC_AES_CRYPTO_DMA
#define AES_LOCK() esp_crypto_dma_lock_acquire()
#define AES_RELEASE() esp_crypto_dma_lock_release()
#include "hal/crypto_dma_ll.h"
#endif

static const char *TAG = "esp-aes";

void esp_aes_acquire_hardware( void )
{
    /* Released by esp_aes_release_hardware()*/
    AES_LOCK();

    AES_RCC_ATOMIC() {
        aes_ll_enable_bus_clock(true);
#if SOC_AES_CRYPTO_DMA
        crypto_dma_ll_enable_bus_clock(true);
#endif
        aes_ll_reset_register();
#if SOC_AES_CRYPTO_DMA
        crypto_dma_ll_reset_register();
#endif
    }
}

/* Function to disable AES and Crypto DMA clocks and release locks */
void esp_aes_release_hardware( void )
{
    AES_RCC_ATOMIC() {
        aes_ll_enable_bus_clock(false);
#if SOC_AES_CRYPTO_DMA
        crypto_dma_ll_enable_bus_clock(false);
#endif
    }

    AES_RELEASE();
}

static int esp_aes_validate_input(esp_aes_context *ctx, const unsigned char *input,
                                  unsigned char *output )
{
    if (!ctx) {
        ESP_LOGE(TAG, "No AES context supplied");
        return -1;
    }
    if (!input) {
        ESP_LOGE(TAG, "No input supplied");
        return -1;
    }
    if (!output) {
        ESP_LOGE(TAG, "No output supplied");
        return -1;
    }

    return 0;
}


/*
 * AES-ECB single block encryption
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
    aes_hal_mode_init(ESP_AES_BLOCK_MODE_ECB);
    r = esp_aes_process_dma(ctx, input, output, AES_BLOCK_BYTES, NULL);
    esp_aes_release_hardware();

    return r;
}

void esp_aes_encrypt(esp_aes_context *ctx,
                     const unsigned char input[16],
                     unsigned char output[16] )
{
    esp_internal_aes_encrypt(ctx, input, output);
}

/*
 * AES-ECB single block decryption
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
    aes_hal_mode_init(ESP_AES_BLOCK_MODE_ECB);
    r = esp_aes_process_dma(ctx, input, output, AES_BLOCK_BYTES, NULL);
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
    aes_hal_mode_init(ESP_AES_BLOCK_MODE_ECB);
    r = esp_aes_process_dma(ctx, input, output, AES_BLOCK_BYTES, NULL);
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
    int r = -1;
    if (esp_aes_validate_input(ctx, input, output)) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!iv) {
        ESP_LOGE(TAG, "No IV supplied");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    /* For CBC input length should be multiple of
     * AES BLOCK BYTES
     * */
    if ( (length % AES_BLOCK_BYTES) || (length == 0) ) {
        return ERR_ESP_AES_INVALID_INPUT_LENGTH;
    }

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;
    ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, mode);
    aes_hal_mode_init(ESP_AES_BLOCK_MODE_CBC);
    aes_hal_set_iv(iv);

    r = esp_aes_process_dma(ctx, input, output, length, NULL);
    if (r != 0) {
        goto cleanup;
    }

    aes_hal_read_iv(iv);

cleanup:
    esp_aes_release_hardware();
    return r;
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
    int r = -1;
    unsigned char c;
    unsigned char ov[17];
    size_t block_bytes = length - (length % AES_BLOCK_BYTES);

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

    /* The DMA engine will only output correct IV if it runs
       full blocks of input in CFB8 mode
    */
    esp_aes_acquire_hardware();

    if (block_bytes > 0) {

        ctx->key_in_hardware = 0;
        ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, mode);
        aes_hal_mode_init(ESP_AES_BLOCK_MODE_CFB8);
        aes_hal_set_iv(iv);
        r = esp_aes_process_dma(ctx, input, output, block_bytes, NULL);
        if (r != 0) {
            goto cleanup;
        }

        aes_hal_read_iv(iv);

        length -= block_bytes;
        input += block_bytes;
        output += block_bytes;
    }

    // Process remaining bytes block-at-a-time in ECB mode
    if (length > 0) {
        ctx->key_in_hardware = 0;
        ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, MBEDTLS_AES_ENCRYPT);
        aes_hal_mode_init(ESP_AES_BLOCK_MODE_ECB);

        while ( length-- ) {
            memcpy( ov, iv, 16 );

            r = esp_aes_process_dma(ctx, iv, iv, AES_BLOCK_BYTES, NULL);
            if (r != 0) {
                goto cleanup;
            }

            if ( mode == MBEDTLS_AES_DECRYPT ) {
                ov[16] = *input;
            }

            c = *output++ = ( iv[0] ^ *input++ );

            if ( mode == MBEDTLS_AES_ENCRYPT ) {
                ov[16] = c;
            }
            memcpy( iv, ov + 1, 16 );
        }

    }
    r = 0;

cleanup:
    esp_aes_release_hardware();
    return r;
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
    uint8_t c;
    size_t stream_bytes = 0;
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

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    n = *iv_off;

    /* First process the *iv_off bytes
     * which are pending from the previous call to this API
     */
    while (n > 0 && length > 0) {
        if (mode == MBEDTLS_AES_ENCRYPT) {
            iv[n] = *output++ = *input++ ^ iv[n];
        } else {
            c = *input++;
            *output++ = c ^ iv[n];
            iv[n] = c;
        }
        n = (n + 1) % AES_BLOCK_BYTES;
        length--;
    }


    if (length > 0) {
        stream_bytes = length % AES_BLOCK_BYTES;
        esp_aes_acquire_hardware();
        ctx->key_in_hardware = 0;
        ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, mode);
        aes_hal_mode_init(ESP_AES_BLOCK_MODE_CFB128);
        aes_hal_set_iv(iv);

        int r = esp_aes_process_dma(ctx, input, output, length, iv);
        if (r != 0) {
            esp_aes_release_hardware();
            return r;
        }

        if (stream_bytes == 0) {
            // if we didn't need the partial 'stream block' then the new IV is in the IV register
            aes_hal_read_iv(iv);
        } else {
            // if we did process a final partial block the new IV is already processed via DMA (and has some bytes of output in it),
            // In decrypt mode any partial bytes are output plaintext (iv ^ c) and need to be swapped back to ciphertext (as the next
            // block uses ciphertext as its IV input)
            //
            // Note: It may be more efficient to not process the partial block via DMA in this case.
            if (mode == MBEDTLS_AES_DECRYPT) {
                memcpy(iv, input + length - stream_bytes, stream_bytes);
            }
        }
        esp_aes_release_hardware();
    }

    *iv_off = n + stream_bytes;
    return 0;
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
    size_t n;
    size_t stream_bytes = 0;

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

    /* If there is an offset then use the output of the previous AES block
        (the updated IV) to calculate the new output */
    while (n > 0 && length > 0) {
        *output++ = (*input++ ^ iv[n]);
        n = (n + 1) & 0xF;
        length--;
    }
    if (length > 0) {
        stream_bytes = (length % AES_BLOCK_BYTES);

        esp_aes_acquire_hardware();
        ctx->key_in_hardware = 0;
        ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, ESP_AES_DECRYPT);
        aes_hal_mode_init(ESP_AES_BLOCK_MODE_OFB);
        aes_hal_set_iv(iv);

        int r = esp_aes_process_dma(ctx, input, output, length, iv);
        if (r != 0) {
            esp_aes_release_hardware();
            return r;
        }

        aes_hal_read_iv(iv);
        esp_aes_release_hardware();
    }

    *iv_off = n + stream_bytes;

    return 0;
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
    size_t n;

    if (esp_aes_validate_input(ctx, input, output)) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!stream_block) {
        ESP_LOGE(TAG, "No stream supplied");
        return -1;
    }

    if (!nonce_counter) {
        ESP_LOGE(TAG, "No nonce supplied");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!nc_off) {
        ESP_LOGE(TAG, "No nonce offset supplied");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    n = *nc_off;

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    /* Process any unprocessed bytes left in stream block from
       last operation */
    while (n > 0 && length > 0) {
        *output++ = (unsigned char)(*input++ ^ stream_block[n]);
        n = (n + 1) & 0xF;
        length--;
    }

    if (length > 0) {

        esp_aes_acquire_hardware();
        ctx->key_in_hardware = 0;
        ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, ESP_AES_DECRYPT);

        aes_hal_mode_init(ESP_AES_BLOCK_MODE_CTR);
        aes_hal_set_iv(nonce_counter);

        int r = esp_aes_process_dma(ctx, input, output, length, stream_block);

        if (r != 0) {
            esp_aes_release_hardware();
            return r;
        }

        aes_hal_read_iv(nonce_counter);

        esp_aes_release_hardware();

    }
    *nc_off = n + (length % AES_BLOCK_BYTES);

    return 0;
}
