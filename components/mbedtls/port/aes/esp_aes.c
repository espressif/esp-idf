/*
 * \brief AES block and DMA hardware accelerated version
 * Based on mbedTLS FIPS-197 compliant version.
 *
 * SPDX-FileCopyrightText: The Mbed TLS Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2025 Espressif Systems (Shanghai) CO LTD
 */
/*
 *  The AES block cipher was designed by Vincent Rijmen and Joan Daemen.
 *
 *  http://csrc.nist.gov/encryption/aes/rijndael/Rijndael.pdf
 *  http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
 */

#include <string.h>
#include "esp_log.h"
#include "esp_crypto_lock.h"
#include "hal/aes_hal.h"
#include "esp_aes_internal.h"
#include "esp_crypto_periph_clk.h"
#include "soc/soc_caps.h"
#include "sdkconfig.h"
#include "mbedtls/platform_util.h"

#if SOC_AES_GDMA
#define AES_LOCK() esp_crypto_sha_aes_lock_acquire()
#define AES_RELEASE() esp_crypto_sha_aes_lock_release()
#elif SOC_AES_CRYPTO_DMA
#define AES_LOCK() esp_crypto_dma_lock_acquire()
#define AES_RELEASE() esp_crypto_dma_lock_release()
#include "hal/crypto_dma_ll.h"
#endif

#define MBEDTLS_ERR_AES_BAD_INPUT_DATA                  -0x0021  /**< Invalid input data. */
#define MBEDTLS_ERR_AES_INVALID_KEY_LENGTH              -0x0020  /**< Invalid key length. */
#define MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH            -0x0022  /**< Invalid data input length. */

static const char *TAG = "esp-aes";

#if CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM
#if CONFIG_IDF_TARGET_ESP32P4
#define AES_DMA_MODE_THRESHOLD 512
#else
#define AES_DMA_MODE_THRESHOLD 128
#endif
#endif /* CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM */

#if !SOC_AES_SUPPORT_DMA
#include <freertos/FreeRTOS.h>

/* AES uses a spinlock mux not a lock as the underlying block operation
   only takes 208 cycles (to write key & compute block), +600 cycles
   for DPORT protection but +3400 cycles again if you use a full sized lock.

   For CBC, CFB, etc. this may mean that interrupts are disabled for a longer
   period of time for bigger lengths. However at the moment this has to happen
   anyway due to DPORT protection...
*/
static portMUX_TYPE aes_spinlock = portMUX_INITIALIZER_UNLOCKED;
#endif

void esp_aes_acquire_hardware( void )
{
    /* Released by esp_aes_release_hardware()*/
#if SOC_AES_SUPPORT_DMA
    AES_LOCK();
#else
    portENTER_CRITICAL(&aes_spinlock);
#endif
    esp_crypto_aes_enable_periph_clk(true);
}

/* Function to disable AES and Crypto DMA clocks and release locks */
void esp_aes_release_hardware( void )
{
    esp_crypto_aes_enable_periph_clk(false);
#if SOC_AES_SUPPORT_DMA
    AES_RELEASE();
#else
    portEXIT_CRITICAL(&aes_spinlock);
#endif
}

