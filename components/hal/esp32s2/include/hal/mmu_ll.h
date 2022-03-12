/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for MMU register operations

#pragma once

#include "soc/extmem_reg.h"
#include "soc/ext_mem_defs.h"
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
    //On esp32s2, MMU Page size is always 64KB
    (void)mmu_id;
    return MMU_PAGE_64KB;
}

/**
 * Set MMU page size
 *
 * @param size  See `mmu_page_size_t`
 *
 * @note On esp32s2, only supports `MMU_PAGE_64KB`
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

    return (ADDRESS_IN_DROM0(vaddr_start) && ADDRESS_IN_DROM0(vaddr_end)) ||
           (ADDRESS_IN_IRAM1(vaddr_start) && ADDRESS_IN_IRAM1(vaddr_end)) ||
           (ADDRESS_IN_IRAM0_CACHE(vaddr_start) && ADDRESS_IN_IRAM0_CACHE(vaddr_end)) ||
           (ADDRESS_IN_DPORT_CACHE(vaddr_start) && ADDRESS_IN_DPORT_CACHE(vaddr_end)) ||
           (ADDRESS_IN_DRAM1(vaddr_start) && ADDRESS_IN_DRAM1(vaddr_end)) ||
           (ADDRESS_IN_DRAM0_CACHE(vaddr_start) && ADDRESS_IN_DRAM0_CACHE(vaddr_end));
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
    uint32_t offset = 0;

    if (ADDRESS_IN_DROM0(vaddr)) {
        offset = PRO_CACHE_IBUS2_MMU_START / 4;
    } else if (ADDRESS_IN_IRAM0_CACHE(vaddr)) {
        offset = PRO_CACHE_IBUS0_MMU_START / 4;
    } else if (ADDRESS_IN_IRAM1(vaddr)) {
        offset = PRO_CACHE_IBUS1_MMU_START / 4;
    } else if (ADDRESS_IN_DPORT_CACHE(vaddr)) {
        offset = PRO_CACHE_DBUS2_MMU_START / 4;
    } else if (ADDRESS_IN_DRAM1(vaddr)) {
        offset = PRO_CACHE_DBUS1_MMU_START / 4;
    } else if (ADDRESS_IN_DRAM0_CACHE(vaddr)) {
        offset = PRO_CACHE_DBUS0_MMU_START / 4;
    } else {
        HAL_ASSERT(false);
    }

    return offset + ((vaddr & MMU_VADDR_MASK) >> 16);
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
    return paddr >> 16;
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
    HAL_ASSERT(entry_id < MMU_MAX_ENTRY_NUM);

    uint32_t target_code = (target == MMU_TARGET_FLASH0) ? MMU_ACCESS_FLASH : MMU_ACCESS_SPIRAM;
    *(uint32_t *)(DR_REG_MMU_TABLE + entry_id * 4) = mmu_val | target_code | MMU_VALID;
}

#ifdef __cplusplus
}
#endif
