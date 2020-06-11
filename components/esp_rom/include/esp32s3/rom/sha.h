// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
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
#include "ets_sys.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SHA1 = 0,
    SHA2_224,
    SHA2_256,
    SHA2_384,
    SHA2_512,
    SHA2_512224,
    SHA2_512256,
    SHA2_512T,
    SHA_TYPE_MAX
} SHA_TYPE;

typedef struct SHAContext {
    bool start;
    bool in_hardware;               // Is this context currently in peripheral? Needs to be manually cleared if multiple SHAs are interleaved
    SHA_TYPE type;
    uint32_t state[16];             // For SHA1/SHA224/SHA256, used 8, other used 16
    unsigned char buffer[128];      // For SHA1/SHA224/SHA256, used 64, other used 128
    uint32_t total_bits[4];
} SHA_CTX;

void ets_sha_enable(void);

void ets_sha_disable(void);

ets_status_t ets_sha_init(SHA_CTX *ctx, SHA_TYPE type);

ets_status_t ets_sha_starts(SHA_CTX *ctx, uint16_t sha512_t);

void ets_sha_get_state(SHA_CTX *ctx);

void ets_sha_process(SHA_CTX *ctx, const unsigned char *input);

void ets_sha_update(SHA_CTX *ctx, const unsigned char *input, uint32_t input_bytes, bool update_ctx);

ets_status_t ets_sha_finish(SHA_CTX *ctx, unsigned char *output);

#ifdef __cplusplus
}
#endif
