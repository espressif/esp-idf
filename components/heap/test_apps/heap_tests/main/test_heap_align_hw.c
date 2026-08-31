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
#include "soc/soc_caps.h"

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
	int alignment = (expected_alignment > 1) ? expected_alignment : 1;

	void *mem[TEST_ALLOC_COUNT];
	size_t size[TEST_ALLOC_COUNT];

	//First, check if we can allocate memory with these caps anyway.
	if ((caps & MALLOC_CAP_SPIRAM) && heap_caps_get_total_size(MALLOC_CAP_SPIRAM) == 0) {
		printf("skipping caps 0x%" PRIx32 ": no PSRAM heap registered\n", caps);
		return true;
	}
	void *tst=heap_caps_malloc(1, caps);
	if (!tst) {
		printf("caps 0x%" PRIx32 ": cannot allocate 1 byte with alignment %d\n", caps, alignment);
		return false;
	}
	free(tst);

	//Step 1: generate sizes and allocate memory.
	for (int i=0; i<TEST_ALLOC_COUNT; i++) {
		size[i]=rand()&128;
		mem[i]=heap_caps_malloc(size[i], caps);
	}

	// Step 2: check alignment and fill up memory up to the aligned size
	// (which should succeed as we expect to get an integer amount of cache lines)
	for (int i=0; i<TEST_ALLOC_COUNT; i++) {
		if (size[i] == 0) {
			continue;
		}
		if (!mem[i]) {
			printf("caps 0x%" PRIx32 ": allocation %d of %u bytes failed\n", caps, i, (unsigned)size[i]);
			ret = false;
			continue;
		}
		intptr_t off=(intptr_t)mem[i];
		if (off & (alignment - 1)) {
			printf("caps 0x%" PRIx32 ": %p is not aligned to %d\n", caps, mem[i], alignment);
			ret = false;
		}
		size_t size_aligned_up = (size[i] + alignment - 1) & (~(alignment - 1));
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
	int int_cache_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);
	int ext_cache_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_DATA);

	TEST_ASSERT_TRUE_MESSAGE(test_alignment(MALLOC_CAP_DMA, int_cache_size), "MALLOC_CAP_DMA");
	TEST_ASSERT_TRUE_MESSAGE(test_alignment(MALLOC_CAP_DMA_DESC_AHB, int_cache_size), "MALLOC_CAP_DMA_DESC_AHB");
#if SOC_HAS(AXI_GDMA)
	int axi_desc_alignment = int_cache_size > 8 ? int_cache_size : 8;
#else
	int axi_desc_alignment = int_cache_size;
#endif
	TEST_ASSERT_TRUE_MESSAGE(test_alignment(MALLOC_CAP_DMA_DESC_AXI, axi_desc_alignment), "MALLOC_CAP_DMA_DESC_AXI");
	TEST_ASSERT_TRUE_MESSAGE(test_alignment(MALLOC_CAP_DMA|MALLOC_CAP_SPIRAM, ext_cache_size), "MALLOC_CAP_DMA|MALLOC_CAP_SPIRAM");
}
