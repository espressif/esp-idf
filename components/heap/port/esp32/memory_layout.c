/*
 * SPDX-FileCopyrightText: 2010-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <stdint.h>

#include "soc/soc.h"
#include "heap_memory_layout.h"
#include "esp_heap_caps.h"
#include "sdkconfig.h"

#ifdef CONFIG_ESP32_IRAM_AS_8BIT_ACCESSIBLE_MEMORY
#define MALLOC_IRAM_CAP MALLOC_CAP_EXEC|MALLOC_CAP_32BIT|MALLOC_CAP_IRAM_8BIT
#else
#define MALLOC_IRAM_CAP MALLOC_CAP_EXEC|MALLOC_CAP_32BIT
#endif

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
    SOC_MEMORY_TYPE_DRAM = 0,
    SOC_MEMORY_TYPE_DIRAM = 1,
    SOC_MEMORY_TYPE_IRAM = 2,
    SOC_MEMORY_TYPE_SPIRAM = 3,
    SOC_MEMORY_TYPE_RTCRAM = 4,
    SOC_MEMORY_TYPE_NUM,
};

const soc_memory_type_desc_t soc_memory_types[] = {
    //Type 0: Plain ole D-port RAM
    [SOC_MEMORY_TYPE_DRAM]  = { "DRAM", { MALLOC_CAP_8BIT|MALLOC_CAP_DEFAULT, MALLOC_CAP_INTERNAL|MALLOC_CAP_DMA|MALLOC_CAP_32BIT, 0 }},
    //Type 1: Plain ole D-port RAM which has an alias on the I-port
    //(This DRAM is also the region used by ROM during startup, and decrease the allocation priority to avoid MALLOC_CAP_EXEC memory running out too soon)
    [SOC_MEMORY_TYPE_DIRAM]  = { "D/IRAM", { 0, MALLOC_CAP_DMA|MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL|MALLOC_CAP_DEFAULT, MALLOC_CAP_32BIT|MALLOC_CAP_EXEC }},
    //Type 2: IRAM
    [SOC_MEMORY_TYPE_IRAM]  = { "IRAM", { MALLOC_CAP_INTERNAL|MALLOC_IRAM_CAP, 0, 0 }},
    //Type 3: SPI SRAM data
    [SOC_MEMORY_TYPE_SPIRAM]  = { "SPIRAM", { MALLOC_CAP_SPIRAM|MALLOC_CAP_DEFAULT, 0, MALLOC_CAP_8BIT|MALLOC_CAP_32BIT}},
    //Type 4: RTC Fast RAM
    [SOC_MEMORY_TYPE_RTCRAM]  = { "RTCRAM", { MALLOC_CAP_RTCRAM, MALLOC_CAP_8BIT|MALLOC_CAP_DEFAULT, MALLOC_CAP_INTERNAL|MALLOC_CAP_32BIT|MALLOC_CAP_EXEC }},
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
    { 0x3FFAE000, 0x2000, SOC_MEMORY_TYPE_DRAM,     0,         false}, //pool 16 <- used for rom code
    { 0x3FFB0000, 0x8000, SOC_MEMORY_TYPE_DRAM,     0,         false}, //pool 15 <- if BT is enabled, used as BT HW shared memory
    { 0x3FFB8000, 0x8000, SOC_MEMORY_TYPE_DRAM,     0,         false}, //pool 14 <- if BT is enabled, used data memory for BT ROM functions.
    { 0x3FFC0000, 0x2000, SOC_MEMORY_TYPE_DRAM,     0,         false}, //pool 10-13, mmu page 0
    { 0x3FFC2000, 0x2000, SOC_MEMORY_TYPE_DRAM,     0,         false}, //pool 10-13, mmu page 1
    { 0x3FFC4000, 0x2000, SOC_MEMORY_TYPE_DRAM,     0,         false}, //pool 10-13, mmu page 2
    { 0x3FFC6000, 0x2000, SOC_MEMORY_TYPE_DRAM,     0,         false}, //pool 10-13, mmu page 3
    { 0x3FFC8000, 0x2000, SOC_MEMORY_TYPE_DRAM,     0,         false}, //pool 10-13, mmu page 4
    { 0x3FFCA000, 0x2000, SOC_MEMORY_TYPE_DRAM,     0,         false}, //pool 10-13, mmu page 5
    { 0x3FFCC000, 0x2000, SOC_MEMORY_TYPE_DRAM,     0,         false}, //pool 10-13, mmu page 6
    { 0x3FFCE000, 0x2000, SOC_MEMORY_TYPE_DRAM,     0,         false}, //pool 10-13, mmu page 7
    { 0x3FFD0000, 0x2000, SOC_MEMORY_TYPE_DRAM,     0,         false}, //pool 10-13, mmu page 8
    { 0x3FFD2000, 0x2000, SOC_MEMORY_TYPE_DRAM,     0,         false}, //pool 10-13, mmu page 9
    { 0x3FFD4000, 0x2000, SOC_MEMORY_TYPE_DRAM,     0,         false}, //pool 10-13, mmu page 10
    { 0x3FFD6000, 0x2000, SOC_MEMORY_TYPE_DRAM,     0,         false}, //pool 10-13, mmu page 11
    { 0x3FFD8000, 0x2000, SOC_MEMORY_TYPE_DRAM,     0,         false}, //pool 10-13, mmu page 12
    { 0x3FFDA000, 0x2000, SOC_MEMORY_TYPE_DRAM,     0,         false}, //pool 10-13, mmu page 13
    { 0x3FFDC000, 0x2000, SOC_MEMORY_TYPE_DRAM,     0,         false}, //pool 10-13, mmu page 14
    { 0x3FFDE000, 0x2000, SOC_MEMORY_TYPE_DRAM,     0,         false}, //pool 10-13, mmu page 15
    { 0x3FFE0000, 0x4000, SOC_MEMORY_TYPE_DIRAM,    0x400BC000,true},  //pool 9 blk 1
    { 0x3FFE4000, 0x4000, SOC_MEMORY_TYPE_DIRAM,    0x400B8000,true},  //pool 9 blk 0
    { 0x3FFE8000, 0x8000, SOC_MEMORY_TYPE_DIRAM,    0x400B0000,true},  //pool 8 <- can be remapped to ROM, used for MAC dump
    { 0x3FFF0000, 0x8000, SOC_MEMORY_TYPE_DIRAM,    0x400A8000,true},  //pool 7 <- can be used for MAC dump
    { 0x3FFF8000, 0x4000, SOC_MEMORY_TYPE_DIRAM,    0x400A4000,true},  //pool 6 blk 1 <- can be used as trace memory
    { 0x3FFFC000, 0x4000, SOC_MEMORY_TYPE_DIRAM,    0x400A0000,true},  //pool 6 blk 0 <- can be used as trace memory
    { 0x40070000, 0x8000, SOC_MEMORY_TYPE_IRAM,     0,         false}, //pool 0
    { 0x40078000, 0x8000, SOC_MEMORY_TYPE_IRAM,     0,         false}, //pool 1
    { 0x40080000, 0x2000, SOC_MEMORY_TYPE_IRAM,     0,         false}, //pool 2-5, mmu page 0
    { 0x40082000, 0x2000, SOC_MEMORY_TYPE_IRAM,     0,         false}, //pool 2-5, mmu page 1
    { 0x40084000, 0x2000, SOC_MEMORY_TYPE_IRAM,     0,         false}, //pool 2-5, mmu page 2
    { 0x40086000, 0x2000, SOC_MEMORY_TYPE_IRAM,     0,         false}, //pool 2-5, mmu page 3
    { 0x40088000, 0x2000, SOC_MEMORY_TYPE_IRAM,     0,         false}, //pool 2-5, mmu page 4
    { 0x4008A000, 0x2000, SOC_MEMORY_TYPE_IRAM,     0,         false}, //pool 2-5, mmu page 5
    { 0x4008C000, 0x2000, SOC_MEMORY_TYPE_IRAM,     0,         false}, //pool 2-5, mmu page 6
    { 0x4008E000, 0x2000, SOC_MEMORY_TYPE_IRAM,     0,         false}, //pool 2-5, mmu page 7
    { 0x40090000, 0x2000, SOC_MEMORY_TYPE_IRAM,     0,         false}, //pool 2-5, mmu page 8
    { 0x40092000, 0x2000, SOC_MEMORY_TYPE_IRAM,     0,         false}, //pool 2-5, mmu page 9
    { 0x40094000, 0x2000, SOC_MEMORY_TYPE_IRAM,     0,         false}, //pool 2-5, mmu page 10
    { 0x40096000, 0x2000, SOC_MEMORY_TYPE_IRAM,     0,         false}, //pool 2-5, mmu page 11
    { 0x40098000, 0x2000, SOC_MEMORY_TYPE_IRAM,     0,         false}, //pool 2-5, mmu page 12
    { 0x4009A000, 0x2000, SOC_MEMORY_TYPE_IRAM,     0,         false}, //pool 2-5, mmu page 13
    { 0x4009C000, 0x2000, SOC_MEMORY_TYPE_IRAM,     0,         false}, //pool 2-5, mmu page 14
    { 0x4009E000, 0x2000, SOC_MEMORY_TYPE_IRAM,     0,         false}, //pool 2-5, mmu page 15
#ifdef CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP
    { SOC_RTC_DRAM_LOW, 0x2000, SOC_MEMORY_TYPE_RTCRAM, 0, false}, //RTC Fast Memory
#endif
};

const size_t soc_memory_region_count = sizeof(soc_memory_regions)/sizeof(soc_memory_region_t);


/* Reserved memory regions

   These are removed from the soc_memory_regions array when heaps are created.
 */
