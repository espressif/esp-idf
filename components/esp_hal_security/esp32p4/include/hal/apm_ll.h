/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "hal/assert.h"
#include "hal/config.h"
#include "soc/dma_pms_reg.h"
#include "soc/hp2lp_peri_pms_reg.h"
#include "soc/hp_peri_pms_reg.h"
#include "soc/lp2hp_peri_pms_reg.h"
#include "soc/lp_peri_pms_reg.h"

#include "hal/apm_types.h"
#include "soc/apm_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Offsets for indexing R/W attribute registers for DMA masters */
#define APM_LL_DMA_PMS_ATTR_RD_OFFSET      0x04U
#define APM_LL_DMA_PMS_ATTR_WR_OFFSET_1    0x04U
#define APM_LL_DMA_PMS_ATTR_WR_OFFSET_2    0x14U

/* Offsets for indexing HP2LP_PERI_PMS registers by CPU core and security mode */
#define APM_LL_HP2LP_PERI_PMS_CORE_OFFSET  0x08U
#define APM_LL_HP2LP_PERI_PMS_MODE_OFFSET  0x04U

/* Bit-field masks/shifts for extracting LP_PERI_PMS region permission fields */
#define APM_LL_LP_PERI_PMS_REGN0_1_MASK    0x03U
#define APM_LL_LP_PERI_PMS_REGN2_7_SHIFT   2U
#define APM_LL_LP_PERI_PMS_REGN2_7_MASK    0x3FU

/* Helper macro for indexing HP_PERI_PMS registers by core, mode, and register index */
#define APM_LL_HP_PERI_PMS_CORE_OFFSET     0x20U
#define APM_LL_HP_PERI_PMS_MODE_OFFSET     0x10U
#define APM_LL_HP_PERI_PMS_REG_OFFSET      0x04U

#define HP_PERI_PMS_COREn_xM_REGk_REG(cid, mode, regk) \
    (PMS_CORE0_MM_HP_PERI_PMS_REG0_REG                 \
     + ((cid)  * APM_LL_HP_PERI_PMS_CORE_OFFSET)       \
     + ((mode) * APM_LL_HP_PERI_PMS_MODE_OFFSET)       \
     + ((regk) * APM_LL_HP_PERI_PMS_REG_OFFSET))

/* Helper macros for indexing LP_PERI_PMS configurable region address registers */
#define APM_LL_LP_PERI_PMS_REGION_OFFSET   0x08U
#define APM_LL_LP_PERI_PMS_REGION_GAP      0x08U

#define PMS_PERI_REGION_LOW_REG(regn)              \
    (PMS_PERI_REGION0_LOW_REG                      \
     + ((regn) * APM_LL_LP_PERI_PMS_REGION_OFFSET) \
     + (((regn) >= 2U) ? APM_LL_LP_PERI_PMS_REGION_GAP : 0U))

#define PMS_PERI_REGION_HIGH_REG(regn)             \
    (PMS_PERI_REGION0_HIGH_REG                     \
     + ((regn) * APM_LL_LP_PERI_PMS_REGION_OFFSET) \
     + (((regn) >= 2U) ? APM_LL_LP_PERI_PMS_REGION_GAP : 0U))

/**
 * @brief Enable/disable clock gating for DMA_PMS
 *
 * @param enable True to enable, false to disable
 */
static inline void apm_ll_dma_pms_enable_clk_gating(bool enable)
{
    if (enable) {
        REG_CLR_BIT(PMS_DMA_CLK_EN_REG, PMS_DMA_CLK_EN);
    } else {
        REG_SET_BIT(PMS_DMA_CLK_EN_REG, PMS_DMA_CLK_EN);
    }
}

/**
 * @brief Set region start address in DMA_PMS
 *
 * @param regn_num Region number
 * @param addr Start address
 */
