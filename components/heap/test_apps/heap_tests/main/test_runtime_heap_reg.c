/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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

extern void set_leak_threshold(int threshold);

/* NOTE: This is not a well-formed unit test, it leaks memory */
TEST_CASE("Allocate new heap at runtime", "[heap]")
{
    /* The value of the heap overhead is calculated for the worst case scenario
    * where the tlsf has a size of metadata fixed at runtime.
    */
    const size_t HEAP_OVERHEAD_MAX = 3248;
    const size_t BUF_SZ = 3500;
    void *buffer = malloc(BUF_SZ);

    TEST_ASSERT_NOT_NULL(buffer);
    uint32_t before_free = esp_get_free_heap_size();
    TEST_ESP_OK( heap_caps_add_region((intptr_t)buffer, (intptr_t)buffer + BUF_SZ) );
    uint32_t after_free = esp_get_free_heap_size();
    printf("Before %"PRIu32" after %"PRIu32"\n", before_free, after_free);
    /* allow for some 'heap overhead' from accounting structures */
    TEST_ASSERT(after_free >= before_free + BUF_SZ - HEAP_OVERHEAD_MAX);

    // set the leak threshold to a bigger value as this test leaks memory
    set_leak_threshold(-3000);
}

/* NOTE: This is not a well-formed unit test, it leaks memory and
   may fail if run twice in a row without a reset.
*/
TEST_CASE("Allocate new heap with new capability", "[heap]")
{
    const size_t BUF_SZ = 3500;
    const size_t ALLOC_SZ = 64;

    const uint32_t MALLOC_CAP_INVENTED = (1 << 30); /* this must be unused in esp_heap_caps.h */

    /* no memory exists to provide this capability */
    TEST_ASSERT_NULL( heap_caps_malloc(ALLOC_SZ, MALLOC_CAP_INVENTED) );

    void *buffer = malloc(BUF_SZ);
    TEST_ASSERT_NOT_NULL(buffer);
    uint32_t caps[SOC_MEMORY_TYPE_NO_PRIOS] = { MALLOC_CAP_INVENTED };
    TEST_ESP_OK( heap_caps_add_region_with_caps(caps, (intptr_t)buffer, (intptr_t)buffer + BUF_SZ) );

    /* ta-da, it's now possible! */
    TEST_ASSERT_NOT_NULL( heap_caps_malloc(ALLOC_SZ, MALLOC_CAP_INVENTED) );

    // set the leak threshold to a bigger value as this test leaks memory
    set_leak_threshold(-4000);
}

/* NOTE: This is not a well-formed unit test.
 * If run twice without a reset, it will failed.
 */

TEST_CASE("Add .bss memory to heap region runtime", "[heap]")
{
/* The value of the heap overhead is calculated for the worst case scenario
 * where the tlsf has a size of metadata fixed at runtime.
 */
#define HEAP_OVERHEAD_MAX 3248
#define BUF_SZ 3500
    static uint8_t s_buffer[BUF_SZ];

    printf("s_buffer start %08x end %08x\n", (intptr_t)s_buffer, (intptr_t)s_buffer + BUF_SZ);
    uint32_t before_free = esp_get_free_heap_size();
    TEST_ESP_OK( heap_caps_add_region((intptr_t)s_buffer, (intptr_t)s_buffer + BUF_SZ) );
    uint32_t after_free = esp_get_free_heap_size();
    printf("Before %"PRIu32" after %"PRIu32"\n", before_free, after_free);
    /* allow for some 'heap overhead' from accounting structures */
    TEST_ASSERT(after_free >= before_free + BUF_SZ - HEAP_OVERHEAD_MAX);

    /* Twice add must be failed */
    TEST_ASSERT( (heap_caps_add_region((intptr_t)s_buffer, (intptr_t)s_buffer + BUF_SZ) != ESP_OK) );
}

extern esp_err_t heap_caps_check_add_region_allowed(intptr_t heap_start, intptr_t heap_end, intptr_t start, intptr_t end);

TEST_CASE("Add heap region address range checks", "[heap]")
{
    const intptr_t heap_start = 0x1000;
    const intptr_t heap_end = 0x3000;

    TEST_ASSERT_TRUE(heap_caps_check_add_region_allowed(heap_start, heap_end, 0x0, 0x1000));
    TEST_ASSERT_TRUE(heap_caps_check_add_region_allowed(heap_start, heap_end, 0x3000, 0x4000));
    TEST_ASSERT_TRUE(heap_caps_check_add_region_allowed(heap_start, heap_end, 0x1000, 0x2999));
    TEST_ASSERT_TRUE(heap_caps_check_add_region_allowed(heap_start, heap_end, 0x1001, 0x3000));
    TEST_ASSERT_TRUE(heap_caps_check_add_region_allowed(heap_start, heap_end, 0x1001, 0x2999));
    TEST_ASSERT_FALSE(heap_caps_check_add_region_allowed(heap_start, heap_end, 0x0, 0x2000));
    TEST_ASSERT_FALSE(heap_caps_check_add_region_allowed(heap_start, heap_end, 0x0, 0x4000));
    TEST_ASSERT_FALSE(heap_caps_check_add_region_allowed(heap_start, heap_end, 0x1000, 0x4000));
    TEST_ASSERT_FALSE(heap_caps_check_add_region_allowed(heap_start, heap_end, 0x2000, 0x4000));
    TEST_ASSERT_FALSE(heap_caps_check_add_region_allowed(heap_start, heap_end, 0x1000, 0x3000));
}
