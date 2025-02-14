/*
 * SPDX-FileCopyrightText: 2010-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <esp_types.h>
#include "soc/soc_caps.h"
#include "hal/mmu_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * MMU Hal layer initialisation
 */
void mmu_hal_init(void);

/**
 * Unmap all the MMU table. After this all external memory vaddr are not available
 */
void mmu_hal_unmap_all(void);

/**
 * Helper functions to convert the MMU page numbers into bytes. e.g.:
 * - When MMU page size is 16KB, page_num = 2 will be converted into 32KB
 * - When MMU page size is 32KB, page_num = 2 will be converted into 64KB
 *
 * @param mmu_id    MMU ID
 * @param page_num  page numbers
 *
 * @return
 *         length in byte
 */
uint32_t mmu_hal_pages_to_bytes(uint32_t mmu_id, uint32_t page_num);

/**
 * Helper functions to convert bytes into MMU page numbers. e.g.:
 * - When MMU page size is 16KB, bytes = 64KB will be converted into 4 pages
 * - When MMU page size is 32KB, bytes = 64KB will be converted into 2 pages
 *
 * @param mmu_id    MMU ID
 * @param bytes     length in byte
 *
 * @return
 *         length in CONFIG_MMU_PAGE_SIZE
 */
uint32_t mmu_hal_bytes_to_pages(uint32_t mmu_id, uint32_t bytes);

/**
 * To map a virtual address block to a physical memory block
 *
 * @param mmu_id       MMU ID
 * @param mem_type     physical memory type, see `mmu_target_t`
 * @param vaddr        start virtual address to be mapped
 * @param paddr        start physical address to be mapped
 * @param len          length to be mapped, in bytes
 * @param[out] out_len actual mapped length
 *
 * @note vaddr and paddr should be aligned with the mmu page size, see CONFIG_MMU_PAGE_SIZE
 */
void mmu_hal_map_region(uint32_t mmu_id, mmu_target_t mem_type, uint32_t vaddr, uint32_t paddr, uint32_t len, uint32_t *out_len);

/**
 * To unmap a virtual address block that is mapped to a physical memory block previously
 *
 * @param[in] mmu_id  MMU ID
 * @param[in] vaddr   start virtual address
 * @param[in] len     length to be unmapped, in bytes
 */
void mmu_hal_unmap_region(uint32_t mmu_id, uint32_t vaddr, uint32_t len);

/**
 * Convert virtual address to physical address
 *
 * @param mmu_id           MMU ID
 * @param vaddr            virtual address
 * @param[out] out_paddr   physical address
 * @param[out] out_target  Indicating the vaddr/paddr is mapped on which target, see `mmu_target_t`
 *
 * @return
 *        - true: virtual address is valid
 *        - false: virtual address isn't valid
 */
bool mmu_hal_vaddr_to_paddr(uint32_t mmu_id, uint32_t vaddr, uint32_t *out_paddr, mmu_target_t *out_target);

/**
 * Convert physical address to virtual address
 *
 * @note This function can only find the first match virtual address.
 *       However it is possible that a physical address is mapped to multiple virtual addresses.
 *
 * @param mmu_id          MMU ID
 * @param paddr           physical address
 * @param target          physical memory target, see `mmu_target_t`
 * @param type            virtual address type, could be instruction or data
 * @param[out] out_vaddr  virtual address
 *
 * @return
 *        - true: found a matched vaddr
 *        - false: not found a matched vaddr
 */
bool mmu_hal_paddr_to_vaddr(uint32_t mmu_id, uint32_t paddr, mmu_target_t target, mmu_vaddr_t type, uint32_t *out_vaddr);


/**
 * Check if the vaddr region is valid
 *
 * @param mmu_id      MMU ID
 * @param vaddr_start start of the virtual address
 * @param len         length, in bytes
 * @param type        virtual address type, could be instruction type or data type. See `mmu_vaddr_t`
 *
 * @return
 *         True for valid
 */
bool mmu_hal_check_valid_ext_vaddr_region(uint32_t mmu_id, uint32_t vaddr_start, uint32_t len, mmu_vaddr_t type);

#if SOC_MMU_PER_EXT_MEM_TARGET
/**
 * Get MMU ID from MMU target
 *
 * @param target      MMU target
 *
 * @return
 *        MMU ID
 */
uint32_t mmu_hal_get_id_from_target(mmu_target_t target);

/**
 * Get MMU ID from vaddr
 *
 * @param vaddr       Virtual address
 *
 * @return
 *        MMU ID
 */
uint32_t mmu_hal_get_id_from_vaddr(uint32_t vaddr);
#endif

#ifdef __cplusplus
}
#endif
