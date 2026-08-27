/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/*
 Generic test for malloc/free
*/

#include <esp_types.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "unity.h"
#include "esp_heap_caps.h"
#include "esp_system.h"

#include "sdkconfig.h"


static int **allocatedMem;
static int noAllocated;

static int tryAllocMem(void) {
    int i, j;
    const int allocateMaxK=1024*5; //try to allocate a max of 5MiB

    allocatedMem=malloc(sizeof(int *)*allocateMaxK);
    if (!allocatedMem) return 0;

    for (i=0; i<allocateMaxK; i++) {
        allocatedMem[i]=malloc(1024);
        if (allocatedMem[i]==NULL) break;
        for (j=0; j<1024/4; j++) allocatedMem[i][j]=(0xdeadbeef);
    }
    noAllocated=i;
    return i;
}


static void tryAllocMemFree(void) {
    int i, j;
    for (i=0; i<noAllocated; i++) {
        for (j=0; j<1024/4; j++) {
            TEST_ASSERT(allocatedMem[i][j]==(0xdeadbeef));
        }
        free(allocatedMem[i]);
    }
    free(allocatedMem);
}


TEST_CASE("Malloc/overwrite, then free all available DRAM", "[heap]")
{
    int m1=0, m2=0;
    m1=tryAllocMem();
    tryAllocMemFree();
    m2=tryAllocMem();
    tryAllocMemFree();
    printf("Could allocate %dK on first try, %dK on 2nd try.\n", m1, m2);
    TEST_ASSERT(m1==m2);
}

#if CONFIG_SPIRAM_USE_MALLOC

#if (CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL > 1024)
TEST_CASE("Check if reserved DMA pool still can allocate even when malloc()'ed memory is exhausted", "[heap]")
{
    char** dmaMem=malloc(sizeof(char*)*512);
    assert(dmaMem);
    int m=tryAllocMem();
    int i=0;
    for (i=0; i<512; i++) {
        dmaMem[i]=heap_caps_malloc(1024, MALLOC_CAP_DMA);
        if (dmaMem[i]==NULL) break;
    }
    for (int j=0; j<i; j++) free(dmaMem[j]);
    free(dmaMem);
    tryAllocMemFree();
    printf("Could allocate %dK of DMA memory after allocating all of %dK of normal memory.\n", i, m);
    TEST_ASSERT(i);
}
#endif

#endif


/* As you see, we are desperately trying to outsmart the compiler, so that it
 * doesn't warn about oversized allocations in the next two unit tests.
 * To be removed when we switch to GCC 8.2 and add
 * -Wno-alloc-size-larger-than=PTRDIFF_MAX to CFLAGS for this file.
 */
void* (*g_test_malloc_ptr)(size_t) = &malloc;
void* (*g_test_calloc_ptr)(size_t, size_t) = &calloc;

void* test_malloc_wrapper(size_t size)
{
    return (*g_test_malloc_ptr)(size);
}

void* test_calloc_wrapper(size_t count, size_t size)
{
    return (*g_test_calloc_ptr)(count, size);
}

TEST_CASE("alloc overflows should all fail", "[heap]")
{
    /* allocates 8 bytes if size_t overflows */
    TEST_ASSERT_NULL(test_calloc_wrapper(SIZE_MAX / 2 + 4, 2));

    /* will overflow if any poisoning is enabled
       (should fail for sensible OOM reasons, otherwise) */
    TEST_ASSERT_NULL(test_malloc_wrapper(SIZE_MAX - 1));
    TEST_ASSERT_NULL(test_calloc_wrapper(SIZE_MAX - 1, 1));

    /* will overflow when the size is rounded up to word align it */
    TEST_ASSERT_NULL(heap_caps_malloc(SIZE_MAX-1, MALLOC_CAP_32BIT));

    TEST_ASSERT_NULL(heap_caps_malloc(SIZE_MAX-1, MALLOC_CAP_EXEC));
}

TEST_CASE("unreasonable allocs should all fail", "[heap]")
{
    TEST_ASSERT_NULL(test_calloc_wrapper(16, 1024*1024));
    TEST_ASSERT_NULL(test_malloc_wrapper(16*1024*1024));
    TEST_ASSERT_NULL(test_malloc_wrapper(SIZE_MAX / 2));
    TEST_ASSERT_NULL(test_malloc_wrapper(SIZE_MAX - 256));
    TEST_ASSERT_NULL(test_malloc_wrapper(esp_get_free_heap_size() - 1));
}

