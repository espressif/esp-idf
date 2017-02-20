/*
 Tests for the capabilities-based memory allocator.
*/

#include <esp_types.h>
#include <stdio.h>
#include "unity.h"
#include "rom/ets_sys.h"
#include "esp_heap_alloc_caps.h"
#include <stdlib.h>


TEST_CASE("Capabilities allocator test", "[esp32]")
{
    char *m1, *m2[10];
    int x;
    size_t free8start, free32start, free8, free32;
    free8start=xPortGetFreeHeapSizeCaps(MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    free32start=xPortGetFreeHeapSizeCaps(MALLOC_CAP_32BIT|MALLOC_CAP_INTERNAL);
    printf("Free 8bit-capable memory: %dK, 32-bit capable memory %dK\n", free8start, free32start);
    TEST_ASSERT(free32start>free8start);
    printf("Allocating 10K of internal 8-bit capable RAM\n");
    m1=pvPortMallocCaps(10*1024, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    printf("--> %p\n", m1);
    free8=xPortGetFreeHeapSizeCaps(MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    free32=xPortGetFreeHeapSizeCaps(MALLOC_CAP_32BIT|MALLOC_CAP_INTERNAL);
    printf("Free 8bit-capable memory: %dK, 32-bit capable memory %dK\n", free8, free32);
    //Both should have gone down by 10K; 8bit capable ram is also 32-bit capable
    TEST_ASSERT(free8<(free8start-10*1024));
    TEST_ASSERT(free32<(free32start-10*1024));
    //Assume we got DRAM back
    TEST_ASSERT((((int)m1)&0xFF000000)==0x3F000000);
    free(m1);
    printf("Freeing; allocating 10K of 32K-capable RAM\n");
    m1=pvPortMallocCaps(10*1024, MALLOC_CAP_32BIT|MALLOC_CAP_INTERNAL);
    printf("--> %p\n", m1);
    free8=xPortGetFreeHeapSizeCaps(MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    free32=xPortGetFreeHeapSizeCaps(MALLOC_CAP_32BIT|MALLOC_CAP_INTERNAL);
    printf("Free 8bit-capable memory: %dK, 32-bit capable memory %dK\n", free8, free32);
    //Only 32-bit should have gone down by 10K: 32-bit isn't necessarily 8bit capable
    TEST_ASSERT(free32<(free32start-10*1024));
    TEST_ASSERT(free8==free8start);
    //Assume we got IRAM back
    TEST_ASSERT((((int)m1)&0xFF000000)==0x40000000);
    free(m1);
    printf("Allocating impossible caps\n");
    m1=pvPortMallocCaps(10*1024, MALLOC_CAP_8BIT|MALLOC_CAP_EXEC|MALLOC_CAP_INTERNAL);
    printf("--> %p\n", m1);
    TEST_ASSERT(m1==NULL);
    printf("Testing changeover iram -> dram");
    for (x=0; x<10; x++) {
        m2[x]=pvPortMallocCaps(10*1024, MALLOC_CAP_32BIT|MALLOC_CAP_INTERNAL);
        printf("--> %p\n", m2[x]);
    }
    TEST_ASSERT((((int)m2[0])&0xFF000000)==0x40000000);
    TEST_ASSERT((((int)m2[9])&0xFF000000)==0x3F000000);
    printf("Test if allocating executable code still gives IRAM, even with dedicated IRAM region depleted\n");
    m1=pvPortMallocCaps(10*1024, MALLOC_CAP_EXEC|MALLOC_CAP_INTERNAL);
    printf("--> %p\n", m1);
    TEST_ASSERT((((int)m1)&0xFF000000)==0x40000000);
    free(m1);
    for (x=0; x<10; x++) free(m2[x]);

#if CONFIG_MEMMAP_SPIRAM_ENABLE
    printf("Testing if small malloc allocates internal memory");
    m1=malloc(CONFIG_MEMMAP_SPIRAM_ALLOC_LIMIT_INTERNAL/2);
    TEST_ASSERT((((int)m1)&0xFFE00000)==0x3FA00000);
    free(m1);
    printf("Testing if large malloc allocates external memory");
    m1=malloc(CONFIG_MEMMAP_SPIRAM_ALLOC_LIMIT_INTERNAL*2);
    TEST_ASSERT((((int)m1)&0xFFE00000)==0x3F800000);
    free(m1);

    printf("Testing explicit internal memory alloc");
    m1=pvPortMallocCaps(10*1024, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    TEST_ASSERT((((int)m1)&0xFFE00000)==0x3FA00000);
    free(m1);
    printf("Testing explicit external memory alloc");
    m1=pvPortMallocCaps(10*1024, MALLOC_CAP_8BIT|MALLOC_CAP_SPIRAM);
    TEST_ASSERT((((int)m1)&0xFFE00000)==0x3F800000);
    free(m1);
#else
    printf("Not testing PSRAM because PSRAM disabled in menuconfig.\n");
#endif
    printf("Done.\n");
}
