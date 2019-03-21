/*
  ROM functions for hardware SHA support.

  It is not recommended to use these functions directly.  If using
  them from esp-idf then use the esp_sha_lock_engine() and
  esp_sha_lock_memory_block() functions in hwcrypto/sha.h to ensure
  exclusive access.
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
#ifndef _ROM_SHA_H_
#define _ROM_SHA_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SHAContext {
    bool start;
    uint32_t total_input_bits[4];
} SHA_CTX;

enum SHA_TYPE {
    SHA1 = 0,
    SHA2_256,
    SHA2_384,
    SHA2_512,


    SHA_INVALID = -1,
};

void ets_sha_init(SHA_CTX *ctx);

void ets_sha_enable(void);

void ets_sha_disable(void);

void ets_sha_update(SHA_CTX *ctx, enum SHA_TYPE type, const uint8_t *input, size_t input_bits);

void ets_sha_finish(SHA_CTX *ctx, enum SHA_TYPE type, uint8_t *output);

#ifdef __cplusplus
}
#endif

#endif /* _ROM_SHA_H_ */