SOC_RESERVE_MEMORY_REGION(SOC_CACHE_PRO_LOW, SOC_CACHE_PRO_HIGH, cpu0_cache);
#ifndef CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
SOC_RESERVE_MEMORY_REGION(SOC_CACHE_APP_LOW, SOC_CACHE_APP_HIGH, cpu1_cache);
#endif

    /* Warning: The ROM stack is located in the 0x3ffe0000 area. We do not specifically disable that area here because
       after the scheduler has started, the ROM stack is not used anymore by anything. We handle it instead by not allowing
       any mallocs memory regions with the startup_stack flag set (these are the IRAM/DRAM region) until the
       scheduler has started.

       The 0x3ffe0000 region also contains static RAM for various ROM functions. The following lines
       reserve the regions for UART and ETSC, so these functions are usable. Libraries like xtos, which are
       not usable in FreeRTOS anyway, are commented out in the linker script so they cannot be used; we
       do not disable their memory regions here and they will be used as general purpose heap memory.

       Enabling the heap allocator for this region but disabling allocation here until FreeRTOS is started up
       is a somewhat risky action in theory, because on initializing the allocator, the multi_heap implementation
       will go and write metadata at the start and end of all regions. For the ESP32, these linked
       list entries happen to end up in a region that is not touched by the stack; they can be placed safely there.
    */

