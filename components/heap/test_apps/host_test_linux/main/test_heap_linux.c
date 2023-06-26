/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "esp_heap_caps.h"
#include "unity.h"

#define MALLOC_LEN 1000
#define TEST_VAL 0xAB

TEST_CASE("Alloc APIs", "[heap]")
{
    uint8_t * p = heap_caps_malloc(MALLOC_LEN, MALLOC_CAP_DEFAULT);
    TEST_ASSERT_NOT_NULL(p);
    memset(p, TEST_VAL, MALLOC_LEN);

    p = heap_caps_realloc(p, 2*MALLOC_LEN, MALLOC_CAP_DEFAULT);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EACH_EQUAL_HEX8(TEST_VAL, p, MALLOC_LEN);

    heap_caps_realloc(p, 0, MALLOC_CAP_DEFAULT);

    p = heap_caps_calloc(MALLOC_LEN, sizeof(uint8_t), MALLOC_CAP_DEFAULT);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EACH_EQUAL_HEX8(0, p, MALLOC_LEN);

    /*Alloc prefer APIs*/
    p = heap_caps_malloc_prefer(MALLOC_LEN, MALLOC_CAP_DEFAULT);
    TEST_ASSERT_NOT_NULL(p);
    memset(p, TEST_VAL, MALLOC_LEN);

    p = heap_caps_realloc_prefer(p, 2*MALLOC_LEN, MALLOC_CAP_DEFAULT);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EACH_EQUAL_HEX8(TEST_VAL, p, MALLOC_LEN);

    heap_caps_realloc_prefer(p, 0, MALLOC_CAP_DEFAULT);

    p = heap_caps_calloc_prefer(MALLOC_LEN, sizeof(uint8_t), MALLOC_CAP_DEFAULT);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EACH_EQUAL_HEX8(0, p, MALLOC_LEN);

    heap_caps_free(p);
}

TEST_CASE("Size APIs", "[heap]")
{
    /* These functions doesnt provide any useful information on linux
       These "tests" are simply included to check that all the functions in the header
       are actually mocked
    */
    printf("heap_caps_get_total_size: %zu\n", heap_caps_get_total_size(MALLOC_CAP_DEFAULT));
    printf("heap_caps_get_free_size: %zu\n", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
    printf("heap_caps_get_minimum_free_size: %zu\n", heap_caps_get_minimum_free_size(MALLOC_CAP_DEFAULT));
    printf("heap_caps_get_largest_free_block: %zu\n", heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT));

    multi_heap_info_t info;
    heap_caps_get_info(&info, MALLOC_CAP_DEFAULT);
    heap_caps_check_integrity_all(true);
    heap_caps_check_integrity(MALLOC_CAP_DEFAULT, true);
    heap_caps_check_integrity_addr((intptr_t)&info, true);
    heap_caps_malloc_extmem_enable(100);

    heap_caps_dump(MALLOC_CAP_DEFAULT);
    heap_caps_dump_all();
    TEST_ASSERT_TRUE(heap_caps_get_allocated_size(&info) == 0);
}

#define TEST_ALIGNMENT 0x1F

TEST_CASE("Aligned alloc APIs", "[heap]")
{
    uint8_t * p = heap_caps_aligned_alloc(TEST_ALIGNMENT, MALLOC_LEN, MALLOC_CAP_DEFAULT);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_TRUE(((intptr_t)p & (0x1F -1)) == 0);
    memset(p, TEST_VAL, MALLOC_LEN);
    TEST_ASSERT_EACH_EQUAL_HEX8(TEST_VAL, p, MALLOC_LEN);
    heap_caps_free(p);

    p = heap_caps_aligned_calloc(TEST_ALIGNMENT, MALLOC_LEN, sizeof(uint8_t), MALLOC_CAP_DEFAULT);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_TRUE(((intptr_t)p & (0x1F -1)) == 0);
    TEST_ASSERT_EACH_EQUAL_HEX8(0, p, MALLOC_LEN);
    heap_caps_free(p);
}

static bool alloc_failed;

void alloc_failed_cb(size_t size, uint32_t caps, const char * function_name)
{
    printf("Failed alloc of size %zu bytes at %s", size, function_name);
    alloc_failed = true;
}

TEST_CASE("Misc APIs", "[heap]")
{
    heap_caps_register_failed_alloc_callback(alloc_failed_cb);

    heap_caps_malloc(SIZE_MAX, MALLOC_CAP_DEFAULT);
    TEST_ASSERT_TRUE(alloc_failed);
}

void app_main(void)
{
    printf("Running heap linux API host test app");
    unity_run_menu();
}
