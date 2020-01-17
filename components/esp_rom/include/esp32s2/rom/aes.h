/*
  ROM functions for hardware AES support.

  It is not recommended to use these functions directly,
  use the wrapper functions in hwcrypto/aes.h instead.

 */
// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

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
