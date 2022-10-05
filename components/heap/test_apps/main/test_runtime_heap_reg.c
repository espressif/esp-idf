/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/*
 Tests for registering new heap memory at runtime
*/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "unity.h"
#include "esp_heap_caps_init.h"
#include "esp_system.h"
#include "heap_memory_layout.h"

#include "../tlsf/tlsf.h"

/* NOTE: This is not a well-formed unit test, it leaks memory */
TEST_CASE("Allocate new heap at runtime", "[heap][ignore]")
{
    // 60 bytes of overhead in multi_heap + size of control_t from tlsf
    const size_t HEAP_OVERHEAD_MAX = tlsf_size() + 60;
    const size_t MIN_HEAP_SIZE = HEAP_OVERHEAD_MAX + tlsf_block_size_min();
    const size_t BUF_SZ = MIN_HEAP_SIZE;
    void *buffer = malloc(BUF_SZ);

    TEST_ASSERT_NOT_NULL(buffer);
    uint32_t before_free = esp_get_free_heap_size();
    TEST_ESP_OK( heap_caps_add_region((intptr_t)buffer, (intptr_t)buffer + BUF_SZ) );
    uint32_t after_free = esp_get_free_heap_size();
    printf("Before %"PRIu32" after %"PRIu32"\n", before_free, after_free);
    /* allow for some 'heap overhead' from accounting structures */
    TEST_ASSERT(after_free >= before_free + BUF_SZ - HEAP_OVERHEAD_MAX);
}

/* NOTE: This is not a well-formed unit test, it leaks memory and
   may fail if run twice in a row without a reset.
*/
TEST_CASE("Allocate new heap with new capability", "[heap][ignore]")
{
    // 60 bytes of multi_heap structures overhead + size of control_t from tlsf
    const size_t HEAP_OVERHEAD = tlsf_size() + 60;
    const size_t MIN_HEAP_SIZE = HEAP_OVERHEAD + tlsf_block_size_min();
    const size_t BUF_SZ = MIN_HEAP_SIZE;
    const size_t ALLOC_SZ = tlsf_block_size_min();

    const uint32_t MALLOC_CAP_INVENTED = (1 << 30); /* this must be unused in esp_heap_caps.h */

    /* no memory exists to provide this capability */
    TEST_ASSERT_NULL( heap_caps_malloc(ALLOC_SZ, MALLOC_CAP_INVENTED) );

    void *buffer = malloc(BUF_SZ);
    TEST_ASSERT_NOT_NULL(buffer);
    uint32_t caps[SOC_MEMORY_TYPE_NO_PRIOS] = { MALLOC_CAP_INVENTED };
    TEST_ESP_OK( heap_caps_add_region_with_caps(caps, (intptr_t)buffer, (intptr_t)buffer + BUF_SZ) );

    /* ta-da, it's now possible! */
    TEST_ASSERT_NOT_NULL( heap_caps_malloc(ALLOC_SZ, MALLOC_CAP_INVENTED) );
}

/* NOTE: This is not a well-formed unit test.
 * If run twice without a reset, it will failed.
 */

TEST_CASE("Add .bss memory to heap region runtime", "[heap][ignore]")
{
#define HEAP_OVERHEAD_MAX 3248
#define BUF_SZ 3260
    static uint8_t s_buffer[BUF_SZ];

    printf("s_buffer start %08x end %08x\n", (intptr_t)s_buffer, (intptr_t)s_buffer + BUF_SZ);
    uint32_t before_free = esp_get_free_heap_size();
    TEST_ESP_OK( heap_caps_add_region((intptr_t)s_buffer, (intptr_t)s_buffer + BUF_SZ) );
    uint32_t after_free = esp_get_free_heap_size();
    printf("Before %"PRIu32" after %"PRIu32"\n", before_free, after_free);
    /* allow for some 'heap overhead' from accounting structures */
    TEST_ASSERT(after_free >= before_free + BUF_SZ - HEAP_OVERHEAD_MAX);
}

extern esp_err_t heap_caps_check_add_region_allowed(intptr_t heap_start, intptr_t heap_end, intptr_t start, intptr_t end);

TEST_CASE("Add heap region address range checks", "[heap]")
{
    const intptr_t heap_start = 0x1000;
    const intptr_t heap_end = 0x3000;

    TEST_ASSERT_TRUE(heap_caps_check_add_region_allowed(heap_start, heap_end, 0x0, 0x1000));
    TEST_ASSERT_TRUE(heap_caps_check_add_region_allowed(heap_start, heap_end, 0x1000, 0x2000));
    TEST_ASSERT_TRUE(heap_caps_check_add_region_allowed(heap_start, heap_end, 0x1000, 0x3000));
    TEST_ASSERT_TRUE(heap_caps_check_add_region_allowed(heap_start, heap_end, 0x3000, 0x4000));
    TEST_ASSERT_FALSE(heap_caps_check_add_region_allowed(heap_start, heap_end, 0x0, 0x2000));
    TEST_ASSERT_FALSE(heap_caps_check_add_region_allowed(heap_start, heap_end, 0x0, 0x4000));
    TEST_ASSERT_FALSE(heap_caps_check_add_region_allowed(heap_start, heap_end, 0x1000, 0x4000));
    TEST_ASSERT_FALSE(heap_caps_check_add_region_allowed(heap_start, heap_end, 0x2000, 0x4000));
}
