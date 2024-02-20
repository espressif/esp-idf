/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
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
    SOC_MEMORY_TYPE_RAM            = 0,
    SOC_MEMORY_TYPE_RETENTION_RAM  = 1,
    SOC_MEMORY_TYPE_RTCRAM         = 2,
    SOC_MEMORY_TYPE_NUM,
};

/* COMMON_CAPS is the set of attributes common to all types of memory on this chip */
#ifdef CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
#define ESP32C3_MEM_COMMON_CAPS (MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL | MALLOC_CAP_32BIT | MALLOC_CAP_8BIT)
#else
#define ESP32C3_MEM_COMMON_CAPS (MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL | MALLOC_CAP_32BIT | MALLOC_CAP_8BIT | MALLOC_CAP_EXEC)
#endif

/**
 * Defined the attributes and allocation priority of each memory on the chip,
 * The heap allocator will traverse all types of memory types in column High Priority Matching and match the specified caps at first,
 * if no memory caps matched or the allocation is failed, it will go to columns Medium Priorty Matching and Low Priority Matching
 * in turn to continue matching.
 */
const soc_memory_type_desc_t soc_memory_types[SOC_MEMORY_TYPE_NUM] = {
    /*                                   Mem Type Name   | High Priority Matching                  | Medium Priorty Matching                 | Low Priority Matching */
    [SOC_MEMORY_TYPE_RAM]           = { "RAM",           { ESP32C3_MEM_COMMON_CAPS | MALLOC_CAP_DMA, 0 ,                                       0}},
    [SOC_MEMORY_TYPE_RETENTION_RAM] = { "Retention RAM", { MALLOC_CAP_RETENTION,                     ESP32C3_MEM_COMMON_CAPS | MALLOC_CAP_DMA, 0}},
    [SOC_MEMORY_TYPE_RTCRAM]        = { "RTCRAM",        { MALLOC_CAP_RTCRAM,                        0,                                        ESP32C3_MEM_COMMON_CAPS }},
};

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
    { 0x3FC80000,           0x20000,                                   SOC_MEMORY_TYPE_RAM,             0x40380000, false}, //D/IRAM level1, can be used as trace memory
    { 0x3FCA0000,           0x20000,                                   SOC_MEMORY_TYPE_RAM,             0x403A0000, false}, //D/IRAM level2, can be used as trace memory
    { 0x3FCC0000,           (APP_USABLE_DRAM_END-0x3FCC0000),          SOC_MEMORY_TYPE_RETENTION_RAM,   0x403C0000, false}, //D/IRAM level3, backup dma accessible, can be used as trace memory
    { APP_USABLE_DRAM_END,  (SOC_DIRAM_DRAM_HIGH-APP_USABLE_DRAM_END), SOC_MEMORY_TYPE_RETENTION_RAM,   MAP_DRAM_TO_IRAM(APP_USABLE_DRAM_END), true}, //D/IRAM level3, backup dma accessible, can be used as trace memory (ROM reserved area)
#ifdef CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP
    { 0x50000000,           0x2000,                                    SOC_MEMORY_TYPE_RTCRAM,          0, false}, //Fast RTC memory
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

SOC_RESERVE_MEMORY_REGION((intptr_t)&_rtc_reserved_start, (intptr_t)&_rtc_reserved_end, rtc_reserved_data);
