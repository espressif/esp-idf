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
#include "mbedtls/platform_util.h"
#include "esp32/aes.h"
#include "soc/hwcrypto_periph.h"
#include <sys/lock.h>

#include <freertos/FreeRTOS.h>

#include "soc/cpu.h"
#include <stdio.h>
#include "driver/periph_ctrl.h"


/* AES uses a spinlock mux not a lock as the underlying block operation
   only takes 208 cycles (to write key & compute block), +600 cycles
   for DPORT protection but +3400 cycles again if you use a full sized lock.

   For CBC, CFB, etc. this may mean that interrupts are disabled for a longer
   period of time for bigger lengths. However at the moment this has to happen
   anyway due to DPORT protection...
*/
static portMUX_TYPE aes_spinlock = portMUX_INITIALIZER_UNLOCKED;

static inline bool valid_key_length(const esp_aes_context *ctx)
{
    return ctx->key_bytes == 128/8 || ctx->key_bytes == 192/8 || ctx->key_bytes == 256/8;
}

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
    ctx->key_in_hardware = 0;
    return 0;
}

/*
 * Helper function to copy key from esp_aes_context buffer
 * to hardware key registers.
 *
 * Call only while holding esp_aes_acquire_hardware().
 */
static void esp_aes_setkey_hardware(esp_aes_context *ctx, int mode)
{
    const uint32_t MODE_DECRYPT_BIT = 4;
    unsigned mode_reg_base = (mode == ESP_AES_ENCRYPT) ? 0 : MODE_DECRYPT_BIT;

    ctx->key_in_hardware = 0;

    for (int i = 0; i < ctx->key_bytes/4; ++i) {
        DPORT_REG_WRITE(AES_KEY_BASE + i * 4, *(((uint32_t *)ctx->key) + i));
        ctx->key_in_hardware += 4;
    }

    DPORT_REG_WRITE(AES_MODE_REG, mode_reg_base + ((ctx->key_bytes / 8) - 2));

    /* Fault injection check: all words of key data should have been written to hardware */
    if (ctx->key_in_hardware < 16
        || ctx->key_in_hardware != ctx->key_bytes) {
        abort();
    }
}

/* Run a single 16 byte block of AES, using the hardware engine.
 *
 * Call only while holding esp_aes_acquire_hardware().
 */
static int esp_aes_block(esp_aes_context *ctx, const void *input, void *output)
{
    const uint32_t *input_words = (const uint32_t *)input;
    uint32_t i0, i1, i2, i3;
    uint32_t *output_words = (uint32_t *)output;

    /* If no key is written to hardware yet, either the user hasn't called
       mbedtls_aes_setkey_enc/mbedtls_aes_setkey_dec - meaning we also don't
       know which mode to use - or a fault skipped the
       key write to hardware. Treat this as a fatal error and zero the output block.
    */
    if (ctx->key_in_hardware != ctx->key_bytes) {
        bzero(output, 16);
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }

    /* Storing i0,i1,i2,i3 in registers not an array
       helps a lot with optimisations at -Os level */
    i0 = input_words[0];
    DPORT_REG_WRITE(AES_TEXT_BASE, i0);

    i1 = input_words[1];
    DPORT_REG_WRITE(AES_TEXT_BASE + 4, i1);

    i2 = input_words[2];
    DPORT_REG_WRITE(AES_TEXT_BASE + 8, i2);

    i3 = input_words[3];
    DPORT_REG_WRITE(AES_TEXT_BASE + 12, i3);

    DPORT_REG_WRITE(AES_START_REG, 1);

    while (DPORT_REG_READ(AES_IDLE_REG) != 1) { }

    esp_dport_access_read_buffer(output_words, AES_TEXT_BASE, 4);

    /* Physical security check: Verify the AES accelerator actually ran, and wasn't
       skipped due to external fault injection while starting the peripheral.

       Note that i0,i1,i2,i3 are copied from input buffer in case input==output.

       Bypassing this check requires at least one additional fault.
    */
    if(i0 == output_words[0] && i1 == output_words[1] && i2 == output_words[2] && i3 == output_words[3]) {
        // calling zeroing functions to narrow the
        // window for a double-fault of the abort step, here
        memset(output, 0, 16);
        mbedtls_platform_zeroize(output, 16);
        abort();
    }

    return 0;
}

