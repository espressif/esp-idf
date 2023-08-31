/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for MMU register operations

#pragma once

#include <stdbool.h>
#include "soc/ext_mem_defs.h"
#include "soc/dport_reg.h"
#include "soc/dport_access.h"
#include "hal/assert.h"
#include "hal/mmu_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MMU_LL_PSRAM_ENTRY_START_ID    1152

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
    //ONly supports `MMU_PAGE_64KB`
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

    if (type & MMU_VADDR_DATA) {
        valid |= (SOC_ADDRESS_IN_DRAM1_CACHE(vaddr_start) && SOC_ADDRESS_IN_DRAM1_CACHE(vaddr_end)) ||
                (SOC_ADDRESS_IN_DROM0_CACHE(vaddr_start) && SOC_ADDRESS_IN_DROM0_CACHE(vaddr_end));
    }

    if (type & MMU_VADDR_INSTRUCTION) {
        valid |= (SOC_ADDRESS_IN_IRAM0_CACHE(vaddr_start) && SOC_ADDRESS_IN_IRAM0_CACHE(vaddr_end)) ||
                (SOC_ADDRESS_IN_IRAM1_CACHE(vaddr_start) && SOC_ADDRESS_IN_IRAM1_CACHE(vaddr_end)) ||
                (SOC_ADDRESS_IN_IROM0_CACHE(vaddr_start) && SOC_ADDRESS_IN_IROM0_CACHE(vaddr_end));
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
    uint32_t offset = 0;
    uint32_t shift_code = 0;
    uint32_t vaddr_mask = 0;

    //On ESP32, we only use PID0 and PID1
    if (SOC_ADDRESS_IN_DROM0_CACHE(vaddr)) {
        offset = 0;
        shift_code = 16;
        vaddr_mask = SOC_MMU_VADDR_MASK;
    } else if (SOC_ADDRESS_IN_IRAM0_CACHE(vaddr)) {
        offset = 64;
        shift_code = 16;
        vaddr_mask = SOC_MMU_VADDR_MASK;
    } else if (SOC_ADDRESS_IN_IRAM1_CACHE(vaddr)) {
        offset = 128;
        shift_code = 16;
        vaddr_mask = SOC_MMU_VADDR_MASK;
    } else if (SOC_ADDRESS_IN_IROM0_CACHE(vaddr)) {
        offset = 192;
        shift_code = 16;
        vaddr_mask = SOC_MMU_VADDR_MASK;
    } else if (SOC_ADDRESS_IN_DRAM1_CACHE(vaddr)) {
        //PSRAM page size 32KB
        offset = MMU_LL_PSRAM_ENTRY_START_ID;
        shift_code = 15;
        vaddr_mask = SOC_MMU_VADDR_MASK >> 1;
    } else {
        HAL_ASSERT(false);
    }

    return offset + ((vaddr & vaddr_mask) >> shift_code);
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
    uint32_t shift_code = 0;

    if (target == MMU_TARGET_FLASH0) {
        shift_code = 16;
    } else {
        //PSRAM page size 32KB
        shift_code = 15;
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
    (void)target;

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
    HAL_ASSERT(entry_id < SOC_MMU_ENTRY_NUM);
    DPORT_INTERRUPT_DISABLE();
    switch (mmu_id) {
        case MMU_TABLE_CORE0:
            DPORT_WRITE_PERI_REG((uint32_t)&DPORT_PRO_FLASH_MMU_TABLE[entry_id], SOC_MMU_INVALID);
            break;
        case MMU_TABLE_CORE1:
            DPORT_WRITE_PERI_REG((uint32_t)&DPORT_APP_FLASH_MMU_TABLE[entry_id], SOC_MMU_INVALID);
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
    (void)mmu_id;
    HAL_ASSERT(entry_id < SOC_MMU_ENTRY_NUM);

    DPORT_INTERRUPT_DISABLE();
    uint32_t mmu_value = DPORT_SEQUENCE_REG_READ((uint32_t)&DPORT_PRO_FLASH_MMU_TABLE[entry_id]);
    DPORT_INTERRUPT_RESTORE();

    return (mmu_value & SOC_MMU_INVALID) ? false : true;
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
    HAL_ASSERT(entry_id < SOC_MMU_ENTRY_NUM);
    HAL_ASSERT(mmu_ll_check_entry_valid(mmu_id, entry_id));

    return (entry_id >= MMU_LL_PSRAM_ENTRY_START_ID) ? MMU_TARGET_PSRAM0 : MMU_TARGET_FLASH0;
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

    DPORT_INTERRUPT_DISABLE();
    uint32_t mmu_value = DPORT_SEQUENCE_REG_READ((uint32_t)&DPORT_PRO_FLASH_MMU_TABLE[entry_id]);
    DPORT_INTERRUPT_RESTORE();

    return (entry_id >= MMU_LL_PSRAM_ENTRY_START_ID) ? (mmu_value << 15) : (mmu_value << 16);
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
    (void)target;

    DPORT_INTERRUPT_DISABLE();
    if (target == MMU_TARGET_FLASH0) {
        for (int i = 0; i < SOC_MMU_ENTRY_NUM; i++) {
            uint32_t mmu_value = DPORT_SEQUENCE_REG_READ((uint32_t)&DPORT_PRO_FLASH_MMU_TABLE[i]);
            if (!(mmu_value & SOC_MMU_INVALID)) {
                if (mmu_value == mmu_val) {
                    DPORT_INTERRUPT_RESTORE();
                    return i;
                }
            }
        }
    } else {
        //For PSRAM, we only use PID 0/1. Its start entry ID is MMU_LL_PSRAM_ENTRY_START_ID (1152), and 128 entries are used for PSRAM
        for (int i = MMU_LL_PSRAM_ENTRY_START_ID; i < 1280; i++) {
            uint32_t mmu_value = DPORT_SEQUENCE_REG_READ((uint32_t)&DPORT_PRO_FLASH_MMU_TABLE[i]);
            if (!(mmu_value & SOC_MMU_INVALID)) {
                if (mmu_value == mmu_val) {
                    DPORT_INTERRUPT_RESTORE();
                    return i;
                }
            }
        }
    }
    DPORT_INTERRUPT_RESTORE();

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
    (void)type;
    uint32_t vaddr_base = 0;
    uint32_t shift_code = 0;

    if (entry_id < 64) {
        //first 64 entries are for DROM0
        if (type != MMU_VADDR_DATA) {
            return 0;
        }
        entry_id -= 0;
        shift_code = 16;
        vaddr_base = 0x3f400000;
    } else if (entry_id >= 64 && entry_id < 128) {
        //second 64 entries are for IRAM0
        if (type != MMU_VADDR_INSTRUCTION) {
            return 0;
        }
        entry_id -= 64;
        shift_code = 16;
        vaddr_base = 0x40000000;
    } else if (entry_id >= 128 && entry_id < 192) {
        //third 64 entries are for IRAM1
        if (type != MMU_VADDR_INSTRUCTION) {
            return 0;
        }
        entry_id -= 128;
        shift_code = 16;
        vaddr_base = 0x40000000;
    } else if (entry_id >= 192 && entry_id < 256) {
        //fourth 64 entries are for IROM0
        if (type != MMU_VADDR_INSTRUCTION) {
            return 0;
        }
        entry_id -= 192;
        shift_code = 16;
        vaddr_base = 0x40000000;
    } else if (entry_id >= MMU_LL_PSRAM_ENTRY_START_ID) {
        //starting from 1152, 128 entries are for DRAM1
        if (type != MMU_VADDR_DATA) {
            return 0;
        }
        entry_id -= MMU_LL_PSRAM_ENTRY_START_ID;
        shift_code = 15;
        vaddr_base = 0x3f800000;
    } else {
        HAL_ASSERT(false);
    }

    return vaddr_base + (entry_id << shift_code);
}

#ifdef __cplusplus
}
#endif
