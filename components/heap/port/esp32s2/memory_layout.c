/*
 * SPDX-FileCopyrightText: 2010-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <stdint.h>

#include "sdkconfig.h"
#include "soc/soc.h"
#include "soc/tracemem_config.h"
#include "heap_memory_layout.h"
#include "esp_heap_caps.h"

/* Memory layout for ESP32 SoC */

/*
Memory type descriptors. These describe the capabilities of a type of memory in the SoC. Each type of memory
map consist of one or more regions in the address space.

Each type contains an array of prioritised capabilities; types with later entries are only taken if earlier
ones can't fulfill the memory request.

The prioritised capabilities work roughly like this:
- For a normal malloc (MALLOC_CAP_DEFAULT), give away the DRAM-only memory first, then pass off any dual-use IRAM regions,
  finally eat into the application memory.
- For a malloc where 32-bit-aligned-only access is okay, first allocate IRAM, then DRAM, finally application IRAM.
- Application mallocs (PIDx) will allocate IRAM first, if possible, then DRAM.
- Most other malloc caps only fit in one region anyway.
*/

enum {
    SOC_MEMORY_TYPE_DIRAM = 0,
    SOC_MEMORY_TYPE_SPIRAM = 1,
    SOC_MEMORY_TYPE_RTCRAM = 2,
    SOC_MEMORY_TYPE_NUM,
};

/* COMMON_CAPS is the set of attributes common to all types of memory on this chip */
#define ESP32S2_MEM_COMMON_CAPS (MALLOC_CAP_DEFAULT | MALLOC_CAP_32BIT | MALLOC_CAP_8BIT)

#ifdef CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
#define MALLOC_DIRAM_BASE_CAPS    ESP32S2_MEM_COMMON_CAPS | MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA
#define MALLOC_RTCRAM_BASE_CAPS   ESP32S2_MEM_COMMON_CAPS | MALLOC_CAP_INTERNAL
#else
#define MALLOC_DIRAM_BASE_CAPS    ESP32S2_MEM_COMMON_CAPS | MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_EXEC
#define MALLOC_RTCRAM_BASE_CAPS   ESP32S2_MEM_COMMON_CAPS | MALLOC_CAP_INTERNAL | MALLOC_CAP_EXEC
#endif

/**
 * Defined the attributes and allocation priority of each memory on the chip,
 * The heap allocator will traverse all types of memory types in column High Priority Matching and match the specified caps at first,
 * if no memory caps matched or the allocation is failed, it will go to columns Medium Priorty Matching and Low Priority Matching
 * in turn to continue matching.
 */
const soc_memory_type_desc_t soc_memory_types[] = {
    /*                       Mem Type Name | High Priority Matching     | Medium Priorty Matching  | Low Priority Matching */
    [SOC_MEMORY_TYPE_DIRAM]  = { "RAM",    { MALLOC_DIRAM_BASE_CAPS,    0,                         0 }},
    //TODO, in fact, part of them support EDMA, to be supported.
    [SOC_MEMORY_TYPE_SPIRAM] = { "SPIRAM", { MALLOC_CAP_SPIRAM,         ESP32S2_MEM_COMMON_CAPS,   0 }},
    [SOC_MEMORY_TYPE_RTCRAM] = { "RTCRAM", { MALLOC_CAP_RTCRAM,         0,                         MALLOC_RTCRAM_BASE_CAPS }},
};

const size_t soc_memory_type_count = sizeof(soc_memory_types)/sizeof(soc_memory_type_desc_t);

