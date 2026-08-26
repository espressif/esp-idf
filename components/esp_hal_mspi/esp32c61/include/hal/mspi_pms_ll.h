/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The ll is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "hal/assert.h"
#include "hal/config.h"
#include "hal/mspi_ll.h"
#include "hal/mspi_pms_types.h"
#include "soc/spi_mem_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MSPI_LL_PMS_REGION_NUM              4
#define MSPI_LL_PMS_REGION_ADDR_ALIGN       0x1000

/**
 * @brief Enable/Disable MSPI PMS permission management
 *
 * @param mem    Memory type (shared register - value ignored on this target)
 * @param enable true to enable, false to disable
 */
__attribute__((always_inline))
static inline void mspi_ll_pms_enable(mspi_pms_mem_t mem, bool enable)
{
    (void)mem;
    SPIMEM0.mem_pms_reject.mem_pm_en = enable;
}

/**
 * @brief Set MSPI PMS region start address
 *
 * @param mem  Memory type (flash or PSRAM)
 * @param regn Region number (0-3)
 * @param addr Start address of the PMS region
 */
__attribute__((always_inline))
static inline void mspi_ll_pms_set_region_addr(mspi_pms_mem_t mem, uint32_t regn, uint32_t addr)
{
    HAL_ASSERT(mem == MSPI_PMS_MEM_FLASH || mem == MSPI_PMS_MEM_PSRAM);
    HAL_ASSERT(regn < MSPI_LL_PMS_REGION_NUM);

    if (mem == MSPI_PMS_MEM_FLASH) {
        SPIMEM0.fmem_pmsn_addr[regn].fmem_pmsn_addr_s = addr;
    } else {
        SPIMEM0.smem_pmsn_addr[regn].smem_pmsn_addr_s = addr;
    }
}

/**
 * @brief Set MSPI PMS region size (in bytes)
 *
 * @param mem  Memory type (flash or PSRAM)
 * @param regn Region number (0-3)
 * @param size Size in bytes (must be a multiple of MSPI_LL_PMS_REGION_ADDR_ALIGN)
 */
__attribute__((always_inline))
static inline void mspi_ll_pms_set_region_size(mspi_pms_mem_t mem, uint32_t regn, uint32_t size)
{
    HAL_ASSERT(mem == MSPI_PMS_MEM_FLASH || mem == MSPI_PMS_MEM_PSRAM);
    HAL_ASSERT(regn < MSPI_LL_PMS_REGION_NUM);
    HAL_ASSERT((size % MSPI_LL_PMS_REGION_ADDR_ALIGN) == 0);

    if (mem == MSPI_PMS_MEM_FLASH) {
        SPIMEM0.fmem_pmsn_size[regn].fmem_pmsn_size = size / MSPI_LL_PMS_REGION_ADDR_ALIGN;
    } else {
        SPIMEM0.smem_pmsn_size[regn].smem_pmsn_size = size / MSPI_LL_PMS_REGION_ADDR_ALIGN;
    }
}

/**
 * @brief Get MSPI PMS region start address
 *
 * @param mem  Memory type (flash or PSRAM)
 * @param regn Region number (0-3)
 * @return Start address of the PMS region
 */
__attribute__((always_inline))
static inline uint32_t mspi_ll_pms_get_region_addr(mspi_pms_mem_t mem, uint32_t regn)
{
    HAL_ASSERT(mem == MSPI_PMS_MEM_FLASH || mem == MSPI_PMS_MEM_PSRAM);
    HAL_ASSERT(regn < MSPI_LL_PMS_REGION_NUM);

    uint32_t addr = 0;
    if (mem == MSPI_PMS_MEM_FLASH) {
        addr = SPIMEM0.fmem_pmsn_addr[regn].fmem_pmsn_addr_s;
    } else {
        addr = SPIMEM0.smem_pmsn_addr[regn].smem_pmsn_addr_s;
    }
    return addr;
}

/**
 * @brief Get MSPI PMS region size (in bytes)
 *
 * @param mem  Memory type (flash or PSRAM)
 * @param regn Region number (0-3)
 * @return Size of the PMS region in bytes
 */
__attribute__((always_inline))
static inline uint32_t mspi_ll_pms_get_region_size(mspi_pms_mem_t mem, uint32_t regn)
{
    HAL_ASSERT(mem == MSPI_PMS_MEM_FLASH || mem == MSPI_PMS_MEM_PSRAM);
    HAL_ASSERT(regn < MSPI_LL_PMS_REGION_NUM);

    uint32_t size = 0;
    if (mem == MSPI_PMS_MEM_FLASH) {
        size = SPIMEM0.fmem_pmsn_size[regn].fmem_pmsn_size;
    } else {
        size = SPIMEM0.smem_pmsn_size[regn].smem_pmsn_size;
    }
    return size * MSPI_LL_PMS_REGION_ADDR_ALIGN;
}

/**
 * @brief Set MSPI PMS region attributes for a specific security mode
 *
 * @param mem  Memory type (flash or PSRAM)
 * @param regn Region number (0-3)
 * @param mode Security mode
 * @param attr Region attribute flags - RD/WR/ECC (mspi_pms_attr_t)
 */
__attribute__((always_inline))
static inline void mspi_ll_pms_set_region_attr(mspi_pms_mem_t mem, uint32_t regn, mspi_pms_mode_t mode, mspi_pms_attr_t attr)
{
    HAL_ASSERT(mem == MSPI_PMS_MEM_FLASH || mem == MSPI_PMS_MEM_PSRAM);
    HAL_ASSERT(regn < MSPI_LL_PMS_REGION_NUM);

    if (mode != MSPI_PMS_MODE_TEE) {
        return;
    }

    uint32_t bits = attr & (MSPI_PMS_ATTR_RD | MSPI_PMS_ATTR_WR | MSPI_PMS_ATTR_ECC);
    if (mem == MSPI_PMS_MEM_FLASH) {
        SPIMEM0.fmem_pmsn_attr[regn].val = bits;
    } else {
        SPIMEM0.smem_pmsn_attr[regn].val = bits;
    }
}

/**
 * @brief Get MSPI PMS reject address
 *
 * @param mem Memory type (shared register - value ignored on this target)
 * @return Address that caused PMS violation
 */
__attribute__((always_inline))
static inline uint32_t mspi_ll_pms_get_reject_addr(mspi_pms_mem_t mem)
{
    (void)mem;
    return SPIMEM0.mem_pms_reject_addr.mem_reject_addr;
}

/**
 * @brief Get MSPI PMS error mask
 *
 * @param mem Memory type (shared register - value ignored on this target)
 * @return Bitmask of error types (mspi_pms_err_t), or MSPI_PMS_ERR_NONE if no error
 */
__attribute__((always_inline))
static inline uint32_t mspi_ll_pms_get_err_mask(mspi_pms_mem_t mem)
{
    (void)mem;
    uint32_t err = MSPI_PMS_ERR_NONE;
    if (SPIMEM0.mem_pms_reject.mem_pms_multi_hit) {
        err |= MSPI_PMS_ERR_ADDR_MULTI;
    }
    if (SPIMEM0.mem_pms_reject.mem_pms_ivd) {
        err |= MSPI_PMS_ERR_ADDR_MISS;
    }
    if (SPIMEM0.mem_pms_reject.mem_pms_st) {
        err |= MSPI_PMS_ERR_WRITE;
    }
    if (SPIMEM0.mem_pms_reject.mem_pms_ld) {
        err |= MSPI_PMS_ERR_READ;
    }
    return err;
}

#ifdef __cplusplus
}
#endif
