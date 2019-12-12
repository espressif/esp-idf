// Copyright 2010-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef BOOTLOADER_BUILD

#include <stdlib.h>
#include <stdint.h>

#include "soc/soc.h"
#include "soc/soc_memory_layout.h"
#include "esp_heap_caps.h"
#include "sdkconfig.h"

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
const soc_memory_type_desc_t soc_memory_types[] = {
    //Type 0: DRAM
    { "DRAM", { MALLOC_CAP_8BIT|MALLOC_CAP_DEFAULT, MALLOC_CAP_INTERNAL|MALLOC_CAP_DMA|MALLOC_CAP_32BIT, 0 }, false, false},
    // Type 1: DRAM used for startup stacks
    { "DRAM", { MALLOC_CAP_8BIT|MALLOC_CAP_DEFAULT, MALLOC_CAP_INTERNAL|MALLOC_CAP_DMA|MALLOC_CAP_32BIT, 0 }, false, true},
    //Type 2: DRAM which has an alias on the I-port
    { "D/IRAM", { 0, MALLOC_CAP_DMA|MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL|MALLOC_CAP_DEFAULT, MALLOC_CAP_32BIT|MALLOC_CAP_EXEC }, true, false},
    //Type 3: IRAM
    //In ESP32S2beta, All IRAM region are available by D-port (D/IRAM).
    { "IRAM", { MALLOC_CAP_EXEC|MALLOC_CAP_32BIT|MALLOC_CAP_INTERNAL, 0, 0 }, false, false},
    //Type 4: SPI SRAM data
    //TODO, in fact, part of them support EDMA, to be supported.
    { "SPIRAM", { MALLOC_CAP_SPIRAM|MALLOC_CAP_DEFAULT, 0, MALLOC_CAP_8BIT|MALLOC_CAP_32BIT}, false, false},
    //Type 5: SPI SRAM data from AHB DBUS3, slower than normal
    //TODO, add a bit to control the access of it
#if CONFIG_USE_AHB_DBUS3_ACCESS_SPIRAM
    { "SPIRAM(Slow)", { MALLOC_CAP_SPIRAM|MALLOC_CAP_DEFAULT, 0, MALLOC_CAP_8BIT|MALLOC_CAP_32BIT}, false, false},
#endif
};

const size_t soc_memory_type_count = sizeof(soc_memory_types)/sizeof(soc_memory_type_desc_t);

