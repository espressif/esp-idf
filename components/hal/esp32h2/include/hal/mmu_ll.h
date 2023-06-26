/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for MMU register operations

#pragma once

#include "soc/spi_mem_reg.h"
#include "soc/ext_mem_defs.h"
#include "hal/assert.h"
#include "hal/mmu_types.h"
#include "hal/efuse_ll.h"


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
    //On ESP32C6, I/D share the same vaddr range
    return SOC_MMU_IBUS_VADDR_BASE | laddr;
}


__attribute__((always_inline)) static inline bool mmu_ll_cache_encryption_enabled(void)
{
    unsigned cnt = efuse_ll_get_flash_crypt_cnt();
    // 3 bits wide, any odd number - 1 or 3 - bits set means encryption is on
    cnt = ((cnt >> 2) ^ (cnt >> 1) ^ cnt) & 0x1;
    return (cnt == 1);
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
     uint32_t page_size_code = REG_GET_FIELD(SPI_MEM_MMU_POWER_CTRL_REG(0), SPI_MEM_MMU_PAGE_SIZE);
    return  (page_size_code == 0) ? MMU_PAGE_64KB : \
            (page_size_code == 1) ? MMU_PAGE_32KB : \
            (page_size_code == 2) ? MMU_PAGE_16KB : \
            MMU_PAGE_8KB;
}

/**
 * Set MMU page size
 *
 * @param size  MMU page size
 */
