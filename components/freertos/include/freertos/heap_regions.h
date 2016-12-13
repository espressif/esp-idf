// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#ifndef _HEAP_REGIONS_H
#define _HEAP_REGIONS_H

#include "freertos/FreeRTOS.h"

/* The maximum amount of tags in use */
#define HEAPREGIONS_MAX_TAGCOUNT 16

/**
 * @brief Structure to define a memory region
 */
typedef struct HeapRegionTagged
{
    uint8_t *pucStartAddress;       ///< Start address of the region
    size_t xSizeInBytes;            ///< Size of the region
    BaseType_t xTag;                ///< Tag for the region
    uint32_t xExecAddr;             ///< If non-zero, indicates the region also has an alias in IRAM.
} HeapRegionTagged_t;

/**
 * @brief Initialize the heap allocator by feeding it the usable memory regions and their tags.
 *
 * This takes an array of heapRegionTagged_t structs, the last entry of which is a dummy entry
 * which has pucStartAddress set to NULL. It will initialize the heap allocator to serve memory
 * from these ranges.
 *
 * @param  pxHeapRegions Array of region definitions
 */

void vPortDefineHeapRegionsTagged( const HeapRegionTagged_t * const pxHeapRegions );


/**
 * @brief Allocate memory from a region with a certain tag
 *
 * Like pvPortMalloc, this returns an allocated chunk of memory. This function,
 * however, forces the allocator to allocate from a region specified by a
 * specific tag.
 *
 * @param  xWantedSize Size needed, in bytes
 * @param  tag Tag of the memory region the allocation has to be from
 *
 * @return Pointer to allocated memory if succesful.
 *         NULL if unsuccesful.
 */
void *pvPortMallocTagged( size_t xWantedSize, BaseType_t tag );

/**
 * @brief Free memory allocated with pvPortMallocTagged
 *
 * This is basically an implementation of free().
 *
 * @param  pv Pointer to region allocated by pvPortMallocTagged
 */
void vPortFreeTagged( void *pv );

/**
 * @brief Get the lowest amount of memory free for a certain tag
 *
 * This function allows the user to see what the least amount of
 * free memory for a certain tag is.
 * 
 * @param  tag Tag of the memory region
 *
 * @return Minimum amount of free bytes available in the runtime of
 *         the program
 */
size_t xPortGetMinimumEverFreeHeapSizeTagged( BaseType_t tag );

/**
 * @brief Get the amount of free bytes in a certain tagged region
 *
 * Works like xPortGetFreeHeapSize but allows the user to specify
 * a specific tag
 * 
 * @param  tag Tag of the memory region
 *
 * @return Remaining amount of free bytes in region
 */
size_t xPortGetFreeHeapSizeTagged( BaseType_t tag );


#endif