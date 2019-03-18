/*
 Generic test for malloc/free
*/

#include <esp_types.h>
#include <stdio.h>
#include "rom/ets_sys.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "unity.h"
#include "soc/uart_reg.h"
#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include "esp_heap_caps.h"

#include "esp_panic.h"
#include "sdkconfig.h"


static int **allocatedMem;
static int noAllocated;


static int tryAllocMem() {
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


static void tryAllocMemFree() {
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
    TEST_ASSERT_NULL(test_malloc_wrapper(xPortGetFreeHeapSize() - 1));
}

