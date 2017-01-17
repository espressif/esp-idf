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

#ifndef ESP_SPI_FLASH_H
#define ESP_SPI_FLASH_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "esp_err.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_ERR_FLASH_BASE       0x10010
#define ESP_ERR_FLASH_OP_FAIL    (ESP_ERR_FLASH_BASE + 1)
#define ESP_ERR_FLASH_OP_TIMEOUT (ESP_ERR_FLASH_BASE + 2)

#define SPI_FLASH_SEC_SIZE  4096    /**< SPI Flash sector size */

/**
 * @brief  Initialize SPI flash access driver
 *
 *  This function must be called exactly once, before any other 
 *  spi_flash_* functions are called.
 *  Currently this function is called from startup code. There is
 *  no need to call it from application code.
 *
 */
void spi_flash_init();

/**
 * @brief  Get flash chip size, as set in binary image header
 *
 * @note This value does not necessarily match real flash size.
 *
 * @return size of flash chip, in bytes
 */
size_t spi_flash_get_chip_size();

/**
 * @brief  Erase the Flash sector.
 *
 * @param  sector  Sector number, the count starts at sector 0, 4KB per sector.
 *
 * @return esp_err_t
 */
esp_err_t spi_flash_erase_sector(size_t sector);

/**
 * @brief  Erase a range of flash sectors
 *
 * @param  start_address  Address where erase operation has to start.
 *                                  Must be 4kB-aligned
 * @param  size  Size of erased range, in bytes. Must be divisible by 4kB.
 *
 * @return esp_err_t
 */
esp_err_t spi_flash_erase_range(size_t start_address, size_t size);


/**
 * @brief  Write data to Flash.
 *
 * @note If source address is in DROM, this function will return
 *       ESP_ERR_INVALID_ARG.
 *
 * @param  dest_addr destination address in Flash. Must be a multiple of 4 bytes.
 * @param  src       pointer to the source buffer.
 * @param  size      length of data, in bytes. Must be a multiple of 4 bytes.
 *
 * @return esp_err_t
 */
esp_err_t spi_flash_write(size_t dest_addr, const void *src, size_t size);


/**
 * @brief  Write data encrypted to Flash.
 *
 * @note Flash encryption must be enabled for this function to work.
 *
 * @note Address in flash, dest, has to be 32-byte aligned.
 *
 * @note If source address is in DROM, this function will return
 *       ESP_ERR_INVALID_ARG.
 *
 * @param  dest_addr destination address in Flash. Must be a multiple of 32 bytes.
 * @param  src       pointer to the source buffer.
 * @param  size      length of data, in bytes. Must be a multiple of 32 bytes.
 *
 * @return esp_err_t
 */
esp_err_t spi_flash_write_encrypted(size_t dest_addr, const void *src, size_t size);

/**
 * @brief  Read data from Flash.
 *
 * @param  src_addr source address of the data in Flash.
 * @param  dest     pointer to the destination buffer
 * @param  size     length of data
 *
 * @return esp_err_t
 */
esp_err_t spi_flash_read(size_t src_addr, void *dest, size_t size);

/**
 * @brief Enumeration which specifies memory space requested in an mmap call
 */
typedef enum {
    SPI_FLASH_MMAP_DATA,    /**< map to data memory (Vaddr0), allows byte-aligned access, 4 MB total */
    SPI_FLASH_MMAP_INST,    /**< map to instruction memory (Vaddr1-3), allows only 4-byte-aligned access, 11 MB total */
} spi_flash_mmap_memory_t;

/**
 * @brief Opaque handle for memory region obtained from spi_flash_mmap.
 */
typedef uint32_t spi_flash_mmap_handle_t;

