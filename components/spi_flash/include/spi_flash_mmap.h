/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * This file contains `spi_flash_mmap_xx` APIs, mainly for doing memory mapping
 * to an SPI0-connected external Flash, as well as some helper functions to
 * convert between virtual and physical address
 **/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "esp_err.h"
#include "sdkconfig.h"
#include "esp_spi_flash_counters.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_ERR_FLASH_OP_FAIL    (ESP_ERR_FLASH_BASE + 1)
#define ESP_ERR_FLASH_OP_TIMEOUT (ESP_ERR_FLASH_BASE + 2)

#define SPI_FLASH_SEC_SIZE  4096    /**< SPI Flash sector size */

#define SPI_FLASH_MMU_PAGE_SIZE CONFIG_MMU_PAGE_SIZE /**< Flash cache MMU mapping page size */

/**
 * @brief Enumeration which specifies memory space requested in an mmap call
 */
typedef enum {
    SPI_FLASH_MMAP_DATA,    /**< map to data memory, allows byte-aligned access*/
    SPI_FLASH_MMAP_INST,    /**< map to instruction memory, allows only 4-byte-aligned access*/
} spi_flash_mmap_memory_t;

/**
 * @brief Opaque handle for memory region obtained from spi_flash_mmap.
 */
typedef uint32_t spi_flash_mmap_handle_t;

/**
 * @brief Map region of flash memory into data or instruction address space
 *
 * This function allocates sufficient number of 64kB MMU pages and configures
 * them to map the requested region of flash memory into the address space.
 * It may reuse MMU pages which already provide the required mapping.
 *
 * As with any allocator, if mmap/munmap are heavily used then the address space
 * may become fragmented. To troubleshoot issues with page allocation, use
 * spi_flash_mmap_dump() function.
 *
 * @param src_addr  Physical address in flash where requested region starts.
 *                  This address *must* be aligned to 64kB boundary
 *                  (SPI_FLASH_MMU_PAGE_SIZE)
 * @param size  Size of region to be mapped. This size will be rounded
 *              up to a 64kB boundary
 * @param memory  Address space where the region should be mapped (data or instruction)
 * @param[out] out_ptr  Output, pointer to the mapped memory region
 * @param[out] out_handle  Output, handle which should be used for spi_flash_munmap call
 *
 * @return  ESP_OK on success, ESP_ERR_NO_MEM if pages can not be allocated
 */
esp_err_t spi_flash_mmap(size_t src_addr, size_t size, spi_flash_mmap_memory_t memory,
                         const void** out_ptr, spi_flash_mmap_handle_t* out_handle);

/**
 * @brief Map sequences of pages of flash memory into data or instruction address space
 *
 * This function allocates sufficient number of 64kB MMU pages and configures
 * them to map the indicated pages of flash memory contiguously into address space.
 * In this respect, it works in a similar way as spi_flash_mmap() but it allows mapping
 * a (maybe non-contiguous) set of pages into a contiguous region of memory.
 *
 * @param pages An array of numbers indicating the 64kB pages in flash to be mapped
 *              contiguously into memory. These indicate the indexes of the 64kB pages,
 *              not the byte-size addresses as used in other functions.
 *              Array must be located in internal memory.
 * @param page_count  Number of entries in the pages array
 * @param memory  Address space where the region should be mapped (instruction or data)
 * @param[out] out_ptr  Output, pointer to the mapped memory region
 * @param[out] out_handle  Output, handle which should be used for spi_flash_munmap call
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM if pages can not be allocated
 *      - ESP_ERR_INVALID_ARG if pagecount is zero or pages array is not in
 *        internal memory
 */
esp_err_t spi_flash_mmap_pages(const int *pages, size_t page_count, spi_flash_mmap_memory_t memory,
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
void spi_flash_mmap_dump(void);

/**
 * @brief get free pages number which can be mmap
 *
 * This function will return number of free pages available in mmu table. This could be useful
 * before calling actual spi_flash_mmap (maps flash range to DCache or ICache memory) to check
 * if there is sufficient space available for mapping.
 *
 * @param memory memory type of MMU table free page
 *
 * @return number of free pages which can be mmaped
 */
uint32_t spi_flash_mmap_get_free_pages(spi_flash_mmap_memory_t memory);


#define SPI_FLASH_CACHE2PHYS_FAIL UINT32_MAX /*<! Result from spi_flash_cache2phys() if flash cache address is invalid */

/**
 * @brief Given a memory address where flash is mapped, return the corresponding physical flash offset.
 *
 * Cache address does not have have been assigned via spi_flash_mmap(), any address in memory mapped flash space can be looked up.
 *
 * @param cached Pointer to flashed cached memory.
 *
 * @return
 * - SPI_FLASH_CACHE2PHYS_FAIL If cache address is outside flash cache region, or the address is not mapped.
 * - Otherwise, returns physical offset in flash
 */
size_t spi_flash_cache2phys(const void *cached);

/** @brief Given a physical offset in flash, return the address where it is mapped in the memory space.
 *
 * Physical address does not have to have been assigned via spi_flash_mmap(), any address in flash can be looked up.
 *
 * @note Only the first matching cache address is returned. If MMU flash cache table is configured so multiple entries
 * point to the same physical address, there may be more than one cache address corresponding to that physical
 * address. It is also possible for a single physical address to be mapped to both the IROM and DROM regions.
 *
 * @note This function doesn't impose any alignment constraints, but if memory argument is SPI_FLASH_MMAP_INST and
 * phys_offs is not 4-byte aligned, then reading from the returned pointer will result in a crash.
 *
 * @param phys_offs Physical offset in flash memory to look up.
 * @param memory Address space type to look up a flash cache address mapping for (instruction or data)
 *
 * @return
 * - NULL if the physical address is invalid or not mapped to flash cache of the specified memory type.
 * - Cached memory address (in IROM or DROM space) corresponding to phys_offs.
 */
const void *spi_flash_phys2cache(size_t phys_offs, spi_flash_mmap_memory_t memory);

#ifdef __cplusplus
}
#endif
