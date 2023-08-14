/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for Cache register operations

#pragma once

#include "soc/cache_reg.h"
#include "soc/ext_mem_defs.h"
#include "hal/cache_types.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Given a L2MEM cached address, get the corresponding non-cacheable address
 * @example 0x4FF0_0000 => 0x8FF0_0000
 */
#define CACHE_LL_L2MEM_NON_CACHE_ADDR(addr) ((intptr_t)(addr) + 0x40000000)

#define CACHE_LL_ENABLE_DISABLE_STATE_SW            1   //There's no register indicating cache enable/disable state, we need to use software way for this state.

#define CACHE_LL_DEFAULT_IBUS_MASK                  CACHE_BUS_IBUS0
#define CACHE_LL_DEFAULT_DBUS_MASK                  CACHE_BUS_DBUS0

//TODO: IDF-7516
#define CACHE_LL_L1_ACCESS_EVENT_MASK               (0x3f)
// #define CACHE_LL_L1_ACCESS_EVENT_DBUS_WR_IC         (1<<5)
// #define CACHE_LL_L1_ACCESS_EVENT_DBUS_REJECT        (1<<4)
// #define CACHE_LL_L1_ACCESS_EVENT_DBUS_ACS_MSK_IC    (1<<3)
// #define CACHE_LL_L1_ACCESS_EVENT_IBUS_REJECT        (1<<2)
// #define CACHE_LL_L1_ACCESS_EVENT_IBUS_WR_IC         (1<<1)
// #define CACHE_LL_L1_ACCESS_EVENT_IBUS_ACS_MSK_IC    (1<<0)

// #define CACHE_LL_L1_ILG_EVENT_MASK                  (0x23)
// #define CACHE_LL_L1_ILG_EVENT_MMU_ENTRY_FAULT       (1<<5)
// #define CACHE_LL_L1_ILG_EVENT_PRELOAD_OP_FAULT      (1<<1)
// #define CACHE_LL_L1_ILG_EVENT_SYNC_OP_FAULT         (1<<0)

/**
 * @brief Get the buses of a particular cache that are mapped to a virtual address range
 *
 * External virtual address can only be accessed when the involved cache buses are enabled.
 * This API is to get the cache buses where the memory region (from `vaddr_start` to `vaddr_start + len`) reside.
 *
 * This api in esp32p4 is not used. There is no hardware interface to mask the i/dbus to l2 cache. Needs check, TODO: IDF-7516
 *
 * @param cache_id          cache ID (when l1 cache is per core)
 * @param vaddr_start       virtual address start
 * @param len               vaddr length
 */
#if !BOOTLOADER_BUILD
__attribute__((always_inline))
#endif
static inline cache_bus_mask_t cache_ll_l1_get_bus(uint32_t cache_id, uint32_t vaddr_start, uint32_t len)
{
    HAL_ASSERT(cache_id == 0 || cache_id == 1);
    cache_bus_mask_t mask = 0;

    uint32_t vaddr_end = vaddr_start + len - 1;
    if (vaddr_start >= DRAM_FLASH_ADDRESS_LOW && vaddr_end < DRAM_FLASH_ADDRESS_HIGH) {
        mask |= CACHE_BUS_IBUS0;
        mask |= CACHE_BUS_DBUS0;
    } else if (vaddr_start >= DRAM_PSRAM_ADDRESS_LOW && vaddr_end < DRAM_PSRAM_ADDRESS_HIGH) {
        mask |= CACHE_BUS_IBUS1;
        mask |= CACHE_BUS_DBUS1;
    } else {
        HAL_ASSERT(0);          //Out of region
    }

    return mask;
}

/**
 * Enable the Cache Buses
 *
 * This api is not used in esp32p4. Needs check, TODO: IDF-7516
 *
 * @param cache_id    cache ID (when l1 cache is per core)
 * @param mask        To know which buses should be enabled
 */
#if !BOOTLOADER_BUILD
__attribute__((always_inline))
#endif
static inline void cache_ll_l1_enable_bus(uint32_t cache_id, cache_bus_mask_t mask)
{
}

/**
 * Disable the Cache Buses
 *
 * This api is not used in esp32p4. Needs check, TODO: IDF-7516
 *
 * @param cache_id    cache ID (when l1 cache is per core)
 * @param mask        To know which buses should be disabled
 */
__attribute__((always_inline))
static inline void cache_ll_l1_disable_bus(uint32_t cache_id, cache_bus_mask_t mask)
{
}

/*------------------------------------------------------------------------------
 * Interrupt
 *----------------------------------------------------------------------------*/
/**
 * @brief Enable Cache access error interrupt
 *
 * @param cache_id    Cache ID
 * @param mask        Interrupt mask
 */
static inline void cache_ll_l1_enable_access_error_intr(uint32_t cache_id, uint32_t mask)
{
}

/**
 * @brief Clear Cache access error interrupt status
 *
 * @param cache_id    Cache ID
 * @param mask        Interrupt mask
 */
static inline void cache_ll_l1_clear_access_error_intr(uint32_t cache_id, uint32_t mask)
{
}

/**
 * @brief Get Cache access error interrupt status
 *
 * @param cache_id    Cache ID
 * @param mask        Interrupt mask
 *
 * @return            Status mask
 */
static inline uint32_t cache_ll_l1_get_access_error_intr_status(uint32_t cache_id, uint32_t mask)
{
    return 0;
}

#ifdef __cplusplus
}
#endif
