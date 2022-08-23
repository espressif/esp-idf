/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for MMU register operations

#pragma once

#include <stdbool.h>
#include "soc/extmem_reg.h"
#include "soc/ext_mem_defs.h"
#include "hal/assert.h"
#include "hal/mmu_types.h"
#include "sdkconfig.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The real MMU page size get from Kconfig.
 *
 * @note Only used in this file
 */
#define MMU_LL_PAGE_SIZE                   (CONFIG_MMU_PAGE_SIZE)

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
    (void)mmu_id;
    uint32_t page_size_code = REG_GET_FIELD(EXTMEM_CACHE_CONF_MISC_REG, EXTMEM_CACHE_MMU_PAGE_SIZE);
    return (page_size_code == 0) ? MMU_PAGE_16KB : (page_size_code == 1) ? MMU_PAGE_32KB : MMU_PAGE_64KB;
}

/**
 * Set MMU page size
 *
 * @param size  MMU page size
 */
__attribute__((always_inline))
static inline void mmu_ll_set_page_size(uint32_t mmu_id, uint32_t size)
{
    uint8_t reg_val = (size == MMU_PAGE_16KB) ? 0 : (size == MMU_PAGE_32KB) ? 1 : 2;
    REG_SET_FIELD(EXTMEM_CACHE_CONF_MISC_REG, EXTMEM_CACHE_MMU_PAGE_SIZE, reg_val);
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
    uint32_t vaddr_end = vaddr_start + len - 1;
    return (ADDRESS_IN_IRAM0_CACHE(vaddr_start, MMU_LL_PAGE_SIZE) && ADDRESS_IN_IRAM0_CACHE(vaddr_end, MMU_LL_PAGE_SIZE)) || (ADDRESS_IN_DRAM0_CACHE(vaddr_start, MMU_LL_PAGE_SIZE) && ADDRESS_IN_DRAM0_CACHE(vaddr_end, MMU_LL_PAGE_SIZE));
}

/**
 * To get the MMU table entry id to be mapped
 *
 * @param mmu_id  MMU ID
 * @param vaddr   virtual address to be mapped
 *
 * @return
 *         MMU table entry id
 */
__attribute__((always_inline))
static inline uint32_t mmu_ll_get_entry_id(uint32_t mmu_id, uint32_t vaddr)
{
    (void)mmu_id;

    mmu_page_size_t page_size = mmu_ll_get_page_size(mmu_id);
    uint32_t shift_code = 0;
    switch (page_size) {
        case MMU_PAGE_64KB:
            shift_code = 16;
            break;
        case MMU_PAGE_32KB:
            shift_code = 15;
            break;
        case MMU_PAGE_16KB:
            shift_code = 14;
            break;
        default:
            HAL_ASSERT(shift_code);
    }

    return ((vaddr & MMU_VADDR_MASK(page_size)) >> shift_code);
}

/**
 * Format the paddr to be mappable
 *
 * @param mmu_id  MMU ID
 * @param paddr   physical address to be mapped
 *
 * @return
 *         mmu_val - paddr in MMU table supported format
 */
__attribute__((always_inline))
static inline uint32_t mmu_ll_format_paddr(uint32_t mmu_id, uint32_t paddr)
{
    (void)mmu_id;

    mmu_page_size_t page_size = mmu_ll_get_page_size(mmu_id);
    uint32_t shift_code = 0;
    switch (page_size) {
        case MMU_PAGE_64KB:
            shift_code = 16;
            break;
        case MMU_PAGE_32KB:
            shift_code = 15;
            break;
        case MMU_PAGE_16KB:
            shift_code = 14;
            break;
        default:
            HAL_ASSERT(shift_code);
    }

    return paddr >> shift_code;
}

/**
 * Write to the MMU table to map the virtual memory and the physical memory
 *
 * @param mmu_id   MMU ID
 * @param entry_id MMU entry ID
 * @param mmu_val  Value to be set into an MMU entry, for physical address
 * @param target   MMU target physical memory.
 */
__attribute__((always_inline))
static inline void mmu_ll_write_entry(uint32_t mmu_id, uint32_t entry_id, uint32_t mmu_val, mmu_target_t target)
{
    (void)mmu_id;
    HAL_ASSERT(target == MMU_TARGET_FLASH0);
    HAL_ASSERT(entry_id < MMU_ENTRY_NUM);

    *(uint32_t *)(DR_REG_MMU_TABLE + entry_id * 4) = mmu_val | MMU_ACCESS_FLASH | MMU_VALID;
}

/**
 * Read the raw value from MMU table
 *
 * @param mmu_id   MMU ID
 * @param entry_id MMU entry ID
 * @param mmu_val  Value to be read from MMU table
 */
__attribute__((always_inline))
static inline uint32_t mmu_ll_read_entry(uint32_t mmu_id, uint32_t entry_id)
{
    (void)mmu_id;
    HAL_ASSERT(entry_id < MMU_ENTRY_NUM);

    return *(uint32_t *)(DR_REG_MMU_TABLE + entry_id * 4);
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
    (void)mmu_id;
    HAL_ASSERT(entry_id < MMU_ENTRY_NUM);

    *(uint32_t *)(DR_REG_MMU_TABLE + entry_id * 4) = MMU_INVALID;
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

/**
 * Get MMU table entry is invalid
 *
 * @param mmu_id   MMU ID
 * @param entry_id MMU entry ID
 * return ture for MMU entry is invalid, false for valid
 */
__attribute__((always_inline))
static inline bool mmu_ll_get_entry_is_invalid(uint32_t mmu_id, uint32_t entry_id)
{
    (void)mmu_id;
    HAL_ASSERT(entry_id < MMU_ENTRY_NUM);

    return (*(uint32_t *)(DR_REG_MMU_TABLE + entry_id * 4) & MMU_INVALID) ? true : false;
}

#ifdef __cplusplus
}
#endif
