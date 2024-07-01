/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _ROM_SHA_H_
#define _ROM_SHA_H_

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

#endif /* _ROM_SHA_H_ */
