/*
  ROM functions for hardware SHA support.

  It is not recommended to use these functions directly.  If using
  them from esp-idf then use the esp_sha_lock_engine() and
  esp_sha_lock_memory_block() functions in esp32/sha.h to ensure
  exclusive access.
 */
/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _ROM_SHA_H_
#define _ROM_SHA_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

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

/* Do not use these function in multi core mode due to
 * inside they have no safe implementation (without DPORT workaround).
*/
void ets_sha_init(SHA_CTX *ctx);

void ets_sha_enable(void);

void ets_sha_disable(void);

void ets_sha_update(SHA_CTX *ctx, enum SHA_TYPE type, const uint8_t *input, size_t input_bits);

void ets_sha_finish(SHA_CTX *ctx, enum SHA_TYPE type, uint8_t *output);

#ifdef __cplusplus
}
#endif

#endif /* _ROM_SHA_H_ */
