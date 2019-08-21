/*
 *  ESP32 hardware accelerated SHA1/256/512 implementation
 *  based on mbedTLS FIPS-197 compliant version.
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  Additions Copyright (C) 2016, Espressif Systems (Shanghai) PTE Ltd
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */
/*
 *  The SHA-1 standard was published by NIST in 1993.
 *
 *  http://www.itl.nist.gov/fipspubs/fip180-1.htm
 */

#include <string.h>
#include <stdio.h>
#include <sys/lock.h>
#include <assert.h>

#include "esp32s2beta/sha.h"
#include "esp32s2beta/rom/ets_sys.h"
#include "soc/dport_reg.h"
#include "soc/hwcrypto_reg.h"

/* Single lock for SHA engine
*/
static _lock_t s_sha_lock;

/* This API was designed for ESP32, which has seperate
   engines for SHA1,256,512. ESP32C has a single engine.
*/

/* Return block size (in bytes) for a given SHA type */
inline static size_t block_length(esp_sha_type type) {
    switch(type) {
    case SHA1:
    case SHA2_224:
    case SHA2_256:
        return 64;
    case SHA2_384:
    case SHA2_512:
        return 128;
    default:
        return 0;
    }
}

/* Return state size (in bytes) for a given SHA type */
inline static size_t state_length(esp_sha_type type) {
    switch(type) {
    case SHA1:
        return 160/8;
    case SHA2_224:
    case SHA2_256:
        return 256/8;
    case SHA2_384:
    case SHA2_512:
        return 512/8;
    default:
        return 0;
    }
}

/* Copy words in memory (to/from a memory block), byte swapping as we go. */
static void memcpy_endianswap(void *to, const void *from, size_t num_bytes)
{
    uint32_t *to_words = (uint32_t *)to;
    const uint32_t *from_words = (const uint32_t *)from;
    assert(num_bytes % 4 == 0);
    for (int i = 0; i < num_bytes / 4; i++) {
        to_words[i] = __builtin_bswap32(from_words[i]);
    }
    asm volatile ("memw");
}

static void memcpy_swapwords(void *to, const void *from, size_t num_bytes)
{
    uint32_t *to_words = (uint32_t *)to;
    const uint32_t *from_words = (const uint32_t *)from;
    assert(num_bytes % 8 == 0);
    for (int i = 0; i < num_bytes / 4; i += 2) {
        to_words[i] = from_words[i+1];
        to_words[i+1] = from_words[i];
    }
    asm volatile ("memw");
}

static void esp_sha_lock_engine_inner(void);

bool esp_sha_try_lock_engine(esp_sha_type sha_type)
{
  if(_lock_try_acquire(&s_sha_lock) != 0) {
        /* SHA engine is already in use */
        return false;
    } else {
        esp_sha_lock_engine_inner();
        return true;
    }
}

void esp_sha_lock_engine(esp_sha_type sha_type)
{
    _lock_acquire(&s_sha_lock);
    esp_sha_lock_engine_inner();
}

static void esp_sha_lock_engine_inner(void)
{
    ets_sha_enable();
}

void esp_sha_unlock_engine(esp_sha_type sha_type)
{
    ets_sha_disable();
    _lock_release(&s_sha_lock);
}

void esp_sha_wait_idle(void)
{
    while(DPORT_REG_READ(SHA_BUSY_REG) != 0) { }
}

void esp_sha_read_digest_state(esp_sha_type sha_type, void *digest_state)
{
    /* engine should be locked */
    esp_sha_wait_idle();
    if (sha_type != SHA2_512 && sha_type != SHA2_384) {
        /* <SHA-512, read out directly */
        memcpy(digest_state, (void *)SHA_H_BASE, state_length(sha_type));
    } else {
        /* SHA-512, read out with each pair of words swapped */
        memcpy_swapwords(digest_state, (void *)SHA_H_BASE, state_length(sha_type));
    }
}

void esp_sha_block(esp_sha_type sha_type, const void *data_block, bool is_first_block)
{
    /* engine should be locked */

    REG_WRITE(SHA_MODE_REG, sha_type);

    /* ESP32C SHA unit can be loaded while previous block is processing */
    memcpy_endianswap((void *)SHA_M_BASE, data_block, block_length(sha_type));

    esp_sha_wait_idle();
    if (is_first_block) {
        REG_WRITE(SHA_START_REG, 1);
    } else {
        REG_WRITE(SHA_CONTINUE_REG, 1);
    }

    /* Note: deliberately not waiting for this operation to complete,
       as a performance tweak - delay waiting until the next time we need the SHA
       unit, instead.
    */
}

void esp_sha(esp_sha_type sha_type, const unsigned char *input, size_t ilen, unsigned char *output)
{
    SHA_CTX ctx;

    esp_sha_lock_engine(sha_type);

    ets_sha_init(&ctx, sha_type);
    ets_sha_starts(&ctx, 0);
    ets_sha_update(&ctx, input, ilen, false);
    ets_sha_finish(&ctx, output);

    esp_sha_unlock_engine(sha_type);
}
