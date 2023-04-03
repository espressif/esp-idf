/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/*
 Tests for the capabilities-based memory allocator.
*/

#include <esp_types.h>
#include <stdio.h>
#include "unity.h"
#include "esp_attr.h"
#include "esp_heap_caps.h"
#include "spi_flash_mmap.h"
#include "esp_memory_utils.h"
#include "esp_private/spi_flash_os.h"
#include <stdlib.h>
#include <sys/param.h>

#ifndef CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
TEST_CASE("Capabilities allocator test", "[heap]")
{
    char *m1, *m2[10];
    int x;
    size_t free8start, free32start, free8, free32;

    /* It's important we printf() something before we take the empty heap sizes,
       as the first printf() in a task allocates heap resources... */
    printf("Testing capabilities allocator...\n");

    free8start = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    free32start = heap_caps_get_free_size(MALLOC_CAP_32BIT);
    printf("Free 8bit-capable memory (start): %dK, 32-bit capable memory %dK\n", free8start, free32start);
    TEST_ASSERT(free32start >= free8start);

    printf("Allocating 10K of 8-bit capable RAM\n");
    m1= heap_caps_malloc(10*1024, MALLOC_CAP_8BIT);
    printf("--> %p\n", m1);
    free8 = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    free32 = heap_caps_get_free_size(MALLOC_CAP_32BIT);
    printf("Free 8bit-capable memory (both reduced): %dK, 32-bit capable memory %dK\n", free8, free32);
    //Both should have gone down by 10K; 8bit capable ram is also 32-bit capable
    TEST_ASSERT(free8<=(free8start-10*1024));
    TEST_ASSERT(free32<=(free32start-10*1024));
    //Assume we got DRAM back
    TEST_ASSERT(esp_ptr_in_dram(m1));
    free(m1);

    //The goal here is to allocate from IRAM. Since there is no external IRAM (yet)
    //the following gives size of IRAM-only (not D/IRAM) memory.
    size_t free_iram = heap_caps_get_free_size(MALLOC_CAP_INTERNAL) -
                           heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    size_t alloc32 = MIN(free_iram / 2, 10*1024) & (~3);
    if(free_iram) {
        printf("Freeing; allocating %u bytes of 32K-capable RAM\n", alloc32);
        m1 = heap_caps_malloc(alloc32, MALLOC_CAP_32BIT);
        printf("--> %p\n", m1);
        //Check that we got IRAM back
        TEST_ASSERT((((int)m1)&0xFF000000)==0x40000000);
        free8 = heap_caps_get_free_size(MALLOC_CAP_8BIT);
        free32 = heap_caps_get_free_size(MALLOC_CAP_32BIT);
        printf("Free 8bit-capable memory (after 32-bit): %dK, 32-bit capable memory %dK\n", free8, free32);
        //Only 32-bit should have gone down by alloc32: 32-bit isn't necessarily 8bit capable
        TEST_ASSERT(free32<=(free32start-alloc32));
        TEST_ASSERT(free8==free8start);
        free(m1);
    } else {
        printf("This platform has no 32-bit only capable RAM, jumping to next test \n");
    }

    printf("Allocating impossible caps\n");
    m1= heap_caps_malloc(10*1024, MALLOC_CAP_8BIT|MALLOC_CAP_EXEC);
    printf("--> %p\n", m1);
    TEST_ASSERT(m1==NULL);

    if(free_iram) {
        printf("Testing changeover iram -> dram");
        // priorities will exhaust IRAM first, then start allocating from DRAM
        for (x=0; x<10; x++) {
            m2[x]= heap_caps_malloc(alloc32, MALLOC_CAP_32BIT);
            printf("--> %p\n", m2[x]);
        }
        TEST_ASSERT((((int)m2[0])&0xFF000000)==0x40000000);
        TEST_ASSERT((((int)m2[9])&0xFF000000)==0x3F000000);

    } else {
        printf("This platform has no IRAM-only so changeover will never occur, jumping to next test\n");
    }

    printf("Test if allocating executable code still gives IRAM, even with dedicated IRAM region depleted\n");
    if(free_iram) {
        // (the allocation should come from D/IRAM)
        free_iram = heap_caps_get_free_size(MALLOC_CAP_EXEC);
        m1= heap_caps_malloc(MIN(free_iram / 2, 10*1024), MALLOC_CAP_EXEC);
        printf("--> %p\n", m1);
        TEST_ASSERT((((int)m1)&0xFF000000)==0x40000000);
        for (x=0; x<10; x++) free(m2[x]);

    } else {
        // (the allocation should come from D/IRAM)
        free_iram = heap_caps_get_free_size(MALLOC_CAP_EXEC);
        m1= heap_caps_malloc(MIN(free_iram / 2, 10*1024), MALLOC_CAP_EXEC);
        printf("--> %p\n", m1);
        TEST_ASSERT((((int)m1)&0xFF000000)==0x40000000);
    }

    free(m1);
    printf("Done.\n");
}
#endif // CONFIG_ESP_SYSTEM_MEMPROT_FEATURE

