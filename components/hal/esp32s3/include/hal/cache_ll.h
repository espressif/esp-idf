/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for Cache register operations

#pragma once

#include <stdbool.h>
#include "soc/extmem_reg.h"
#include "soc/ext_mem_defs.h"
#include "hal/cache_types.h"
#include "hal/assert.h"


#ifdef __cplusplus
extern "C" {
#endif


#define CACHE_LL_DEFAULT_IBUS_MASK                       CACHE_BUS_IBUS0
#define CACHE_LL_DEFAULT_DBUS_MASK                       CACHE_BUS_DBUS0

#define CACHE_LL_L1_ACCESS_EVENT_MASK                    (0x1f)
#define CACHE_LL_L1_ACCESS_EVENT_DBUS_REJECT             (1<<4)
#define CACHE_LL_L1_ACCESS_EVENT_DBUS_ACS_MSK_DC_INT     (1<<3)
#define CACHE_LL_L1_ACCESS_EVENT_IBUS_REJECT             (1<<2)
#define CACHE_LL_L1_ACCESS_EVENT_IBUS_WR_IC              (1<<1)
#define CACHE_LL_L1_ACCESS_EVENT_IBUS_ACS_MSK_IC         (1<<0)

#define CACHE_LL_L1_ILG_EVENT_MASK                       (0x3f)
#define CACHE_LL_L1_ILG_EVENT_MMU_ENTRY_FAULT            (1<<5)
#define CACHE_LL_L1_ILG_EVENT_DCACHE_WRITE_FLASH         (1<<4)
#define CACHE_LL_L1_ILG_EVENT_DCACHE_PRELOAD_OP_FAULT    (1<<3)
#define CACHE_LL_L1_ILG_EVENT_DCACHE_SYNC_OP_FAULT       (1<<2)
#define CACHE_LL_L1_ILG_EVENT_ICACHE_PRELOAD_OP_FAULT    (1<<1)
#define CACHE_LL_L1_ILG_EVENT_ICACHE_SYNC_OP_FAULT       (1<<0)

/**
 * @brief Get the status of cache if it is enabled or not
 *
 * @param   cache_id    cache ID (when l1 cache is per core)
 * @param   type        see `cache_type_t`
 * @return  enabled or not
 */
__attribute__((always_inline))
static inline bool cache_ll_l1_is_cache_enabled(uint32_t cache_id, cache_type_t type)
{
    HAL_ASSERT(cache_id == 0 || cache_id == 1);

    bool enabled;
    if (type == CACHE_TYPE_INSTRUCTION) {
        enabled = REG_GET_BIT(EXTMEM_ICACHE_CTRL_REG, EXTMEM_ICACHE_ENABLE);
    } else if (type == CACHE_TYPE_DATA) {
        enabled = REG_GET_BIT(EXTMEM_DCACHE_CTRL_REG, EXTMEM_DCACHE_ENABLE);
    } else {
        enabled = REG_GET_BIT(EXTMEM_ICACHE_CTRL_REG, EXTMEM_ICACHE_ENABLE);
        enabled = enabled && REG_GET_BIT(EXTMEM_DCACHE_CTRL_REG, EXTMEM_DCACHE_ENABLE);
    }
    return enabled;
}

/**
 * @brief Get the buses of a particular cache that are mapped to a virtual address range
 *
 * External virtual address can only be accessed when the involved cache buses are enabled.
 * This API is to get the cache buses where the memory region (from `vaddr_start` to `vaddr_start + len`) reside.
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
    if (vaddr_start >= IRAM0_CACHE_ADDRESS_LOW && vaddr_end < IRAM0_CACHE_ADDRESS_HIGH) {
        mask |= CACHE_BUS_IBUS0;    //Both cores have their own IBUS0
    } else if (vaddr_start >= DRAM0_CACHE_ADDRESS_LOW && vaddr_end < DRAM0_CACHE_ADDRESS_HIGH) {
        mask |= CACHE_BUS_DBUS0;    //Both cores have their own DBUS0
    } else {
        HAL_ASSERT(0);      //Out of region
    }

    return mask;
}

/**
 * Enable the Cache Buses
 *
 * @param cache_id    cache ID (when l1 cache is per core)
 * @param mask        To know which buses should be enabled
 */
#if !BOOTLOADER_BUILD
__attribute__((always_inline))
#endif
static inline void cache_ll_l1_enable_bus(uint32_t cache_id, cache_bus_mask_t mask)
{
    HAL_ASSERT(cache_id == 0 || cache_id == 1);
    //On esp32s3, only `CACHE_BUS_IBUS0` and `CACHE_BUS_DBUS0` are supported. Use `cache_ll_l1_get_bus()` to get your bus first
    HAL_ASSERT((mask & (CACHE_BUS_IBUS1 | CACHE_BUS_IBUS2| CACHE_BUS_DBUS1 | CACHE_BUS_DBUS2)) == 0);

    uint32_t ibus_mask = 0;
    if (cache_id == 0) {
        ibus_mask |= (mask & CACHE_BUS_IBUS0) ? EXTMEM_ICACHE_SHUT_CORE0_BUS : 0;
    } else {
        ibus_mask |= (mask & CACHE_BUS_IBUS0) ? EXTMEM_ICACHE_SHUT_CORE1_BUS : 0;
    }
    REG_CLR_BIT(EXTMEM_ICACHE_CTRL1_REG, ibus_mask);

    uint32_t dbus_mask = 0;
    if (cache_id == 1) {
        dbus_mask |= (mask & CACHE_BUS_DBUS0) ? EXTMEM_DCACHE_SHUT_CORE0_BUS : 0;
    } else {
        dbus_mask |= (mask & CACHE_BUS_DBUS0) ? EXTMEM_DCACHE_SHUT_CORE1_BUS : 0;
    }
    REG_CLR_BIT(EXTMEM_DCACHE_CTRL1_REG, dbus_mask);
}

/**
 * Returns enabled buses for a given core
 *
 * @param cache_id    cache ID (when l1 cache is per core)
 *
 * @return State of enabled buses
 */
__attribute__((always_inline))
static inline cache_bus_mask_t cache_ll_l1_get_enabled_bus(uint32_t cache_id)
{
    cache_bus_mask_t mask = 0;
    HAL_ASSERT(cache_id == 0 || cache_id == 1);
    //On esp32s3, only `CACHE_BUS_IBUS0` and `CACHE_BUS_DBUS0` are supported. Use `cache_ll_l1_get_bus()` to get your bus first

    uint32_t ibus_mask = REG_READ(EXTMEM_ICACHE_CTRL1_REG);
    if (cache_id == 0) {
        mask |= (!(ibus_mask & EXTMEM_ICACHE_SHUT_CORE0_BUS)) ? CACHE_BUS_IBUS0 : 0;
    } else {
        mask |= (!(ibus_mask & EXTMEM_ICACHE_SHUT_CORE1_BUS)) ? CACHE_BUS_IBUS0 : 0;
    }

    uint32_t dbus_mask = REG_READ(EXTMEM_DCACHE_CTRL1_REG);
    if (cache_id == 1) {
        mask |= (!(dbus_mask & EXTMEM_DCACHE_SHUT_CORE0_BUS)) ? CACHE_BUS_DBUS0 : 0;
    } else {
        mask |= (!(dbus_mask & EXTMEM_DCACHE_SHUT_CORE1_BUS)) ? CACHE_BUS_DBUS0 : 0;
    }

    return mask;
}

/**
 * Disable the Cache Buses
 *
 * @param cache_id    cache ID (when l1 cache is per core)
 * @param mask        To know which buses should be disabled
 */
__attribute__((always_inline))
static inline void cache_ll_l1_disable_bus(uint32_t cache_id, cache_bus_mask_t mask)
{
    HAL_ASSERT(cache_id == 0 || cache_id == 1);
    //On esp32s3, only `CACHE_BUS_IBUS0` and `CACHE_BUS_DBUS0` are supported. Use `cache_ll_l1_get_bus()` to get your bus first
    HAL_ASSERT((mask & (CACHE_BUS_IBUS1 | CACHE_BUS_IBUS2| CACHE_BUS_DBUS1 | CACHE_BUS_DBUS2)) == 0);

    uint32_t ibus_mask = 0;
    if (cache_id == 0) {
        ibus_mask |= (mask & CACHE_BUS_IBUS0) ? EXTMEM_ICACHE_SHUT_CORE0_BUS : 0;
    } else {
        ibus_mask |= (mask & CACHE_BUS_IBUS0) ? EXTMEM_ICACHE_SHUT_CORE1_BUS : 0;
    }
    REG_SET_BIT(EXTMEM_ICACHE_CTRL1_REG, ibus_mask);

    uint32_t dbus_mask = 0;
    if (cache_id == 1) {
        dbus_mask |= (mask & CACHE_BUS_DBUS0) ? EXTMEM_DCACHE_SHUT_CORE0_BUS : 0;
    } else {
        dbus_mask |= (mask & CACHE_BUS_DBUS0) ? EXTMEM_DCACHE_SHUT_CORE1_BUS : 0;
    }
    REG_SET_BIT(EXTMEM_DCACHE_CTRL1_REG, dbus_mask);
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
    if (cache_id == 0) {
        SET_PERI_REG_MASK(EXTMEM_CORE0_ACS_CACHE_INT_ENA_REG, mask);
    } else {
        SET_PERI_REG_MASK(EXTMEM_CORE1_ACS_CACHE_INT_ENA_REG, mask);
    }
}

/**
 * @brief Clear Cache access error interrupt status
 *
 * @param cache_id    Cache ID
 * @param mask        Interrupt mask
 */
static inline void cache_ll_l1_clear_access_error_intr(uint32_t cache_id, uint32_t mask)
{
    if (cache_id == 0) {
        SET_PERI_REG_MASK(EXTMEM_CORE0_ACS_CACHE_INT_CLR_REG, mask);
    } else {
        SET_PERI_REG_MASK(EXTMEM_CORE1_ACS_CACHE_INT_CLR_REG, mask);
    }
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
    if (cache_id == 0) {
        return GET_PERI_REG_MASK(EXTMEM_CORE0_ACS_CACHE_INT_ST_REG, mask);
    } else {
        return GET_PERI_REG_MASK(EXTMEM_CORE1_ACS_CACHE_INT_ST_REG, mask);
    }
}

/**
 * @brief Enable Cache illegal error interrupt
 *
 * @param cache_id    Cache ID
 * @param mask        Interrupt mask
 */
static inline void cache_ll_l1_enable_illegal_error_intr(uint32_t cache_id, uint32_t mask)
{
    SET_PERI_REG_MASK(EXTMEM_CACHE_ILG_INT_ENA_REG, mask);
}

/**
 * @brief Clear Cache illegal error interrupt status
 *
 * @param cache_id    Cache ID
 * @param mask        Interrupt mask
 */
static inline void cache_ll_l1_clear_illegal_error_intr(uint32_t cache_id, uint32_t mask)
{
    SET_PERI_REG_MASK(EXTMEM_CACHE_ILG_INT_CLR_REG, mask);
}

/**
 * @brief Get Cache illegal error interrupt status
 *
 * @param cache_id    Cache ID
 * @param mask        Interrupt mask
 *
 * @return            Status mask
 */
static inline uint32_t cache_ll_l1_get_illegal_error_intr_status(uint32_t cache_id, uint32_t mask)
{
    return GET_PERI_REG_MASK(EXTMEM_CACHE_ILG_INT_ST_REG, mask);
}

#ifdef __cplusplus
}
#endif
