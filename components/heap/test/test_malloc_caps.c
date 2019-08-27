/*
 Tests for the capabilities-based memory allocator.
*/

#include <esp_types.h>
#include <stdio.h>
#include "unity.h"
#include "esp_attr.h"
#include "esp_heap_caps.h"
#include "esp_spi_flash.h"
#include <stdlib.h>
#include <sys/param.h>

TEST_CASE_ESP32("Capabilities allocator test", "[heap]")
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
    TEST_ASSERT(free32start>free8start);

    printf("Allocating 10K of 8-bit capable RAM\n");
    m1= heap_caps_malloc(10*1024, MALLOC_CAP_8BIT);
    printf("--> %p\n", m1);
    free8 = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    free32 = heap_caps_get_free_size(MALLOC_CAP_32BIT);
    printf("Free 8bit-capable memory (both reduced): %dK, 32-bit capable memory %dK\n", free8, free32);
    //Both should have gone down by 10K; 8bit capable ram is also 32-bit capable
    TEST_ASSERT(free8<(free8start-10*1024));
    TEST_ASSERT(free32<(free32start-10*1024));
    //Assume we got DRAM back
    TEST_ASSERT((((int)m1)&0xFF000000)==0x3F000000);
    free(m1);

    //The goal here is to allocate from IRAM. Since there is no external IRAM (yet)
    //the following gives size of IRAM-only (not D/IRAM) memory.
    size_t free_iram = heap_caps_get_free_size(MALLOC_CAP_INTERNAL) -
                           heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    size_t alloc32 = MIN(free_iram / 2, 10*1024) & (~3);
    printf("Freeing; allocating %u bytes of 32K-capable RAM\n", alloc32);
    m1 = heap_caps_malloc(alloc32, MALLOC_CAP_32BIT);
    printf("--> %p\n", m1);
    //Check that we got IRAM back
    TEST_ASSERT((((int)m1)&0xFF000000)==0x40000000);
    free8 = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    free32 = heap_caps_get_free_size(MALLOC_CAP_32BIT);
    printf("Free 8bit-capable memory (after 32-bit): %dK, 32-bit capable memory %dK\n", free8, free32);
    //Only 32-bit should have gone down by alloc32: 32-bit isn't necessarily 8bit capable
    TEST_ASSERT(free32<(free32start-alloc32));
    TEST_ASSERT(free8==free8start);
    free(m1);

    printf("Allocating impossible caps\n");
    m1= heap_caps_malloc(10*1024, MALLOC_CAP_8BIT|MALLOC_CAP_EXEC);
    printf("--> %p\n", m1);
    TEST_ASSERT(m1==NULL);
    printf("Testing changeover iram -> dram");
    // priorities will exhaust IRAM first, then start allocating from DRAM
    for (x=0; x<10; x++) {
        m2[x]= heap_caps_malloc(alloc32, MALLOC_CAP_32BIT);
        printf("--> %p\n", m2[x]);
    }
    TEST_ASSERT((((int)m2[0])&0xFF000000)==0x40000000);
    TEST_ASSERT((((int)m2[9])&0xFF000000)==0x3F000000);
    printf("Test if allocating executable code still gives IRAM, even with dedicated IRAM region depleted\n");
    // (the allocation should come from D/IRAM)
    free_iram = heap_caps_get_free_size(MALLOC_CAP_EXEC);
    m1= heap_caps_malloc(MIN(free_iram / 2, 10*1024), MALLOC_CAP_EXEC);
    printf("--> %p\n", m1);
    TEST_ASSERT((((int)m1)&0xFF000000)==0x40000000);
    free(m1);
    for (x=0; x<10; x++) free(m2[x]);
    printf("Done.\n");
}

TEST_CASE("heap_caps metadata test", "[heap]")
{
    /* need to print something as first printf allocates some heap */
    printf("heap_caps metadata test\n");
    heap_caps_print_heap_info(MALLOC_CAP_8BIT);
    heap_caps_print_heap_info(MALLOC_CAP_32BIT);

    multi_heap_info_t original;
    heap_caps_get_info(&original, MALLOC_CAP_8BIT);

    void *b = heap_caps_malloc(original.largest_free_block, MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(b);

    printf("After allocating %d bytes:\n", original.largest_free_block);
    heap_caps_print_heap_info(MALLOC_CAP_8BIT);

    multi_heap_info_t after;
    heap_caps_get_info(&after, MALLOC_CAP_8BIT);
    TEST_ASSERT(after.largest_free_block < original.largest_free_block);
    TEST_ASSERT(after.total_free_bytes < original.total_free_bytes);

    free(b);
    heap_caps_get_info(&after, MALLOC_CAP_8BIT);
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
static IRAM_ATTR __attribute__((noinline)) bool iram_malloc_test(void)
{
    spi_flash_guard_get()->start(); // Disables flash cache

    bool result = true;
    void *x = heap_caps_malloc(64, MALLOC_CAP_32BIT);
    result = result && (x != NULL);
    void *y = heap_caps_realloc(x, 32, MALLOC_CAP_32BIT);
    result = result && (y != NULL);
    heap_caps_free(y);

    spi_flash_guard_get()->end(); // Re-enables flash cache

    return result;
}

TEST_CASE("heap_caps_xxx functions work with flash cache disabled", "[heap]")
{
    TEST_ASSERT( iram_malloc_test() );
}