/**
 * @brief Map region of flash memory into data or instruction address space
 *
 * This function allocates sufficient number of 64k MMU pages and configures
 * them to map request region of flash memory into data address space or into
 * instruction address space. It may reuse MMU pages which already provide
 * required mapping. As with any allocator, there is possibility of fragmentation
 * of address space if mmap/munmap are heavily used. To troubleshoot issues with
 * page allocation, use spi_flash_mmap_dump function.
 *
 * @param src_addr  Physical address in flash where requested region starts.
 *                  This address *must* be aligned to 64kB boundary.
 * @param size  Size of region which has to be mapped. This size will be rounded
 *              up to a 64k boundary.
 * @param memory  Memory space where the region should be mapped
 * @param out_ptr  Output, pointer to the mapped memory region
 * @param out_handle  Output, handle which should be used for spi_flash_munmap call
 *
 * @return  ESP_OK on success, ESP_ERR_NO_MEM if pages can not be allocated
 */
esp_err_t spi_flash_mmap(size_t src_addr, size_t size, spi_flash_mmap_memory_t memory,
                         const void** out_ptr, spi_flash_mmap_handle_t* out_handle);

/**
 * @brief Release region previously obtained using spi_flash_mmap
 *
 * @note Calling this function will not necessarily unmap memory region.
 *       Region will only be unmapped when there are no other handles which
 *       reference this region. In case of partially overlapping regions
 *       it is possible that memory will be unmapped partially.
 *
 * @param handle  Handle obtained from spi_flash_mmap
 */
void spi_flash_munmap(spi_flash_mmap_handle_t handle);

/**
 * @brief Display information about mapped regions
 *
 * This function lists handles obtained using spi_flash_mmap, along with range
 * of pages allocated to each handle. It also lists all non-zero entries of
 * MMU table and corresponding reference counts.
 */
void spi_flash_mmap_dump();

/**
 * @brief SPI flash critical section enter function.
 */
typedef void (*spi_flash_guard_start_func_t)(void);
/**
 * @brief SPI flash critical section exit function.
 */
typedef void (*spi_flash_guard_end_func_t)(void);

/**
 * Structure holding SPI flash access critical section management functions
 *
 * @note Structure and corresponding guard functions should not reside in flash.
 *       For example structure can be placed in DRAM and functions in IRAM sections.
 */
typedef struct {
    spi_flash_guard_start_func_t    start;  /**< critical section start func */
    spi_flash_guard_end_func_t      end;    /**< critical section end func */
} spi_flash_guard_funcs_t;

/**
 * @brief  Sets guard functions to access flash.
 *
 * @note Pointed structure and corresponding guard functions should not reside in flash.
 *       For example structure can be placed in DRAM and functions in IRAM sections.
 *
 * @param funcs pointer to structure holding flash access guard functions.
 */
void spi_flash_guard_set(const spi_flash_guard_funcs_t* funcs);

/**
 * @brief Default OS-aware flash access guard functions
 */
extern const spi_flash_guard_funcs_t g_flash_guard_default_ops;

/**
 * @brief Non-OS flash access guard functions
 *
 * @note This version of flash guard functions is to be used when no OS is present or from panic handler.
 *       It does not use any OS primitives and IPC and implies that only calling CPU is active.
 */
extern const spi_flash_guard_funcs_t g_flash_guard_no_os_ops;

#if CONFIG_SPI_FLASH_ENABLE_COUNTERS

/**
 * Structure holding statistics for one type of operation
 */
typedef struct {
    uint32_t count;     // number of times operation was executed
    uint32_t time;      // total time taken, in microseconds
    uint32_t bytes;     // total number of bytes
} spi_flash_counter_t;

typedef struct {
    spi_flash_counter_t read;
    spi_flash_counter_t write;
    spi_flash_counter_t erase;
} spi_flash_counters_t;

/**
 * @brief  Reset SPI flash operation counters
 */
void spi_flash_reset_counters();

/**
 * @brief  Print SPI flash operation counters
 */
void spi_flash_dump_counters();

/**
 * @brief  Return current SPI flash operation counters
 *
 * @return  pointer to the spi_flash_counters_t structure holding values
 *          of the operation counters
 */
const spi_flash_counters_t* spi_flash_get_counters();

#endif //CONFIG_SPI_FLASH_ENABLE_COUNTERS

#ifdef __cplusplus
}
#endif


#endif /* ESP_SPI_FLASH_H */

