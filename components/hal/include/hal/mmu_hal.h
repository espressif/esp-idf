/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "hal/mmu_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Unmap all the MMU table. After this all external memory vaddr are not available
 */
void mmu_hal_init(void);

#if !CONFIG_IDF_TARGET_ESP32
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
 * To map a virtual address region to a physical memory region
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
#endif

#ifdef __cplusplus
}
#endif
