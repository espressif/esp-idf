/**
 * \brief AES block cipher, ESP32-S2 hardware accelerated version
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


/* Below XTS implementation is copied aes.c of mbedtls library.
 * When MBEDTLS_AES_ALT is defined mbedtls expects alternate
 * definition of XTS functions to be available. Even if this
 * could have been avoided, it is done for consistency reason.
 */

#include <stdio.h>
#include <string.h>
#include <sys/lock.h>
#include "mbedtls/aes.h"

#include "aes/esp_aes.h"

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

    switch ( keybits ) {
    case 256: break;
    case 512: break;
    default : return ( MBEDTLS_ERR_AES_INVALID_KEY_LENGTH );
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
    if ( ret != 0 ) {
        return ( ret );
    }

    /* Set the tweak key. Always set tweak key for the encryption mode. */
    ret = esp_aes_setkey( &ctx->tweak, key2, key2bits );
    if ( ret != 0 ) {
        return ( ret );
    }

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
    if ( ret != 0 ) {
        return ( ret );
    }

    /* Set the tweak key. Always set tweak key for encryption. */
    ret = esp_aes_setkey( &ctx->tweak, key2, key2bits );
    if ( ret != 0 ) {
        return ( ret );
    }

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
 * XTS-AES buffer encryption/decryption
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
    if ( length < 16 ) {
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }

    /* NIST SP 80-38E disallows data units larger than 2**20 blocks. */
    if ( length > ( 1 << 20 ) * 16 ) {
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }

    /* Compute the tweak. */
    ret = esp_aes_crypt_ecb( &ctx->tweak, MBEDTLS_AES_ENCRYPT,
                             data_unit, tweak );
    if ( ret != 0 ) {
        return ( ret );
    }

    while ( blocks-- ) {
        size_t i;

        if ( leftover && ( mode == MBEDTLS_AES_DECRYPT ) && blocks == 0 ) {
            /* We are on the last block in a decrypt operation that has
             * leftover bytes, so we need to use the next tweak for this block,
             * and this tweak for the lefover bytes. Save the current tweak for
             * the leftovers and then update the current tweak for use on this,
             * the last full block. */
            memcpy( prev_tweak, tweak, sizeof( tweak ) );
            esp_gf128mul_x_ble( tweak, tweak );
        }

        for ( i = 0; i < 16; i++ ) {
            tmp[i] = input[i] ^ tweak[i];
        }

        ret = esp_aes_crypt_ecb( &ctx->crypt, mode, tmp, tmp );
        if ( ret != 0 ) {
            return ( ret );
        }

        for ( i = 0; i < 16; i++ ) {
            output[i] = tmp[i] ^ tweak[i];
        }

        /* Update the tweak for the next block. */
        esp_gf128mul_x_ble( tweak, tweak );

        output += 16;
        input += 16;
    }

    if ( leftover ) {
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
        for ( i = 0; i < leftover; i++ ) {
            output[i] = prev_output[i];
            tmp[i] = input[i] ^ t[i];
        }

        /* Copy ciphertext bytes from the previous block for input in this
         * round. */
        for ( ; i < 16; i++ ) {
            tmp[i] = prev_output[i] ^ t[i];
        }

        ret = esp_aes_crypt_ecb( &ctx->crypt, mode, tmp, tmp );
        if ( ret != 0 ) {
            return ret;
        }

        /* Write the result back to the previous block, overriding the previous
         * output we copied. */
        for ( i = 0; i < 16; i++ ) {
            prev_output[i] = tmp[i] ^ t[i];
        }
    }

    return ( 0 );
}
