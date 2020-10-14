// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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
#ifndef __SOC_SPINLOCK_H
#define __SOC_SPINLOCK_H

#include <stdint.h>
#include <stdbool.h>
#include "soc/cpu.h"
#include "soc/soc_memory_layout.h"
#include "soc/compare_set.h"
#include "xtensa/xtruntime.h"


#ifdef CONFIG_SPIRAM_WORKAROUND_NEED_VOLATILE_SPINLOCK
#define NEED_VOLATILE_MUX volatile
#else
#define NEED_VOLATILE_MUX
#endif

#define SPINLOCK_FREE          0xB33FFFFF
#define SPINLOCK_WAIT_FOREVER  (-1)  
#define SPINLOCK_NO_WAIT        0  
#define SPINLOCK_INITIALIZER   {.owner = SPINLOCK_FREE,.count = 0}
#define CORE_ID_REGVAL_XOR_SWAP (0xCDCD ^ 0xABAB)

typedef struct {
    NEED_VOLATILE_MUX uint32_t owner;
    NEED_VOLATILE_MUX uint32_t count;
}spinlock_t;

/**
 * @brief Initialize a lock to its default state - unlocked
 * @param lock - spinlock object to initialize
 */
static inline void __attribute__((always_inline)) spinlock_initialize(spinlock_t *lock)
{
    assert(lock);
#if !CONFIG_FREERTOS_UNICORE
    lock->owner = SPINLOCK_FREE;
    lock->count = 0;
#endif
}

/**
 * @brief Top level spinlock acquire function, spins until get the lock
 * @param lock - target spinlock object
 * @param timeout - cycles to wait, passing SPINLOCK_WAIT_FOREVER blocs indefinitely
 */
static inline bool __attribute__((always_inline)) spinlock_acquire(spinlock_t *lock, int32_t timeout)
{
#if !CONFIG_FREERTOS_UNICORE
    uint32_t result;
    uint32_t irq_status;
    uint32_t ccount_start;
    uint32_t core_id, other_core_id;
   
    assert(lock);
    irq_status = XTOS_SET_INTLEVEL(XCHAL_EXCM_LEVEL);    
 
    if(timeout != SPINLOCK_WAIT_FOREVER){
        RSR(CCOUNT, ccount_start);   
    }

    /*spin until we own a core */
    RSR(PRID, core_id);

    /* Note: coreID is the full 32 bit core ID (CORE_ID_REGVAL_PRO/CORE_ID_REGVAL_APP) */
    
    other_core_id = CORE_ID_REGVAL_XOR_SWAP ^ core_id;
    do {

        /* lock->owner should be one of SPINLOCK_FREE, CORE_ID_REGVAL_PRO, 
         * CORE_ID_REGVAL_APP:
         *  - If SPINLOCK_FREE, we want to atomically set to 'core_id'.
         *  - If "our" core_id, we can drop through immediately.
         *  - If "other_core_id", we spin here.
         */
        result = core_id;

#if defined(CONFIG_ESP32_SPIRAM_SUPPORT)
        if (esp_ptr_external_ram(lock)) {
            compare_and_set_extram(&lock->owner, SPINLOCK_FREE, &result);
        } else {
#endif
        compare_and_set_native(&lock->owner, SPINLOCK_FREE, &result);
#if defined(CONFIG_ESP32_SPIRAM_SUPPORT)
        }
#endif
        if(result != other_core_id) {
            break;
        }

        if (timeout != SPINLOCK_WAIT_FOREVER) {
            uint32_t ccount_now;
            RSR(CCOUNT, ccount_now);
            if (ccount_now - ccount_start > (unsigned)timeout) {
                XTOS_RESTORE_INTLEVEL(irq_status);
                return false;
            }
        }
    }while(1);

    /* any other value implies memory corruption or uninitialized mux */
    assert(result == core_id || result == SPINLOCK_FREE); 
    assert((result == SPINLOCK_FREE) == (lock->count == 0)); /* we're first to lock iff count is zero */
    assert(lock->count < 0xFF); /* Bad count value implies memory corruption */

    lock->count++;
    XTOS_RESTORE_INTLEVEL(irq_status);
    return true;

#else 
    return true;
#endif    

}

/**
 * @brief Top level spinlock unlock function, unlocks a previously locked spinlock
 * @param lock - target, locked before, spinlock object
 */
static inline void __attribute__((always_inline)) spinlock_release(spinlock_t *lock)
{
    
#if !CONFIG_FREERTOS_UNICORE
    uint32_t irq_status;  
    uint32_t core_id;

    assert(lock);
    irq_status = XTOS_SET_INTLEVEL(XCHAL_EXCM_LEVEL);
        
    RSR(PRID, core_id);
    assert(core_id == lock->owner); // This is a mutex we didn't lock, or it's corrupt
    lock->count--;

    if(!lock->count) {
        lock->owner = SPINLOCK_FREE;
    } else {
        assert(lock->count < 0x100); // Indicates memory corruption
    }

    XTOS_RESTORE_INTLEVEL(irq_status);
#endif    
}

#endif