static int esp_aes_validate_input(esp_aes_context *ctx, const unsigned char *input,
                                  unsigned char *output, unsigned char *iv, bool validate_iv)
{
    if (!ctx) {
        ESP_LOGE(TAG, "No AES context supplied");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!input) {
        ESP_LOGE(TAG, "No input supplied");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!output) {
        ESP_LOGE(TAG, "No output supplied");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    if (validate_iv && !iv) {
        ESP_LOGE(TAG, "No IV supplied");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    return 0;
}

#if !SOC_AES_SUPPORT_DMA || CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM
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
        memset(output, 0, 16);
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }
    i0 = input_words[0];
    i1 = input_words[1];
    i2 = input_words[2];
    i3 = input_words[3];

#ifdef CONFIG_MBEDTLS_AES_USE_PSEUDO_ROUND_FUNC
    esp_aes_enable_pseudo_rounds(CONFIG_MBEDTLS_AES_USE_PSEUDO_ROUND_FUNC_STRENGTH);
#endif /* CONFIG_MBEDTLS_AES_USE_PSEUDO_ROUND_FUNC */

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
#endif /* !SOC_AES_SUPPORT_DMA || CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM */

/*
 * AES-ECB single block encryption
 */
int esp_internal_aes_encrypt(esp_aes_context *ctx,
                             const unsigned char input[16],
                             unsigned char output[16] )
{
    return esp_aes_crypt_ecb(ctx, ESP_AES_ENCRYPT, input, output);
}

/*
 * AES-ECB single block decryption
 */
int esp_internal_aes_decrypt(esp_aes_context *ctx,
                             const unsigned char input[16],
                             unsigned char output[16] )
{
    return esp_aes_crypt_ecb(ctx, ESP_AES_DECRYPT, input, output);
}

/*
 * AES-ECB block encryption/decryption
 */
int esp_aes_crypt_ecb(esp_aes_context *ctx,
                      int mode,
                      const unsigned char input[16],
                      unsigned char output[16] )
{
    // Use helper for validation
    int ret = esp_aes_validate_input(ctx, input, output, NULL, false);
    if (ret != 0) {
        return ret;
    }

    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;
    ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, mode);

#if SOC_AES_SUPPORT_DMA && !CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM
    aes_hal_mode_init(ESP_AES_BLOCK_MODE_ECB);
    ret = esp_aes_process_dma(ctx, input, output, AES_BLOCK_BYTES, NULL);
#else
    ret = esp_aes_block(ctx, input, output);
#endif

    esp_aes_release_hardware();
    return ret;
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
    // Use helpers for validation
    int ret = esp_aes_validate_input(ctx, input, output, iv, true);
    if (ret != 0) {
        return ret;
    }

    /* For CBC input length should be multiple of AES BLOCK BYTES */
    if ( (length % AES_BLOCK_BYTES) || (length == 0) ) {
        return ERR_ESP_AES_INVALID_INPUT_LENGTH;
    }

    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;

#if SOC_AES_SUPPORT_DMA
#if CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM
    if (length > AES_DMA_MODE_THRESHOLD) {
#endif /* CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM */
        ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, mode);
        aes_hal_mode_init(ESP_AES_BLOCK_MODE_CBC);
        aes_hal_set_iv(iv);

        ret = esp_aes_process_dma(ctx, input, output, length, NULL);
        if (ret != 0) {
            goto cleanup;
        }

        aes_hal_read_iv(iv);
#if CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM
    } else
#endif /* CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM */
#endif /* SOC_AES_SUPPORT_DMA */
#if !SOC_AES_SUPPORT_DMA || CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM
    {
        ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, mode);

        uint32_t *output_words = (uint32_t *)output;
        const uint32_t *input_words = (const uint32_t *)input;
        uint32_t *iv_words = (uint32_t *)iv;
        unsigned char temp[16];

        if (mode == ESP_AES_DECRYPT) {
            while (length > 0) {
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
            while (length > 0) {

                output_words[0] = input_words[0] ^ iv_words[0];
                output_words[1] = input_words[1] ^ iv_words[1];
                output_words[2] = input_words[2] ^ iv_words[2];
                output_words[3] = input_words[3] ^ iv_words[3];

                ret = esp_aes_block(ctx, output_words, output_words);
                if (ret != 0) {
                    goto cleanup;
                }

                memcpy(iv_words, output_words, 16);

                input_words  += 4;
                output_words += 4;
                length -= 16;
            }
        }
        ret = 0;
    }
#endif /* !SOC_AES_SUPPORT_DMA || CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM */

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
    unsigned char c;
    unsigned char ov[17];

    // Use helpers for validation
    int ret = esp_aes_validate_input(ctx, input, output, iv, true);
    if (ret != 0) {
        return ret;
    }

    esp_aes_acquire_hardware();

