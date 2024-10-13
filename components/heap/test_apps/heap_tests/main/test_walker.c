/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "unity.h"
#include "stdio.h"

#include "esp_heap_caps.h"
#include "esp_rom_sys.h"
#define PASS_CODE 0x9876BAAB
#define ALLOC_SIZE 16

static void calculate_block_metadata_size(size_t *header, size_t *footer)
{
    *header = 0;
    *footer = 0;
#if !CONFIG_HEAP_POISONING_DISABLED
    *header += 8; // sizeof(poison_head_t)
    *footer += 4; // sizeof(poison_tail_t)
#endif
#if CONFIG_HEAP_TASK_TRACKING
    *header += 4; // sizeof(TaskHandle_t)
#endif
}

static bool block_found = false;
static bool heap_corrupted = false;
static bool heap_walker(walker_heap_into_t heap_info, walker_block_info_t block_info, void* user_data)
{
    if ((intptr_t)heap_info.end - (intptr_t)block_info.ptr < block_info.size)
    {
        heap_corrupted = true;
        return false;
    }

    TEST_ASSERT(*(uint32_t*)user_data == PASS_CODE);
    TEST_ASSERT_NOT_NULL(block_info.ptr);

    size_t metadata_size_head = 0;
    size_t metadata_size_tail = 0;
    calculate_block_metadata_size(&metadata_size_head, &metadata_size_tail);

    /* look for the first 4 bytes pass code to identify the memory allocated in the test */
    const uint32_t pass_code = *((uint32_t*)block_info.ptr + (metadata_size_head / sizeof(uint32_t)));
    if (pass_code == PASS_CODE) {
        TEST_ASSERT(block_info.size == ALLOC_SIZE + metadata_size_head + metadata_size_tail);
        TEST_ASSERT_TRUE(block_info.used);
        block_found = true;
    }

    return true;
}

/* This test assures the proper functioning of heap_caps_walk and heap_caps_walk_all
 * when a corruption occurs in a random heap. The callback which detects the corruption
 * returns false to the walker which should result in the interruption of the heap traversal
 * by the walker instead of a crash.
 */
TEST_CASE("heap walker", "[heap]")
{
    /* Allocate memory using the MALLOC_CAP_DEFAULT capability */
    void *default_ptr = heap_caps_malloc(ALLOC_SIZE, MALLOC_CAP_DEFAULT);
    TEST_ASSERT_NOT_NULL(default_ptr);

    /* Write the pass code in the first word of the allocated memory */
    *((uint32_t*)default_ptr) = (uint32_t)PASS_CODE;

    /* call the heap_caps_walker to make sure the hook function is triggered
     * and check that the allocated memory is found while walking the heap */
    uint32_t user_code = PASS_CODE;
    heap_caps_walk_all(heap_walker, &user_code);
    TEST_ASSERT_TRUE(block_found);
}

/* This test assures the proper functioning of heap_caps_walk and heap_caps_walk_all
 */
TEST_CASE("heap walker corrupted heap detection", "[heap]")
{
    /* Allocate memory using the MALLOC_CAP_DEFAULT capability */
    void *default_ptr = heap_caps_malloc(ALLOC_SIZE, MALLOC_CAP_DEFAULT);
    TEST_ASSERT_NOT_NULL(default_ptr);

    size_t metadata_size_head = 0;
    size_t metadata_size_tail = 0;
    calculate_block_metadata_size(&metadata_size_head, &metadata_size_tail);
    (void)metadata_size_tail;

    /* corrupting the size field of the block metadata with a size bigger
     * than the heap itself */
    *((uint32_t*)default_ptr - (metadata_size_head / 4) - 1) = 0xFF000000;

    /* Write the pass code in the first word of the allocated memory */
    *((uint32_t*)default_ptr) = (uint32_t)PASS_CODE;

    /* call the heap_caps_walker to make sure the hook function is triggered
     * and check that the allocated memory is found while walking the heap */
    uint32_t user_code = PASS_CODE;
    heap_caps_walk_all(heap_walker, &user_code);
    TEST_ASSERT_TRUE(heap_corrupted);
}