TEST_CASE("malloc(0) should return a NULL pointer", "[heap]")
{
    void *p;
    p = malloc(0);
    TEST_ASSERT(p == NULL);
}

static bool failure_occured = false;

static void test_alloc_failure_callback(size_t size, uint32_t caps, const char * function_name)
{
    failure_occured = true;
}

TEST_CASE("malloc/calloc(0) should not call failure callback", "[heap]")
{
    void* ptr = NULL;
    esp_err_t ret = heap_caps_register_failed_alloc_callback(test_alloc_failure_callback);
    TEST_ASSERT(ret == ESP_OK);
    ptr = malloc(0);
    TEST_ASSERT_NULL(ptr);
    /* Check that our callback was NOT called */
    TEST_ASSERT_FALSE(failure_occured);
    /* Do the same thing for calloc */
    ptr = calloc(0, 0);
    TEST_ASSERT_NULL(ptr);
    TEST_ASSERT_FALSE(failure_occured);
}

TEST_CASE("test get allocated size", "[heap]")
{
    // random values to test, some are 4 bytes aligned, some are not
    const size_t alloc_sizes[] = { 1035, 1064, 1541 };
    const size_t iterations = sizeof(alloc_sizes) / sizeof(size_t);
    void *ptr_array[iterations];

    for (size_t i = 0; i < iterations; i++) {
        ptr_array[i] = heap_caps_malloc(alloc_sizes[i], MALLOC_CAP_DEFAULT);
        TEST_ASSERT_NOT_NULL(ptr_array[i]);

        // test that the heap_caps_get_allocated_size() returns the right number of bytes (aligned to 4 bytes
        // since the heap component aligns to 4 bytes)
        const size_t aligned_size = (alloc_sizes[i] + 3) & ~3;
        const size_t real_size = heap_caps_get_allocated_size(ptr_array[i]);
        printf("initial size: %d, requested size : %d, allocated size: %d\n", alloc_sizes[i], aligned_size, real_size);
        TEST_ASSERT_EQUAL(aligned_size, real_size);

        heap_caps_free(ptr_array[i]);
    }
}

#ifdef CONFIG_HEAP_USE_HOOKS
// provide the definition of alloc and free hooks
static const size_t alloc_size = 1234; // make this size atypical to be able to rely on it in the hook
static const size_t expected_calls = 2; // one call for malloc/calloc and one call for realloc
static uint32_t *alloc_ptr = NULL;
static bool test_success = false;
static size_t counter = 0;

static void reset_static_variables(void) {
    test_success = false;
    alloc_ptr = NULL;
    counter = 0;
}

void esp_heap_trace_alloc_hook(void* ptr, size_t size, uint32_t caps)
{
    if (size == alloc_size) {
        counter++;
        if (counter == expected_calls) {
            alloc_ptr = ptr;
        }
    }
}

void esp_heap_trace_free_hook(void* ptr)
{
    if (alloc_ptr == ptr && counter == expected_calls) {
        test_success = true;
    }
}

TEST_CASE("test allocation and free function hooks", "[heap]")
{
    // alloc, realloc and free memory, at the end of the test, test_success will be set
    // to true if both function hooks are called.
    uint32_t *ptr = heap_caps_malloc(alloc_size, MALLOC_CAP_DEFAULT);
    TEST_ASSERT_NOT_NULL(ptr);
    ptr = heap_caps_realloc(ptr, alloc_size, MALLOC_CAP_32BIT);
    heap_caps_free(ptr);

    TEST_ASSERT_TRUE(test_success);

    // re-init the static variables
    reset_static_variables();

    // calloc, realloc and free memory, at the end of the test, test_success will be set
    // to true if both function hooks are called.
    ptr = heap_caps_calloc(1, alloc_size, MALLOC_CAP_DEFAULT);
    TEST_ASSERT_NOT_NULL(ptr);
    ptr = heap_caps_realloc(ptr, alloc_size, MALLOC_CAP_32BIT);
    heap_caps_free(ptr);

    TEST_ASSERT_TRUE(test_success);

    // re-init the static variables
    reset_static_variables();

    // aligned alloc, realloc and aligned free memory, at the end of the test, test_success
    // will be set to true if both function hooks are called.
    ptr = heap_caps_aligned_alloc(0x200, alloc_size, MALLOC_CAP_DEFAULT);
    TEST_ASSERT_NOT_NULL(ptr);
    ptr = heap_caps_realloc(ptr, alloc_size, MALLOC_CAP_32BIT);
    heap_caps_free(ptr);

    TEST_ASSERT_TRUE(test_success);
}
#endif
