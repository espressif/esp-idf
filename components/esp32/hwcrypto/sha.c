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
#include <byteswap.h>
#include <assert.h>

#include "hwcrypto/sha.h"
#include "rom/ets_sys.h"
#include "soc/dport_reg.h"
#include "soc/hwcrypto_reg.h"

inline static uint32_t SHA_LOAD_REG(esp_sha_type sha_type) {
    return SHA_1_LOAD_REG + sha_type * 0x10;
}

inline static uint32_t SHA_BUSY_REG(esp_sha_type sha_type) {
    return SHA_1_BUSY_REG + sha_type * 0x10;
}

inline static uint32_t SHA_START_REG(esp_sha_type sha_type) {
    return SHA_1_START_REG + sha_type * 0x10;
}

inline static uint32_t SHA_CONTINUE_REG(esp_sha_type sha_type) {
    return SHA_1_CONTINUE_REG + sha_type * 0x10;
}

/* Single lock for SHA engine memory block
*/
static _lock_t memory_block_lock;

typedef struct {
    _lock_t lock;
    bool in_use;
} sha_engine_state;

/* Pointer to state of each concurrent SHA engine.

   Indexes:
   0 = SHA1
   1 = SHA2_256
   2 = SHA2_384 or SHA2_512
*/
static sha_engine_state engine_states[3];

/* Index into the sha_engine_state array */
inline static size_t sha_engine_index(esp_sha_type type) {
    switch(type) {
    case SHA1:
        return 0;
    case SHA2_256:
        return 1;
    default:
        return 2;
    }
}

/* Return digest length (in bytes) for a given SHA type */
inline static size_t sha_length(esp_sha_type type) {
    switch(type) {
    case SHA1:
        return 20;
    case SHA2_256:
        return 32;
    case SHA2_384:
        return 48;
    case SHA2_512:
        return 64;
    default:
        return 0;
    }
}

/* Return block size (in bytes) for a given SHA type */
inline static size_t block_length(esp_sha_type type) {
    switch(type) {
    case SHA1:
    case SHA2_256:
        return 64;
    case SHA2_384:
    case SHA2_512:
        return 128;
    default:
        return 0;
    }
}

void esp_sha_lock_memory_block(void)
{
    _lock_acquire(&memory_block_lock);
}

void esp_sha_unlock_memory_block(void)
{
    _lock_release(&memory_block_lock);
}

/* Lock to hold when changing SHA engine state,
   allows checking of sha_engines_all_idle()
*/
static _lock_t state_change_lock;

inline static bool sha_engines_all_idle() {
    return !engine_states[0].in_use
        && !engine_states[1].in_use
        && !engine_states[2].in_use;
}

static void esp_sha_lock_engine_inner(sha_engine_state *engine);

bool esp_sha_try_lock_engine(esp_sha_type sha_type)
{
    sha_engine_state *engine = &engine_states[sha_engine_index(sha_type)];
    if(_lock_try_acquire(&engine->lock) != 0) {
        /* This SHA engine is already in use */
        return false;
    } else {
        esp_sha_lock_engine_inner(engine);
        return true;
    }
}

void esp_sha_lock_engine(esp_sha_type sha_type)
{
    sha_engine_state *engine = &engine_states[sha_engine_index(sha_type)];
    _lock_acquire(&engine->lock);
    esp_sha_lock_engine_inner(engine);
}

static void esp_sha_lock_engine_inner(sha_engine_state *engine)
{
    _lock_acquire(&state_change_lock);

    if (sha_engines_all_idle()) {
        /* Enable SHA hardware */
        DPORT_REG_SET_BIT(DPORT_PERI_CLK_EN_REG, DPORT_PERI_EN_SHA);
        /* also clear reset on secure boot, otherwise SHA is held in reset */
        DPORT_REG_CLR_BIT(DPORT_PERI_RST_EN_REG,
                    DPORT_PERI_EN_SHA
                    | DPORT_PERI_EN_SECUREBOOT);
        ets_sha_enable();
    }

    _lock_release(&state_change_lock);

    assert( !engine->in_use && "in_use flag should be cleared" );
    engine->in_use = true;
}


