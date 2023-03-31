/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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
    SOC_MEMORY_TYPE_STACK_DIRAM = 3,
    SOC_MEMORY_TYPE_RTCRAM      = 4,
    SOC_MEMORY_TYPE_NUM,
};

const soc_memory_type_desc_t soc_memory_types[SOC_MEMORY_TYPE_NUM] = {
    // Type 0: DRAM
    [SOC_MEMORY_TYPE_DRAM] = { "DRAM", { MALLOC_CAP_8BIT | MALLOC_CAP_DEFAULT, MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_32BIT, 0 }, false, false},
    // Type 1: DRAM used for startup stacks
    [SOC_MEMORY_TYPE_STACK_DRAM] = { "STACK/DRAM", { MALLOC_CAP_8BIT | MALLOC_CAP_DEFAULT, MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_32BIT, MALLOC_CAP_RETENTION }, false, true},
    // Type 2: DRAM which has an alias on the I-port
    [SOC_MEMORY_TYPE_DIRAM] = { "D/IRAM", { 0, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL | MALLOC_CAP_DEFAULT, MALLOC_CAP_32BIT | MALLOC_CAP_EXEC }, true, false},
    // Type 3: DIRAM used for startup stacks
    [SOC_MEMORY_TYPE_STACK_DIRAM] = { "STACK/DIRAM", { MALLOC_CAP_8BIT | MALLOC_CAP_DEFAULT | MALLOC_CAP_RETENTION, MALLOC_CAP_EXEC | MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_32BIT, 0 }, true, true},
    // Type 4: RTCRAM   // TODO: IDF-5667 Better to rename to LPRAM
    [SOC_MEMORY_TYPE_RTCRAM] = { "RTCRAM", { MALLOC_CAP_RTCRAM, MALLOC_CAP_8BIT | MALLOC_CAP_DEFAULT, MALLOC_CAP_INTERNAL | MALLOC_CAP_32BIT }, false, false},
};

#ifdef CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
#define SOC_MEMORY_TYPE_DEFAULT SOC_MEMORY_TYPE_DRAM
#define SOC_MEMORY_TYPE_STACK_DEFAULT SOC_MEMORY_TYPE_STACK_DRAM
#else
#define SOC_MEMORY_TYPE_DEFAULT SOC_MEMORY_TYPE_DIRAM
#define SOC_MEMORY_TYPE_STACK_DEFAULT SOC_MEMORY_TYPE_STACK_DIRAM
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
    { 0x40800000,           0x20000,                                   SOC_MEMORY_TYPE_DEFAULT,     0x40800000}, //D/IRAM level0, can be used as trace memory
    { 0x40820000,           0x20000,                                   SOC_MEMORY_TYPE_DEFAULT,     0x40820000}, //D/IRAM level1, can be used as trace memory
    { 0x40840000,           0x20000,                                   SOC_MEMORY_TYPE_DEFAULT,     0x40840000}, //D/IRAM level2, can be used as trace memory
    { 0x40860000,           (APP_USABLE_DRAM_END-0x40860000),          SOC_MEMORY_TYPE_DEFAULT,     0x40860000}, //D/IRAM level3, can be used as trace memory
    { APP_USABLE_DRAM_END,  (SOC_DIRAM_DRAM_HIGH-APP_USABLE_DRAM_END), SOC_MEMORY_TYPE_STACK_DEFAULT,  APP_USABLE_DRAM_END}, //D/IRAM level3, can be used as trace memory (ROM reserved area)
#ifdef CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP
    { 0x50000000,           0x4000,                                    SOC_MEMORY_TYPE_RTCRAM,      0},          //LPRAM
#endif
};

const size_t soc_memory_region_count = sizeof(soc_memory_regions) / sizeof(soc_memory_region_t);


extern int _data_start, _heap_start, _iram_start, _iram_end, _rtc_force_slow_end;
extern int _rtc_reserved_start, _rtc_reserved_end;

/**
 * Reserved memory regions.
 * These are removed from the soc_memory_regions array when heaps are created.
 *
 */

// Static data region. DRAM used by data+bss and possibly rodata
SOC_RESERVE_MEMORY_REGION((intptr_t)&_data_start, (intptr_t)&_heap_start, dram_data);

// Target has a shared D/IRAM virtual address, no need to calculate I_D_OFFSET like previous chips
SOC_RESERVE_MEMORY_REGION((intptr_t)&_iram_start, (intptr_t)&_iram_end, iram_code);

#ifdef CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP
// TODO: IDF-6019 check reserved lp mem region
SOC_RESERVE_MEMORY_REGION(SOC_RTC_DRAM_LOW, (intptr_t)&_rtc_force_slow_end, rtcram_data);
#endif

SOC_RESERVE_MEMORY_REGION((intptr_t)&_rtc_reserved_start, (intptr_t)&_rtc_reserved_end, rtc_reserved_data);
