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
 * Convert MMU virtual address to linear address
 *
 * @param vaddr  virtual address
 *
 * @return linear address
 */
static inline uint32_t mmu_ll_vaddr_to_laddr(uint32_t vaddr)
{
    return vaddr & SOC_MMU_LINEAR_ADDR_MASK;
}

/**
 * Convert MMU linear address to virtual address
 *
 * @param laddr       linear address
 * @param vaddr_type  virtual address type, could be instruction type or data type. See `mmu_vaddr_t`
 *
 * @return virtual address
 */
static inline uint32_t mmu_ll_laddr_to_vaddr(uint32_t laddr, mmu_vaddr_t vaddr_type)
{
    uint32_t vaddr_base = 0;
    if (vaddr_type == MMU_VADDR_DATA) {
        vaddr_base = SOC_MMU_DBUS_VADDR_BASE;
    } else {
        vaddr_base = SOC_MMU_IBUS_VADDR_BASE;
    }

    return vaddr_base | laddr;
}

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
 * @param size  MMU page size
 *
 * @note On esp32, only supports `MMU_PAGE_64KB`
 */
__attribute__((always_inline))
static inline void mmu_ll_set_page_size(uint32_t mmu_id, uint32_t size)
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
    uint32_t vaddr_end = vaddr_start + len - 1;

    return (ADDRESS_IN_IRAM0_CACHE(vaddr_start) && ADDRESS_IN_IRAM0_CACHE(vaddr_end)) ||
           (ADDRESS_IN_IRAM1_CACHE(vaddr_start) && ADDRESS_IN_IRAM1_CACHE(vaddr_end)) ||
           (ADDRESS_IN_IROM0_CACHE(vaddr_start) && ADDRESS_IN_IROM0_CACHE(vaddr_end)) ||
           (ADDRESS_IN_DRAM1_CACHE(vaddr_start) && ADDRESS_IN_DRAM1_CACHE(vaddr_end)) ||
           (ADDRESS_IN_DROM0_CACHE(vaddr_start) && ADDRESS_IN_DROM0_CACHE(vaddr_end));
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
    (void)target;
    HAL_ASSERT(entry_id < MMU_ENTRY_NUM);

    DPORT_INTERRUPT_DISABLE();
    switch (mmu_id) {
        case MMU_TABLE_CORE0:
            DPORT_WRITE_PERI_REG((uint32_t)&DPORT_PRO_FLASH_MMU_TABLE[entry_id], mmu_val);
            break;
        case MMU_TABLE_CORE1:
            DPORT_WRITE_PERI_REG((uint32_t)&DPORT_APP_FLASH_MMU_TABLE[entry_id], mmu_val);
            break;
        default:
            HAL_ASSERT(false);
    }
    DPORT_INTERRUPT_RESTORE();
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
    uint32_t mmu_value;
    HAL_ASSERT(entry_id < MMU_ENTRY_NUM);

    DPORT_INTERRUPT_DISABLE();
    switch (mmu_id) {
        case MMU_TABLE_CORE0:
            mmu_value = DPORT_SEQUENCE_REG_READ((uint32_t)&DPORT_PRO_FLASH_MMU_TABLE[entry_id]);
            break;
        case MMU_TABLE_CORE1:
            mmu_value = DPORT_SEQUENCE_REG_READ((uint32_t)&DPORT_APP_FLASH_MMU_TABLE[entry_id]);
            break;
        default:
            HAL_ASSERT(false);
    }
    DPORT_INTERRUPT_RESTORE();
    return mmu_value;
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
        case MMU_TABLE_CORE0:
            DPORT_WRITE_PERI_REG((uint32_t)&DPORT_PRO_FLASH_MMU_TABLE[entry_id], MMU_INVALID);
            break;
        case MMU_TABLE_CORE1:
            DPORT_WRITE_PERI_REG((uint32_t)&DPORT_APP_FLASH_MMU_TABLE[entry_id], MMU_INVALID);
            break;
        default:
            HAL_ASSERT(false);
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

    DPORT_INTERRUPT_DISABLE();
    uint32_t mmu_value = DPORT_SEQUENCE_REG_READ((uint32_t)&DPORT_PRO_FLASH_MMU_TABLE[entry_id]);
    DPORT_INTERRUPT_RESTORE();

    return (mmu_value & MMU_INVALID) ? true : false;
}

#ifdef __cplusplus
}
#endif
