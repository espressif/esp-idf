/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdlib.h>
#include <stdint.h>
#include "esp_err.h"
#include "esp_bit_defs.h"
#include "hal/mmu_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * MMU Memory Mapping Driver APIs for MMU supported memory
 *
 *
 * Driver Backgrounds:
 *
 * --------------------------------------------------------------------------------------------------------
 *                                            Memory Pool                                                 |
 * --------------------------------------------------------------------------------------------------------
 * |                       Memory Region 0                              | Memory Region 1 |     ...       |
 * --------------------------------------------------------------------------------------------------------
 * | Block 0 | Slot 0 | Block 1 | Block 2 |  ...  | Slot 1 (final slot) |          ...                    |
 * --------------------------------------------------------------------------------------------------------
 *
 * - A memory pool stands for the whole virtual address range that can be mapped to physical memory
 * - A memory region is a range of virtual address with same attributes
 * - A block is a piece of vaddr range that is dynamically mapped.
 * - A Slot is the vaddr range between 2 blocks.
 */


/**
 * MMAP flags
 */
/**
 * @brief Share this mapping
 *
 * - If this flag is set, a paddr block can be mapped to multiple vaddr blocks.
 *   1. This happens when:
 *      - the to-be-mapped paddr block is overlapped with an already mapped paddr block.
 *      - the to-be-mapped paddr block encloses an already mapped paddr block.
 *   2. If the to-be-mapped paddr block is enclosed by an already mapped paddr block, no new mapping will happen, return ESP_ERR_INVALID_STATE. The out pointer will be the already mapped paddr corresponding vaddr.
 *   3. If the to-be-mapped paddr block is identical with an already mapped paddr block, no new mapping will happen, return ESP_ERR_INVALID_STATE. The out pointer will be the corresponding vaddr.
 *
 * - If this flag isn't set, overlapped, enclosed or same to-be-mapped paddr block will lead to ESP_ERR_INVALID_ARG.
 */
#define ESP_MMU_MMAP_FLAG_PADDR_SHARED    BIT(0)

/**
 * @brief Physical memory type
 */
typedef uint32_t esp_paddr_t;

/**
 * @brief Map a physical memory block to external virtual address block, with given capabilities.
 *
 * @note This API does not guarantee thread safety
 *
 * @param[in]  paddr_start  Start address of the physical memory block
 * @param[in]  size         Size to be mapped. Size will be rounded up by to the nearest multiple of MMU page size
 * @param[in]  target       Physical memory target you're going to map to, see `mmu_target_t`
 * @param[in]  caps         Memory capabilities, see `mmu_mem_caps_t`
 * @param[in]  flags        Mmap flags
 * @param[out] out_ptr      Start address of the mapped virtual memory
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_ARG:   Invalid argument, see printed logs
 *        - ESP_ERR_NOT_SUPPORTED: Only on ESP32, PSRAM is not a supported physical memory target
 *        - ESP_ERR_NOT_FOUND:     No enough size free block to use
 *        - ESP_ERR_NO_MEM:        Out of memory, this API will allocate some heap memory for internal usage
 *        - ESP_ERR_INVALID_STATE: Paddr is mapped already, this API will return corresponding vaddr_start of the previously mapped block.
 *                                 Only to-be-mapped paddr block is totally enclosed by a previously mapped block will lead to this error. (Identical scenario will behave similarly)
 *                                 new_block_start               new_block_end
 *                                              |-------- New Block --------|
 *                                      |--------------- Block ---------------|
 *                                 block_start                              block_end
 *
 */
esp_err_t esp_mmu_map(esp_paddr_t paddr_start, size_t size, mmu_target_t target, mmu_mem_caps_t caps, int flags, void **out_ptr);

/**
 * @brief Unmap a previously mapped virtual memory block
 *
 * @note This API does not guarantee thread safety
 *
 * @param[in] ptr  Start address of the virtual memory
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_ARG:   Null pointer
 *        - ESP_ERR_NOT_FOUND:     Vaddr is not in external memory, or it's not mapped yet
 */
esp_err_t esp_mmu_unmap(void *ptr);

/**
 * @brief Get largest consecutive free external virtual memory block size, with given capabilities and given physical target
 *
 * @param[in] caps      Bitwise OR of MMU_MEM_CAP_* flags indicating the memory block
 * @param[in] target    Physical memory target you're going to map to, see `mmu_target_t`.
 * @param[out] out_len  Largest free block length, in bytes.
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_ARG: Invalid arguments, could be null pointer
 */
esp_err_t esp_mmu_map_get_max_consecutive_free_block_size(mmu_mem_caps_t caps, mmu_target_t target, size_t *out_len);

/**
 * Dump all the previously mapped blocks
 *
 * @note This API shall not be called from an ISR.
 * @note This API does not guarantee thread safety
 *
 * @param stream stream to print information to; use stdout or stderr to print
 *               to the console; use fmemopen/open_memstream to print to a
 *               string buffer.
 * @return
 *        - ESP_OK
 */
esp_err_t esp_mmu_map_dump_mapped_blocks(FILE* stream);

/**
 * @brief Convert virtual address to physical address
 *
 * @param[in]  vaddr       Virtual address
 * @param[out] out_paddr   Physical address
 * @param[out] out_target  Physical memory target, see `mmu_target_t`
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_ARG:  Null pointer, or vaddr is not within external memory
 *        - ESP_ERR_NOT_FOUND:    Vaddr is not mapped yet
 */
esp_err_t esp_mmu_vaddr_to_paddr(void *vaddr, esp_paddr_t *out_paddr, mmu_target_t *out_target);

/**
 * @brief Convert physical address to virtual address
 *
 * @param[in]  paddr      Physical address
 * @param[in]  target     Physical memory target, see `mmu_target_t`
 * @param[in]  type       Virtual address type, could be either instruction or data
 * @param[out] out_vaddr  Virtual address
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_ARG:  Null pointer
 *        - ESP_ERR_NOT_FOUND:    Paddr is not mapped yet
 */
esp_err_t esp_mmu_paddr_to_vaddr(esp_paddr_t paddr, mmu_target_t target, mmu_vaddr_t type, void **out_vaddr);

/**
 * @brief If the physical address is mapped, this API will provide the capabilities of the virtual address where the physical address is mapped to.
 *
 * @note: Only return value is ESP_OK(which means physically address is successfully mapped), then caps you get make sense.
 * @note This API only check one page (see CONFIG_MMU_PAGE_SIZE), starting from the `paddr`
 *
 * @param[in]  paddr     Physical address
 * @param[out] out_caps  Bitwise OR of MMU_MEM_CAP_* flags indicating the capabilities of a virtual address where the physical address is mapped to.
 * @return
 *      - ESP_OK: Physical address successfully mapped.
 *      - ESP_ERR_INVALID_ARG: Null pointer
 *      - ESP_ERR_NOT_FOUND: Physical address is not mapped successfully.
 */
esp_err_t esp_mmu_paddr_find_caps(const esp_paddr_t paddr, mmu_mem_caps_t *out_caps);

#ifdef __cplusplus
}
#endif
