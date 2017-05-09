/**
 * \brief AES block cipher, ESP32 hardware accelerated version
 * Based on mbedTLS FIPS-197 compliant version.
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  Additions Copyright (C) 2016, Espressif Systems (Shanghai) PTE Ltd
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
#include "hwcrypto/aes.h"
#include "rom/aes.h"
#include "soc/dport_reg.h"
#include <sys/lock.h>

static _lock_t aes_lock;

void esp_aes_acquire_hardware( void )
{
    /* newlib locks lazy initialize on ESP-IDF */
    _lock_acquire(&aes_lock);
    /* Enable AES hardware */
    DPORT_REG_SET_BIT(DPORT_PERI_CLK_EN_REG, DPORT_PERI_EN_AES);
    /* Clear reset on digital signature & secure boot units,
       otherwise AES unit is held in reset also. */
    DPORT_REG_CLR_BIT(DPORT_PERI_RST_EN_REG,
                DPORT_PERI_EN_AES
                | DPORT_PERI_EN_DIGITAL_SIGNATURE
                | DPORT_PERI_EN_SECUREBOOT);
}

void esp_aes_release_hardware( void )
{
    /* Disable AES hardware */
    DPORT_REG_SET_BIT(DPORT_PERI_RST_EN_REG, DPORT_PERI_EN_AES);
    /* Don't return other units to reset, as this pulls
       reset on RSA & SHA units, respectively. */
    DPORT_REG_CLR_BIT(DPORT_PERI_CLK_EN_REG, DPORT_PERI_EN_AES);
    _lock_release(&aes_lock);
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

/* Translate number of bits to an AES_BITS enum */
static int keybits_to_aesbits(unsigned int keybits)
{
    switch (keybits) {
    case 128:
        return AES128;
    case 192:
        return AES192;
        break;
    case 256:
        return AES256;
    default:
        return ( ERR_ESP_AES_INVALID_KEY_LENGTH );
    }
}

/*
 * AES key schedule (encryption)
 *
 */
int esp_aes_setkey_enc( esp_aes_context *ctx, const unsigned char *key,
                        unsigned int keybits )
{
    uint16_t keybytes = keybits / 8;
    int aesbits = keybits_to_aesbits(keybits);
    if (aesbits < 0) {
        return aesbits;
    }
    ctx->enc.aesbits = aesbits;
    bzero(ctx->enc.key, sizeof(ctx->enc.key));
    memcpy(ctx->enc.key, key, keybytes);
    return 0;
}

/*
 * AES key schedule (decryption)
 *
 */
int esp_aes_setkey_dec( esp_aes_context *ctx, const unsigned char *key,
                        unsigned int keybits )
{
    uint16_t keybytes = keybits / 8;
    int aesbits = keybits_to_aesbits(keybits);
    if (aesbits < 0) {
        return aesbits;
    }
    ctx->dec.aesbits = aesbits;
    bzero(ctx->dec.key, sizeof(ctx->dec.key));
    memcpy(ctx->dec.key, key, keybytes);
    return 0;
}

/*
 * Helper function to copy key from esp_aes_context buffer
 * to hardware key registers.
 *
 * Only call when protected by esp_aes_acquire_hardware().
 */
static inline int esp_aes_setkey_hardware( esp_aes_context *ctx, int mode)
{
    if ( mode == ESP_AES_ENCRYPT ) {
        ets_aes_setkey_enc(ctx->enc.key, ctx->enc.aesbits);
    } else {
        ets_aes_setkey_dec(ctx->dec.key, ctx->dec.aesbits);
    }
    return 0;
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
    ets_aes_crypt(input, output);
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
    ets_aes_crypt(input, output);
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
    ets_aes_crypt(input, output);
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
    unsigned char temp[16];

    if ( length % 16 ) {
        return ( ERR_ESP_AES_INVALID_INPUT_LENGTH );
    }

    esp_aes_acquire_hardware();
    esp_aes_setkey_hardware(ctx, mode);

    if ( mode == ESP_AES_DECRYPT ) {
        while ( length > 0 ) {
            memcpy( temp, input, 16 );
            ets_aes_crypt(input, output);

            for ( i = 0; i < 16; i++ ) {
                output[i] = (unsigned char)( output[i] ^ iv[i] );
            }

            memcpy( iv, temp, 16 );

            input  += 16;
            output += 16;
            length -= 16;
        }
    } else {
        while ( length > 0 ) {
            for ( i = 0; i < 16; i++ ) {
                output[i] = (unsigned char)( input[i] ^ iv[i] );
            }

            ets_aes_crypt(output, output);
            memcpy( iv, output, 16 );

            input  += 16;
            output += 16;
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
                ets_aes_crypt(iv, iv );
            }

            c = *input++;
            *output++ = (unsigned char)( c ^ iv[n] );
            iv[n] = (unsigned char) c;

            n = ( n + 1 ) & 0x0F;
        }
    } else {
        while ( length-- ) {
            if ( n == 0 ) {
                ets_aes_crypt(iv, iv );
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
        ets_aes_crypt(iv, iv);

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
            ets_aes_crypt(nonce_counter, stream_block);

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