#ifdef CONFIG_ESP32_IRAM_AS_8BIT_ACCESSIBLE_MEMORY
TEST_CASE("IRAM_8BIT capability test", "[heap]")
{
    uint8_t *ptr;
    size_t free_size, free_size32, largest_free_size;

    /* need to print something as first printf allocates some heap */
    printf("IRAM_8BIT capability test\n");

    free_size = heap_caps_get_free_size(MALLOC_CAP_IRAM_8BIT);
    free_size32 = heap_caps_get_free_size(MALLOC_CAP_32BIT);

    largest_free_size = heap_caps_get_largest_free_block(MALLOC_CAP_IRAM_8BIT);

    ptr = heap_caps_malloc(largest_free_size, MALLOC_CAP_IRAM_8BIT);

    TEST_ASSERT((((int)ptr)&0xFF000000)==0x40000000);

    /* As the heap allocator may present an overhead for allocated blocks,
     * we need to check that the free heap size is now smaller or equal to the former free size. */
    TEST_ASSERT(heap_caps_get_free_size(MALLOC_CAP_IRAM_8BIT) <= (free_size - heap_caps_get_allocated_size(ptr)));
    TEST_ASSERT(heap_caps_get_free_size(MALLOC_CAP_32BIT) <= (free_size32 - heap_caps_get_allocated_size(ptr)));

    free(ptr);
}
#endif

