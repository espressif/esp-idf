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
#include <machine/endian.h>
#include <assert.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp32/sha.h"
#include "esp32/rom/ets_sys.h"
#include "soc/hwcrypto_periph.h"
#include "driver/periph_ctrl.h"

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

/* Single spinlock for SHA engine memory block
*/
static portMUX_TYPE memory_block_lock = portMUX_INITIALIZER_UNLOCKED;


/* Binary semaphore managing the state of each concurrent SHA engine.

   Available = noone is using this SHA engine
   Taken = a SHA session is running on this SHA engine

   Indexes:
   0 = SHA1
   1 = SHA2_256
   2 = SHA2_384 or SHA2_512
*/
static SemaphoreHandle_t engine_states[3];

static uint8_t engines_in_use;

/* Spinlock for engines_in_use counter
*/
static portMUX_TYPE engines_in_use_lock = portMUX_INITIALIZER_UNLOCKED;

/* Index into the engine_states array */
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
    portENTER_CRITICAL(&memory_block_lock);
}

void esp_sha_unlock_memory_block(void)
{
    portEXIT_CRITICAL(&memory_block_lock);
}

static SemaphoreHandle_t sha_get_engine_state(esp_sha_type sha_type)
{
    unsigned idx = sha_engine_index(sha_type);
    volatile SemaphoreHandle_t *engine = &engine_states[idx];
    SemaphoreHandle_t result = *engine;
    uint32_t set_engine = 0;

    if (result == NULL) {
        // Create a new semaphore for 'in use' flag
        SemaphoreHandle_t new_engine = xSemaphoreCreateBinary();
        assert(new_engine != NULL);
        xSemaphoreGive(new_engine); // start available

        // try to atomically set the previously NULL *engine to new_engine
        set_engine = (uint32_t)new_engine;
        uxPortCompareSet((volatile uint32_t *)engine, 0, &set_engine);

        if (set_engine != 0) { // we lost a race setting *engine
            vSemaphoreDelete(new_engine);
        }
        result = *engine;
    }
    return result;
}

static bool esp_sha_lock_engine_common(esp_sha_type sha_type, TickType_t ticks_to_wait);

bool esp_sha_try_lock_engine(esp_sha_type sha_type)
{
    return esp_sha_lock_engine_common(sha_type, 0);
}

void esp_sha_lock_engine(esp_sha_type sha_type)
{
    esp_sha_lock_engine_common(sha_type, portMAX_DELAY);
}

static bool esp_sha_lock_engine_common(esp_sha_type sha_type, TickType_t ticks_to_wait)
{
    SemaphoreHandle_t engine_state = sha_get_engine_state(sha_type);
    BaseType_t result = xSemaphoreTake(engine_state, ticks_to_wait);

    if (result == pdFALSE) {
        // failed to take semaphore
        return false;
    }

    portENTER_CRITICAL(&engines_in_use_lock);

    if (engines_in_use == 0) {
        /* Just locked first engine,
           so enable SHA hardware */
        periph_module_enable(PERIPH_SHA_MODULE);
    }

    engines_in_use++;
    assert(engines_in_use <= 3);

    portEXIT_CRITICAL(&engines_in_use_lock);

    return true;
}


void esp_sha_unlock_engine(esp_sha_type sha_type)
{
    SemaphoreHandle_t *engine_state = sha_get_engine_state(sha_type);

    portENTER_CRITICAL(&engines_in_use_lock);

    engines_in_use--;

    if (engines_in_use == 0) {
        /* About to release last engine, so
           disable SHA hardware */
        periph_module_disable(PERIPH_SHA_MODULE);
    }

    portEXIT_CRITICAL(&engines_in_use_lock);

    xSemaphoreGive(engine_state);
}

void esp_sha_wait_idle(void)
{
    while(1) {
        if(DPORT_REG_READ(SHA_1_BUSY_REG) == 0
           && DPORT_REG_READ(SHA_256_BUSY_REG) == 0
           && DPORT_REG_READ(SHA_384_BUSY_REG) == 0
           && DPORT_REG_READ(SHA_512_BUSY_REG) == 0) {
            break;
        }
    }
}

void esp_sha_read_digest_state(esp_sha_type sha_type, void *digest_state)
{
    uint32_t *digest_state_words = NULL;
    uint32_t *reg_addr_buf = NULL;
    uint32_t word_len = sha_length(sha_type)/4;
#ifndef NDEBUG
    {
        SemaphoreHandle_t *engine_state = sha_get_engine_state(sha_type);
        assert(uxSemaphoreGetCount(engine_state) == 0 &&
               "SHA engine should be locked" );
    }
#endif

    // preemptively do this before entering the critical section, then re-check once in it
    esp_sha_wait_idle();

    esp_sha_lock_memory_block();

    esp_sha_wait_idle();

    DPORT_REG_WRITE(SHA_LOAD_REG(sha_type), 1);
    while(DPORT_REG_READ(SHA_BUSY_REG(sha_type)) == 1) { }
    digest_state_words = (uint32_t *)digest_state;
    reg_addr_buf = (uint32_t *)(SHA_TEXT_BASE);
    if(sha_type == SHA2_384 || sha_type == SHA2_512) {
        /* for these ciphers using 64-bit states, swap each pair of words */
        DPORT_INTERRUPT_DISABLE(); // Disable interrupt only on current CPU.
        for(int i = 0; i < word_len; i += 2) {
            digest_state_words[i+1] = DPORT_SEQUENCE_REG_READ((uint32_t)&reg_addr_buf[i]);
            digest_state_words[i]   = DPORT_SEQUENCE_REG_READ((uint32_t)&reg_addr_buf[i+1]);
        }
        DPORT_INTERRUPT_RESTORE(); // restore the previous interrupt level
    } else {
        esp_dport_access_read_buffer(digest_state_words, (uint32_t)&reg_addr_buf[0], word_len);
    }
    esp_sha_unlock_memory_block();

    /* Fault injection check: verify SHA engine actually ran,
       state is not all zeroes.
    */
    for (int i = 0; i < word_len; i++) {
        if (digest_state_words[i] != 0) {
            return;
        }
    }
    abort(); // SHA peripheral returned all zero state, probably due to fault injection
}

void esp_sha_block(esp_sha_type sha_type, const void *data_block, bool is_first_block)
{
    uint32_t *reg_addr_buf = NULL;
    uint32_t *data_words = NULL;
#ifndef NDEBUG
    {
        SemaphoreHandle_t *engine_state = sha_get_engine_state(sha_type);
        assert(uxSemaphoreGetCount(engine_state) == 0 &&
               "SHA engine should be locked" );
    }
#endif

    // preemptively do this before entering the critical section, then re-check once in it
    esp_sha_wait_idle();

    esp_sha_lock_memory_block();

    esp_sha_wait_idle();

    /* Fill the data block */
    reg_addr_buf = (uint32_t *)(SHA_TEXT_BASE);
    data_words = (uint32_t *)data_block;
    for (int i = 0; i < block_length(sha_type) / 4; i++) {
        reg_addr_buf[i] = __builtin_bswap32(data_words[i]);
    }
    asm volatile ("memw");

    if(is_first_block) {
        DPORT_REG_WRITE(SHA_START_REG(sha_type), 1);
    } else {
        DPORT_REG_WRITE(SHA_CONTINUE_REG(sha_type), 1);
    }

    esp_sha_unlock_memory_block();

    /* Note: deliberately not waiting for this operation to complete,
       as a performance tweak - delay waiting until the next time we need the SHA
       unit, instead.
    */
}
