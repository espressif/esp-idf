/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ROM_AES_H_
#define _ROM_AES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ets_sys.h"
#include "hal/efuse_hal.h"

enum AES_TYPE {
    AES_ENC,
    AES_DEC,
};

enum AES_BITS {
    AES128,
    AES256 = 2, /* skipping enum value 1 to keep compatibility with chips that support AES-192 */
};

// OR these with AES_BITS when writing to AES_MODE_REG
#define AES_MODE_ENCRYPT (0)
#define AES_MODE_DECRYPT (1<<2)

void ets_aes_enable(void);

void ets_aes_disable(void);

ETS_STATUS ets_aes_setkey(enum AES_TYPE type, const void *key, enum AES_BITS bits);

ETS_STATUS ets_aes_setkey_enc(const void *key, enum AES_BITS bits) __attribute__((deprecated));

ETS_STATUS ets_aes_setkey_dec(const void *key, enum AES_BITS bits) __attribute__((deprecated));

void ets_aes_block(const void *input, void *output);

#ifdef __cplusplus
}
#endif

#endif /* _ROM_AES_H_ */
