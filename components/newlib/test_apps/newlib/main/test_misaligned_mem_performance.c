/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdint.h>
#include <string.h>
#include "esp_heap_caps.h"
#include "hal/cpu_ll.h"
#include "unity.h"

#define MAX_MEMTEST_SIZE 4096

uint32_t test_function_dest_src_size(void (*foo)(), bool pass_size)
{
    uint32_t ccount1, ccount2;
    char* test_des = heap_caps_aligned_alloc(32, MAX_MEMTEST_SIZE, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    char* test_src = heap_caps_aligned_alloc(32, MAX_MEMTEST_SIZE, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

    TEST_ASSERT_NOT_NULL(test_des);
    TEST_ASSERT_NOT_NULL(test_src);

    /* Prepare arrays for X-cmp functions to make the algorithm go through whole buffers. */
    memset(test_src, 'a', MAX_MEMTEST_SIZE);
    test_src[MAX_MEMTEST_SIZE - 1] = 0;
    memset(test_des, 'a', MAX_MEMTEST_SIZE);
    test_des[MAX_MEMTEST_SIZE - 1] = 'b';
    test_des[MAX_MEMTEST_SIZE - 1] = 0;

    ccount1 = esp_cpu_get_cycle_count();
    if (pass_size) {
        foo(test_des + 1, test_src + 2, MAX_MEMTEST_SIZE - 2);
    } else {
        foo(test_des + 1, test_src + 2);
    }
    ccount2 = esp_cpu_get_cycle_count();

    heap_caps_free(test_des);
    heap_caps_free(test_src);

    return ccount2 - ccount1;
}

TEST_CASE("memcpy", "[misaligned_mem]")
{
    uint32_t ccount = test_function_dest_src_size(memcpy, true);
    /* esp32c2: 4128 cycles instead 28676.  */
    TEST_ASSERT_LESS_THAN(5000, ccount);
}

TEST_CASE("memcmp", "[misaligned_mem]")
{
    uint32_t ccount = test_function_dest_src_size(memcmp, true);
    /* esp32c2: 14259 cycles instead 49147.  */
    TEST_ASSERT_LESS_THAN(16000, ccount);
}

TEST_CASE("memmove", "[misaligned_mem]")
{
    uint32_t ccount = test_function_dest_src_size(memmove, true);
    /* esp32c2: 8086 cycles instead 33896.  */
    TEST_ASSERT_LESS_THAN(10000, ccount);
}

TEST_CASE("memmove - overlapping", "[misaligned_mem]")
{
    uint32_t ccount1, ccount2;
    char* buf = heap_caps_aligned_alloc(32, MAX_MEMTEST_SIZE, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

    TEST_ASSERT_NOT_NULL(buf);

    ccount1 = esp_cpu_get_cycle_count();
    memmove(buf + 5, buf + 2, MAX_MEMTEST_SIZE - 5);
    ccount2 = esp_cpu_get_cycle_count();

    heap_caps_free(buf);

    /* esp32c2: 11503 cycles instead 45024.  */
    TEST_ASSERT_LESS_THAN(15000, ccount2 - ccount1);
}

TEST_CASE("strcpy", "[misaligned_mem]")
{
    uint32_t ccount = test_function_dest_src_size(strcpy, false);
    /* esp32c2: 17313 cycles instead 32771.  */
    TEST_ASSERT_LESS_THAN(20000, ccount);
}

TEST_CASE("strcmp", "[misaligned_mem]")
{
    uint32_t ccount = test_function_dest_src_size(strcmp, false);
    /* esp32c2: 13191 cycles instead 32775.  */
    TEST_ASSERT_LESS_THAN(15000, ccount);
}

TEST_CASE("strncpy", "[misaligned_mem]")
{
    uint32_t ccount = test_function_dest_src_size(strncpy, true);
    /* esp32c2: 21475 cycles instead 36859.  */
    TEST_ASSERT_LESS_THAN(25000, ccount);
}

TEST_CASE("strncmp", "[misaligned_mem]")
{
    uint32_t ccount = test_function_dest_src_size(strncmp, true);
    /* esp32c2: 24369 cycles instead 49141.  */
    TEST_ASSERT_LESS_THAN(27000, ccount);
}
