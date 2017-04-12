// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef ESP_SPI_FLASH_CACHE_UTILS_H
#define ESP_SPI_FLASH_CACHE_UTILS_H

/**
 * This header file contains declarations of cache manipulation functions
 * used both in flash_ops.c and flash_mmap.c.
 *
 * These functions are considered internal and are not designed to be called from applications.
 */

// Init mutex protecting access to spi_flash_* APIs
void spi_flash_init_lock();

// Take mutex protecting access to spi_flash_* APIs
void spi_flash_op_lock();

// Release said mutex
void spi_flash_op_unlock();

// Suspend the scheduler on both CPUs, disable cache.
// Contrary to its name this doesn't do anything with interrupts, yet.
// Interrupt disabling capability will be added once we implement
// interrupt allocation API.
void spi_flash_disable_interrupts_caches_and_other_cpu();

// Enable cache, enable interrupts (to be added in future), resume scheduler
void spi_flash_enable_interrupts_caches_and_other_cpu();

// Disables non-IRAM interrupt handlers on current CPU and caches on both CPUs.
// This function is implied to be called when other CPU is not running or running code from IRAM.
void spi_flash_disable_interrupts_caches_and_other_cpu_no_os();

// Enable cache, enable interrupts on current CPU.
// This function is implied to be called when other CPU is not running or running code from IRAM.
void spi_flash_enable_interrupts_caches_no_os();

// Mark the pages containing a flash region as having been
// erased or written to. This means the flash cache needs
// to be evicted before these pages can be flash_mmap()ed again,
// as they may contain stale data
//
// Only call this while holding spi_flash_op_lock()
void spi_flash_mark_modified_region(uint32_t start_addr, uint32_t length);

#endif //ESP_SPI_FLASH_CACHE_UTILS_H
