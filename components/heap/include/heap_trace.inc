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
#include <string.h>
#include <sdkconfig.h>
#include "soc/soc_memory_layout.h"
#include "esp_attr.h"
#include "esp_cpu.h"
#include "esp_macros.h"

/* Encode the CPU ID in the LSB of the ccount value */
inline static uint32_t get_ccount(void)
{
    uint32_t ccount = esp_cpu_get_cycle_count() & ~3;
#ifndef CONFIG_FREERTOS_UNICORE
    ccount |= xPortGetCoreID();
#endif
    return ccount;
}

/* Architecture-specific return value of __builtin_return_address which
 * should be interpreted as an invalid address.
 */
#ifdef __XTENSA__
#define HEAP_ARCH_INVALID_PC  0x40000000
#else
#define HEAP_ARCH_INVALID_PC  0x00000000
#endif

// Caller is 2 stack frames deeper than we care about
#define STACK_OFFSET  2

#define TEST_STACK(N) do {                                              \
        if (STACK_DEPTH == N) {                                         \
            return;                                                     \
        }                                                               \
        callers[N] = __builtin_return_address(N+STACK_OFFSET);          \
        if (!esp_ptr_executable(callers[N])                             \
            || callers[N] == (void*) HEAP_ARCH_INVALID_PC) {            \
            callers[N] = 0;                                             \
            return;                                                     \
        }                                                               \
    } while(0)

/* Static function to read the call stack for a traced heap call.

   Calls to __builtin_return_address are "unrolled" via TEST_STACK macro as gcc requires the
   argument to be a compile-time constant.
*/
static HEAP_IRAM_ATTR __attribute__((noinline)) void get_call_stack(void **callers)
{
    bzero(callers, sizeof(void *) * STACK_DEPTH);
    TEST_STACK(0);
    TEST_STACK(1);
    TEST_STACK(2);
    TEST_STACK(3);
    TEST_STACK(4);
    TEST_STACK(5);
    TEST_STACK(6);
    TEST_STACK(7);
    TEST_STACK(8);
    TEST_STACK(9);
}

ESP_STATIC_ASSERT(STACK_DEPTH >= 0 && STACK_DEPTH <= 10, "CONFIG_HEAP_TRACING_STACK_DEPTH must be in range 0-10");


typedef enum {
    TRACE_MALLOC_CAPS,
    TRACE_MALLOC_DEFAULT
} trace_malloc_mode_t;


void *__real_heap_caps_malloc(size_t size, uint32_t caps);
void *__real_heap_caps_malloc_default( size_t size );
void *__real_heap_caps_realloc_default( void *ptr, size_t size );

/* trace any 'malloc' event */
static HEAP_IRAM_ATTR __attribute__((noinline)) void *trace_malloc(size_t size, uint32_t caps, trace_malloc_mode_t mode)
{
    uint32_t ccount = get_ccount();
    void *p;

    if ( mode == TRACE_MALLOC_CAPS ) {
        p = __real_heap_caps_malloc(size, caps);
    } else { //TRACE_MALLOC_DEFAULT
        p = __real_heap_caps_malloc_default(size);
    }

    heap_trace_record_t rec = {
        .address = p,
        .ccount = ccount,
        .size = size,
    };
    get_call_stack(rec.alloced_by);
    record_allocation(&rec);
    return p;
}

void __real_heap_caps_free(void *p);

/* trace any 'free' event */
static HEAP_IRAM_ATTR __attribute__((noinline)) void trace_free(void *p)
{
    void *callers[STACK_DEPTH];
    get_call_stack(callers);
    record_free(p, callers);

    __real_heap_caps_free(p);
}

void * __real_heap_caps_realloc(void *p, size_t size, uint32_t caps);

/* trace any 'realloc' event */
static HEAP_IRAM_ATTR __attribute__((noinline)) void *trace_realloc(void *p, size_t size, uint32_t caps, trace_malloc_mode_t mode)
{
    void *callers[STACK_DEPTH];
    uint32_t ccount = get_ccount();
    void *r;

    /* trace realloc as free-then-alloc */
    get_call_stack(callers);
    record_free(p, callers);

    if (mode == TRACE_MALLOC_CAPS ) {
        r = __real_heap_caps_realloc(p, size, caps);
    } else { //TRACE_MALLOC_DEFAULT
        r = __real_heap_caps_realloc_default(p, size);
    }
    /* realloc with zero size is a free */
    if (size != 0) {
        heap_trace_record_t rec = {
            .address = r,
            .ccount = ccount,
            .size = size,
        };
        memcpy(rec.alloced_by, callers, sizeof(void *) * STACK_DEPTH);
        record_allocation(&rec);
    }
    return r;
}

/* Note: this changes the behaviour of libc malloc/realloc/free a bit,
   as they no longer go via the libc functions in ROM. But more or less
   the same in the end. */

HEAP_IRAM_ATTR void *__wrap_malloc(size_t size)
{
    return trace_malloc(size, 0, TRACE_MALLOC_DEFAULT);
}

HEAP_IRAM_ATTR void __wrap_free(void *p)
{
    trace_free(p);
}

HEAP_IRAM_ATTR void *__wrap_realloc(void *p, size_t size)
{
    return trace_realloc(p, size, 0, TRACE_MALLOC_DEFAULT);
}

HEAP_IRAM_ATTR void *__wrap_calloc(size_t nmemb, size_t size)
{
    size = size * nmemb;
    void *result = trace_malloc(size, 0, TRACE_MALLOC_DEFAULT);
    if (result != NULL) {
        memset(result, 0, size);
    }
    return result;
}

HEAP_IRAM_ATTR void *__wrap_heap_caps_malloc(size_t size, uint32_t caps)
{
    return trace_malloc(size, caps, TRACE_MALLOC_CAPS);
}

void __wrap_heap_caps_free(void *p) __attribute__((alias("__wrap_free")));

HEAP_IRAM_ATTR void *__wrap_heap_caps_realloc(void *p, size_t size, uint32_t caps)
{
    return trace_realloc(p, size, caps, TRACE_MALLOC_CAPS);
}

HEAP_IRAM_ATTR void *__wrap_heap_caps_malloc_default( size_t size )
{
    return trace_malloc(size, 0, TRACE_MALLOC_DEFAULT);
}

HEAP_IRAM_ATTR void *__wrap_heap_caps_realloc_default( void *ptr, size_t size )
{
    return trace_realloc(ptr, size, 0, TRACE_MALLOC_DEFAULT);
}
