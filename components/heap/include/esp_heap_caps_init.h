/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_err.h"
#include "esp_heap_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the capability-aware heap allocator.
 *
 * This is called once in the IDF startup code. Do not call it
 * at other times.
 */
void heap_caps_init(void);

/**
 * @brief Enable heap(s) in memory regions where the startup stacks are located.
 *
 * On startup, the pro/app CPUs have a certain memory region they use as stack, so we
 * cannot do allocations in the regions these stack frames are. When FreeRTOS is
 * completely started, they do not use that memory anymore and heap(s) there can
 * be enabled.
 */
void heap_caps_enable_nonos_stack_heaps(void);

/**
 * @brief Add a region of memory to the collection of heaps at runtime.
 *
 * Most memory regions are defined in soc_memory_layout.c for the SoC,
 * and are registered via heap_caps_init(). Some regions can't be used
 * immediately and are later enabled via heap_caps_enable_nonos_stack_heaps().
 *
 * Call this function to add a region of memory to the heap at some later time.
 *
 * This function does not consider any of the "reserved" regions or other data in soc_memory_layout, caller needs to
 * consider this themselves.
 *
 * All memory within the region specified by start & end parameters must be otherwise unused.
 *
 * The capabilities of the newly registered memory will be determined by the start address, as looked up in the regions
 * specified in soc_memory_layout.c.
 *
 * Use heap_caps_add_region_with_caps() to register a region with custom capabilities.
 *
 * @note Please refer to following example for memory regions allowed for addition to heap based on an existing region
 * (address range for demonstration purpose only):
 @verbatim
       Existing region: 0x1000 <-> 0x3000
       New region:      0x1000 <-> 0x3000 (Allowed)
       New region:      0x1000 <-> 0x2000 (Allowed)
       New region:      0x0000 <-> 0x1000 (Allowed)
       New region:      0x3000 <-> 0x4000 (Allowed)
       New region:      0x0000 <-> 0x2000 (NOT Allowed)
       New region:      0x0000 <-> 0x4000 (NOT Allowed)
       New region:      0x1000 <-> 0x4000 (NOT Allowed)
       New region:      0x2000 <-> 0x4000 (NOT Allowed)
 @endverbatim
 *
 * @param start Start address of new region.
 * @param end End address of new region.
 *
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG if a parameter is invalid, ESP_ERR_NOT_FOUND if the
 * specified start address doesn't reside in a known region, or any error returned by heap_caps_add_region_with_caps().
 */
esp_err_t heap_caps_add_region(intptr_t start, intptr_t end);


/**
 * @brief Add a region of memory to the collection of heaps at runtime, with custom capabilities.
 *
 * Similar to heap_caps_add_region(), only custom memory capabilities are specified by the caller.
 *
 * @note Please refer to following example for memory regions allowed for addition to heap based on an existing region
 * (address range for demonstration purpose only):
 @verbatim
       Existing region: 0x1000 <-> 0x3000
       New region:      0x1000 <-> 0x3000 (Allowed)
       New region:      0x1000 <-> 0x2000 (Allowed)
       New region:      0x0000 <-> 0x1000 (Allowed)
       New region:      0x3000 <-> 0x4000 (Allowed)
       New region:      0x0000 <-> 0x2000 (NOT Allowed)
       New region:      0x0000 <-> 0x4000 (NOT Allowed)
       New region:      0x1000 <-> 0x4000 (NOT Allowed)
       New region:      0x2000 <-> 0x4000 (NOT Allowed)
 @endverbatim
 *
 * @param caps Ordered array of capability masks for the new region, in order of priority. Must have length
 * SOC_MEMORY_TYPE_NO_PRIOS. Does not need to remain valid after the call returns.
 * @param start Start address of new region.
 * @param end End address of new region.
 *
 * @return
 *         - ESP_OK on success
 *         - ESP_ERR_INVALID_ARG if a parameter is invalid
 *         - ESP_ERR_NO_MEM if no memory to register new heap.
 *         - ESP_ERR_INVALID_SIZE if the memory region is too small to fit a heap
 *         - ESP_FAIL if region overlaps the start and/or end of an existing region
 */
esp_err_t heap_caps_add_region_with_caps(const uint32_t caps[], intptr_t start, intptr_t end);


#ifdef __cplusplus
}
#endif