SOC_RESERVE_MEMORY_REGION(0x3ffe0000, 0x3ffe0440, rom_pro_data); //Reserve ROM PRO data region
#ifndef CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
SOC_RESERVE_MEMORY_REGION(0x3ffe3f20, 0x3ffe4350, rom_app_data); //Reserve ROM APP data region
#endif

SOC_RESERVE_MEMORY_REGION(0x3ffae000, 0x3ffae6e0, rom_data);

#if CONFIG_ESP32_MEMMAP_TRACEMEM
#if CONFIG_ESP32_MEMMAP_TRACEMEM_TWOBANKS
SOC_RESERVE_MEMORY_REGION(0x3fff8000, 0x40000000, trace_mem); //Reserve trace mem region, 32K for both cpu
#else
SOC_RESERVE_MEMORY_REGION(0x3fffc000, 0x40000000, trace_mem); //Reserve trace mem region, 16K (upper-half) for pro cpu
#endif
#endif

#ifdef CONFIG_SPIRAM
/* Reserve the whole possible SPIRAM region here, spiram.c will add some or all of this
 * memory to heap depending on the actual SPIRAM chip size. */
SOC_RESERVE_MEMORY_REGION(SOC_EXTRAM_DATA_LOW, SOC_EXTRAM_DATA_HIGH, spi_ram);
#endif

extern int _data_start, _heap_start, _heap_end, _iram_start, _iram_end, _rtc_force_fast_end, _rtc_noinit_end;
extern int _rtc_fast_reserved_start, _rtc_fast_reserved_end;
extern int _rtc_slow_reserved_start, _rtc_slow_reserved_end;

// Static data region. DRAM used by data+bss and possibly rodata
SOC_RESERVE_MEMORY_REGION((intptr_t)&_data_start, (intptr_t)&_heap_start, dram_data);

// IRAM code region
// ESP32 has an IRAM-only region 0x4008_0000 - 0x4009_FFFF, reserve the used part
SOC_RESERVE_MEMORY_REGION((intptr_t)&_iram_start, (intptr_t)&_iram_end, iram_code);

// If IRAM spans into SRAM1 due to CONFIG_ESP_SYSTEM_ESP32_SRAM1_REGION_AS_IRAM, reserve the corresponding part of DRAM
#ifdef CONFIG_ESP_SYSTEM_ESP32_SRAM1_REGION_AS_IRAM
SOC_RESERVE_MEMORY_REGION((intptr_t) &_heap_end, 0x40000000, sram1_iram);
#endif

// RTC Fast RAM region
#ifdef CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP
#ifdef CONFIG_ESP32_RTCDATA_IN_FAST_MEM
SOC_RESERVE_MEMORY_REGION(SOC_RTC_DRAM_LOW, (intptr_t)&_rtc_noinit_end, rtcram_data);
#else
SOC_RESERVE_MEMORY_REGION(SOC_RTC_DRAM_LOW, (intptr_t)&_rtc_force_fast_end, rtcram_data);
#endif
#endif

SOC_RESERVE_MEMORY_REGION((intptr_t)&_rtc_fast_reserved_start, (intptr_t)&_rtc_fast_reserved_end, rtc_fast_reserved_data);
SOC_RESERVE_MEMORY_REGION((intptr_t)&_rtc_slow_reserved_start, (intptr_t)&_rtc_slow_reserved_end, rtc_reserved_data);