void esp_sha_unlock_engine(esp_sha_type sha_type)
{
    sha_engine_state *engine = &engine_states[sha_engine_index(sha_type)];

    _lock_acquire(&state_change_lock);

    assert( engine->in_use && "in_use flag should be set" );
    engine->in_use = false;

    if (sha_engines_all_idle()) {
        /* Disable SHA hardware */
        /* Don't assert reset on secure boot, otherwise AES is held in reset */
        DPORT_REG_SET_BIT(DPORT_PERI_RST_EN_REG, DPORT_PERI_EN_SHA);
        DPORT_REG_CLR_BIT(DPORT_PERI_CLK_EN_REG, DPORT_PERI_EN_SHA);
    }

    _lock_release(&state_change_lock);

    _lock_release(&engine->lock);
}

void esp_sha_wait_idle(void)
{
    while(REG_READ(SHA_1_BUSY_REG) == 1) {}
    while(REG_READ(SHA_256_BUSY_REG) == 1) {}
    while(REG_READ(SHA_384_BUSY_REG) == 1) {}
    while(REG_READ(SHA_512_BUSY_REG) == 1) {}
}

void esp_sha_read_digest_state(esp_sha_type sha_type, void *digest_state)
{
    sha_engine_state *engine = &engine_states[sha_engine_index(sha_type)];
    assert(engine->in_use && "SHA engine should be locked" );

    esp_sha_lock_memory_block();

    esp_sha_wait_idle();

    REG_WRITE(SHA_LOAD_REG(sha_type), 1);
    while(REG_READ(SHA_BUSY_REG(sha_type)) == 1) { }

    uint32_t *digest_state_words = (uint32_t *)digest_state;
    uint32_t *reg_addr_buf = (uint32_t *)(SHA_TEXT_BASE);
    if(sha_type == SHA2_384 || sha_type == SHA2_512) {
        /* for these ciphers using 64-bit states, swap each pair of words */
        for(int i = 0; i < sha_length(sha_type)/4; i += 2) {
            digest_state_words[i+1] = reg_addr_buf[i];
            digest_state_words[i]= reg_addr_buf[i+1];
        }
    } else {
        memcpy(digest_state_words, reg_addr_buf, sha_length(sha_type));
    }
    asm volatile ("memw");

    esp_sha_unlock_memory_block();
}

void esp_sha_block(esp_sha_type sha_type, const void *data_block, bool is_first_block)
{
    sha_engine_state *engine = &engine_states[sha_engine_index(sha_type)];
    assert(engine->in_use && "SHA engine should be locked" );

    esp_sha_lock_memory_block();

    esp_sha_wait_idle();

    /* Fill the data block */
    uint32_t *reg_addr_buf = (uint32_t *)(SHA_TEXT_BASE);
    uint32_t *data_words = (uint32_t *)data_block;
    for (int i = 0; i < block_length(sha_type) / 4; i++) {
        reg_addr_buf[i] = __bswap_32(data_words[i]);
    }
    asm volatile ("memw");

    if(is_first_block) {
        REG_WRITE(SHA_START_REG(sha_type), 1);
    } else {
        REG_WRITE(SHA_CONTINUE_REG(sha_type), 1);
    }

    esp_sha_unlock_memory_block();

    /* Note: deliberately not waiting for this operation to complete,
       as a performance tweak - delay waiting until the next time we need the SHA
       unit, instead.
    */
}

void esp_sha(esp_sha_type sha_type, const unsigned char *input, size_t ilen, unsigned char *output)
{
    size_t block_len = block_length(sha_type);

    esp_sha_lock_engine(sha_type);

    SHA_CTX ctx;
    ets_sha_init(&ctx);
    while(ilen > 0) {
        size_t chunk_len = (ilen > block_len) ? block_len : ilen;
        esp_sha_lock_memory_block();
        esp_sha_wait_idle();
        ets_sha_update(&ctx, sha_type, input, chunk_len * 8);
        esp_sha_unlock_memory_block();
        input += chunk_len;
        ilen -= chunk_len;
    }
    esp_sha_lock_memory_block();
    esp_sha_wait_idle();
    ets_sha_finish(&ctx, sha_type, output);
    esp_sha_unlock_memory_block();

    esp_sha_unlock_engine(sha_type);
}
