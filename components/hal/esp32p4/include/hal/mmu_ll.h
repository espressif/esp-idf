/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for MMU register operations

#pragma once

#include "soc/spi_mem_c_reg.h"
#include "soc/spi_mem_s_reg.h"
#include "soc/ext_mem_defs.h"
#include "hal/assert.h"
#include "hal/mmu_types.h"
#include "hal/efuse_ll.h"


#ifdef __cplusplus
extern "C" {
#endif

///< MMU is per target
#define MMU_LL_MMU_PER_TARGET    1

#define MMU_LL_FLASH_MMU_ID      0
#define MMU_LL_PSRAM_MMU_ID      1

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
 * @param target      virtual address aimed physical memory target
 *
 * @return virtual address
 */
static inline uint32_t mmu_ll_laddr_to_vaddr(uint32_t laddr, mmu_vaddr_t vaddr_type, mmu_target_t target)
{
    (void)vaddr_type;
    uint32_t vaddr_base = 0;
    if (target == MMU_TARGET_FLASH0) {
        vaddr_base = SOC_MMU_FLASH_VADDR_BASE;
    } else {
        vaddr_base = SOC_MMU_PSRAM_VADDR_BASE;
    }

    return vaddr_base | laddr;
}

/**
 * Convert MMU virtual address to its target
 *
 * @param vaddr    virtual address
 *
 * @return target  paddr memory target
 */
__attribute__((always_inline))
static inline mmu_target_t mmu_ll_vaddr_to_target(uint32_t vaddr)
{
    mmu_target_t target = MMU_TARGET_FLASH0;

    if (SOC_ADDRESS_IN_DRAM_FLASH(vaddr)) {
        target = MMU_TARGET_FLASH0;
    } else if (SOC_ADDRESS_IN_DRAM_PSRAM(vaddr)) {
        target = MMU_TARGET_PSRAM0;
    } else {
        HAL_ASSERT(false);
    }

    return target;
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
    return MMU_PAGE_64KB;
}

/**
 * Set MMU page size
 *
 * @param size  MMU page size
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
    uint32_t vaddr_end = vaddr_start + len - 1;
    return (SOC_ADDRESS_IN_DRAM_FLASH(vaddr_start) && SOC_ADDRESS_IN_DRAM_FLASH(vaddr_end)) || (SOC_ADDRESS_IN_DRAM_PSRAM(vaddr_start) && SOC_ADDRESS_IN_DRAM_PSRAM(vaddr_end));
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
    int max_paddr_page_num = 0;
    if (mmu_id == MMU_LL_FLASH_MMU_ID) {
        max_paddr_page_num = SOC_MMU_FLASH_MAX_PADDR_PAGE_NUM;
    } else if (mmu_id == MMU_LL_PSRAM_MMU_ID) {
        max_paddr_page_num = SOC_MMU_PSRAM_MAX_PADDR_PAGE_NUM;
    } else {
        HAL_ASSERT(false);
    }

    return (paddr_start < (mmu_ll_get_page_size(mmu_id) * max_paddr_page_num)) &&
           (len < (mmu_ll_get_page_size(mmu_id) * max_paddr_page_num)) &&
           ((paddr_start + len - 1) < (mmu_ll_get_page_size(mmu_id) * max_paddr_page_num));
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
__attribute__((always_inline)) static inline void mmu_ll_write_entry(uint32_t mmu_id, uint32_t entry_id, uint32_t mmu_val, mmu_target_t target)
{
    uint32_t index_reg = 0;
    uint32_t content_reg = 0;
    uint32_t sensitive = 0;

    if (mmu_id == MMU_LL_FLASH_MMU_ID) {
        index_reg = SPI_MEM_C_MMU_ITEM_INDEX_REG;
        content_reg = SPI_MEM_C_MMU_ITEM_CONTENT_REG;
        sensitive = SOC_MMU_FLASH_SENSITIVE;
        mmu_val |= SOC_MMU_FLASH_VALID;
        mmu_val |= SOC_MMU_ACCESS_FLASH;
    } else if (mmu_id == MMU_LL_PSRAM_MMU_ID) {
        index_reg = SPI_MEM_S_MMU_ITEM_INDEX_REG;
        content_reg = SPI_MEM_S_MMU_ITEM_CONTENT_REG;
        sensitive = SOC_MMU_PSRAM_SENSITIVE;
        mmu_val |= SOC_MMU_PSRAM_VALID;
        mmu_val |= SOC_MMU_ACCESS_PSRAM;
    } else {
        HAL_ASSERT(false);
    }

    if (mmu_ll_cache_encryption_enabled()) {
        mmu_val |= sensitive;
    }

    REG_WRITE(index_reg, entry_id);
    REG_WRITE(content_reg, mmu_val);
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
    uint32_t index_reg = 0;
    uint32_t content_reg = 0;
    uint32_t mmu_val = 0;

    if (mmu_id == MMU_LL_FLASH_MMU_ID) {
        index_reg = SPI_MEM_C_MMU_ITEM_INDEX_REG;
        content_reg = SPI_MEM_C_MMU_ITEM_CONTENT_REG;
    } else if (mmu_id == MMU_LL_PSRAM_MMU_ID) {
        index_reg = SPI_MEM_S_MMU_ITEM_INDEX_REG;
        content_reg = SPI_MEM_S_MMU_ITEM_CONTENT_REG;
    } else {
        HAL_ASSERT(false);
    }

    REG_WRITE(index_reg, entry_id);
    mmu_val = REG_READ(content_reg);

    return mmu_val;
}

/**
 * Set MMU table entry as invalid
 *
 * @param mmu_id   MMU ID
 * @param entry_id MMU entry
 */
__attribute__((always_inline)) static inline void mmu_ll_set_entry_invalid(uint32_t mmu_id, uint32_t entry_id)
{
    uint32_t index_reg = 0;
    uint32_t content_reg = 0;
    uint32_t invalid_mask = 0;

    if (mmu_id == MMU_LL_FLASH_MMU_ID) {
        index_reg = SPI_MEM_C_MMU_ITEM_INDEX_REG;
        content_reg = SPI_MEM_C_MMU_ITEM_CONTENT_REG;
        invalid_mask = SOC_MMU_FLASH_INVALID;
    } else if (mmu_id == MMU_LL_PSRAM_MMU_ID) {
        index_reg = SPI_MEM_S_MMU_ITEM_INDEX_REG;
        content_reg = SPI_MEM_S_MMU_ITEM_CONTENT_REG;
        invalid_mask = SOC_MMU_PSRAM_INVALID;
    } else {
        HAL_ASSERT(false);
    }

    REG_WRITE(index_reg, entry_id);
    REG_WRITE(content_reg, invalid_mask);
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
 * @return         Ture for MMU entry is valid; False for invalid
 */
static inline bool mmu_ll_check_entry_valid(uint32_t mmu_id, uint32_t entry_id)
{
    uint32_t mmu_raw_value = 0;
    uint32_t index_reg = 0;
    uint32_t content_reg = 0;
    uint32_t valid_mask = 0;

    if (mmu_id == MMU_LL_FLASH_MMU_ID) {
        index_reg = SPI_MEM_C_MMU_ITEM_INDEX_REG;
        content_reg = SPI_MEM_C_MMU_ITEM_CONTENT_REG;
        valid_mask = SOC_MMU_FLASH_VALID;
    } else if (mmu_id == MMU_LL_PSRAM_MMU_ID) {
        index_reg = SPI_MEM_S_MMU_ITEM_INDEX_REG;
        content_reg = SPI_MEM_S_MMU_ITEM_CONTENT_REG;
        valid_mask = SOC_MMU_PSRAM_VALID;
    } else {
        HAL_ASSERT(false);
    }

    REG_WRITE(index_reg, entry_id);
    mmu_raw_value = REG_READ(content_reg);

    bool is_valid = false;
    if (mmu_raw_value & valid_mask) {
        is_valid = true;
    }

    return is_valid;
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
    (void)entry_id;
    mmu_target_t target = MMU_TARGET_FLASH0;

    if (mmu_id == MMU_LL_FLASH_MMU_ID) {
        target = MMU_TARGET_FLASH0;
    } else if (mmu_id == MMU_LL_PSRAM_MMU_ID) {
        target = MMU_TARGET_PSRAM0;
    } else {
        HAL_ASSERT(false);
    }

    return target;
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
    HAL_ASSERT(entry_id < SOC_MMU_ENTRY_NUM);

    uint32_t paddr_base = 0;
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
    if (mmu_id == MMU_LL_FLASH_MMU_ID) {
        REG_WRITE(SPI_MEM_C_MMU_ITEM_INDEX_REG, entry_id);
        paddr_base = (REG_READ(SPI_MEM_C_MMU_ITEM_CONTENT_REG) & SOC_MMU_FLASH_VALID_VAL_MASK) << shift_code;
    } else if (mmu_id == MMU_LL_PSRAM_MMU_ID) {
        REG_WRITE(SPI_MEM_S_MMU_ITEM_INDEX_REG, entry_id);
        paddr_base = (REG_READ(SPI_MEM_S_MMU_ITEM_CONTENT_REG) & SOC_MMU_PSRAM_VALID_VAL_MASK) << shift_code;
    } else {
        HAL_ASSERT(false);
    }

    return paddr_base;
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
    uint32_t index_reg = 0;
    uint32_t content_reg = 0;
    uint32_t valid_val_mask = 0;

    if (mmu_id == MMU_LL_FLASH_MMU_ID) {
        index_reg = SPI_MEM_C_MMU_ITEM_INDEX_REG;
        content_reg = SPI_MEM_C_MMU_ITEM_CONTENT_REG;
        valid_val_mask = SOC_MMU_FLASH_VALID_VAL_MASK;
    } else if (mmu_id == MMU_LL_PSRAM_MMU_ID) {
        index_reg = SPI_MEM_S_MMU_ITEM_INDEX_REG;
        content_reg = SPI_MEM_S_MMU_ITEM_CONTENT_REG;
        valid_val_mask = SOC_MMU_PSRAM_VALID_VAL_MASK;
    } else {
        HAL_ASSERT(false);
    }

    for (int i = 0; i < SOC_MMU_ENTRY_NUM; i++) {
        if (mmu_ll_check_entry_valid(mmu_id, i)) {
            if (mmu_ll_get_entry_target(mmu_id, i) == target) {
                REG_WRITE(index_reg, i);
                if ((REG_READ(content_reg) & valid_val_mask) == mmu_val) {
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
    return mmu_ll_laddr_to_vaddr(laddr, type, (mmu_id == MMU_LL_FLASH_MMU_ID) ? MMU_TARGET_FLASH0 : MMU_TARGET_PSRAM0);
}

#ifdef __cplusplus
}
#endif