static inline void apm_ll_dma_pms_set_region_start_addr(uint32_t regn_num, uint32_t addr)
{
    REG_WRITE(PMS_DMA_REGION0_LOW_REG + APM_DMA_PMS_REGION_ADDR_OFFSET * regn_num, addr);
}

/**
 * @brief Set region end address in DMA_PMS
 *
 * @param regn_num Region number
 * @param addr End address
 */
static inline void apm_ll_dma_pms_set_region_end_addr(uint32_t regn_num, uint32_t addr)
{
    REG_WRITE(PMS_DMA_REGION0_HIGH_REG + APM_DMA_PMS_REGION_ADDR_OFFSET * regn_num, addr);
}

/**
 * @brief Set read-access region mask for a DMA master in DMA_PMS
 *
 * @param mid DMA master ID
 * @param regn_mask Bitmask of regions the master is permitted to read
 */
static inline void apm_ll_dma_pms_set_region_attr_r(apm_master_dma_id_t mid, uint32_t regn_mask)
{
    uint32_t offs = APM_DMA_PMS_REGION_ATTR_OFFSET * (uint32_t)mid;
    if (mid >= APM_MASTER_DMA_AXI_PDMA_LCDCAM) {
        offs += APM_LL_DMA_PMS_ATTR_RD_OFFSET;
    }

    REG_WRITE(PMS_DMA_GDMA_CH0_R_PMS_REG + offs, regn_mask);
}

/**
 * @brief Set write-access region mask for a DMA master in DMA_PMS
 *
 * @param mid DMA master ID
 * @param regn_mask Bitmask of regions the master is permitted to write
 */
static inline void apm_ll_dma_pms_set_region_attr_w(apm_master_dma_id_t mid, uint32_t regn_mask)
{
    uint32_t offs = APM_DMA_PMS_REGION_ATTR_OFFSET * (uint32_t)mid;
    if (mid == APM_MASTER_DMA_AHB_PDMA_RMT) {
        offs += APM_LL_DMA_PMS_ATTR_WR_OFFSET_2;
    } else if (mid >= APM_MASTER_DMA_AXI_PDMA_LCDCAM) {
        offs += APM_LL_DMA_PMS_ATTR_WR_OFFSET_1;
    }

    REG_WRITE(PMS_DMA_GDMA_CH0_W_PMS_REG + offs, regn_mask);
}

/**
 * @brief Enable/disable clock gating for HP_PERI_PMS
 *
 * @param enable True to enable, false to disable
 */
static inline void apm_ll_hp_peri_pms_enable_clk_gating(bool enable)
{
    if (enable) {
        REG_CLR_BIT(PMS_HP_PERI_PMS_CLK_EN_REG, PMS_HP_PERI_PMS_CLK_EN);
    } else {
        REG_SET_BIT(PMS_HP_PERI_PMS_CLK_EN_REG, PMS_HP_PERI_PMS_CLK_EN);
    }
}

/**
 * @brief Enable/disable HP CPU access to a CPU peripheral in HP_PERI_PMS
 *
 * @param core_id CPU core ID
 * @param mode Security mode
 * @param sid CPU peripheral slave ID
 * @param enable True to enable access, false to disable
 */
static inline void apm_ll_hp_peri_pms_enable_hpcpu_cpu_peri_access(const int core_id, apm_security_mode_t mode, apm_slave_cpu_periph_id_t sid, bool enable)
{
    if (enable) {
        REG_SET_BIT(HP_PERI_PMS_COREn_xM_REGk_REG(core_id, mode, 0), BIT(sid));
    } else {
        REG_CLR_BIT(HP_PERI_PMS_COREn_xM_REGk_REG(core_id, mode, 0), BIT(sid));
    }
}

/**
 * @brief Enable/disable HP CPU access to an HP peripheral (group 0) in HP_PERI_PMS
 *
 * @param core_id CPU core ID
 * @param mode Security mode
 * @param sid HP peripheral group 0 slave ID
 * @param enable True to enable access, false to disable
 */
