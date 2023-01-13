/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

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

int ets_aes_setkey(enum AES_TYPE type, const void *key, enum AES_BITS bits);

int ets_aes_setkey_enc(const void *key, enum AES_BITS bits);

int ets_aes_setkey_dec(const void *key, enum AES_BITS bits);

void ets_aes_block(const void *input, void *output);

#ifdef __cplusplus
}
#endif
