/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#if !CONFIG_IDF_TARGET_ESP32
#error esp_himem is only supported on ESP32
#else

#include <stddef.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

//Opaque pointers as handles for ram/range data
typedef struct esp_himem_ramdata_t *esp_himem_handle_t;
typedef struct esp_himem_rangedata_t *esp_himem_rangehandle_t;

//ESP32 MMU block size
#define ESP_HIMEM_BLKSZ (0x8000)

#define ESP_HIMEM_MAPFLAG_RO 1 /*!< Indicates that a mapping will only be read from. Note that this is unused for now. */

/**
 * @brief Allocate a block in high memory
 *
 * @param size Size of the to-be-allocated block, in bytes. Note that this needs to be
 *             a multiple of the external RAM mmu block size (32K).
 * @param[out] handle_out Handle to be returned
 * @returns - ESP_OK if succesful
 *          - ESP_ERR_NO_MEM if out of memory
 *          - ESP_ERR_INVALID_SIZE if size is not a multiple of 32K
 */
esp_err_t esp_himem_alloc(size_t size, esp_himem_handle_t *handle_out);

/**
 * @brief Allocate a memory region to map blocks into
 *
 * This allocates a contiguous CPU memory region that can be used to map blocks
 * of physical memory into.
 *
 * @param size Size of the range to be allocated. Note this needs to be a multiple of
 *             the external RAM mmu block size (32K).
 * @param[out] handle_out Handle to be returned
 * @returns - ESP_OK if succesful
 *          - ESP_ERR_NO_MEM if out of memory or address space
 *          - ESP_ERR_INVALID_SIZE if size is not a multiple of 32K
 */
esp_err_t esp_himem_alloc_map_range(size_t size, esp_himem_rangehandle_t *handle_out);

/**
 * @brief Map a block of high memory into the CPUs address space
 *
 * This effectively makes the block available for read/write operations.
 *
 * @note The region to be mapped needs to have offsets and sizes that are aligned to the
 *       SPI RAM MMU block size (32K)
 *
 * @param handle Handle to the block of memory, as given by esp_himem_alloc
 * @param range Range handle to map the memory in
 * @param ram_offset Offset into the block of physical memory of the block to map
 * @param range_offset Offset into the address range where the block will be mapped
 * @param len Length of region to map
 * @param flags One of ESP_HIMEM_MAPFLAG_*
 * @param[out] out_ptr Pointer to variable to store resulting memory pointer in
 * @returns - ESP_OK if the memory could be mapped
 *          - ESP_ERR_INVALID_ARG if offset, range or len aren't MMU-block-aligned (32K)
 *          - ESP_ERR_INVALID_SIZE if the offsets/lengths don't fit in the allocated memory or range
 *          - ESP_ERR_INVALID_STATE if a block in the selected ram offset/length is already mapped, or
 *                                  if a block in the selected range offset/length already has a mapping.
 */
esp_err_t esp_himem_map(esp_himem_handle_t handle, esp_himem_rangehandle_t range, size_t ram_offset, size_t range_offset, size_t len, int flags, void **out_ptr);

/**
 * @brief Free a block of physical memory
 *
 * This clears out the associated handle making the memory available for re-allocation again.
 * This will only succeed if none of the memory blocks currently have a mapping.
 *
 * @param handle Handle to the block of memory, as given by esp_himem_alloc
 * @returns - ESP_OK if the memory is succesfully freed
 *          - ESP_ERR_INVALID_ARG if the handle still is (partially) mapped
 */
esp_err_t esp_himem_free(esp_himem_handle_t handle);

/**
 * @brief Free a mapping range
 *
 * This clears out the associated handle making the range available for re-allocation again.
 * This will only succeed if none of the range blocks currently are used for a mapping.
 *
 * @param handle Handle to the range block, as given by esp_himem_alloc_map_range
 * @returns - ESP_OK if the memory is succesfully freed
 *          - ESP_ERR_INVALID_ARG if the handle still is (partially) mapped to
 */
esp_err_t esp_himem_free_map_range(esp_himem_rangehandle_t handle);

/**
 * @brief Unmap a region
 *
 * @param range Range handle
 * @param ptr Pointer returned by esp_himem_map
 * @param len Length of the block to be unmapped. Must be aligned to the SPI RAM MMU blocksize (32K)
 * @returns - ESP_OK if the memory is succesfully unmapped,
 *          - ESP_ERR_INVALID_ARG if ptr or len are invalid.
 */
esp_err_t esp_himem_unmap(esp_himem_rangehandle_t range, void *ptr, size_t len);

/**
 * @brief Get total amount of memory under control of himem API
 *
 * @returns Amount of memory, in bytes
 */
size_t esp_himem_get_phys_size(void);

/**
 * @brief Get free amount of memory under control of himem API
 *
 * @returns Amount of free memory, in bytes
 */
size_t esp_himem_get_free_size(void);

/**
 * @brief Get amount of SPI memory address space needed for bankswitching
 *
 * @note This is also weakly defined in esp32/spiram.c and returns 0 there, so
 *       if no other function in this file is used, no memory is reserved.
 *
 * @returns Amount of reserved area, in bytes
 */
size_t esp_himem_reserved_area_size(void);

#ifdef __cplusplus
}
#endif

#endif // !CONFIG_IDF_TARGET_ESP32