TEST_CASE("heap_caps metadata test", "[heap]")
{
    /* need to print something as first printf allocates some heap */
    printf("heap_caps metadata test\n");
    heap_caps_print_heap_info(MALLOC_CAP_8BIT);

    multi_heap_info_t original;
    heap_caps_get_info(&original, MALLOC_CAP_8BIT);

    void *b = heap_caps_malloc(original.largest_free_block, MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(b);

    printf("After allocating %d bytes:\n", original.largest_free_block);
    heap_caps_print_heap_info(MALLOC_CAP_8BIT);

    multi_heap_info_t after;
    heap_caps_get_info(&after, MALLOC_CAP_8BIT);
    TEST_ASSERT(after.largest_free_block <= original.largest_free_block);
    TEST_ASSERT(after.total_free_bytes <= original.total_free_bytes);

    free(b);
    heap_caps_get_info(&after, MALLOC_CAP_8BIT);

    printf("\n\n After test, heap status:\n");
    heap_caps_print_heap_info(MALLOC_CAP_8BIT);

    /* Allow some leeway here, because LWIP sometimes allocates up to 144 bytes in the background
       as part of timer management.
    */
    TEST_ASSERT_INT32_WITHIN(200, after.total_free_bytes, original.total_free_bytes);
    TEST_ASSERT_INT32_WITHIN(200, after.largest_free_block, original.largest_free_block);
    TEST_ASSERT(after.minimum_free_bytes < original.total_free_bytes);
}

/* Small function runs from IRAM to check that malloc/free/realloc
   all work OK when cache is disabled...
*/
#if !CONFIG_ESP_SYSTEM_MEMPROT_FEATURE && !CONFIG_HEAP_PLACE_FUNCTION_INTO_FLASH
static IRAM_ATTR __attribute__((noinline)) bool iram_malloc_test(void)
{
    spi_flash_guard_get()->start(); // Disables flash cache

    bool result = true;
    void *x = heap_caps_malloc(64, MALLOC_CAP_EXEC);
    result = result && (x != NULL);
    void *y = heap_caps_realloc(x, 32, MALLOC_CAP_EXEC);
    result = result && (y != NULL);
    heap_caps_free(y);

    spi_flash_guard_get()->end(); // Re-enables flash cache

    return result;
}


TEST_CASE("heap_caps_xxx functions work with flash cache disabled", "[heap]")
{
    TEST_ASSERT( iram_malloc_test() );
}
#endif // CONFIG_ESP_SYSTEM_MEMPROT_FEATURE

#ifdef CONFIG_HEAP_ABORT_WHEN_ALLOCATION_FAILS
TEST_CASE("When enabled, allocation operation failure generates an abort", "[heap][reset=abort,SW_CPU_RESET]")
{
    const size_t stupid_allocation_size = (128 * 1024 * 1024);
    void *ptr = heap_caps_malloc(stupid_allocation_size, MALLOC_CAP_DEFAULT);
    (void)ptr;
    TEST_FAIL_MESSAGE("should not be reached");
}
#endif

static bool called_user_failed_hook = false;

void heap_caps_alloc_failed_hook(size_t requested_size, uint32_t caps, const char *function_name)
{
    printf("%s was called but failed to allocate %d bytes with 0x%lX capabilities. \n",function_name, requested_size, caps);
    called_user_failed_hook = true;
}

TEST_CASE("user provided alloc failed hook must be called when allocation fails", "[heap]")
{
    TEST_ASSERT(heap_caps_register_failed_alloc_callback(heap_caps_alloc_failed_hook) == ESP_OK);

    const size_t stupid_allocation_size = (128 * 1024 * 1024);
    void *ptr = heap_caps_malloc(stupid_allocation_size, MALLOC_CAP_DEFAULT);
    TEST_ASSERT(called_user_failed_hook != false);

    called_user_failed_hook = false;
    ptr = heap_caps_realloc(ptr, stupid_allocation_size, MALLOC_CAP_DEFAULT);
    TEST_ASSERT(called_user_failed_hook != false);

    called_user_failed_hook = false;
    ptr = heap_caps_aligned_alloc(0x200, stupid_allocation_size, MALLOC_CAP_DEFAULT);
    TEST_ASSERT(called_user_failed_hook != false);

    (void)ptr;
}

TEST_CASE("allocation with invalid capability should also trigger the alloc failed hook", "[heap]")
{
    const size_t allocation_size = 64;
    const uint32_t invalid_cap = MALLOC_CAP_INVALID;

    TEST_ASSERT(heap_caps_register_failed_alloc_callback(heap_caps_alloc_failed_hook) == ESP_OK);

    called_user_failed_hook = false;
    void *ptr = heap_caps_malloc(allocation_size, invalid_cap);
    TEST_ASSERT(called_user_failed_hook != false);

    called_user_failed_hook = false;
    ptr = heap_caps_realloc(ptr, allocation_size, invalid_cap);
    TEST_ASSERT(called_user_failed_hook != false);

    called_user_failed_hook = false;
    ptr = heap_caps_aligned_alloc(0x200, allocation_size, invalid_cap);
    TEST_ASSERT(called_user_failed_hook != false);

    (void)ptr;
}

#ifdef CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP
/**
 * In MR 16031, the priority of RTC memory has been adjusted to the lowest.
 * RTC memory will not be consumed a lot during the startup process.
 */
TEST_CASE("RTC memory shoule be lowest priority and its free size should be big enough", "[heap]")
{
    const size_t allocation_size = 1024 * 4;
    void *ptr = NULL;
    size_t free_size = 0;

    ptr = heap_caps_malloc(allocation_size, MALLOC_CAP_DEFAULT);
    TEST_ASSERT_NOT_NULL(ptr);
    TEST_ASSERT(!esp_ptr_in_rtc_dram_fast(ptr));

    free_size = heap_caps_get_free_size(MALLOC_CAP_RTCRAM);
    TEST_ASSERT_GREATER_OR_EQUAL(1024 * 4, free_size);

    free(ptr);
}
#endif

TEST_CASE("test memory protection features", "[heap][mem_prot]")
{
    // try to allocate memory in IRAM and check that if memory protection is active,
    // no memory is being allocated
    uint32_t *iram_ptr = heap_caps_malloc(4, MALLOC_CAP_EXEC);

#ifndef CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
    // System memory protection not active, check that iram_ptr is not null
    // Check that iram_ptr is in IRAM
    TEST_ASSERT_NOT_NULL(iram_ptr);
    TEST_ASSERT(true == esp_ptr_in_iram(iram_ptr));

    // free the memory
    heap_caps_free(iram_ptr);
#else
    // System memory protection is active, DIRAM seen as DRAM, iram_ptr should be null
    TEST_ASSERT_NULL(iram_ptr);
#endif // CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
}
