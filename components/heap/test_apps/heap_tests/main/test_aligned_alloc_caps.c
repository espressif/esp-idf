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
#include <stdlib.h>
#include <sys/param.h>
#include <string.h>
#include <malloc.h>
#include <inttypes.h>

TEST_CASE("Capabilities aligned allocator test", "[heap]")
{
    uint32_t alignments = 0;

    printf("[ALIGNED_ALLOC] Allocating from default CAP: \n");

    for(;alignments <= 1024; alignments++) {
        uint8_t *buf = (uint8_t *)memalign(alignments, (alignments + 137));
        if(((alignments & (alignments - 1)) != 0) || (!alignments)) {
            TEST_ASSERT( buf == NULL );
            //printf("[ALIGNED_ALLOC] alignment: %"PRIu32" is not a power of two, don't allow allocation \n", aligments);
        } else {
            TEST_ASSERT( buf != NULL );
            printf("[ALIGNED_ALLOC] alignment required: %"PRIu32" \n", alignments);
            printf("[ALIGNED_ALLOC] address of allocated memory: %p \n\n", (void *)buf);
            //Address of obtained block must be aligned with selected value
            TEST_ASSERT(((intptr_t)buf & (alignments - 1)) == 0);

            //Write some data, if it corrupts memory probably the heap
            //canary verification will fail:
            memset(buf, 0xA5, (alignments + 137));

            free(buf);
        }
    }

    //Alloc from a non permitted area:
    uint32_t *not_permitted_buf = (uint32_t *)heap_caps_aligned_alloc(alignments, (alignments + 137), MALLOC_CAP_EXEC | MALLOC_CAP_32BIT);
    TEST_ASSERT( not_permitted_buf == NULL );

#if CONFIG_SPIRAM
    alignments = 0;
    printf("[ALIGNED_ALLOC] Allocating from external memory: \n");

    for(;alignments <= 1024 * 512; alignments++) {
        //Now try to take aligned memory from IRAM:
        uint8_t *buf = (uint8_t *)heap_caps_aligned_alloc(alignments, 10*1024, MALLOC_CAP_SPIRAM);
        if(((alignments & (alignments - 1)) != 0) || (!alignments)) {
            TEST_ASSERT( buf == NULL );
            //printf("[ALIGNED_ALLOC] alignment: %"PRIu32" is not a power of two, don't allow allocation \n", aligments);
        } else {
            TEST_ASSERT( buf != NULL );
            printf("[ALIGNED_ALLOC] alignment required: %"PRIu32" \n", alignments);
            printf("[ALIGNED_ALLOC] address of allocated memory: %p \n\n", (void *)buf);
            //Address of obtained block must be aligned with selected value
            TEST_ASSERT(((intptr_t)buf & (alignments - 1)) == 0);

            //Write some data, if it corrupts memory probably the heap
            //canary verification will fail:
            memset(buf, 0xA5, (10*1024));
            heap_caps_free(buf);
        }
    }
#endif

}

TEST_CASE("Capabilities aligned calloc test", "[heap]")
{
    uint32_t alignments = 0;

    printf("[ALIGNED_ALLOC] Allocating from default CAP: \n");

    for(;alignments <= 1024; alignments++) {
        uint8_t *buf = (uint8_t *)heap_caps_aligned_calloc(alignments, 1, (alignments + 137), MALLOC_CAP_DEFAULT);
        if(((alignments & (alignments - 1)) != 0) || (!alignments)) {
            TEST_ASSERT( buf == NULL );
            //printf("[ALIGNED_ALLOC] alignment: %"PRIu32" is not a power of two, don't allow allocation \n", aligments);
        } else {
            TEST_ASSERT( buf != NULL );
            printf("[ALIGNED_ALLOC] alignment required: %"PRIu32" \n", alignments);
            printf("[ALIGNED_ALLOC] address of allocated memory: %p \n\n", (void *)buf);
            //Address of obtained block must be aligned with selected value
            TEST_ASSERT(((intptr_t)buf & (alignments - 1)) == 0);

            //Write some data, if it corrupts memory probably the heap
            //canary verification will fail:
            memset(buf, 0xA5, (alignments + 137));

            heap_caps_free(buf);
        }
    }

    //Check if memory is initialized with zero:
    uint8_t byte_array[1024];
    memset(&byte_array, 0, sizeof(byte_array));
    uint8_t *buf = (uint8_t *)heap_caps_aligned_calloc(1024, 1, 1024, MALLOC_CAP_DEFAULT);
    TEST_ASSERT(memcmp(byte_array, buf, sizeof(byte_array)) == 0);
    heap_caps_free(buf);

    //Same size, but different chunk:
    buf = (uint8_t *)heap_caps_aligned_calloc(1024, 1024, 1, MALLOC_CAP_DEFAULT);
    TEST_ASSERT(memcmp(byte_array, buf, sizeof(byte_array)) == 0);
    heap_caps_free(buf);

    //Alloc from a non permitted area:
    uint32_t *not_permitted_buf = (uint32_t *)heap_caps_aligned_calloc(alignments, 1, (alignments + 137), MALLOC_CAP_32BIT);
    TEST_ASSERT( not_permitted_buf == NULL );

#if CONFIG_SPIRAM
    alignments = 0;
    printf("[ALIGNED_ALLOC] Allocating from external memory: \n");
    for(;alignments <= 1024 * 512; alignments++) {
        //Now try to take aligned memory from IRAM:
        uint8_t *buf = (uint8_t *)(uint8_t *)heap_caps_aligned_calloc(alignments, 1, 10*1024, MALLOC_CAP_SPIRAM);
        if(((alignments & (alignments - 1)) != 0) || (!alignments)) {
            TEST_ASSERT( buf == NULL );
            //printf("[ALIGNED_ALLOC] alignment: %"PRIu32" is not a power of two, don't allow allocation \n", aligments);
        } else {
            TEST_ASSERT( buf != NULL );
            printf("[ALIGNED_ALLOC] alignment required: %"PRIu32" \n", alignments);
            printf("[ALIGNED_ALLOC] address of allocated memory: %p \n\n", (void *)buf);
            //Address of obtained block must be aligned with selected value
            TEST_ASSERT(((intptr_t)buf & (alignments - 1)) == 0);

            //Write some data, if it corrupts memory probably the heap
            //canary verification will fail:
            memset(buf, 0xA5, (10*1024));
            heap_caps_free(buf);
        }
    }
#endif

}

TEST_CASE("aligned_alloc(0) should return a NULL pointer", "[heap]")
{
    void *p;
    p = heap_caps_aligned_alloc(32, 0, MALLOC_CAP_DEFAULT);
    TEST_ASSERT(p == NULL);
}