/*
Region descriptors. These describe all regions of memory available, and map them to a type in the above type.

Because of requirements in the coalescing code which merges adjacent regions, this list should always be sorted
from low to high start address.
*/
const soc_memory_region_t soc_memory_regions[] = {
#ifdef CONFIG_SPIRAM
    { SOC_EXTRAM_DATA_LOW, SOC_EXTRAM_DATA_HIGH - SOC_EXTRAM_DATA_LOW, 4, 0}, //SPI SRAM, if available
#if CONFIG_USE_AHB_DBUS3_ACCESS_SPIRAM
    { SOC_SLOW_EXTRAM_DATA_LOW, SOC_SLOW_EXTRAM_DATA_HIGH - SOC_SLOW_EXTRAM_DATA_LOW, 5, 0}, //SPI SRAM, if available
#endif
#endif
#if CONFIG_ESP32S2_INSTRUCTION_CACHE_8KB
#if CONFIG_ESP32S2_DATA_CACHE_0KB
    { 0x3FFB2000, 0x2000, 2, 0x40022000}, //Block 1, can be use as I/D cache memory
    { 0x3FFB4000, 0x2000, 2, 0x40024000}, //Block 2, can be use as D cache memory
    { 0x3FFB6000, 0x2000, 2, 0x40026000}, //Block 3, can be use as D cache memory
#elif CONFIG_ESP32S2_DATA_CACHE_8KB
    { 0x3FFB4000, 0x2000, 2, 0x40024000}, //Block 2, can be use as D cache memory
    { 0x3FFB6000, 0x2000, 2, 0x40026000}, //Block 3, can be use as D cache memory
#else
    { 0x3FFB6000, 0x2000, 2, 0x40026000}, //Block 3, can be use as D cache memory
#endif
#else
#if CONFIG_ESP32S2_DATA_CACHE_0KB
    { 0x3FFB4000, 0x2000, 2, 0x40024000}, //Block 2, can be use as D cache memory
    { 0x3FFB6000, 0x2000, 2, 0x40026000}, //Block 3, can be use as D cache memory
#elif CONFIG_ESP32S2_DATA_CACHE_8KB
    { 0x3FFB6000, 0x2000, 2, 0x40026000}, //Block 3, can be use as D cache memory
#endif
#endif
    { 0x3FFB8000, 0x4000, 2, 0x40028000}, //Block 4,  can be remapped to ROM, can be used as trace memory
    { 0x3FFBC000, 0x4000, 2, 0x4002C000}, //Block 5,  can be remapped to ROM, can be used as trace memory
    { 0x3FFC0000, 0x4000, 2, 0x40030000}, //Block 6,  can be used as trace memory
    { 0x3FFC4000, 0x4000, 2, 0x40034000}, //Block 7,  can be used as trace memory
    { 0x3FFC8000, 0x4000, 2, 0x40038000}, //Block 8,  can be used as trace memory
    { 0x3FFCC000, 0x4000, 2, 0x4003C000}, //Block 9,  can be used as trace memory

    { 0x3FFD0000, 0x4000, 2, 0x40040000}, //Block 10,  can be used as trace memory
    { 0x3FFD4000, 0x4000, 2, 0x40044000}, //Block 11,  can be used as trace memory
    { 0x3FFD8000, 0x4000, 2, 0x40048000}, //Block 12,  can be used as trace memory
    { 0x3FFDC000, 0x4000, 2, 0x4004C000}, //Block 13,  can be used as trace memory
    { 0x3FFE0000, 0x4000, 2, 0x40050000}, //Block 14,  can be used as trace memory
    { 0x3FFE4000, 0x4000, 2, 0x40054000}, //Block 15,  can be used as trace memory
    { 0x3FFE8000, 0x4000, 2, 0x40058000}, //Block 16,  can be used as trace memory
    { 0x3FFEC000, 0x4000, 2, 0x4005C000}, //Block 17,  can be used as trace memory
    { 0x3FFF0000, 0x4000, 2, 0x40060000}, //Block 18,  can be used for MAC dump, can be used as trace memory
    { 0x3FFF4000, 0x4000, 2, 0x40064000}, //Block 19,  can be used for MAC dump, can be used as trace memory
    { 0x3FFF8000, 0x4000, 2, 0x40068000}, //Block 20,  can be used for MAC dump, can be used as trace memory
    { 0x3FFFC000, 0x4000, 1, 0x4006C000}, //Block 21,  can be used for MAC dump, can be used as trace memory, used for startup stack
};

const size_t soc_memory_region_count = sizeof(soc_memory_regions)/sizeof(soc_memory_region_t);


extern int _data_start_xtos;
/* Reserved memory regions

   These are removed from the soc_memory_regions array when heaps are created.
 */
//ROM data region
SOC_RESERVE_MEMORY_REGION(0x3fffc000, (intptr_t)&_data_start_xtos, rom_data_region);

// TODO: soc_memory_layout: handle trace memory regions - IDF-750

#ifdef CONFIG_SPIRAM
SOC_RESERVE_MEMORY_REGION( SOC_EXTRAM_DATA_LOW, SOC_EXTRAM_DATA_HIGH, extram_data_region); //SPI RAM gets added later if needed, in spiram.c; reserve it for now
#if CONFIG_USE_AHB_DBUS3_ACCESS_SPIRAM
SOC_RESERVE_MEMORY_REGION( SOC_SLOW_EXTRAM_DATA_LOW, SOC_SLOW_EXTRAM_DATA_HIGH, extram_slow_data_region); //SPI RAM(Slow) gets added later if needed, in spiram.c; reserve it for now
#endif
#endif


#endif