static inline void apm_ll_hp_peri_pms_enable_hpcpu_hp_peri0_access(const int core_id, apm_security_mode_t mode, apm_slave_hp_periph0_id_t sid, bool enable)
{
    if (enable) {
        REG_SET_BIT(HP_PERI_PMS_COREn_xM_REGk_REG(core_id, mode, 1), BIT(sid));
    } else {
        REG_CLR_BIT(HP_PERI_PMS_COREn_xM_REGk_REG(core_id, mode, 1), BIT(sid));
    }
}

/**
 * @brief Enable/disable HP CPU access to an HP peripheral (group 1) in HP_PERI_PMS
 *
 * @param core_id CPU core ID
 * @param mode Security mode
 * @param sid HP peripheral group 1 slave ID
 * @param enable True to enable access, false to disable
 */
static inline void apm_ll_hp_peri_pms_enable_hpcpu_hp_peri1_access(const int core_id, apm_security_mode_t mode, apm_slave_hp_periph1_id_t sid, bool enable)
{
    uint32_t regk = (sid < 32) ? 2U : 3U;
    if (enable) {
        REG_SET_BIT(HP_PERI_PMS_COREn_xM_REGk_REG(core_id, mode, regk), BIT(sid & 0x1FU));
    } else {
        REG_CLR_BIT(HP_PERI_PMS_COREn_xM_REGk_REG(core_id, mode, regk), BIT(sid & 0x1FU));
    }
}

/**
 * @brief Enable/disable clock gating for HP2LP_PERI_PMS
 *
 * @param enable True to enable, false to disable
 */
static inline void apm_ll_hp2lp_peri_pms_enable_clk_gating(bool enable)
{
    if (enable) {
        REG_CLR_BIT(PMS_HP2LP_PERI_PMS_CLK_EN_REG, PMS_HP2LP_PERI_PMS_CLK_EN);
    } else {
        REG_SET_BIT(PMS_HP2LP_PERI_PMS_CLK_EN_REG, PMS_HP2LP_PERI_PMS_CLK_EN);
    }
}

/**
 * @brief Enable/disable HP CPU access to an LP peripheral in HP2LP_PERI_PMS
 *
 * @param core_id CPU core ID
 * @param mode Security mode
 * @param sid LP peripheral slave ID
 * @param enable True to enable access, false to disable
 */
static inline void apm_ll_hp2lp_peri_pms_enable_hpcpu_access(const int core_id, apm_security_mode_t mode, apm_slave_lp_periph_t sid, bool enable)
{
    uint32_t reg = PMS_HP_CORE0_MM_PMS_REG0_REG
                   + (core_id * APM_LL_HP2LP_PERI_PMS_CORE_OFFSET)
                   + (mode    * APM_LL_HP2LP_PERI_PMS_MODE_OFFSET);
    if (enable) {
        REG_SET_BIT(reg, BIT(sid));
    } else {
        REG_CLR_BIT(reg, BIT(sid));
    }
}

/**
 * @brief Enable/disable clock gating for LP_PERI_PMS
 *
 * @param enable True to enable, false to disable
 */
static inline void apm_ll_lp_peri_pms_enable_clk_gating(bool enable)
{
    if (enable) {
        REG_CLR_BIT(PMS_LP_PERI_PMS_CLK_EN_REG, PMS_LP_PERI_PMS_CLK_EN);
    } else {
        REG_SET_BIT(PMS_LP_PERI_PMS_CLK_EN_REG, PMS_LP_PERI_PMS_CLK_EN);
    }
}

/**
 * @brief Enable/disable LP CPU access to an LP peripheral in LP_PERI_PMS
 *
 * @param sid LP peripheral slave ID
 * @param enable True to enable access, false to disable
 */
static inline void apm_ll_lp_peri_pms_enable_lpcpu_access(apm_slave_lp_periph_t sid, bool enable)
{
    if (enable) {
        REG_SET_BIT(PMS_LP_MM_LP_PERI_PMS_REG0_REG, BIT(sid));
    } else {
        REG_CLR_BIT(PMS_LP_MM_LP_PERI_PMS_REG0_REG, BIT(sid));
    }
}

