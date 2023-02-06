/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/*
 Generic test for realloc
*/

#include <stdlib.h>
#include <string.h>
#include "unity.h"
#include "sdkconfig.h"
#include "esp_heap_caps.h"
#include "soc/soc_memory_layout.h"


#ifndef CONFIG_HEAP_POISONING_COMPREHENSIVE
/* (can't realloc in place if comprehensive is enabled) */

TEST_CASE("realloc shrink buffer in place", "[heap]")
{
    // pointers converted to int to avoid warning -Wuse-after-free
    int x = (int) malloc(64);
    TEST_ASSERT(x);
    int y = (int) realloc((void *) x, 48);
    TEST_ASSERT_EQUAL_UINT32((uint32_t) x, (uint32_t) y);
}

#endif

#ifndef CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
TEST_CASE("realloc shrink buffer with EXEC CAPS", "[heap]")
{
    const size_t buffer_size = 64;

    void *x = heap_caps_malloc(buffer_size, MALLOC_CAP_EXEC);
    TEST_ASSERT(x);
    void *y = heap_caps_realloc(x, buffer_size - 16, MALLOC_CAP_EXEC);
    TEST_ASSERT(y);

    //y needs to fall in a compatible memory area of IRAM:
    TEST_ASSERT(esp_ptr_executable(y)|| esp_ptr_in_iram(y) || esp_ptr_in_diram_iram(y));

    free(y);
}

TEST_CASE("realloc move data to a new heap type", "[heap]")
{
    const char *test = "I am some test content to put in the heap";
    char buf[64];
    memset(buf, 0xEE, 64);
    strlcpy(buf, test, 64);

    char *a = malloc(64);
    memcpy(a, buf, 64);
    // move data from 'a' to IRAM
    char *b = heap_caps_realloc(a, 64, MALLOC_CAP_EXEC);
    TEST_ASSERT_NOT_NULL(b);
    TEST_ASSERT(heap_caps_check_integrity(MALLOC_CAP_INVALID, true));
    TEST_ASSERT_EQUAL_HEX32_ARRAY(buf, b, 64 / sizeof(uint32_t));

    // Move data back to DRAM
    char *c = heap_caps_realloc(b, 48, MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(c);
    TEST_ASSERT(heap_caps_check_integrity(MALLOC_CAP_INVALID, true));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(buf, c, 48);

    free(c);
}
#endif // CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
