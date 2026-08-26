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
#include "soc/spi_mem_s_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MSPI_LL_PMS_REGION_NUM              4
#define MSPI_LL_PMS_REGION_ADDR_ALIGN       0x1000
/* Each security-mode field in the PMS attribute register is 3 bits wide:
   bit 0/3: RD, bit 1/4: WR, bit 2/5: ECC. */
#define MSPI_LL_PMS_MODE_FIELD_WIDTH        3
#define MSPI_LL_PMS_FIELD_MASK              ((1U << MSPI_LL_PMS_MODE_FIELD_WIDTH) - 1)

/**
 * @brief Enable/Disable MSPI PMS permission management
 *
 * @param mem    Memory type (flash or PSRAM)
 * @param enable true to enable, false to disable
 */
__attribute__((always_inline))
static inline void mspi_ll_pms_enable(mspi_pms_mem_t mem, bool enable)
{
    if (mem == MSPI_PMS_MEM_FLASH) {
        SPIMEM0.pms_reject.pm_en = enable;
    } else {
        SPIMEM2.mem_pms_reject.mem_pm_en = enable;
    }
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
        SPIMEM2.smem_pmsn_addr[regn].smem_pms_addr_s = addr;
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
        SPIMEM2.smem_pmsn_size[regn].smem_pms_size = size / MSPI_LL_PMS_REGION_ADDR_ALIGN;
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
        addr = SPIMEM2.smem_pmsn_addr[regn].smem_pms_addr_s;
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
        size = SPIMEM2.smem_pmsn_size[regn].smem_pms_size;
    }
    return size * MSPI_LL_PMS_REGION_ADDR_ALIGN;
}

/**
 * @brief Set MSPI PMS region attributes for a specific security mode
 *
 * @param mem  Memory type (flash or PSRAM)
 * @param regn Region number (0-3)
 * @param mode Security mode (TEE or REE)
 * @param attr Region attribute flags - RD/WR/ECC (mspi_pms_attr_t)
 */
__attribute__((always_inline))
static inline void mspi_ll_pms_set_region_attr(mspi_pms_mem_t mem, uint32_t regn, mspi_pms_mode_t mode, mspi_pms_attr_t attr)
{
    HAL_ASSERT(mem == MSPI_PMS_MEM_FLASH || mem == MSPI_PMS_MEM_PSRAM);
    HAL_ASSERT(regn < MSPI_LL_PMS_REGION_NUM);

    /* Each mode field is 3 bits wide [RD, WR, ECC]: TEE at bits [2:0],
       REE at bits [5:3] - replace the whole field, shifted by mode * 3. */
    uint32_t shift = (uint32_t)mode * MSPI_LL_PMS_MODE_FIELD_WIDTH;
    uint32_t mask  = MSPI_LL_PMS_FIELD_MASK << shift;
    uint32_t bits  = (uint32_t)attr << shift;

    if (mem == MSPI_PMS_MEM_FLASH) {
        SPIMEM0.fmem_pmsn_attr[regn].val = (SPIMEM0.fmem_pmsn_attr[regn].val & ~mask) | bits;
    } else {
        SPIMEM2.smem_pmsn_attr[regn].val = (SPIMEM2.smem_pmsn_attr[regn].val & ~mask) | bits;
    }
}

/**
 * @brief Get MSPI PMS reject address
 *
 * @param mem Memory type (flash or PSRAM)
 * @return Address that caused PMS violation
 */
__attribute__((always_inline))
static inline uint32_t mspi_ll_pms_get_reject_addr(mspi_pms_mem_t mem)
{
    uint32_t addr = 0;
    if (mem == MSPI_PMS_MEM_FLASH) {
        addr = SPIMEM0.pms_reject.reject_addr;
    } else {
        addr = SPIMEM2.mem_pms_reject.mem_reject_addr;
    }
    return addr;
}

/**
 * @brief Get MSPI PMS error mask
 *
 * @param mem Memory type (flash or PSRAM)
 * @return Bitmask of error types (mspi_pms_err_t), or MSPI_PMS_ERR_NONE if no error
 */
__attribute__((always_inline))
static inline uint32_t mspi_ll_pms_get_err_mask(mspi_pms_mem_t mem)
{
    bool multi_hit, ivd, st, ld;
    if (mem == MSPI_PMS_MEM_FLASH) {
        spi_mem_c_pms_reject_reg_t r = { .val = SPIMEM0.pms_reject.val };
        multi_hit = r.pms_multi_hit;
        ivd       = r.pms_ivd;
        st        = r.pms_st;
        ld        = r.pms_ld;
    } else {
        spi_mem_s_pms_reject_reg_t r = { .val = SPIMEM2.mem_pms_reject.val };
        multi_hit = r.mem_pms_multi_hit;
        ivd       = r.mem_pms_ivd;
        st        = r.mem_pms_st;
        ld        = r.mem_pms_ld;
    }

    uint32_t err = MSPI_PMS_ERR_NONE;
    if (multi_hit) {
        err |= MSPI_PMS_ERR_ADDR_MULTI;
    }
    if (ivd) {
        err |= MSPI_PMS_ERR_ADDR_MISS;
    }
    if (st) {
        err |= MSPI_PMS_ERR_WRITE;
    }
    if (ld) {
        err |= MSPI_PMS_ERR_READ;
    }
    return err;
}

#ifdef __cplusplus
}
#endif
