/**
 * \brief AES block cipher, ESP32 hardware accelerated version
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
#include "hwcrypto/aes.h"
#include "soc/dport_reg.h"
#include "soc/hwcrypto_reg.h"
#include <sys/lock.h>

#include <freertos/FreeRTOS.h>

#include "soc/cpu.h"
#include <stdio.h>


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
    /* newlib locks lazy initialize on ESP-IDF */
    portENTER_CRITICAL(&aes_spinlock);

    DPORT_STALL_OTHER_CPU_START();
    {
        /* Enable AES hardware */
        _DPORT_REG_SET_BIT(DPORT_PERI_CLK_EN_REG, DPORT_PERI_EN_AES);
        /* Clear reset on digital signature & secure boot units,
           otherwise AES unit is held in reset also. */
        _DPORT_REG_CLR_BIT(DPORT_PERI_RST_EN_REG,
                           DPORT_PERI_EN_AES
                           | DPORT_PERI_EN_DIGITAL_SIGNATURE
                           | DPORT_PERI_EN_SECUREBOOT);
    }
    DPORT_STALL_OTHER_CPU_END();
}

void esp_aes_release_hardware( void )
{
    DPORT_STALL_OTHER_CPU_START();
    {
        /* Disable AES hardware */
        _DPORT_REG_SET_BIT(DPORT_PERI_RST_EN_REG, DPORT_PERI_EN_AES);
        /* Don't return other units to reset, as this pulls
           reset on RSA & SHA units, respectively. */
        _DPORT_REG_CLR_BIT(DPORT_PERI_CLK_EN_REG, DPORT_PERI_EN_AES);
    }
    DPORT_STALL_OTHER_CPU_END();

    portEXIT_CRITICAL(&aes_spinlock);
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
    if (keybits != 128 && keybits != 192 && keybits != 256) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }
    ctx->key_bytes = keybits / 8;
    memcpy(ctx->key, key, ctx->key_bytes);
    return 0;
}

/*
 * Helper function to copy key from esp_aes_context buffer
 * to hardware key registers.
 *
 * Call only while holding esp_aes_acquire_hardware().
 */
static inline void esp_aes_setkey_hardware( esp_aes_context *ctx, int mode)
{
    const uint32_t MODE_DECRYPT_BIT = 4;
    unsigned mode_reg_base = (mode == ESP_AES_ENCRYPT) ? 0 : MODE_DECRYPT_BIT;

    memcpy((uint32_t *)AES_KEY_BASE, ctx->key, ctx->key_bytes);
    DPORT_REG_WRITE(AES_MODE_REG, mode_reg_base + ((ctx->key_bytes / 8) - 2));
}

/* Run a single 16 byte block of AES, using the hardware engine.
 *
 * Call only while holding esp_aes_acquire_hardware().
 */
static inline void esp_aes_block(const void *input, void *output)
{
    const uint32_t *input_words = (const uint32_t *)input;
    uint32_t *output_words = (uint32_t *)output;
    uint32_t *mem_block = (uint32_t *)AES_TEXT_BASE;

    for(int i = 0; i < 4; i++) {
        mem_block[i] = input_words[i];
    }

    DPORT_REG_WRITE(AES_START_REG, 1);

    DPORT_STALL_OTHER_CPU_START();
    {
        while (_DPORT_REG_READ(AES_IDLE_REG) != 1) { }
        for (int i = 0; i < 4; i++) {
            output_words[i] = mem_block[i];
        }
    }
    DPORT_STALL_OTHER_CPU_END();
}

/*
 * AES-ECB block encryption
 */
void esp_aes_encrypt( esp_aes_context *ctx,
                      const unsigned char input[16],
                      unsigned char output[16] )
{
    esp_aes_acquire_hardware();
    esp_aes_setkey_hardware(ctx, ESP_AES_ENCRYPT);
    esp_aes_block(input, output);
    esp_aes_release_hardware();
}

/*
 * AES-ECB block decryption
 */

void esp_aes_decrypt( esp_aes_context *ctx,
                      const unsigned char input[16],
                      unsigned char output[16] )
{
    esp_aes_acquire_hardware();
    esp_aes_setkey_hardware(ctx, ESP_AES_DECRYPT);
    esp_aes_block(input, output);
    esp_aes_release_hardware();
}


/*
 * AES-ECB block encryption/decryption
 */
int esp_aes_crypt_ecb( esp_aes_context *ctx,
                       int mode,
                       const unsigned char input[16],
                       unsigned char output[16] )
{
    esp_aes_acquire_hardware();
    esp_aes_setkey_hardware(ctx, mode);
    esp_aes_block(input, output);
    esp_aes_release_hardware();

    return 0;
}


