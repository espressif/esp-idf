/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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


#ifdef __cplusplus
extern "C" {
#endif

#define MMU_LL_END_DROM_ENTRY_VADDR         (SOC_DRAM_FLASH_ADDRESS_HIGH - SOC_MMU_PAGE_SIZE)
#define MMU_LL_END_DROM_ENTRY_ID            (SOC_MMU_ENTRY_NUM - 1)

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
 * @param target      virtual address aimed physical memory target, not used
 *
 * @return virtual address
 */
static inline uint32_t mmu_ll_laddr_to_vaddr(uint32_t laddr, mmu_vaddr_t vaddr_type, mmu_target_t target)
{
    (void)target;
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
 * @param type        virtual address type, could be instruction type or data type. See `mmu_vaddr_t`
 *
 * @return
 *         True for valid
 */
__attribute__((always_inline))
static inline bool mmu_ll_check_valid_ext_vaddr_region(uint32_t mmu_id, uint32_t vaddr_start, uint32_t len, mmu_vaddr_t type)
{
    (void)mmu_id;
    uint32_t vaddr_end = vaddr_start + len - 1;
    bool valid = false;

    if (type & MMU_VADDR_INSTRUCTION) {
        valid |= (SOC_ADDRESS_IN_IRAM0_CACHE(vaddr_start) && SOC_ADDRESS_IN_IRAM0_CACHE(vaddr_end));
    }

    if (type & MMU_VADDR_DATA) {
        valid |= (SOC_ADDRESS_IN_DRAM0_CACHE(vaddr_start) && SOC_ADDRESS_IN_DRAM0_CACHE(vaddr_end));
    }

    return valid;
}

/**
 * Check if the paddr region is valid
 *
 * @param mmu_id      MMU ID
 * @param paddr_start start of the physical address
 * @param len         length, in bytes
 *
 * @return
 *         True for valid
 */
static inline bool mmu_ll_check_valid_paddr_region(uint32_t mmu_id, uint32_t paddr_start, uint32_t len)
{
    (void)mmu_id;
    return (paddr_start < (mmu_ll_get_page_size(mmu_id) * SOC_MMU_MAX_PADDR_PAGE_NUM)) &&
           (len < (mmu_ll_get_page_size(mmu_id) * SOC_MMU_MAX_PADDR_PAGE_NUM)) &&
           ((paddr_start + len - 1) < (mmu_ll_get_page_size(mmu_id) * SOC_MMU_MAX_PADDR_PAGE_NUM));
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

    return ((vaddr & SOC_MMU_VADDR_MASK) >> shift_code);
}

/**
 * Format the paddr to be mappable
 *
 * @param mmu_id  MMU ID
 * @param paddr   physical address to be mapped
 * @param target  paddr memory target, not used
 *
 * @return
 *         mmu_val - paddr in MMU table supported format
 */
__attribute__((always_inline))
static inline uint32_t mmu_ll_format_paddr(uint32_t mmu_id, uint32_t paddr, mmu_target_t target)
{
    (void)mmu_id;
    (void)target;

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
    HAL_ASSERT(entry_id < SOC_MMU_ENTRY_NUM);

    *(uint32_t *)(DR_REG_MMU_TABLE + entry_id * 4) = mmu_val | SOC_MMU_ACCESS_FLASH | SOC_MMU_VALID;
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
    HAL_ASSERT(entry_id < SOC_MMU_ENTRY_NUM);

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
    HAL_ASSERT(entry_id < SOC_MMU_ENTRY_NUM);

    *(uint32_t *)(DR_REG_MMU_TABLE + entry_id * 4) = SOC_MMU_INVALID;
}

/**
 * Unmap all the items in the MMU table
 *
 * @param mmu_id MMU ID
 */
__attribute__((always_inline))
static inline void mmu_ll_unmap_all(uint32_t mmu_id)
{
    for (int i = 0; i < SOC_MMU_ENTRY_NUM; i++) {
        mmu_ll_set_entry_invalid(mmu_id, i);
    }
}

/**
 * Check MMU table entry value is valid
 *
 * @param mmu_id   MMU ID
 * @param entry_id MMU entry ID
 *
 * @return         True for MMU entry is valid; False for invalid
 */
static inline bool mmu_ll_check_entry_valid(uint32_t mmu_id, uint32_t entry_id)
{
    (void)mmu_id;
    HAL_ASSERT(entry_id < SOC_MMU_ENTRY_NUM);

    return (*(uint32_t *)(DR_REG_MMU_TABLE + entry_id * 4) & SOC_MMU_INVALID) ? false : true;
}

/**
 * Get the MMU table entry target
 *
 * @param mmu_id   MMU ID
 * @param entry_id MMU entry ID
 *
 * @return         Target, see `mmu_target_t`
 */
static inline mmu_target_t mmu_ll_get_entry_target(uint32_t mmu_id, uint32_t entry_id)
{
    (void)mmu_id;
    return MMU_TARGET_FLASH0;
}

/**
 * Convert MMU entry ID to paddr base
 *
 * @param mmu_id   MMU ID
 * @param entry_id MMU entry ID
 *
 * @return         paddr base
 */
static inline uint32_t mmu_ll_entry_id_to_paddr_base(uint32_t mmu_id, uint32_t entry_id)
{
    (void)mmu_id;
    HAL_ASSERT(entry_id < SOC_MMU_ENTRY_NUM);

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

    return ((*(uint32_t *)(DR_REG_MMU_TABLE + entry_id * 4)) & SOC_MMU_VALID_VAL_MASK) << shift_code;
}

/**
 * Find the MMU table entry ID based on table map value
 * @note This function can only find the first match entry ID. However it is possible that a physical address
 *       is mapped to multiple virtual addresses
 *
 * @param mmu_id   MMU ID
 * @param mmu_val  map value to be read from MMU table standing for paddr
 * @param target   physical memory target, see `mmu_target_t`
 *
 * @return         MMU entry ID, -1 for invalid
 */
static inline int mmu_ll_find_entry_id_based_on_map_value(uint32_t mmu_id, uint32_t mmu_val, mmu_target_t target)
{
    (void)mmu_id;
    for (int i = 0; i < SOC_MMU_ENTRY_NUM; i++) {
        if (mmu_ll_check_entry_valid(mmu_id, i)) {
            if (mmu_ll_get_entry_target(mmu_id, i) == target) {
                if (((*(uint32_t *)(DR_REG_MMU_TABLE + i * 4)) & SOC_MMU_VALID_VAL_MASK) == mmu_val) {
                    return i;
                }
            }
        }
    }

    return -1;
}

/**
 * Convert MMU entry ID to vaddr base
 *
 * @param mmu_id   MMU ID
 * @param entry_id MMU entry ID
 * @param type     virtual address type, could be instruction type or data type. See `mmu_vaddr_t`
 */
static inline uint32_t mmu_ll_entry_id_to_vaddr_base(uint32_t mmu_id, uint32_t entry_id, mmu_vaddr_t type)
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
    uint32_t laddr = entry_id << shift_code;

    /**
     * For `mmu_ll_laddr_to_vaddr`, target is for compatibility on this chip.
     * Here we just pass MMU_TARGET_FLASH0 to get vaddr
     */
    return mmu_ll_laddr_to_vaddr(laddr, type, MMU_TARGET_FLASH0);
}

#ifdef __cplusplus
}
#endif
