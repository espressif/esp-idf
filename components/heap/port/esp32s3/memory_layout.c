/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BOOTLOADER_BUILD

#include <stdint.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "soc/soc.h"
#include "soc/dport_reg.h"
#include "soc/tracemem_config.h"
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
const soc_memory_type_desc_t soc_memory_types[] = {
    // Type 0: DRAM
    { "DRAM", { MALLOC_CAP_8BIT | MALLOC_CAP_DEFAULT, MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_32BIT, 0 }, false, false},
    // Type 1: DRAM used for startup stacks
    { "STACK/DRAM", { MALLOC_CAP_8BIT | MALLOC_CAP_DEFAULT, MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_32BIT, 0 }, false, true},
    // Type 2: DRAM which has an alias on the I-port
    { "D/IRAM", { 0, MALLOC_CAP_DMA | MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL | MALLOC_CAP_DEFAULT, MALLOC_CAP_32BIT | MALLOC_CAP_EXEC }, true, false},
    // Type 3: IRAM
    { "IRAM", { MALLOC_CAP_EXEC | MALLOC_CAP_32BIT | MALLOC_CAP_INTERNAL, 0, 0 }, false, false},
    // Type 4: SPI SRAM data
    { "SPIRAM", { MALLOC_CAP_SPIRAM | MALLOC_CAP_DEFAULT, 0, MALLOC_CAP_8BIT | MALLOC_CAP_32BIT}, false, false},
    // Type 5: DRAM which is not DMA accesible
    { "NON_DMA_DRAM", { MALLOC_CAP_8BIT | MALLOC_CAP_DEFAULT, MALLOC_CAP_INTERNAL | MALLOC_CAP_32BIT, 0 }, false, false},
    // Type 6: RTC Fast RAM
    { "RTCRAM", { MALLOC_CAP_RTCRAM, MALLOC_CAP_8BIT | MALLOC_CAP_DEFAULT, MALLOC_CAP_INTERNAL | MALLOC_CAP_32BIT }, false, false},
};

const size_t soc_memory_type_count = sizeof(soc_memory_types) / sizeof(soc_memory_type_desc_t);

/**
 * @brief Region descriptors. These describe all regions of memory available, and map them to a type in the above type.
 *
 * @note Because of requirements in the coalescing code which merges adjacent regions,
 *       this list should always be sorted from low to high by start address.
 *
 */
const soc_memory_region_t soc_memory_regions[] = {
#ifdef CONFIG_SPIRAM
    { SOC_EXTRAM_DATA_LOW, SOC_EXTRAM_DATA_SIZE, 4, 0}, //SPI SRAM, if available
#endif
#if CONFIG_ESP32S3_INSTRUCTION_CACHE_16KB
    { 0x40374000, 0x4000,  3, 0},          //Level 1, IRAM
#endif
    { 0x3FC88000, 0x8000,  2, 0x40378000}, //Level 2, IDRAM, can be used as trace memroy
    { 0x3FC90000, 0x10000, 2, 0x40380000}, //Level 3, IDRAM, can be used as trace memroy
    { 0x3FCA0000, 0x10000, 2, 0x40390000}, //Level 4, IDRAM, can be used as trace memroy
    { 0x3FCB0000, 0x10000, 2, 0x403A0000}, //Level 5, IDRAM, can be used as trace memroy
    { 0x3FCC0000, 0x10000, 2, 0x403B0000}, //Level 6, IDRAM, can be used as trace memroy
    { 0x3FCD0000, 0x10000, 2, 0x403C0000}, //Level 7, IDRAM, can be used as trace memroy
    { 0x3FCE0000, 0x10000, 1, 0},          //Level 8, IDRAM, can be used as trace memroy, contains stacks used by startup flow, recycled by heap allocator in app_main task
#if CONFIG_ESP32S3_DATA_CACHE_16KB || CONFIG_ESP32S3_DATA_CACHE_32KB
    { 0x3FCF0000, 0x8000,  0, 0},          //Level 9, DRAM
#endif
#if CONFIG_ESP32S3_DATA_CACHE_16KB
    { 0x3C000000, 0x4000,  5, 0}
#endif
#ifdef CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP
    { 0x600fe000, 0x2000,  6, 0}, //Fast RTC memory
#endif
};

const size_t soc_memory_region_count = sizeof(soc_memory_regions) / sizeof(soc_memory_region_t);

extern int _data_start, _heap_start, _iram_start, _iram_end, _rtc_force_fast_end, _rtc_noinit_end; // defined in sections.ld.in

/**
 * Reserved memory regions.
 * These are removed from the soc_memory_regions array when heaps are created.
 *
 */

// Static data region. DRAM used by data+bss and possibly rodata
SOC_RESERVE_MEMORY_REGION((intptr_t)&_data_start, (intptr_t)&_heap_start, dram_data);

// ESP32S3 has a big D/IRAM region, the part used by code is reserved
// The address of the D/I bus are in the same order, directly shift IRAM address to get reserved DRAM address
#define I_D_OFFSET (SOC_DIRAM_IRAM_LOW - SOC_DIRAM_DRAM_LOW)
// .text region in diram. DRAM used by text (shared with IBUS).
SOC_RESERVE_MEMORY_REGION((intptr_t)&_iram_start - I_D_OFFSET, (intptr_t)&_iram_end - I_D_OFFSET, iram_code);

#if CONFIG_ESP32S3_INSTRUCTION_CACHE_16KB
SOC_RESERVE_MEMORY_REGION((intptr_t)&_iram_start, (intptr_t)&_iram_end, iram_code_2);
#endif

#ifdef CONFIG_SPIRAM
/* Reserve the whole possible SPIRAM region here, spiram.c will add some or all of this
 * memory to heap depending on the actual SPIRAM chip size. */
SOC_RESERVE_MEMORY_REGION( SOC_EXTRAM_DATA_LOW, SOC_EXTRAM_DATA_HIGH, extram_data_region);
#endif

#if CONFIG_ESP32S3_TRACEMEM_RESERVE_DRAM > 0
SOC_RESERVE_MEMORY_REGION(TRACEMEM_BLK0_ADDR, TRACEMEM_BLK0_ADDR + CONFIG_ESP32S3_TRACEMEM_RESERVE_DRAM / 2, trace_mem0);
SOC_RESERVE_MEMORY_REGION(TRACEMEM_BLK1_ADDR, TRACEMEM_BLK1_ADDR + CONFIG_ESP32S3_TRACEMEM_RESERVE_DRAM / 2, trace_mem1);
#endif

// RTC Fast RAM region
#ifdef CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP
#ifdef CONFIG_ESP32S3_RTCDATA_IN_FAST_MEM
SOC_RESERVE_MEMORY_REGION(SOC_RTC_DRAM_LOW, (intptr_t)&_rtc_noinit_end, rtcram_data);
#else
SOC_RESERVE_MEMORY_REGION(SOC_RTC_DRAM_LOW, (intptr_t)&_rtc_force_fast_end, rtcram_data);
#endif
#endif

#endif // BOOTLOADER_BUILD
