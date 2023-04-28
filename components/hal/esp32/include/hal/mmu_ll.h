/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for MMU register operations

#pragma once

#include "soc/ext_mem_defs.h"
#include "soc/dport_reg.h"
#include "soc/dport_access.h"
#include "hal/assert.h"
#include "hal/mmu_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get MMU page size
 *
 * @param mmu_id  MMU ID
 *
 * @return MMU page size code
 */
__attribute__((always_inline))
static inline mmu_page_size_t mmu_ll_get_page_size(uint32_t mmu_id)
{
    //On esp32, MMU Page size is always 64KB
    (void)mmu_id;
    return MMU_PAGE_64KB;
}

/**
 * Set MMU page size
 *
 * @param size  See `mmu_page_size_t`
 *
 * @note On esp32, only supports `MMU_PAGE_64KB`
 */
__attribute__((always_inline))
static inline void mmu_ll_set_page_size(uint32_t mmu_id, mmu_page_size_t size)
{
    HAL_ASSERT(size == MMU_PAGE_64KB);
}

/**
 * Check if the external memory vaddr region is valid
 *
 * @param mmu_id      MMU ID
 * @param vaddr_start start of the virtual address
 * @param len         length, in bytes
 *
 * @return
 *         True for valid
 */
__attribute__((always_inline))
static inline bool mmu_ll_check_valid_ext_vaddr_region(uint32_t mmu_id, uint32_t vaddr_start, uint32_t len)
{
    (void)mmu_id;
    uint32_t vaddr_end = vaddr_start + len;

    return (ADDRESS_IN_IRAM0_CACHE(vaddr_start) && ADDRESS_IN_IRAM0_CACHE(vaddr_end)) ||
           (ADDRESS_IN_IRAM1_CACHE(vaddr_start) && ADDRESS_IN_IRAM1_CACHE(vaddr_end)) ||
           (ADDRESS_IN_IROM0_CACHE(vaddr_start) && ADDRESS_IN_IROM0_CACHE(vaddr_end)) ||
           (ADDRESS_IN_DRAM1_CACHE(vaddr_start) && ADDRESS_IN_DRAM1_CACHE(vaddr_end)) ||
           (ADDRESS_IN_DROM0_CACHE(vaddr_start) && ADDRESS_IN_DROM0_CACHE(vaddr_end));
}

/**
 * Set MMU table entry as invalid
 *
 * @param mmu_id   MMU ID
 * @param entry_id MMU entry ID
 */
__attribute__((always_inline))
static inline void mmu_ll_set_entry_invalid(uint32_t mmu_id, uint32_t entry_id)
{
    HAL_ASSERT(entry_id < MMU_ENTRY_NUM);

    DPORT_INTERRUPT_DISABLE();
    switch (mmu_id) {
        case 0:
            DPORT_WRITE_PERI_REG((uint32_t)&DPORT_PRO_FLASH_MMU_TABLE[entry_id], DPORT_FLASH_MMU_TABLE_INVALID_VAL);
            break;
        case 1:
            DPORT_WRITE_PERI_REG((uint32_t)&DPORT_APP_FLASH_MMU_TABLE[entry_id], DPORT_FLASH_MMU_TABLE_INVALID_VAL);
            break;
        default:
            HAL_ASSERT(false && "invalid mmu_id");
    }
    DPORT_INTERRUPT_RESTORE();
}

/**
 * Unmap all the items in the MMU table
 *
 * @param mmu_id MMU ID
 */
__attribute__((always_inline))
static inline void mmu_ll_unmap_all(uint32_t mmu_id)
{
    for (int i = 0; i < MMU_ENTRY_NUM; i++) {
        mmu_ll_set_entry_invalid(mmu_id, i);
    }
}

#ifdef __cplusplus
}
#endif
