/*
 Generic test for realloc
*/

#include <stdlib.h>
#include <string.h>
#include "unity.h"
#include "sdkconfig.h"
#include "esp_heap_caps.h"


#ifndef CONFIG_HEAP_POISONING_COMPREHENSIVE
/* (can't realloc in place if comprehensive is enabled) */

TEST_CASE("realloc shrink buffer in place", "[heap]")
{
    void *x = malloc(64);
    TEST_ASSERT(x);
    void *y = realloc(p, 48);
    TEST_ASSERT_EQUAL_PTR(x, y);
}

#endif

TEST_CASE_ESP32("realloc move data to a new heap type", "[heap]")
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
    TEST_ASSERT_NOT_EQUAL(a, b);
    TEST_ASSERT(heap_caps_check_integrity(MALLOC_CAP_INVALID, true));
    TEST_ASSERT_EQUAL_HEX32_ARRAY(buf, b, 64/sizeof(uint32_t));

    // Move data back to DRAM
    char *c = heap_caps_realloc(b, 48, MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(c);
    TEST_ASSERT_NOT_EQUAL(b, c);
    TEST_ASSERT(heap_caps_check_integrity(MALLOC_CAP_INVALID, true));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(buf, c, 48);

    free(c);
}
