/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for MMU register operations

#pragma once

#include "soc/ext_mem_defs.h"
#include "hal/assert.h"
#include "hal/mmu_types.h"
#include "soc/mmu.h"
#include "soc/dport_access.h"

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
    HAL_ASSERT(entry_id < MMU_MAX_ENTRY_NUM);

    DPORT_INTERRUPT_DISABLE();
    switch (mmu_id) {
        case MMU_TABLE_PRO:
            DPORT_WRITE_PERI_REG((uint32_t)&SOC_MMU_DPORT_PRO_FLASH_MMU_TABLE[entry_id], SOC_MMU_INVALID_ENTRY_VAL);
            break;
        case MMU_TABLE_APP:
            DPORT_WRITE_PERI_REG((uint32_t)&SOC_MMU_DPORT_APP_FLASH_MMU_TABLE[entry_id], SOC_MMU_INVALID_ENTRY_VAL);
            break;
        default:
            HAL_ASSERT(false && "invalid mmu_id");
    }
    DPORT_INTERRUPT_RESTORE();
}

#ifdef __cplusplus
}
#endif