#if SOC_AES_SUPPORT_DMA
#if CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM
    if (length > AES_DMA_MODE_THRESHOLD) {
#endif /* CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM */
        size_t block_bytes = length - (length % AES_BLOCK_BYTES);

        /* The DMA engine will only output correct IV if it runs
        full blocks of input in CFB8 mode
        */
        if (block_bytes > 0) {
            ctx->key_in_hardware = 0;
            ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, mode);
            aes_hal_mode_init(ESP_AES_BLOCK_MODE_CFB8);
            aes_hal_set_iv(iv);

            ret = esp_aes_process_dma(ctx, input, output, block_bytes, NULL);
            if (ret != 0) {
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
            ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, ESP_AES_ENCRYPT);
            aes_hal_mode_init(ESP_AES_BLOCK_MODE_ECB);

            while ( length-- ) {
                memcpy( ov, iv, 16 );

                ret = esp_aes_process_dma(ctx, iv, iv, AES_BLOCK_BYTES, NULL);
                if (ret != 0) {
                    goto cleanup;
                }

                if ( mode == ESP_AES_DECRYPT ) {
                    ov[16] = *input;
                }

                c = *output++ = ( iv[0] ^ *input++ );

                if ( mode == ESP_AES_ENCRYPT ) {
                    ov[16] = c;
                }
                memcpy( iv, ov + 1, 16 );
            }
        }
        ret = 0;
#if CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM
    } else
#endif /* CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM */
#endif /* SOC_AES_SUPPORT_DMA */
#if !SOC_AES_SUPPORT_DMA || CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM
    {
        ctx->key_in_hardware = 0;
        ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, ESP_AES_ENCRYPT);

        while (length--) {
            memcpy( ov, iv, 16 );
            ret = esp_aes_block(ctx, iv, iv);
            if (ret != 0) {
                goto cleanup;
            }

            if (mode == ESP_AES_DECRYPT) {
                ov[16] = *input;
            }

            c = *output++ = (unsigned char)(iv[0] ^ *input++);

            if (mode == ESP_AES_ENCRYPT) {
                ov[16] = c;
            }

            memcpy( iv, ov + 1, 16 );
        }
        ret = 0;
    }