/**
 * @brief Set region start address in LP_PERI_PMS
 *
 * @param regn_num Region number
 * @param addr Start address
 */
static inline void apm_ll_lp_peri_pms_set_region_start_addr(uint32_t regn_num, uint32_t addr)
{
    REG_WRITE(PMS_PERI_REGION_LOW_REG(regn_num), addr);
}

/**
 * @brief Set region end address in LP_PERI_PMS
 *
 * @param regn_num Region number
 * @param addr End address
 */
static inline void apm_ll_lp_peri_pms_set_region_end_addr(uint32_t regn_num, uint32_t addr)
{
    REG_WRITE(PMS_PERI_REGION_HIGH_REG(regn_num), addr);
}

/**
 * @brief Set LP_PERI_PMS region access permissions for HP CPU core 0
 *
 * @param mode Security mode
 * @param regn_mask Bitmask of regions the core is permitted to access
 */
static inline void apm_ll_lp_peri_pms_hpcpu0_set_region_access(apm_security_mode_t mode, uint32_t regn_mask)
{
    uint32_t regn01 = regn_mask & APM_LL_LP_PERI_PMS_REGN0_1_MASK;
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
    uint32_t regn27 = (regn_mask >> APM_LL_LP_PERI_PMS_REGN2_7_SHIFT) & APM_LL_LP_PERI_PMS_REGN2_7_MASK;
#endif

    switch (mode) {
    case APM_SEC_MODE_TEE:
        /* regions 0–1 */
        REG_SET_FIELD(PMS_PERI_REGION_PMS_REG, PMS_HP_CORE0_MM_REGION_PMS, regn01);
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
        /* regions 2–7 */
        REG_SET_FIELD(PMS_PERI_REGION_2_TO_7_PMS_REG, PMS_HP_CORE0_MM_REGION_2_TO_7_PMS, regn27);
#endif
        break;
    case APM_SEC_MODE_REE:
        /* regions 0–1 */
        REG_SET_FIELD(PMS_PERI_REGION_PMS_REG, PMS_HP_CORE0_UM_REGION_PMS, regn01);
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
        /* regions 2–7 */
        REG_SET_FIELD(PMS_PERI_REGION_2_TO_7_PMS_REG, PMS_HP_CORE0_UM_REGION_2_TO_7_PMS, regn27);
#endif
        break;
    default:
        break;
    }
}

/**
 * @brief Set LP_PERI_PMS region access permissions for HP CPU core 1
 *
 * @param mode Security mode
 * @param regn_mask Bitmask of regions the core is permitted to access
 */
static inline void apm_ll_lp_peri_pms_hpcpu1_set_region_access(apm_security_mode_t mode, uint32_t regn_mask)
{
    uint32_t regn01 = regn_mask & APM_LL_LP_PERI_PMS_REGN0_1_MASK;
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
    uint32_t regn27 = (regn_mask >> APM_LL_LP_PERI_PMS_REGN2_7_SHIFT) & APM_LL_LP_PERI_PMS_REGN2_7_MASK;
#endif

    switch (mode) {
    case APM_SEC_MODE_TEE:
        /* regions 0–1 */
        REG_SET_FIELD(PMS_PERI_REGION_PMS_REG, PMS_HP_CORE1_MM_REGION_PMS, regn01);
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
        /* regions 2–7 */
        REG_SET_FIELD(PMS_PERI_REGION_2_TO_7_PMS_REG, PMS_HP_CORE1_MM_REGION_2_TO_7_PMS, regn27);
#endif
        break;
    case APM_SEC_MODE_REE:
        /* regions 0–1 */
        REG_SET_FIELD(PMS_PERI_REGION_PMS_REG, PMS_HP_CORE1_UM_REGION_PMS, regn01);
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
        /* regions 2–7 */
        REG_SET_FIELD(PMS_PERI_REGION_2_TO_7_PMS_REG, PMS_HP_CORE1_UM_REGION_2_TO_7_PMS, regn27);
#endif
        break;
    default:
        break;
    }
}

