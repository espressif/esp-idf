/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdio.h>

#if SOC_AES_SUPPORTED

void aes_crypt_cbc_block(int mode,
                        uint8_t key_bytes,
                        const uint8_t key[32],
                        size_t length,
                        unsigned char iv[16],
                        const unsigned char *input,
                        unsigned char *output);

void aes_crypt_ctr_block(uint8_t key_bytes,
                        const uint8_t key[32],
                        size_t length,
                        size_t *nc_off,
                        unsigned char nonce_counter[16],
                        unsigned char stream_block[16],
                        const unsigned char *input,
                        unsigned char *output );

#endif
