/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ROM_AES_H_
#define _ROM_AES_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AES_BLOCK_SIZE 16

enum AES_TYPE {
    AES_ENC,
    AES_DEC,
};

enum AES_BITS {
    AES128,
    AES192,
    AES256
};

void ets_aes_enable(void);

void ets_aes_disable(void);

void ets_aes_set_endian(bool key_word_swap, bool key_byte_swap,
                        bool in_word_swap, bool in_byte_swap,
                        bool out_word_swap, bool out_byte_swap);

int ets_aes_setkey(enum AES_TYPE type, const void *key, enum AES_BITS bits);

int ets_aes_setkey_enc(const void *key, enum AES_BITS bits);

int ets_aes_setkey_dec(const void *key, enum AES_BITS bits);

void ets_aes_block(const void *input, void *output);

#ifdef __cplusplus
}
#endif

#endif /* _ROM_AES_H_ */
