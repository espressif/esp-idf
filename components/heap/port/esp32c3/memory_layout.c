/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdlib.h>
#include "esp_attr.h"
#include "sdkconfig.h"
#include "soc/soc.h"
#include "heap_memory_layout.h"
#include "esp_heap_caps.h"

/**
 * @brief Memory type descriptors. These describe the capabilities of a type of memory in the SoC.
 * Each type of memory map consists of one or more regions in the address space.
 * Each type contains an array of prioritized capabilities.
 * Types with later entries are only taken if earlier ones can't fulfill the memory request.
 *
 * - For a normal malloc (MALLOC_CAP_DEFAULT), give away the DRAM-only memory first, then pass off any dual-use IRAM regions, finally eat into the application memory.
 * - For a malloc where 32-bit-aligned-only access is okay, first allocate IRAM, then DRAM, finally application IRAM.
 * - Application mallocs (PIDx) will allocate IRAM first, if possible, then DRAM.
 * - Most other malloc caps only fit in one region anyway.
 *
 */

/* Index of memory in `soc_memory_types[]` */
enum {
    SOC_MEMORY_TYPE_DRAM        = 0,
    SOC_MEMORY_TYPE_STACK_DRAM  = 1,
    SOC_MEMORY_TYPE_DIRAM       = 2,
    SOC_MEMORY_TYPE_RTCRAM      = 3,
    SOC_MEMORY_TYPE_NUM,
};

const soc_memory_type_desc_t soc_memory_types[SOC_MEMORY_TYPE_NUM] = {
    // Type 0: DRAM
    [SOC_MEMORY_TYPE_DRAM] = { "DRAM", { MALLOC_CAP_8BIT | MALLOC_CAP_DEFAULT, MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_32BIT, 0 }, false, false},
    // Type 1: DRAM used for startup stacks
    [SOC_MEMORY_TYPE_STACK_DRAM] = { "STACK/DRAM", { MALLOC_CAP_8BIT | MALLOC_CAP_DEFAULT, MALLOC_CAP_EXEC | MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_32BIT, MALLOC_CAP_RETENTION }, false, true},
    // Type 2: DRAM which has an alias on the I-port
    [SOC_MEMORY_TYPE_DIRAM] = { "D/IRAM", { 0, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL | MALLOC_CAP_DEFAULT, MALLOC_CAP_32BIT | MALLOC_CAP_EXEC }, true, false},
    // Type 3: RTCRAM
    [SOC_MEMORY_TYPE_RTCRAM] = { "RTCRAM", { MALLOC_CAP_RTCRAM, MALLOC_CAP_8BIT | MALLOC_CAP_DEFAULT, MALLOC_CAP_INTERNAL | MALLOC_CAP_32BIT }, false, false},
};

#ifdef CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
#define SOC_MEMORY_TYPE_DEFAULT SOC_MEMORY_TYPE_DRAM
#else
#define SOC_MEMORY_TYPE_DEFAULT SOC_MEMORY_TYPE_DIRAM
#endif

const size_t soc_memory_type_count = sizeof(soc_memory_types) / sizeof(soc_memory_type_desc_t);

/**
 * @brief Region descriptors. These describe all regions of memory available, and map them to a type in the above type.
 *
 * @note Because of requirements in the coalescing code which merges adjacent regions,
 *       this list should always be sorted from low to high by start address.
 *
 */

/**
 * Register the shared buffer area of the last memory block into the heap during heap initialization
 */
#define APP_USABLE_DRAM_END           (SOC_ROM_STACK_START - SOC_ROM_STACK_SIZE)

const soc_memory_region_t soc_memory_regions[] = {
    { 0x3FC80000,           0x20000,                                   SOC_MEMORY_TYPE_DEFAULT,     0x40380000}, //D/IRAM level1, can be used as trace memory
    { 0x3FCA0000,           0x20000,                                   SOC_MEMORY_TYPE_DEFAULT,     0x403A0000}, //D/IRAM level2, can be used as trace memory
    { 0x3FCC0000,           (APP_USABLE_DRAM_END-0x3FCC0000),          SOC_MEMORY_TYPE_DEFAULT,     0x403C0000}, //D/IRAM level3, can be used as trace memory
    { APP_USABLE_DRAM_END,  (SOC_DIRAM_DRAM_HIGH-APP_USABLE_DRAM_END), SOC_MEMORY_TYPE_STACK_DRAM,  MAP_DRAM_TO_IRAM(APP_USABLE_DRAM_END)}, //D/IRAM level3, can be used as trace memory (ROM reserved area)
#ifdef CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP
    { 0x50000000, 0x2000,  SOC_MEMORY_TYPE_RTCRAM,      0},          //Fast RTC memory
#endif
};

const size_t soc_memory_region_count = sizeof(soc_memory_regions) / sizeof(soc_memory_region_t);


extern int _data_start, _heap_start, _iram_start, _iram_end, _rtc_force_slow_end;

/**
 * Reserved memory regions.
 * These are removed from the soc_memory_regions array when heaps are created.
 *
 */

// Static data region. DRAM used by data+bss and possibly rodata
SOC_RESERVE_MEMORY_REGION((intptr_t)&_data_start, (intptr_t)&_heap_start, dram_data);

// Target has a big D/IRAM region, the part used by code is reserved
// The address of the D/I bus are in the same order, directly shift IRAM address to get reserved DRAM address
#define I_D_OFFSET (SOC_DIRAM_IRAM_LOW - SOC_DIRAM_DRAM_LOW)
SOC_RESERVE_MEMORY_REGION((intptr_t)&_iram_start - I_D_OFFSET, (intptr_t)&_iram_end - I_D_OFFSET, iram_code);

#ifdef CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP
/* We use _rtc_force_slow_end not _rtc_noinit_end here, as rtc "fast" memory ends up in RTC SLOW
   region on C3, no differentiation. And _rtc_force_slow_end is the end of all the static RTC sections.
*/
SOC_RESERVE_MEMORY_REGION(SOC_RTC_DRAM_LOW, (intptr_t)&_rtc_force_slow_end, rtcram_data);
#endif