/*
Region descriptors. These describe all regions of memory available, and map them to a type in the above type.

Because of requirements in the coalescing code which merges adjacent regions, this list should always be sorted
from low to high start address.
*/
const soc_memory_region_t soc_memory_regions[] = {
#ifdef CONFIG_SPIRAM
    { SOC_EXTRAM_DATA_LOW, SOC_EXTRAM_DATA_SIZE, SOC_MEMORY_TYPE_SPIRAM, 0, false}, //SPI SRAM, if available
#endif
#if CONFIG_ESP32S2_INSTRUCTION_CACHE_8KB
#if CONFIG_ESP32S2_DATA_CACHE_0KB
    { 0x3FFB2000, 0x2000, SOC_MEMORY_TYPE_DIRAM, 0x40022000, false}, //Block 1, can be use as I/D cache memory
    { 0x3FFB4000, 0x2000, SOC_MEMORY_TYPE_DIRAM, 0x40024000, false}, //Block 2, can be use as D cache memory
    { 0x3FFB6000, 0x2000, SOC_MEMORY_TYPE_DIRAM, 0x40026000, false}, //Block 3, can be use as D cache memory
#elif CONFIG_ESP32S2_DATA_CACHE_8KB
    { 0x3FFB4000, 0x2000, SOC_MEMORY_TYPE_DIRAM, 0x40024000, false}, //Block 2, can be use as D cache memory
    { 0x3FFB6000, 0x2000, SOC_MEMORY_TYPE_DIRAM, 0x40026000, false}, //Block 3, can be use as D cache memory
#else
    { 0x3FFB6000, 0x2000, SOC_MEMORY_TYPE_DIRAM, 0x40026000, false}, //Block 3, can be use as D cache memory
#endif
#else
#if CONFIG_ESP32S2_DATA_CACHE_0KB
    { 0x3FFB4000, 0x2000, SOC_MEMORY_TYPE_DIRAM, 0x40024000, false}, //Block 3, can be use as D cache memory
    { 0x3FFB6000, 0x2000, SOC_MEMORY_TYPE_DIRAM, 0x40026000, false}, //Block 3, can be use as D cache memory
#elif CONFIG_ESP32S2_DATA_CACHE_8KB
    { 0x3FFB6000, 0x2000, SOC_MEMORY_TYPE_DIRAM, 0x40026000, false}, //Block 3, can be use as D cache memory
#endif
#endif
    { 0x3FFB8000, 0x4000, SOC_MEMORY_TYPE_DIRAM, 0x40028000, false}, //Block 4,  can be remapped to ROM, can be used as trace memory
    { 0x3FFBC000, 0x4000, SOC_MEMORY_TYPE_DIRAM, 0x4002C000, false}, //Block 5,  can be remapped to ROM, can be used as trace memory
    { 0x3FFC0000, 0x4000, SOC_MEMORY_TYPE_DIRAM, 0x40030000, false}, //Block 6,  can be used as trace memory
    { 0x3FFC4000, 0x4000, SOC_MEMORY_TYPE_DIRAM, 0x40034000, false}, //Block 7,  can be used as trace memory
    { 0x3FFC8000, 0x4000, SOC_MEMORY_TYPE_DIRAM, 0x40038000, false}, //Block 8,  can be used as trace memory
    { 0x3FFCC000, 0x4000, SOC_MEMORY_TYPE_DIRAM, 0x4003C000, false}, //Block 9,  can be used as trace memory

    { 0x3FFD0000, 0x4000, SOC_MEMORY_TYPE_DIRAM, 0x40040000, false}, //Block 10,  can be used as trace memory
    { 0x3FFD4000, 0x4000, SOC_MEMORY_TYPE_DIRAM, 0x40044000, false}, //Block 11,  can be used as trace memory
    { 0x3FFD8000, 0x4000, SOC_MEMORY_TYPE_DIRAM, 0x40048000, false}, //Block 12,  can be used as trace memory
    { 0x3FFDC000, 0x4000, SOC_MEMORY_TYPE_DIRAM, 0x4004C000, false}, //Block 13,  can be used as trace memory
    { 0x3FFE0000, 0x4000, SOC_MEMORY_TYPE_DIRAM, 0x40050000, false}, //Block 14,  can be used as trace memory
    { 0x3FFE4000, 0x4000, SOC_MEMORY_TYPE_DIRAM, 0x40054000, false}, //Block 15,  can be used as trace memory
    { 0x3FFE8000, 0x4000, SOC_MEMORY_TYPE_DIRAM, 0x40058000, false}, //Block 16,  can be used as trace memory
    { 0x3FFEC000, 0x4000, SOC_MEMORY_TYPE_DIRAM, 0x4005C000, false}, //Block 17,  can be used as trace memory
    { 0x3FFF0000, 0x4000, SOC_MEMORY_TYPE_DIRAM, 0x40060000, false}, //Block 18,  can be used for MAC dump, can be used as trace memory
    { 0x3FFF4000, 0x4000, SOC_MEMORY_TYPE_DIRAM, 0x40064000, false}, //Block 19,  can be used for MAC dump, can be used as trace memory
    { 0x3FFF8000, 0x4000, SOC_MEMORY_TYPE_DIRAM, 0x40068000, false}, //Block 20,  can be used for MAC dump, can be used as trace memory
    { 0x3FFFC000, 0x4000, SOC_MEMORY_TYPE_DIRAM, 0x4006C000, true},  //Block 21,  can be used for MAC dump, can be used as trace memory, used for startup stack
#ifdef CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP
    { SOC_RTC_DRAM_LOW, 0x2000, SOC_MEMORY_TYPE_RTCRAM, 0, false}, //RTC Fast Memory
#endif
};

