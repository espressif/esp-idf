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
#ifndef HEAP_ALLOC_CAPS_H
#define HEAP_ALLOC_CAPS_H

/**
 * @brief Flags to indicate the capabilities of the various memory systems
 */
#define MALLOC_CAP_EXEC             (1<<0)  ///< Memory must be able to run executable code
#define MALLOC_CAP_32BIT            (1<<1)  ///< Memory must allow for aligned 32-bit data accesses
#define MALLOC_CAP_8BIT             (1<<2)  ///< Memory must allow for 8/16/...-bit data accesses
#define MALLOC_CAP_DMA              (1<<3)  ///< Memory must be able to accessed by DMA
#define MALLOC_CAP_PID2             (1<<4)  ///< Memory must be mapped to PID2 memory space
#define MALLOC_CAP_PID3             (1<<5)  ///< Memory must be mapped to PID3 memory space
#define MALLOC_CAP_PID4             (1<<6)  ///< Memory must be mapped to PID4 memory space
#define MALLOC_CAP_PID5             (1<<7)  ///< Memory must be mapped to PID5 memory space
#define MALLOC_CAP_PID6             (1<<8)  ///< Memory must be mapped to PID6 memory space
#define MALLOC_CAP_PID7             (1<<9)  ///< Memory must be mapped to PID7 memory space
#define MALLOC_CAP_SPISRAM          (1<<10) ///< Memory must be in SPI SRAM
#define MALLOC_CAP_INVALID          (1<<31) ///< Memory can't be used / list end marker


/**
 * @brief Initialize the capability-aware heap allocator.
 *
 * For the ESP32, this is called once in the startup code.
 */
void heap_alloc_caps_init();

/**
 * @brief Enable the memory region where the startup stacks are located for allocation
 *
 * On startup, the pro/app CPUs have a certain memory region they use as stack, so we 
 * cannot do allocations in the regions these stack frames are. When FreeRTOS is 
 * completely started, they do not use that memory anymore and allocation there can 
 * be re-enabled.
 */
void heap_alloc_enable_nonos_stack_tag();

/**
 * @brief Allocate a chunk of memory which has the given capabilities
 *
 * @param xWantedSize Size, in bytes, of the amount of memory to allocate
 * @param caps        Bitwise OR of MALLOC_CAP_* flags indicating the type
 *                    of memory to be returned
 *
 * @return A pointer to the memory allocated on success, NULL on failure
 */
void *pvPortMallocCaps(size_t xWantedSize, uint32_t caps);

/**
 * @brief Get the total free size of all the regions that have the given capabilities
 *
 * This function takes all regions capable of having the given capabilities allocated in them
 * and adds up the free space they have.
 *
 * @param caps        Bitwise OR of MALLOC_CAP_* flags indicating the type
 *                    of memory
 *
 * @return Amount of free bytes in the regions
 */
size_t xPortGetFreeHeapSizeCaps( uint32_t caps );

/**
 * @brief Get the total minimum free memory of all regions with the given capabilities
 *
 * This adds all the lowmarks of the regions capable of delivering the memory with the 
 * given capabilities
 *
 * @param caps        Bitwise OR of MALLOC_CAP_* flags indicating the type
 *                    of memory
 *
 * @return Amount of free bytes in the regions
 */
size_t xPortGetMinimumEverFreeHeapSizeCaps( uint32_t caps );



#endif