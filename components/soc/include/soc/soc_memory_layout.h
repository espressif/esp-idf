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
#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "soc/soc.h"
#include "sdkconfig.h"
#include "esp_attr.h"

#define SOC_MEMORY_TYPE_NO_PRIOS 3

/* Type descriptor holds a description for a particular type of memory on a particular SoC.
 */
typedef struct {
    const char *name;  ///< Name of this memory type
    uint32_t caps[SOC_MEMORY_TYPE_NO_PRIOS]; ///< Capabilities for this memory type (as a prioritised set)
    bool aliased_iram;   ///< If true, this is data memory that is is also mapped in IRAM
    bool startup_stack; ///< If true, memory of this type is used for ROM stack during startup
} soc_memory_type_desc_t;

/* Constant table of tag descriptors for all this SoC's tags */
extern const soc_memory_type_desc_t soc_memory_types[];
extern const size_t soc_memory_type_count;

/* Region descriptor holds a description for a particular region of memory on a particular SoC.
 */
typedef struct
{
    intptr_t start;  ///< Start address of the region
    size_t size;            ///< Size of the region in bytes
    size_t type;             ///< Type of the region (index into soc_memory_types array)
    intptr_t iram_address; ///< If non-zero, is equivalent address in IRAM
} soc_memory_region_t;

extern const soc_memory_region_t soc_memory_regions[];
extern const size_t soc_memory_region_count;

/* Region descriptor holds a description for a particular region of
   memory reserved on this SoC for a particular use (ie not available
   for stack/heap usage.) */
typedef struct
{
    intptr_t start;
    intptr_t end;
} soc_reserved_region_t;

extern const soc_reserved_region_t soc_reserved_regions[];
extern const size_t soc_reserved_region_count;

inline static bool IRAM_ATTR esp_ptr_dma_capable(const void *p)
{
    return (intptr_t)p >= SOC_DMA_LOW && (intptr_t)p < SOC_DMA_HIGH;
}

inline static bool IRAM_ATTR esp_ptr_executable(const void *p)
{
    intptr_t ip = (intptr_t) p;
    return (ip >= SOC_IROM_LOW && ip < SOC_IROM_HIGH)
        || (ip >= SOC_IRAM_LOW && ip < SOC_IRAM_HIGH)
        || (ip >= SOC_RTC_IRAM_LOW && ip < SOC_RTC_IRAM_HIGH);
}

inline static bool IRAM_ATTR esp_ptr_byte_accessible(const void *p)
{
    bool r;
    r = ((intptr_t)p >= SOC_BYTE_ACCESSIBLE_LOW && (intptr_t)p < SOC_BYTE_ACCESSIBLE_HIGH);
#if CONFIG_SPIRAM_SUPPORT
    r |= ((intptr_t)p >= SOC_EXTRAM_DATA_LOW && (intptr_t)p < SOC_EXTRAM_DATA_HIGH);
#endif
    return r;
}

inline static bool IRAM_ATTR esp_ptr_internal(const void *p) {
    bool r;
    r = ((intptr_t)p >= SOC_MEM_INTERNAL_LOW && (intptr_t)p < SOC_MEM_INTERNAL_HIGH);
    r |= ((intptr_t)p >= SOC_RTC_DATA_LOW && (intptr_t)p < SOC_RTC_DATA_HIGH);
    return r;
}
