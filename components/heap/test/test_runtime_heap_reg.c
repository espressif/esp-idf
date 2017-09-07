/*
 Tests for registering new heap memory at runtime
*/

#include <stdio.h>
#include "unity.h"
#include "esp_heap_caps_init.h"
#include "esp_system.h"
#include <stdlib.h>


/* NOTE: This is not a well-formed unit test, it leaks memory */
TEST_CASE("Allocate new heap at runtime", "[heap][ignore]")
{
    const size_t BUF_SZ = 1000;
    const size_t HEAP_OVERHEAD_MAX = 200;
    void *buffer = malloc(BUF_SZ);
    TEST_ASSERT_NOT_NULL(buffer);
    uint32_t before_free = esp_get_free_heap_size();
    TEST_ESP_OK( heap_caps_add_region((intptr_t)buffer, (intptr_t)buffer + BUF_SZ) );
    uint32_t after_free = esp_get_free_heap_size();
    printf("Before %u after %u\n", before_free, after_free);
    /* allow for some 'heap overhead' from accounting structures */
    TEST_ASSERT(after_free > before_free + BUF_SZ - HEAP_OVERHEAD_MAX);
}

/* NOTE: This is not a well-formed unit test, it leaks memory and
   may fail if run twice in a row without a reset.
*/
TEST_CASE("Allocate new heap with new capability", "[heap][ignore]")
{
    const size_t BUF_SZ = 100;
    const size_t ALLOC_SZ = 64; // More than half of BUF_SZ
    const uint32_t MALLOC_CAP_INVENTED = (1<<30); /* this must be unused in esp_heap_caps.h */

    /* no memory exists to provide this capability */
    TEST_ASSERT_NULL( heap_caps_malloc(ALLOC_SZ, MALLOC_CAP_INVENTED) );

    void *buffer = malloc(BUF_SZ);
    TEST_ASSERT_NOT_NULL(buffer);
    uint32_t caps[SOC_MEMORY_TYPE_NO_PRIOS] = { MALLOC_CAP_INVENTED };
    TEST_ESP_OK( heap_caps_add_region_with_caps(caps, (intptr_t)buffer, (intptr_t)buffer + BUF_SZ) );

    /* ta-da, it's now possible! */
    TEST_ASSERT_NOT_NULL( heap_caps_malloc(ALLOC_SZ, MALLOC_CAP_INVENTED) );
}

