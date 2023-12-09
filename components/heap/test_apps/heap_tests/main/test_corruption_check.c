/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "unity.h"
#include "stdio.h"

#include "esp_heap_caps.h"

// these tests only make sense with poisoning enabled (light or comprehensive)
#if defined(CONFIG_HEAP_POISONING_LIGHT) || defined(CONFIG_HEAP_POISONING_COMPREHENSIVE)

/* executing multi_heap_internal_check_block_poisoning()
 * takes longer on external RAM and therefore the timeout
 * in the test of 30 seconds is exceeded. Execute the test
 * on a smaller memory chunk
 */
#ifdef CONFIG_SPIRAM
const size_t MALLOC_SIZE = 16;
#else
const size_t MALLOC_SIZE = 64;
#endif
const uint8_t CORRUPTED_VALUE = 0xaa;

/* This test will corrupt the memory of a free block in the heap and check
 * that in the case of comprehensive poisoning the heap corruption is detected
 * by heap_caps_check_integrity(). For light poisoning and no poisoning, the test will
 * check that heap_caps_check_integrity() does not report the corruption.
 */
TEST_CASE("multi_heap poisoning detection", "[heap]")
{
    /* malloc some memory to get a pointer */
    uint8_t *ptr = heap_caps_malloc(MALLOC_SIZE, MALLOC_CAP_8BIT);

    /* free the memory to free the block but keep the pointer in mind */
    heap_caps_free(ptr);

    /* variable used in the test */
    uint8_t original_value = 0x00;

    for (size_t i = 0; i < MALLOC_SIZE; i++)
    {
        /* keep the good value in store in order to check that when we set the byte back
         * to its original value, heap_caps_check_integrity() no longer returns the
         * heap corruption. */
        original_value = ptr[i];

        /* set corrupted value in the free memory*/
        ptr[i] = CORRUPTED_VALUE;

        bool is_heap_ok = heap_caps_check_integrity(MALLOC_CAP_8BIT, true);

        /* fix the corruption by restoring the original value at ptr + i.
         * We need to do this before the ASSERT because they may print a message.
         * Using print allocates memory on the heap, so the heap has to be fixed. */
        ptr[i] = original_value;

#if CONFIG_HEAP_POISONING_COMPREHENSIVE
        /* check that heap_caps_check_integrity() detects the corruption */
        TEST_ASSERT_FALSE(is_heap_ok);
#else
        /* the comprehensive corruption is not checked in the heap_caps_check_integrity() */
        TEST_ASSERT_TRUE(is_heap_ok);
#endif

        /* check that heap_caps_check_integrity() stops reporting the corruption */
        is_heap_ok = heap_caps_check_integrity(MALLOC_CAP_8BIT, true);
        TEST_ASSERT_TRUE(is_heap_ok);
    }
}

#ifdef CONFIG_HEAP_TASK_TRACKING
#define HEAD_CANARY_OFFSET  3 // head canary | task tracking | allocated size
#else
#define HEAD_CANARY_OFFSET  2 // head canary | allocated size
#endif // CONFIG_HEAP_TASK_TRACKING

#define TAIL_CANARY_OFFSET 1

/* This test will corrupt the canary of a allocated memory block and call the
 * heap_caps_check_integrity() function to check that the corruption is detected.
 */
TEST_CASE("canary corruption in light or comprehensive poisoning mode", "[heap]")
{
    const uint8_t allocation_size = 1 * sizeof(uint32_t);
    /* malloc some memory to get a pointer */
    uint32_t *ptr = heap_caps_malloc(allocation_size, MALLOC_CAP_DEFAULT);
    TEST_ASSERT_NOT_NULL(ptr);

    /* corrupt the head canary */
    uint32_t canary = ptr[-HEAD_CANARY_OFFSET];
    ptr[-HEAD_CANARY_OFFSET] = 0xdeadbeef;

    /* call the integrity check function and verify that it returns 0 (corruption detected) */
    bool is_corrupted = !heap_caps_check_integrity(MALLOC_CAP_DEFAULT, false);
    TEST_ASSERT_TRUE(is_corrupted);

    /* fix the head canary */
    ptr[-HEAD_CANARY_OFFSET] = canary;

    /* re run the corruption check to make sure the function returns no corruption */
    is_corrupted = !heap_caps_check_integrity(MALLOC_CAP_DEFAULT, false);
    TEST_ASSERT_FALSE(is_corrupted);

    /* corrupt tail canary */
    canary = ptr[TAIL_CANARY_OFFSET];
    ptr[TAIL_CANARY_OFFSET] = 0xdeadbeef;

    /* call the integrity check function and verify that it returns 0 (corruption detected) */
    is_corrupted = !heap_caps_check_integrity(MALLOC_CAP_DEFAULT, false);
    TEST_ASSERT_TRUE(is_corrupted);

    /* clear the corruption and free the pointer before returning */
    ptr[TAIL_CANARY_OFFSET] = canary;
    heap_caps_free(ptr);
}
#endif // CONFIG_HEAP_POISONING_LIGHT && CONFIG_HEAP_LIGHT_POISONING