__attribute__((always_inline))
static inline void mmu_ll_set_page_size(uint32_t mmu_id, uint32_t size)
{
    uint8_t reg_val = (size == MMU_PAGE_64KB) ? 0 : \
                      (size == MMU_PAGE_32KB) ? 1 : \
                      (size == MMU_PAGE_16KB) ? 2 : \
                      (size == MMU_PAGE_8KB) ? 3 : 0;
    REG_SET_FIELD(SPI_MEM_MMU_POWER_CTRL_REG(0), SPI_MEM_MMU_PAGE_SIZE, reg_val);
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
    (void)type;
    uint32_t vaddr_end = vaddr_start + len;
    return (ADDRESS_IN_IRAM0_CACHE(vaddr_start) && ADDRESS_IN_IRAM0_CACHE(vaddr_end)) || (ADDRESS_IN_DRAM0_CACHE(vaddr_start) && ADDRESS_IN_DRAM0_CACHE(vaddr_end));
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
__attribute__((always_inline))
static inline bool mmu_ll_check_valid_paddr_region(uint32_t mmu_id, uint32_t paddr_start, uint32_t len)
{
    (void)mmu_id;
    return (paddr_start < (mmu_ll_get_page_size(mmu_id) * MMU_MAX_PADDR_PAGE_NUM)) &&
           (len < (mmu_ll_get_page_size(mmu_id) * MMU_MAX_PADDR_PAGE_NUM)) &&
           ((paddr_start + len - 1) < (mmu_ll_get_page_size(mmu_id) * MMU_MAX_PADDR_PAGE_NUM));
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
        case MMU_PAGE_8KB:
            shift_code = 13;
            break;
        default:
            HAL_ASSERT(shift_code);
    }

    return ((vaddr & MMU_VADDR_MASK) >> shift_code);
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
        case MMU_PAGE_8KB:
            shift_code = 13;
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
__attribute__((always_inline)) static inline void mmu_ll_write_entry(uint32_t mmu_id, uint32_t entry_id, uint32_t mmu_val, uint32_t target)
{
    (void)mmu_id;
    (void)target;
    uint32_t mmu_raw_value;
    if (mmu_ll_cache_encryption_enabled()) {
        mmu_val |= MMU_SENSITIVE;
    }
    /* Note: for ESP32-H2, invert invalid bit for compatible with upper-layer software */
    mmu_raw_value = mmu_val ^ MMU_INVALID_MASK;
    REG_WRITE(SPI_MEM_MMU_ITEM_INDEX_REG(0), entry_id);
    REG_WRITE(SPI_MEM_MMU_ITEM_CONTENT_REG(0), mmu_raw_value);
}

/**
 * Read the raw value from MMU table
 *
 * @param mmu_id   MMU ID
 * @param entry_id MMU entry ID
 * @param mmu_val  Value to be read from MMU table
 */
__attribute__((always_inline)) static inline uint32_t mmu_ll_read_entry(uint32_t mmu_id, uint32_t entry_id)
{
    (void)mmu_id;

    uint32_t mmu_raw_value;
    uint32_t ret;
    REG_WRITE(SPI_MEM_MMU_ITEM_INDEX_REG(0), entry_id);
    mmu_raw_value = REG_READ(SPI_MEM_MMU_ITEM_CONTENT_REG(0));
    if (mmu_ll_cache_encryption_enabled()) {
        mmu_raw_value &= ~MMU_SENSITIVE;
    }
    /* Note: for ESP32-H2, invert invalid bit for compatible with upper-layer software */
    ret = mmu_raw_value ^ MMU_INVALID_MASK;
    return ret;
}

/**
 * Set MMU table entry as invalid
 *
 * @param mmu_id   MMU ID
 * @param entry_id MMU entry
 */
__attribute__((always_inline)) static inline void mmu_ll_set_entry_invalid(uint32_t mmu_id, uint32_t entry_id)
{
    (void)mmu_id;

    REG_WRITE(SPI_MEM_MMU_ITEM_INDEX_REG(0), entry_id);
    REG_WRITE(SPI_MEM_MMU_ITEM_CONTENT_REG(0), MMU_INVALID);
}

// /**
//  * Get MMU table entry is invalid
//  *
//  * @param mmu_id   MMU ID
//  * @param entry_id MMU entry ID
//  * return ture for MMU entry is invalid, false for valid
//  */
// __attribute__((always_inline)) static inline bool mmu_ll_get_entry_is_invalid(uint32_t mmu_id, uint32_t entry_id)
// {
//     (void)mmu_id;

//     uint32_t mmu_raw_value;
//     REG_WRITE(SPI_MEM_MMU_ITEM_INDEX_REG(0), entry_id);
//     mmu_raw_value = REG_READ(SPI_MEM_MMU_ITEM_CONTENT_REG(0));
//     /* Note: for ESP32-H2, the invalid-bit of MMU: 0 for invalid, 1 for valid */
//     return (mmu_raw_value & MMU_INVALID_MASK) ? false : true;
// }


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
 * Check MMU table entry value is valid
 *
 * @param mmu_id   MMU ID
 * @param entry_id MMU entry ID
 *
 * @return         Ture for MMU entry is valid; False for invalid
 */
static inline bool mmu_ll_check_entry_valid(uint32_t mmu_id, uint32_t entry_id)
{
    (void)mmu_id;
    HAL_ASSERT(entry_id < MMU_ENTRY_NUM);

    REG_WRITE(SPI_MEM_MMU_ITEM_INDEX_REG(0), entry_id);
    return (REG_READ(SPI_MEM_MMU_ITEM_CONTENT_REG(0)) & MMU_VALID) ? true : false;
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
    HAL_ASSERT(entry_id < MMU_ENTRY_NUM);

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
        case MMU_PAGE_8KB:
            shift_code = 13;
            break;
        default:
            HAL_ASSERT(shift_code);
    }

    REG_WRITE(SPI_MEM_MMU_ITEM_INDEX_REG(0), entry_id);
    return (REG_READ(SPI_MEM_MMU_ITEM_CONTENT_REG(0)) & MMU_VALID_VAL_MASK) << shift_code;
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
    for (int i = 0; i < MMU_ENTRY_NUM; i++) {
        if (mmu_ll_check_entry_valid(mmu_id, i)) {
            if (mmu_ll_get_entry_target(mmu_id, i) == target) {
                REG_WRITE(SPI_MEM_MMU_ITEM_INDEX_REG(0), i);
                if ((REG_READ(SPI_MEM_MMU_ITEM_CONTENT_REG(0)) & MMU_VALID_VAL_MASK) == mmu_val) {
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
        case MMU_PAGE_8KB:
            shift_code = 13;
            break;
        default:
            HAL_ASSERT(shift_code);
    }
    uint32_t laddr = entry_id << shift_code;
    return mmu_ll_laddr_to_vaddr(laddr, type);
}


#ifdef __cplusplus
}

#endif
