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
    SOC_MEMORY_TYPE_DRAM        = 0,
    SOC_MEMORY_TYPE_STACK_DRAM  = 1,
    SOC_MEMORY_TYPE_DIRAM       = 2,
    SOC_MEMORY_TYPE_STACK_DIRAM = 3,
    SOC_MEMORY_TYPE_RTCRAM      = 4,
    SOC_MEMORY_TYPE_TCM         = 5,
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
    // Type 5: TCM
    [SOC_MEMORY_TYPE_TCM] = {"TCM", {MALLOC_CAP_TCM, MALLOC_CAP_8BIT | MALLOC_CAP_DEFAULT, MALLOC_CAP_INTERNAL | MALLOC_CAP_32BIT}, false, false},
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
#ifdef CONFIG_SPIRAM
    { SOC_EXTRAM_LOW,  SOC_EXTRAM_HIGH,                                SOC_MEMORY_TYPE_SPIRAM,     0}, //PSRAM, if available
#endif
    // base 192k is always avaible, even if we config l2 cache size to 512k
    { 0x4ff00000,           0x30000,                                   SOC_MEMORY_TYPE_DEFAULT,     0x4ff00000},
    // 64k for rom startup stack
    { 0x4ff30000,           0x10000,                                   SOC_MEMORY_TYPE_STACK_DRAM,     0x4ff30000},
#if CONFIG_ESP32P4_L2_CACHE_256KB // 768-256 = 512k avaible for l2 memory, add extra 256k
    { 0x4ff40000,           0x40000,                                   SOC_MEMORY_TYPE_DEFAULT,     0x4ff40000},
#endif
#if CONFIG_ESP32P4_L2_CACHE_128KB // 768 - 128 = 640k avaible for l2 memory, add extra 384k
    { 0x4ff40000,           0x60000,                                   SOC_MEMORY_TYPE_DEFAULT,     0x4ff40000},
#endif
#ifdef CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP
    { 0x50108000,           0x8000,                                    SOC_MEMORY_TYPE_RTCRAM,      0},          //LPRAM
#endif
    { 0x30100000,           0x2000,                                    SOC_MEMORY_TYPE_TCM,     0},
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
