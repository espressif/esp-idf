/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
    SOC_MEMORY_TYPE_RAM     = 0,
    SOC_MEMORY_TYPE_RTCRAM  = 1,
    SOC_MEMORY_TYPE_SPIRAM  = 2,
    SOC_MEMORY_TYPE_NUM,
};

/* COMMON_CAPS is the set of attributes common to all types of memory on this chip */
#ifdef CONFIG_ESP_SYSTEM_PMP_IDRAM_SPLIT
#define ESP32C5_MEM_COMMON_CAPS (MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL | MALLOC_CAP_32BIT | MALLOC_CAP_8BIT)
#else
#define ESP32C5_MEM_COMMON_CAPS (MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL | MALLOC_CAP_32BIT | MALLOC_CAP_8BIT | MALLOC_CAP_EXEC)
#endif

/**
 * Defined the attributes and allocation priority of each memory on the chip,
 * The heap allocator will traverse all types of memory types in column High Priority Matching and match the specified caps at first,
 * if no memory caps matched or the allocation is failed, it will go to columns Medium Priority Matching and Low Priority Matching
 * in turn to continue matching.
 */
const soc_memory_type_desc_t soc_memory_types[SOC_MEMORY_TYPE_NUM] = {
    /*                           Mem Type Name   High Priority Matching                      Medium Priority Matching    Low Priority Matching */
    [SOC_MEMORY_TYPE_RAM]    = { "RAM",          { ESP32C5_MEM_COMMON_CAPS | MALLOC_CAP_DMA, 0,                         0 }},
    [SOC_MEMORY_TYPE_RTCRAM] = { "RTCRAM",       { MALLOC_CAP_RTCRAM,                        ESP32C5_MEM_COMMON_CAPS,   0 }},
    [SOC_MEMORY_TYPE_SPIRAM] = { "SPIRAM",       { MALLOC_CAP_SPIRAM | MALLOC_CAP_DEFAULT, 0, MALLOC_CAP_8BIT | MALLOC_CAP_32BIT}},
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
#define APP_USABLE_DRAM_END                 (SOC_ROM_STACK_START - SOC_ROM_STACK_SIZE)

const soc_memory_region_t soc_memory_regions[] = {
#if CONFIG_SPIRAM
    { SOC_EXTRAM_DATA_LOW,  (SOC_EXTRAM_DATA_HIGH - SOC_EXTRAM_DATA_LOW),   SOC_MEMORY_TYPE_SPIRAM, 0,                      false}, //SPI SRAM, if available
#endif
    { SOC_DIRAM_DRAM_LOW,   (APP_USABLE_DRAM_END - SOC_DIRAM_DRAM_LOW),     SOC_MEMORY_TYPE_RAM,    SOC_DIRAM_IRAM_LOW,     false}, //D/IRAM, can be used as trace memory
    { APP_USABLE_DRAM_END,  (SOC_DIRAM_DRAM_HIGH - APP_USABLE_DRAM_END),    SOC_MEMORY_TYPE_RAM,    APP_USABLE_DRAM_END,    true},  //D/IRAM, can be used as trace memory (ROM reserved area)
#ifdef CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP
    { SOC_RTC_DATA_LOW,     (SOC_RTC_DATA_HIGH - SOC_RTC_DATA_LOW),         SOC_MEMORY_TYPE_RTCRAM, 0,                      false}, //LPRAM
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
SOC_RESERVE_MEMORY_REGION(SOC_RTC_DRAM_LOW, (intptr_t)&_rtc_force_slow_end, rtcram_data);
#endif

SOC_RESERVE_MEMORY_REGION((intptr_t)&_rtc_reserved_start, (intptr_t)&_rtc_reserved_end, rtc_reserved_data);

#ifdef CONFIG_SPIRAM
/* Reserve the whole possible SPIRAM region here, spiram.c will add some or all of this
 * memory to heap depending on the actual SPIRAM chip size. */
SOC_RESERVE_MEMORY_REGION(SOC_EXTRAM_DATA_LOW, SOC_EXTRAM_DATA_HIGH, extram_data_region);
#endif
