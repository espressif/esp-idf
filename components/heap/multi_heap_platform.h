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

#ifdef ESP_PLATFORM

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <rom/ets_sys.h>
#include <assert.h>

/* Because malloc/free can happen inside an ISR context,
   we need to use portmux spinlocks here not RTOS mutexes */
#define MULTI_HEAP_LOCK(PLOCK) do {               \
        if((PLOCK) != NULL) {                               \
            taskENTER_CRITICAL((portMUX_TYPE *)(PLOCK));    \
        }                                                   \
    } while(0)


#define MULTI_HEAP_UNLOCK(PLOCK) do {                \
        if ((PLOCK) != NULL) {                              \
            taskEXIT_CRITICAL((portMUX_TYPE *)(PLOCK));     \
        }                                                   \
    } while(0)

/* Not safe to use std i/o while in a portmux critical section,
   can deadlock, so we use the ROM equivalent functions. */

#define MULTI_HEAP_PRINTF ets_printf
#define MULTI_HEAP_STDERR_PRINTF(MSG, ...) ets_printf(MSG, __VA_ARGS__)

inline static void multi_heap_assert(bool condition, const char *format, int line, intptr_t address)
{
    /* Can't use libc assert() here as it calls printf() which can cause another malloc() for a newlib lock.

       Also, it's useful to be able to print the memory address where corruption was detected.
    */
#ifndef NDEBUG
    if(!condition) {
#ifndef CONFIG_OPTIMIZATION_ASSERTIONS_SILENT
        ets_printf(format, line, address);
#endif  // CONFIG_OPTIMIZATION_ASSERTIONS_SILENT
        abort();
    }
#else // NDEBUG
    (void) condition;
#endif // NDEBUG
}

#define MULTI_HEAP_ASSERT(CONDITION, ADDRESS) \
    multi_heap_assert((CONDITION), "CORRUPT HEAP: multi_heap.c:%d detected at 0x%08x\n", \
                      __LINE__, (intptr_t)(ADDRESS))

#else // ESP_PLATFORM

#include <assert.h>

#define MULTI_HEAP_PRINTF printf
#define MULTI_HEAP_STDERR_PRINTF(MSG, ...) fprintf(stderr, MSG, __VA_ARGS__)
#define MULTI_HEAP_LOCK(PLOCK)
#define MULTI_HEAP_UNLOCK(PLOCK)

#define MULTI_HEAP_ASSERT(CONDITION, ADDRESS) assert((CONDITION) && "Heap corrupt")
#endif
