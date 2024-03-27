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
#include "esp_cpu.h"

#include "hal/sha_hal.h"
#include "hal/sha_ll.h"
#include "hal/sha_types.h"
#include "sha/sha_parallel_engine.h"
#include "soc/hwcrypto_periph.h"
#include "esp_private/esp_crypto_lock_internal.h"
#include "esp_private/periph_ctrl.h"

/*
     Single spinlock for SHA engine memory block
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

/* Return block size (in words) for a given SHA type */
inline static size_t block_length(esp_sha_type type)
{
    switch (type) {
    case SHA1:
    case SHA2_256:
        return 64 / 4;
    case SHA2_384:
    case SHA2_512:
        return 128 / 4;
    default:
        return 0;
    }
}

/* Index into the engine_states array */
inline static size_t sha_engine_index(esp_sha_type type)
{
    switch (type) {
    case SHA1:
        return 0;
    case SHA2_256:
        return 1;
    default:
        return 2;
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

    if (result == NULL) {
        // Create a new semaphore for 'in use' flag
        SemaphoreHandle_t new_engine = xSemaphoreCreateBinary();
        assert(new_engine != NULL);
        xSemaphoreGive(new_engine); // start available

        // try to atomically set the previously NULL *engine to new_engine
        if (!esp_cpu_compare_and_set((volatile uint32_t *)engine, 0, (uint32_t)new_engine)) {
            // we lost a race setting *engine
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
        SHA_RCC_ATOMIC() {
            sha_ll_enable_bus_clock(true);
            sha_ll_reset_register();
        }
    }

    engines_in_use++;
    assert(engines_in_use <= 3);

    portEXIT_CRITICAL(&engines_in_use_lock);

    return true;
}


void esp_sha_unlock_engine(esp_sha_type sha_type)
{
    SemaphoreHandle_t engine_state = sha_get_engine_state(sha_type);

    portENTER_CRITICAL(&engines_in_use_lock);

    engines_in_use--;

    if (engines_in_use == 0) {
        /* About to release last engine, so
           disable SHA hardware */
        SHA_RCC_ATOMIC() {
           sha_ll_enable_bus_clock(false);
        }
    }

    portEXIT_CRITICAL(&engines_in_use_lock);

    xSemaphoreGive(engine_state);
}

void esp_sha_read_digest_state(esp_sha_type sha_type, void *digest_state)
{
#ifndef NDEBUG
    {
        SemaphoreHandle_t engine_state = sha_get_engine_state(sha_type);
        assert(uxSemaphoreGetCount(engine_state) == 0 &&
               "SHA engine should be locked" );
    }
#endif

    // preemptively do this before entering the critical section, then re-check once in it
    sha_hal_wait_idle();

    esp_sha_lock_memory_block();

    sha_hal_read_digest(sha_type, digest_state);

    esp_sha_unlock_memory_block();
}

void esp_sha_block(esp_sha_type sha_type, const void *data_block, bool first_block)
{
#ifndef NDEBUG
    {
        SemaphoreHandle_t engine_state = sha_get_engine_state(sha_type);
        assert(uxSemaphoreGetCount(engine_state) == 0 &&
               "SHA engine should be locked" );
    }
#endif

    // preemptively do this before entering the critical section, then re-check once in it
    sha_hal_wait_idle();
    esp_sha_lock_memory_block();

    sha_hal_hash_block(sha_type, data_block, block_length(sha_type), first_block);

    esp_sha_unlock_memory_block();
}
