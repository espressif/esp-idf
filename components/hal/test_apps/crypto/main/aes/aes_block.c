/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_private/esp_crypto_lock_internal.h"
#include "hal/aes_types.h"
#include "hal/aes_hal.h"
#include "hal/aes_ll.h"

#if SOC_AES_SUPPORTED

#include "aes_block.h"

void aes_crypt_cbc_block(int mode,
                        uint8_t key_bytes,
                        const uint8_t key[32],
                        size_t length,
                        unsigned char iv[16],
                        const unsigned char *input,
                        unsigned char *output)
{
    uint32_t *output_words = (uint32_t *)output;
    const uint32_t *input_words = (const uint32_t *)input;
    uint32_t *iv_words = (uint32_t *)iv;
    unsigned char temp[16];

    AES_RCC_ATOMIC() {
        aes_ll_enable_bus_clock(true);
        aes_ll_reset_register();
    }

    /* Sets the key used for AES encryption/decryption */
    aes_hal_setkey(key, key_bytes, mode);

    if (mode == ESP_AES_DECRYPT) {
        while ( length > 0 ) {
            memcpy(temp, input_words, 16);
            aes_hal_transform_block(input_words, output_words);

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

            aes_hal_transform_block(output_words, output_words);
            memcpy( iv_words, output_words, 16 );

            input_words  += 4;
            output_words += 4;
            length -= 16;
        }
    }

    AES_RCC_ATOMIC() {
        aes_ll_enable_bus_clock(false);
    }
}


void aes_crypt_ctr_block(uint8_t key_bytes,
                        const uint8_t key[32],
                        size_t length,
                        size_t *nc_off,
                        unsigned char nonce_counter[16],
                        unsigned char stream_block[16],
                        const unsigned char *input,
                        unsigned char *output )
{
    int c, i;
    size_t n = *nc_off;

    AES_RCC_ATOMIC() {
        aes_ll_enable_bus_clock(true);
        aes_ll_reset_register();
    }

    /* Sets the key used for AES encryption/decryption */
    aes_hal_setkey(key, key_bytes, ESP_AES_ENCRYPT);

    while (length--) {
        if ( n == 0 ) {
            aes_hal_transform_block(nonce_counter, stream_block);
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

    AES_RCC_ATOMIC() {
        aes_ll_enable_bus_clock(false);
    }
}

#endif
