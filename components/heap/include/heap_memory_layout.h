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
#include "sdkconfig.h"

#define SOC_MEMORY_TYPE_NO_PRIOS 3

#ifdef __cplusplus
extern "C" {
#endif

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
typedef struct {
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
typedef struct {
    intptr_t start;
    intptr_t end;
} soc_reserved_region_t;

/* Use this macro to reserved a fixed region of RAM (hardcoded addresses)
 * for a particular purpose.
 *
 * Usually used to mark out memory addresses needed for hardware or ROM code
 * purposes.
 *
 * Don't call this macro from user code which can use normal C static allocation
 * instead.
 *
 * @param START Start address to be reserved.
 * @param END One after the address of the last byte to be reserved. (ie length of
 * the reserved region is (END - START) in bytes.
 * @param NAME Name for the reserved region. Must be a valid variable name,
 * unique to this source file.
 */
#define SOC_RESERVE_MEMORY_REGION(START, END, NAME)     \
    __attribute__((section(".reserved_memory_address"))) __attribute__((used)) \
    static soc_reserved_region_t reserved_region_##NAME = { START, END };

/* Return available memory regions for this SoC. Each available memory
 * region is a contiguous piece of memory which is not being used by
 * static data, used by ROM code, or reserved by a component using
 * the SOC_RESERVE_MEMORY_REGION() macro.
 *
 * This result is soc_memory_regions[] minus all regions reserved
 * via the SOC_RESERVE_MEMORY_REGION() macro (which may also split
 * some regions up.)
 *
 * At startup, all available memory returned by this function is
 * registered as heap space.
 *
 * @note OS-level startup function only, not recommended to call from
 * app code.
 *
 * @param regions Pointer to an array for reading available regions into.
 * Size of the array should be at least the result of
 * soc_get_available_memory_region_max_count(). Entries in the array
 * will be ordered by memory address.
 *
 * @return Number of entries copied to 'regions'. Will be no greater than
 * the result of soc_get_available_memory_region_max_count().
 */
size_t soc_get_available_memory_regions(soc_memory_region_t *regions);

/* Return the maximum number of available memory regions which could be
 * returned by soc_get_available_memory_regions(). Used to size the
 * array passed to that function.
 */
size_t soc_get_available_memory_region_max_count(void);


#ifdef __cplusplus
}
#endif
