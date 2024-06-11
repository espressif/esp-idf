/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <sys/param.h>
#include <string.h>
#include "inttypes.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "unity.h"
#include "esp_private/esp_cache_private.h"
#include "esp_memory_utils.h"
#include "hal/cache_ll.h"
#include "hal/cache_hal.h"

TEST_CASE("test heap_caps_malloc_prefer for dma memory", "[hw-align]")
{
    void *ptr = NULL;
    ptr = heap_caps_malloc_prefer(40, 1, MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(ptr);
    TEST_ASSERT(esp_ptr_dma_capable(ptr));

    free(ptr);
}

TEST_CASE("test heap_caps_calloc_prefer for dma memory", "[hw-align]")
{
    void *ptr = NULL;
    ptr = heap_caps_calloc_prefer(40, 1, 1, MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(ptr);
    TEST_ASSERT(esp_ptr_dma_capable(ptr));

    free(ptr);
}

#include "esp_private/heap_align_hw.h"

#define TEST_ALLOC_COUNT 100
static bool test_alignment(uint32_t caps, int expected_alignment) {
	bool ret=true;

	void *mem[TEST_ALLOC_COUNT];
	size_t size[TEST_ALLOC_COUNT];

	//First, check if we can allocate memory with these caps anyway.
	void *tst=heap_caps_malloc(1, caps);
	if (!tst) return true;
	free(tst);

	//Step 1: generate sizes and allocate memory.
	for (int i=0; i<TEST_ALLOC_COUNT; i++) {
		size[i]=rand()&128;
		mem[i]=heap_caps_malloc(size[i], caps);
	}

	// Step 2: check alignment and fill up memory up to the aligned size
	// (which should succeed as we expect to get an integer amount of cache lines)
	for (int i=0; i<TEST_ALLOC_COUNT; i++) {
		intptr_t off=(intptr_t)mem[i];
		if (off&(expected_alignment-1)) ret=false;
		size_t size_aligned_up = (size[i] + expected_alignment - 1) & (~(expected_alignment - 1));
		memset(mem[i], 0xA5, size_aligned_up);
	}

	//Step 3: Free the memory again.
	//This should not lead to heap corruption (from the memset) being detected.
	for (int i=0; i<TEST_ALLOC_COUNT; i++) {
		free(mem[i]);
	}

	return ret;
}


TEST_CASE("test alignment for dma", "[hw-align]")
{
	int int_cache_size=cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);
	int ext_cache_size=cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);

	test_alignment(MALLOC_CAP_DMA, int_cache_size);
	test_alignment(MALLOC_CAP_DMA_DESC_AHB, int_cache_size);
	test_alignment(MALLOC_CAP_DMA_DESC_AXI, int_cache_size>8?int_cache_size:8);
	test_alignment(MALLOC_CAP_DMA|MALLOC_CAP_SPIRAM, ext_cache_size);
}