/**
 * @brief Set LP_PERI_PMS region access permissions for LP CPU
 *
 * @param regn_mask Bitmask of regions the LP CPU is permitted to access
 */
static inline void apm_ll_lp_peri_pms_lpcpu_set_region_access(uint32_t regn_mask)
{
    /* regions 0–1 */
    REG_SET_FIELD(PMS_PERI_REGION_PMS_REG, PMS_LP_CORE_REGION_PMS, regn_mask & APM_LL_LP_PERI_PMS_REGN0_1_MASK);

#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
    /* regions 2–7 */
    REG_SET_FIELD(PMS_PERI_REGION_2_TO_7_PMS_REG, PMS_LP_CORE_REGION_2_TO_7_PMS,
                  (regn_mask >> APM_LL_LP_PERI_PMS_REGN2_7_SHIFT) & APM_LL_LP_PERI_PMS_REGN2_7_MASK);
#endif
}

/**
 * @brief Enable/disable clock gating for LP2HP_PERI_PMS
 *
 * @param enable True to enable, false to disable
 */
static inline void apm_ll_lp2hp_peri_pms_enable_clk_gating(bool enable)
{
    if (enable) {
        REG_CLR_BIT(PMS_LP2HP_PERI_PMS_CLK_EN_REG, PMS_LP2HP_PERI_PMS_CLK_EN);
    } else {
        REG_SET_BIT(PMS_LP2HP_PERI_PMS_CLK_EN_REG, PMS_LP2HP_PERI_PMS_CLK_EN);
    }
}

/**
 * @brief Enable/disable LP CPU access to a CPU peripheral in LP2HP_PERI_PMS
 *
 * @param sid CPU peripheral slave ID
 * @param enable True to enable access, false to disable
 */
static inline void apm_ll_lp2hp_peri_pms_enable_lpcpu_cpu_peri_access(apm_slave_cpu_periph_id_t sid, bool enable)
{
    if (enable) {
        REG_SET_BIT(PMS_LP_MM_PMS_REG0_REG, BIT(sid));
    } else {
        REG_CLR_BIT(PMS_LP_MM_PMS_REG0_REG, BIT(sid));
    }
}

/**
 * @brief Enable/disable LP CPU access to an HP peripheral (group 0) in LP2HP_PERI_PMS
 *
 * @param sid HP peripheral group 0 slave ID
 * @param enable True to enable access, false to disable
 */
static inline void apm_ll_lp2hp_peri_pms_enable_lpcpu_hp_peri0_access(apm_slave_hp_periph0_id_t sid, bool enable)
{
    if (enable) {
        REG_SET_BIT(PMS_LP_MM_PMS_REG1_REG, BIT(sid));
    } else {
        REG_CLR_BIT(PMS_LP_MM_PMS_REG1_REG, BIT(sid));
    }
}

/**
 * @brief Enable/disable LP CPU access to an HP peripheral (group 1) in LP2HP_PERI_PMS
 *
 * @param sid HP peripheral group 1 slave ID
 * @param enable True to enable access, false to disable
 */
static inline void apm_ll_lp2hp_peri_pms_enable_lpcpu_hp_peri1_access(apm_slave_hp_periph1_id_t sid, bool enable)
{
    uint32_t reg = (sid < 32U) ? PMS_LP_MM_PMS_REG2_REG : PMS_LP_MM_PMS_REG3_REG;
    if (enable) {
        REG_SET_BIT(reg, BIT(sid & 0x1FU));
    } else {
        REG_CLR_BIT(reg, BIT(sid & 0x1FU));
    }
}

#ifdef __cplusplus
}
#endif