#endif /* !SOC_AES_SUPPORT_DMA || CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM */
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
    uint8_t c;
    size_t n;

    // Use helpers for validation
    int ret = esp_aes_validate_input(ctx, input, output, iv, true);
    if (ret != 0) {
        return ret;
    }

    if (!iv_off) {
        ESP_LOGE(TAG, "No IV offset supplied");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    n = *iv_off;

#if SOC_AES_SUPPORT_DMA
#if CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM
    if (length > AES_DMA_MODE_THRESHOLD) {
#endif /* CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM */
        size_t stream_bytes = 0;

        /* First process the *iv_off bytes
        * which are pending from the previous call to this API
        */
        while (n > 0 && length > 0) {
            if (mode == ESP_AES_ENCRYPT) {
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

            ret = esp_aes_process_dma(ctx, input, output, length, iv);
            if (ret != 0) {
                esp_aes_release_hardware();
                return ret;
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
                if (mode == ESP_AES_DECRYPT) {
                    memcpy(iv, input + length - stream_bytes, stream_bytes);
                }
            }
            esp_aes_release_hardware();
        }

        *iv_off = n + stream_bytes;
#if CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM
    } else
#endif /* CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM */
#endif /* SOC_AES_SUPPORT_DMA */
#if !SOC_AES_SUPPORT_DMA || CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM
    {
        esp_aes_acquire_hardware();
        ctx->key_in_hardware = 0;
        ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, ESP_AES_ENCRYPT);

        if (mode == ESP_AES_DECRYPT) {
            while (length--) {
                if (n == 0) {
                    ret = esp_aes_block(ctx, iv, iv);
                    if (ret != 0) {
                        esp_aes_release_hardware();
                        return ret;
                    }
                }

                c = *input++;
                *output++ = (unsigned char)( c ^ iv[n] );
                iv[n] = (unsigned char) c;
                n = (n + 1) & 0x0F;
            }
        } else {
            while (length--) {
                if ( n == 0 ) {
                    ret = esp_aes_block(ctx, iv, iv);
                    if (ret != 0) {
                        esp_aes_release_hardware();
                        return ret;
                    }
                }

                iv[n] = *output++ = (unsigned char)(iv[n] ^ *input++);
                n = ( n + 1 ) & 0x0F;
            }
        }

        esp_aes_release_hardware();
        *iv_off = n;
    }
#endif /* !SOC_AES_SUPPORT_DMA || CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM */
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

    // Use helpers for validation
    int ret = esp_aes_validate_input(ctx, input, output, iv, true);
    if (ret != 0) {
        return ret;
    }

    if (!iv_off) {
        ESP_LOGE(TAG, "No IV offset supplied");
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    n = *iv_off;

#if SOC_AES_SUPPORT_DMA
#if CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM
    if (length > AES_DMA_MODE_THRESHOLD) {
#endif /* CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM */
        size_t stream_bytes = 0;

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

            ret = esp_aes_process_dma(ctx, input, output, length, iv);
            if (ret != 0) {
                esp_aes_release_hardware();
                return ret;
            }

            aes_hal_read_iv(iv);
            esp_aes_release_hardware();
        }

        *iv_off = n + stream_bytes;
#if CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM
    } else
#endif /* CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM */
#endif /* SOC_AES_SUPPORT_DMA */
#if !SOC_AES_SUPPORT_DMA || CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM
    {
        // TODO: Check if this is necessary as the DMA version does not have this check
        if (n > 15) {
            return (MBEDTLS_ERR_AES_BAD_INPUT_DATA);
        }

        esp_aes_acquire_hardware();
        ctx->key_in_hardware = 0;
        ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, ESP_AES_ENCRYPT);

        while (length--) {
            if (n == 0) {
                ret = esp_aes_block(ctx, iv, iv);
                if (ret != 0) {
                    esp_aes_release_hardware();
                    return ret;
                }
            }
            *output++ =  *input++ ^ iv[n];
            n = (n + 1) & 0x0F;
        }

        esp_aes_release_hardware();
        *iv_off = n;
    }
#endif /* !SOC_AES_SUPPORT_DMA || CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM */
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

    // Use helper for validation
    int ret = esp_aes_validate_input(ctx, input, output, NULL, false);
    if (ret != 0) {
        return ret;
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

    n = *nc_off;

#if SOC_AES_SUPPORT_DMA
#if CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM
    if (length > AES_DMA_MODE_THRESHOLD) {
#endif /* CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM */
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

            ret = esp_aes_process_dma(ctx, input, output, length, stream_block);
            if (ret != 0) {
                esp_aes_release_hardware();
                return ret;
            }

            aes_hal_read_iv(nonce_counter);
            esp_aes_release_hardware();
        }

        *nc_off = n + (length % AES_BLOCK_BYTES);
#if CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM
    } else
#endif /* CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM */
#endif /* SOC_AES_SUPPORT_DMA */
#if !SOC_AES_SUPPORT_DMA || CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM
    {
        esp_aes_acquire_hardware();
        ctx->key_in_hardware = 0;
        ctx->key_in_hardware = aes_hal_setkey(ctx->key, ctx->key_bytes, ESP_AES_ENCRYPT);

        int c, i;
        while (length--) {
            if (n == 0) {
                ret = esp_aes_block(ctx, nonce_counter, stream_block);
                if (ret != 0) {
                    esp_aes_release_hardware();
                    return ret;
                }

                for (i = 16; i > 0; i--) {
                    if (++nonce_counter[i - 1] != 0) {
                        break;
                    }
                }
            }
            c = *input++;
            *output++ = (unsigned char)(c ^ stream_block[n]);
            n = (n + 1) & 0x0F;
        }

        esp_aes_release_hardware();
        *nc_off = n;
    }
#endif /* !SOC_AES_SUPPORT_DMA || CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM */
    return 0;
}