/*
 * AES-ECB block encryption
 */
int esp_internal_aes_encrypt( esp_aes_context *ctx,
                      const unsigned char input[16],
                      unsigned char output[16] )
{
    int r;

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;
    esp_aes_setkey_hardware(ctx, ESP_AES_ENCRYPT);
    r = esp_aes_block(ctx, input, output);
    esp_aes_release_hardware();
    return r;
}

/*
 * AES-ECB block decryption
 */

int esp_internal_aes_decrypt( esp_aes_context *ctx,
                      const unsigned char input[16],
                      unsigned char output[16] )
{
    int r;

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;
    esp_aes_setkey_hardware(ctx, ESP_AES_DECRYPT);
    r = esp_aes_block(ctx, input, output);
    esp_aes_release_hardware();
    return r;
}

/*
 * AES-ECB block encryption/decryption
 */
int esp_aes_crypt_ecb( esp_aes_context *ctx,
                       int mode,
                       const unsigned char input[16],
                       unsigned char output[16] )
{
    int r;

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;
    esp_aes_setkey_hardware(ctx, mode);
    r = esp_aes_block(ctx, input, output);
    esp_aes_release_hardware();

    return r;
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

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;

    esp_aes_setkey_hardware(ctx, mode);

    if ( mode == ESP_AES_DECRYPT ) {
        while ( length > 0 ) {
            memcpy(temp, input_words, 16);
            esp_aes_block(ctx, input_words, output_words);

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

            esp_aes_block(ctx, output_words, output_words);
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

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;

    esp_aes_setkey_hardware(ctx, ESP_AES_ENCRYPT);

    if ( mode == ESP_AES_DECRYPT ) {
        while ( length-- ) {
            if ( n == 0 ) {
                esp_aes_block(ctx, iv, iv);
            }

            c = *input++;
            *output++ = (unsigned char)( c ^ iv[n] );
            iv[n] = (unsigned char) c;

            n = ( n + 1 ) & 0x0F;
        }
    } else {
        while ( length-- ) {
            if ( n == 0 ) {
                esp_aes_block(ctx, iv, iv);
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

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;

    esp_aes_setkey_hardware(ctx, ESP_AES_ENCRYPT);

    while ( length-- ) {
        memcpy( ov, iv, 16 );
        esp_aes_block(ctx, iv, iv);

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

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();
    ctx->key_in_hardware = 0;

    esp_aes_setkey_hardware(ctx, ESP_AES_ENCRYPT);

    while ( length-- ) {
        if ( n == 0 ) {
            esp_aes_block(ctx, nonce_counter, stream_block);

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

/*
 * AES-OFB (Output Feedback Mode) buffer encryption/decryption
 */
int esp_aes_crypt_ofb( esp_aes_context *ctx,
                           size_t length,
                           size_t *iv_off,
                           unsigned char iv[16],
                           const unsigned char *input,
                           unsigned char *output )
{
    int ret = 0;
    size_t n;

    if ( ctx == NULL || iv_off == NULL || iv == NULL ||
        input == NULL || output == NULL ) {
            return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    n = *iv_off;

    if( n > 15 ) {
        return( MBEDTLS_ERR_AES_BAD_INPUT_DATA );
    }

    if (!valid_key_length(ctx)) {
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    esp_aes_acquire_hardware();

    esp_aes_setkey_hardware(ctx, ESP_AES_ENCRYPT);

    while( length-- ) {
        if( n == 0 ) {
            esp_aes_block(ctx, iv, iv);
        }
        *output++ =  *input++ ^ iv[n];

        n = ( n + 1 ) & 0x0F;
    }

    *iv_off = n;

    esp_aes_release_hardware();

    return( ret );
}

/* Below XTS implementation is copied aes.c of mbedtls library. 
 * When MBEDTLS_AES_ALT is defined mbedtls expects alternate
 * definition of XTS functions to be available. Even if this 
 * could have been avoided, it is done for consistency reason.
 */

void esp_aes_xts_init( esp_aes_xts_context *ctx )
{
    esp_aes_init( &ctx->crypt );
    esp_aes_init( &ctx->tweak );
}

void esp_aes_xts_free( esp_aes_xts_context *ctx )
{
    esp_aes_free( &ctx->crypt );
    esp_aes_free( &ctx->tweak );
}

static int esp_aes_xts_decode_keys( const unsigned char *key,
                                        unsigned int keybits,
                                        const unsigned char **key1,
                                        unsigned int *key1bits,
                                        const unsigned char **key2,
                                        unsigned int *key2bits )
{
    const unsigned int half_keybits = keybits / 2;
    const unsigned int half_keybytes = half_keybits / 8;

    switch( keybits )
    {
        case 256: break;
        case 512: break;
        default : return( MBEDTLS_ERR_AES_INVALID_KEY_LENGTH );
    }

    *key1bits = half_keybits;
    *key2bits = half_keybits;
    *key1 = &key[0];
    *key2 = &key[half_keybytes];

    return 0;
}

int esp_aes_xts_setkey_enc( esp_aes_xts_context *ctx,
                                const unsigned char *key,
                                unsigned int keybits)
{
    int ret;
    const unsigned char *key1, *key2;
    unsigned int key1bits, key2bits;

    ret = esp_aes_xts_decode_keys( key, keybits, &key1, &key1bits,
                                       &key2, &key2bits );
    if( ret != 0 )
        return( ret );

    /* Set the tweak key. Always set tweak key for the encryption mode. */
    ret = esp_aes_setkey( &ctx->tweak, key2, key2bits );
    if( ret != 0 )
        return( ret );

    /* Set crypt key for encryption. */
    return esp_aes_setkey( &ctx->crypt, key1, key1bits );
}

int esp_aes_xts_setkey_dec( esp_aes_xts_context *ctx,
                                const unsigned char *key,
                                unsigned int keybits)
{
    int ret;
    const unsigned char *key1, *key2;
    unsigned int key1bits, key2bits;

    ret = esp_aes_xts_decode_keys( key, keybits, &key1, &key1bits,
                                       &key2, &key2bits );
    if( ret != 0 )
        return( ret );

    /* Set the tweak key. Always set tweak key for encryption. */
    ret = esp_aes_setkey( &ctx->tweak, key2, key2bits );
    if( ret != 0 )
        return( ret );

    /* Set crypt key for decryption. */
    return esp_aes_setkey( &ctx->crypt, key1, key1bits );
}

/* Endianess with 64 bits values */
#ifndef GET_UINT64_LE
#define GET_UINT64_LE(n,b,i)                            \
{                                                       \
    (n) = ( (uint64_t) (b)[(i) + 7] << 56 )             \
        | ( (uint64_t) (b)[(i) + 6] << 48 )             \
        | ( (uint64_t) (b)[(i) + 5] << 40 )             \
        | ( (uint64_t) (b)[(i) + 4] << 32 )             \
        | ( (uint64_t) (b)[(i) + 3] << 24 )             \
        | ( (uint64_t) (b)[(i) + 2] << 16 )             \
        | ( (uint64_t) (b)[(i) + 1] <<  8 )             \
        | ( (uint64_t) (b)[(i)    ]       );            \
}
#endif

#ifndef PUT_UINT64_LE
#define PUT_UINT64_LE(n,b,i)                            \
{                                                       \
    (b)[(i) + 7] = (unsigned char) ( (n) >> 56 );       \
    (b)[(i) + 6] = (unsigned char) ( (n) >> 48 );       \
    (b)[(i) + 5] = (unsigned char) ( (n) >> 40 );       \
    (b)[(i) + 4] = (unsigned char) ( (n) >> 32 );       \
    (b)[(i) + 3] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i)    ] = (unsigned char) ( (n)       );       \
}
#endif

typedef unsigned char esp_be128[16];

/*
 * GF(2^128) multiplication function
 *
 * This function multiplies a field element by x in the polynomial field
 * representation. It uses 64-bit word operations to gain speed but compensates
 * for machine endianess and hence works correctly on both big and little
 * endian machines.
 */
static void esp_gf128mul_x_ble( unsigned char r[16],
                                    const unsigned char x[16] )
{
    uint64_t a, b, ra, rb;

    GET_UINT64_LE( a, x, 0 );
    GET_UINT64_LE( b, x, 8 );

    ra = ( a << 1 )  ^ 0x0087 >> ( 8 - ( ( b >> 63 ) << 3 ) );
    rb = ( a >> 63 ) | ( b << 1 );

    PUT_UINT64_LE( ra, r, 0 );
    PUT_UINT64_LE( rb, r, 8 );
}

/*
 * AES-XTS buffer encryption/decryption
 */
int esp_aes_crypt_xts( esp_aes_xts_context *ctx,
                           int mode,
                           size_t length,
                           const unsigned char data_unit[16],
                           const unsigned char *input,
                           unsigned char *output )
{
    int ret;
    size_t blocks = length / 16;
    size_t leftover = length % 16;
    unsigned char tweak[16];
    unsigned char prev_tweak[16];
    unsigned char tmp[16];

    /* Sectors must be at least 16 bytes. */
    if( length < 16 )
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;

    /* NIST SP 80-38E disallows data units larger than 2**20 blocks. */
    if( length > ( 1 << 20 ) * 16 )
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;

    /* Compute the tweak. */
    ret = esp_aes_crypt_ecb( &ctx->tweak, MBEDTLS_AES_ENCRYPT,
                                 data_unit, tweak );
    if( ret != 0 )
        return( ret );

    while( blocks-- )
    {
        size_t i;

        if( leftover && ( mode == MBEDTLS_AES_DECRYPT ) && blocks == 0 )
        {
            /* We are on the last block in a decrypt operation that has
             * leftover bytes, so we need to use the next tweak for this block,
             * and this tweak for the lefover bytes. Save the current tweak for
             * the leftovers and then update the current tweak for use on this,
             * the last full block. */
            memcpy( prev_tweak, tweak, sizeof( tweak ) );
            esp_gf128mul_x_ble( tweak, tweak );
        }

        for( i = 0; i < 16; i++ )
            tmp[i] = input[i] ^ tweak[i];

        ret = esp_aes_crypt_ecb( &ctx->crypt, mode, tmp, tmp );
        if( ret != 0 )
            return( ret );

        for( i = 0; i < 16; i++ )
            output[i] = tmp[i] ^ tweak[i];

        /* Update the tweak for the next block. */
        esp_gf128mul_x_ble( tweak, tweak );

        output += 16;
        input += 16;
    }

    if( leftover )
    {
        /* If we are on the leftover bytes in a decrypt operation, we need to
         * use the previous tweak for these bytes (as saved in prev_tweak). */
        unsigned char *t = mode == MBEDTLS_AES_DECRYPT ? prev_tweak : tweak;

        /* We are now on the final part of the data unit, which doesn't divide
         * evenly by 16. It's time for ciphertext stealing. */
        size_t i;
        unsigned char *prev_output = output - 16;

        /* Copy ciphertext bytes from the previous block to our output for each
         * byte of cyphertext we won't steal. At the same time, copy the
         * remainder of the input for this final round (since the loop bounds
         * are the same). */
        for( i = 0; i < leftover; i++ )
        {
            output[i] = prev_output[i];
            tmp[i] = input[i] ^ t[i];
        }

        /* Copy ciphertext bytes from the previous block for input in this
         * round. */
        for( ; i < 16; i++ )
            tmp[i] = prev_output[i] ^ t[i];

        ret = esp_aes_crypt_ecb( &ctx->crypt, mode, tmp, tmp );
        if( ret != 0 )
            return ret;

        /* Write the result back to the previous block, overriding the previous
         * output we copied. */
        for( i = 0; i < 16; i++ )
            prev_output[i] = tmp[i] ^ t[i];
    }

    return( 0 );
}
