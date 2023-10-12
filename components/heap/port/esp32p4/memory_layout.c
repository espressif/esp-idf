/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
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
 * - Most other malloc caps only fit in one region anyway.
 *
 */

/* Index of memory in `soc_memory_types[]` */
enum {
    SOC_MEMORY_TYPE_L2MEM   = 0,
    SOC_MEMORY_TYPE_SPIRAM  = 1,
    SOC_MEMORY_TYPE_TCM     = 2,
    SOC_MEMORY_TYPE_RTCRAM  = 3,
    SOC_MEMORY_TYPE_NUM,
};

/* COMMON_CAPS is the set of attributes common to all types of memory on this chip */
#define ESP32P4_MEM_COMMON_CAPS (MALLOC_CAP_DEFAULT | MALLOC_CAP_32BIT | MALLOC_CAP_8BIT)

#ifdef CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
#define MALLOC_L2MEM_BASE_CAPS      ESP32P4_MEM_COMMON_CAPS | MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA
#define MALLOC_RTCRAM_BASE_CAPS     ESP32P4_MEM_COMMON_CAPS | MALLOC_CAP_INTERNAL
#else
#define MALLOC_L2MEM_BASE_CAPS      ESP32P4_MEM_COMMON_CAPS | MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_EXEC
#define MALLOC_RTCRAM_BASE_CAPS     ESP32P4_MEM_COMMON_CAPS | MALLOC_CAP_INTERNAL | MALLOC_CAP_EXEC
#endif

/**
 * Defined the attributes and allocation priority of each memory on the chip,
 * The heap allocator will traverse all types of memory types in column High Priority Matching and match the specified caps at first,
 * if no memory caps matched or the allocation is failed, it will go to columns Medium Priorty Matching and Low Priority Matching
 * in turn to continue matching.
 */
const soc_memory_type_desc_t soc_memory_types[SOC_MEMORY_TYPE_NUM] = {
    /*                       Mem Type Name  | High Priority Matching   | Medium Priorty Matching                      | Low Priority Matching */
    [SOC_MEMORY_TYPE_L2MEM]  = { "RAM",     { MALLOC_L2MEM_BASE_CAPS,    0,                                             0 }},
    [SOC_MEMORY_TYPE_SPIRAM] = { "SPIRAM",  { MALLOC_CAP_SPIRAM,         ESP32P4_MEM_COMMON_CAPS,                       0 }},
    [SOC_MEMORY_TYPE_TCM]    = { "TCM",     { MALLOC_CAP_TCM,            ESP32P4_MEM_COMMON_CAPS | MALLOC_CAP_INTERNAL, 0 }},
    [SOC_MEMORY_TYPE_RTCRAM] = { "RTCRAM",  { MALLOC_CAP_RTCRAM,         0,                                             MALLOC_RTCRAM_BASE_CAPS}},
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
#ifdef CONFIG_SPIRAM
    { SOC_EXTRAM_LOW,   SOC_EXTRAM_SIZE,    SOC_MEMORY_TYPE_SPIRAM, 0,             false}, //PSRAM, if available
#endif
    // base 192k is always avaible, even if we config l2 cache size to 512k
    { 0x4ff00000,       0x30000,            SOC_MEMORY_TYPE_L2MEM,  0x4ff00000,    false},
    // 64k for rom startup stack
    { 0x4ff30000,       0x10000,            SOC_MEMORY_TYPE_L2MEM,  0x4ff30000,    true},
#if CONFIG_ESP32P4_L2_CACHE_256KB // 768-256 = 512k avaible for l2 memory, add extra 256k
    { 0x4ff40000,       0x40000,            SOC_MEMORY_TYPE_L2MEM,  0x4ff40000,    false},
#endif
#if CONFIG_ESP32P4_L2_CACHE_128KB // 768 - 128 = 640k avaible for l2 memory, add extra 384k
    { 0x4ff40000,       0x60000,            SOC_MEMORY_TYPE_L2MEM,  0x4ff40000,    false},
#endif
#ifdef CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP
    { 0x50108000,       0x8000,             SOC_MEMORY_TYPE_RTCRAM, 0,             false}, //LPRAM
#endif
    { 0x30100000,       0x2000,             SOC_MEMORY_TYPE_TCM,    0,             false},
};

const size_t soc_memory_region_count = sizeof(soc_memory_regions) / sizeof(soc_memory_region_t);


extern int _data_start, _heap_start, _iram_start, _iram_end, _rtc_force_slow_end;
extern int _tcm_text_start, _tcm_data_end;

/**
 * Reserved memory regions.
 * These are removed from the soc_memory_regions array when heaps are created.
 *
 */

// Static data region. DRAM used by data+bss and possibly rodata
SOC_RESERVE_MEMORY_REGION((intptr_t)&_data_start, (intptr_t)&_heap_start, dram_data);

// Target has a shared D/IRAM virtual address, no need to calculate I_D_OFFSET like previous chips
SOC_RESERVE_MEMORY_REGION((intptr_t)&_iram_start, (intptr_t)&_iram_end, iram_code);

SOC_RESERVE_MEMORY_REGION((intptr_t)&_tcm_text_start, (intptr_t)&_tcm_data_end, tcm_code_data);

#ifdef CONFIG_SPIRAM
SOC_RESERVE_MEMORY_REGION( SOC_EXTRAM_LOW, SOC_EXTRAM_HIGH, extram_region);
#endif

#ifdef CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP
// TODO: IDF-6019 check reserved lp mem region
SOC_RESERVE_MEMORY_REGION(SOC_RTC_DRAM_LOW, (intptr_t)&_rtc_force_slow_end, rtcram_data);
#endif