const size_t soc_memory_region_count = sizeof(soc_memory_regions)/sizeof(soc_memory_region_t);


extern int _dram0_rtos_reserved_start;
extern int _data_start, _heap_start, _iram_start, _iram_end, _rtc_force_fast_end, _rtc_noinit_end;
extern int _rtc_reserved_start, _rtc_reserved_end;

/* Reserved memory regions

   These are removed from the soc_memory_regions array when heaps are created.
 */
//ROM data region
SOC_RESERVE_MEMORY_REGION((intptr_t)&_dram0_rtos_reserved_start, SOC_BYTE_ACCESSIBLE_HIGH, rom_data_region);

// Static data region. DRAM used by data+bss and possibly rodata
SOC_RESERVE_MEMORY_REGION((intptr_t)&_data_start, (intptr_t)&_heap_start, dram_data);

// ESP32S2 has a big D/IRAM region, the part used by code is reserved
// The address of the D/I bus are in the same order, directly shift IRAM address to get reserved DRAM address
#define I_D_OFFSET (SOC_IRAM_LOW - SOC_DRAM_LOW)
SOC_RESERVE_MEMORY_REGION((intptr_t)&_iram_start - I_D_OFFSET, (intptr_t)&_iram_end - I_D_OFFSET, iram_code);

#ifdef CONFIG_SPIRAM
/* Reserve the whole possible SPIRAM region here, spiram.c will add some or all of this
 * memory to heap depending on the actual SPIRAM chip size. */
SOC_RESERVE_MEMORY_REGION( SOC_EXTRAM_DATA_LOW, SOC_EXTRAM_DATA_HIGH, extram_data_region);
#endif

// Blocks 19 and 20 may be reserved for the trace memory
#if CONFIG_ESP32S2_TRACEMEM_RESERVE_DRAM > 0
SOC_RESERVE_MEMORY_REGION(TRACEMEM_BLK0_ADDR, TRACEMEM_BLK0_ADDR + CONFIG_ESP32S2_TRACEMEM_RESERVE_DRAM / 2, trace_mem0);
SOC_RESERVE_MEMORY_REGION(TRACEMEM_BLK1_ADDR, TRACEMEM_BLK1_ADDR + CONFIG_ESP32S2_TRACEMEM_RESERVE_DRAM / 2, trace_mem1);
#endif

// RTC Fast RAM region
#ifdef CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP
#ifdef CONFIG_ESP32S2_RTCDATA_IN_FAST_MEM
SOC_RESERVE_MEMORY_REGION(SOC_RTC_DRAM_LOW, (intptr_t)&_rtc_noinit_end, rtcram_data);
#else
SOC_RESERVE_MEMORY_REGION(SOC_RTC_DRAM_LOW, (intptr_t)&_rtc_force_fast_end, rtcram_data);
#endif
#endif

SOC_RESERVE_MEMORY_REGION((intptr_t)&_rtc_reserved_start, (intptr_t)&_rtc_reserved_end, rtc_reserved_data);