/*
 * AES-CBC buffer encryption/decryption
 */
int esp_aes_crypt_cbc( esp_aes_context *ctx,
                       int mode,
                       size_t length,
                       unsigned char iv[16],
                       const unsigned char *input,
                       unsigned char *output )
{
    int i;
    uint32_t *output_words = (uint32_t *)output;
    const uint32_t *input_words = (const uint32_t *)input;
    uint32_t *iv_words = (uint32_t *)iv;
    unsigned char temp[16];

    if ( length % 16 ) {
        return ( ERR_ESP_AES_INVALID_INPUT_LENGTH );
    }

    esp_aes_acquire_hardware();

    esp_aes_setkey_hardware(ctx, mode);

    if ( mode == ESP_AES_DECRYPT ) {
        while ( length > 0 ) {
            memcpy(temp, input_words, 16);
            esp_aes_block(input_words, output_words);

            for ( i = 0; i < 4; i++ ) {
                output_words[i] = output_words[i] ^ iv_words[i];
            }

            memcpy( iv_words, temp, 16 );

            input_words += 4;
            output_words += 4;
            length -= 16;
        }
    } else { // ESP_AES_ENCRYPT
        while ( length > 0 ) {

            for ( i = 0; i < 4; i++ ) {
                output_words[i] = input_words[i] ^ iv_words[i];
            }

            esp_aes_block(output_words, output_words);
            memcpy( iv_words, output_words, 16 );

            input_words  += 4;
            output_words += 4;
            length -= 16;
        }
    }

    esp_aes_release_hardware();

    return 0;
}

/*
 * AES-CFB128 buffer encryption/decryption
 */
int esp_aes_crypt_cfb128( esp_aes_context *ctx,
                          int mode,
                          size_t length,
                          size_t *iv_off,
                          unsigned char iv[16],
                          const unsigned char *input,
                          unsigned char *output )
{
    int c;
    size_t n = *iv_off;

    esp_aes_acquire_hardware();

    esp_aes_setkey_hardware(ctx, ESP_AES_ENCRYPT);

    if ( mode == ESP_AES_DECRYPT ) {
        while ( length-- ) {
            if ( n == 0 ) {
                esp_aes_block(iv, iv );
            }

            c = *input++;
            *output++ = (unsigned char)( c ^ iv[n] );
            iv[n] = (unsigned char) c;

            n = ( n + 1 ) & 0x0F;
        }
    } else {
        while ( length-- ) {
            if ( n == 0 ) {
                esp_aes_block(iv, iv );
            }

            iv[n] = *output++ = (unsigned char)( iv[n] ^ *input++ );

            n = ( n + 1 ) & 0x0F;
        }
    }

    *iv_off = n;

    esp_aes_release_hardware();

    return 0;
}

/*
 * AES-CFB8 buffer encryption/decryption
 */
int esp_aes_crypt_cfb8( esp_aes_context *ctx,
                        int mode,
                        size_t length,
                        unsigned char iv[16],
                        const unsigned char *input,
                        unsigned char *output )
{
    unsigned char c;
    unsigned char ov[17];

    esp_aes_acquire_hardware();

    esp_aes_setkey_hardware(ctx, ESP_AES_ENCRYPT);

    while ( length-- ) {
        memcpy( ov, iv, 16 );
        esp_aes_block(iv, iv);

        if ( mode == ESP_AES_DECRYPT ) {
            ov[16] = *input;
        }

        c = *output++ = (unsigned char)( iv[0] ^ *input++ );

        if ( mode == ESP_AES_ENCRYPT ) {
            ov[16] = c;
        }

        memcpy( iv, ov + 1, 16 );
    }

    esp_aes_release_hardware();

    return 0;
}

/*
 * AES-CTR buffer encryption/decryption
 */
int esp_aes_crypt_ctr( esp_aes_context *ctx,
                       size_t length,
                       size_t *nc_off,
                       unsigned char nonce_counter[16],
                       unsigned char stream_block[16],
                       const unsigned char *input,
                       unsigned char *output )
{
    int c, i;
    size_t n = *nc_off;

    esp_aes_acquire_hardware();

    esp_aes_setkey_hardware(ctx, ESP_AES_ENCRYPT);

    while ( length-- ) {
        if ( n == 0 ) {
            esp_aes_block(nonce_counter, stream_block);

            for ( i = 16; i > 0; i-- )
                if ( ++nonce_counter[i - 1] != 0 ) {
                    break;
                }
        }
        c = *input++;
        *output++ = (unsigned char)( c ^ stream_block[n] );

        n = ( n + 1 ) & 0x0F;
    }

    *nc_off = n;

    esp_aes_release_hardware();

    return 0;
}
