/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/pcr_reg.h"
#include "soc/tee_reg.h"
#include "soc/hp_apm_reg.h"
#include "soc/hp_apm_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TEE_LL_MODE_CTRL_REG(master_id) (TEE_M0_MODE_CTRL_REG + 4 * (master_id))
#define APM_LL_REGION_ADDR_START_REG(regn_num) (HP_APM_REGION0_ADDR_START_REG + 0xC * (regn_num))
#define APM_LL_REGION_ADDR_END_REG(regn_num) (HP_APM_REGION0_ADDR_END_REG + 0xC * (regn_num))
#define APM_LL_REGION_ADDR_ATTR_REG(regn_num) (HP_APM_REGION0_PMS_ATTR_REG + 0xC * (regn_num))
#define APM_LL_TEE_EXCP_STATUS_REG(sec_mode) (HP_APM_M0_STATUS_REG + 0x10 * (sec_mode))
#define APM_LL_TEE_EXCP_CLR_REG(sec_mode) (HP_APM_M0_STATUS_CLR_REG + 0x10 * (sec_mode))
#define APM_LL_TEE_EXCP_INFO0_REG(sec_mode) (HP_APM_M0_EXCEPTION_INFO0_REG + 0x10 * (sec_mode))

#define APM_LL_HP_SEC_MODE_REGION_ATTR(sec_mode, regn_pms) ((regn_pms) << (4 * (sec_mode - 1)))
#define APM_LL_HP_SEC_MODE_REGION_ATTR_V  0x00000003U
#define APM_LL_HP_SEC_MODE_REGION_ATTR_M(sec_mode) (APM_LL_HP_SEC_MODE_REGION_ATTR_V << (4 * (sec_mode - 1)))

#define APM_LL_HP_MAX_REGION_NUM 15
#define APM_LL_MASTER_MAX        32

/**
 * @brief APM Master ID
 */
typedef enum {
    APM_LL_MASTER_HPCORE       = 0,
    APM_LL_MASTER_LPCORE       = 1,
    APM_LL_MASTER_REGDMA       = 2,
    APM_LL_MASTER_SDIOSLV      = 3,
    APM_LL_MASTER_MODEM        = 4,
    APM_LL_MASTER_MEM_MONITOR  = 5,
    APM_LL_MASTER_TRACE        = 6,
    APM_LL_MASTER_GDMA         = 16, // The beginning of GDMA master ID
    APM_LL_MASTER_GDMA_SPI2    = 16,
    APM_LL_MASTER_GDMA_UHCI0   = 18,
    APM_LL_MASTER_GDMA_I2S0    = 19,
    APM_LL_MASTER_GDMA_AES     = 22,
    APM_LL_MASTER_GDMA_SHA     = 23,
    APM_LL_MASTER_GDMA_ADC     = 24,
    APM_LL_MASTER_GDMA_PARLIO  = 25,
} apm_ll_master_id_t;

/**
 * @brief APM Secure Mode
 */
typedef enum {
    APM_LL_SECURE_MODE_TEE = 0,  /* Trusted execution environment mode */
    APM_LL_SECURE_MODE_REE0 = 1, /* Rich execution environment mode0 (need to configure APM strategy for this mode) */
    APM_LL_SECURE_MODE_REE1 = 2, /* Rich execution environment mode1 (need to configure APM strategy for this mode) */
    APM_LL_SECURE_MODE_REE2 = 3, /* Rich execution environment mode2 (need to configure APM strategy for this mode) */
} apm_ll_secure_mode_t;

/**
 * @brief APM HP access path
 */
typedef enum {
    APM_LL_HP_ACCESS_PATH_M0 = 0x0,
    APM_LL_HP_ACCESS_PATH_M1 = 0x1,
    APM_LL_HP_ACCESS_PATH_M2 = 0x2,
    APM_LL_HP_ACCESS_PATH_M3 = 0x3,
} apm_ll_hp_access_path_t;

/**
 * @brief APM exception information
 */
typedef struct {
    uint8_t  excp_id;
    apm_ll_secure_mode_t sec_mode;
    uint8_t excp_regn;
    uint8_t excp_mode;
    uint32_t excp_addr;
} apm_hp_m_exception_info_t;

/**
 * @brief Set secure mode
 *
 * @param master_id APM master ID
 * @param mode Secure mode
 */
static inline void apm_tee_ll_set_master_secure_mode(apm_ll_master_id_t master_id, apm_ll_secure_mode_t sec_mode)
{
    REG_WRITE(TEE_LL_MODE_CTRL_REG(master_id), sec_mode);
}

/**
 * @brief TEE controller clock auto gating enable
 *
 * @param enable Flag for HP clock auto gating enable/disable
 */
static inline void apm_tee_ll_clk_gating_enable(bool enable)
{
    if (enable) {
        REG_SET_BIT(TEE_CLOCK_GATE_REG, TEE_CLK_EN);
    } else {
        REG_CLR_BIT(TEE_CLOCK_GATE_REG, TEE_CLK_EN);
    }
}

/**
 * @brief enable/disable HP Region access permission filter
 *
 * @param regn_num Memory Region number
 * @param enable Flag for Region access filter enable/disable
 */
static inline void apm_hp_ll_region_filter_enable(uint32_t regn_num, bool enable)
{
    if (enable) {
        REG_SET_BIT(HP_APM_REGION_FILTER_EN_REG, BIT(regn_num));
    } else {
        REG_CLR_BIT(HP_APM_REGION_FILTER_EN_REG, BIT(regn_num));
    }
}

/**
 * @brief enable/disable HP access path(M[0:3])
 *
 * @param hp_m_path HP access path
 * @param enable    Flag for HP M path filter enable/disable
 */
static inline void apm_hp_ll_m_filter_enable(apm_ll_hp_access_path_t hp_m_path, bool enable)
{
    if (enable) {
        REG_SET_BIT(HP_APM_FUNC_CTRL_REG, BIT(hp_m_path));
    } else {
        REG_CLR_BIT(HP_APM_FUNC_CTRL_REG, BIT(hp_m_path));
    }
}

/**
 * @brief HP Region start address configuration
 *
 * @param regn_num HP Region number to be configured
 * @param addr     Region start address
 */
static inline void apm_hp_ll_set_region_start_address(uint32_t regn_num, uint32_t addr)
{
    REG_WRITE(APM_LL_REGION_ADDR_START_REG(regn_num), addr);
}

/**
 * @brief HP Region end address configuration
 *
 * @param regn_num HP Region number to be configured
 * @param addr     Region end address
 */
static inline void apm_hp_ll_set_region_end_address(uint32_t regn_num, uint32_t addr)
{
    REG_WRITE(APM_LL_REGION_ADDR_END_REG(regn_num), addr);
}

/**
 * @brief HP Region pms attributes configuration
 *
 * @param regn_num Region number to be configured
 * @param sec_mode Secure mode of the Master
 * @param regn_pms XWR permissions for the given secure mode and Region number
 */
static inline void apm_hp_ll_sec_mode_region_attr_config(uint32_t regn_num, apm_ll_secure_mode_t sec_mode, uint32_t regn_pms)
{
    uint32_t val = 0;
    val = REG_READ(APM_LL_REGION_ADDR_ATTR_REG(regn_num));
    val &= ~APM_LL_HP_SEC_MODE_REGION_ATTR_M(sec_mode);
    val |= APM_LL_HP_SEC_MODE_REGION_ATTR(sec_mode, regn_pms);
    REG_WRITE(APM_LL_REGION_ADDR_ATTR_REG(regn_num), val);
}

/**
 * @brief Get HP access path(M[0:3]) exception status
 *
 * @param hp_m_path HP access path
 */
static inline uint8_t apm_hp_ll_m_exception_status(apm_ll_hp_access_path_t hp_m_path)
{
    return REG_READ(APM_LL_TEE_EXCP_STATUS_REG(hp_m_path));
}

/**
 * @brief Clear HP access path(M[0:3]) exception
 *
 * @param hp_m_path HP access path
 */
static inline void apm_hp_ll_m_exception_clear(apm_ll_hp_access_path_t hp_m_path)
{
    REG_SET_BIT(APM_LL_TEE_EXCP_CLR_REG(hp_m_path), HP_APM_M0_REGION_STATUS_CLR);
}

/**
 * @brief Get HP access path(M[0:3]) exception information
 *
 * @param excp_info Exception related information like addr,
 * region, sec_mode and master id
 */
static inline void apm_hp_ll_get_m_exception_info(apm_hp_m_exception_info_t *excp_info)
{
    excp_info->excp_id = REG_GET_FIELD(APM_LL_TEE_EXCP_INFO0_REG(excp_info->sec_mode), HP_APM_M0_EXCEPTION_ID);
    excp_info->excp_mode = REG_GET_FIELD(APM_LL_TEE_EXCP_INFO0_REG(excp_info->sec_mode), HP_APM_M0_EXCEPTION_MODE);
    excp_info->excp_regn = REG_GET_FIELD(APM_LL_TEE_EXCP_INFO0_REG(excp_info->sec_mode), HP_APM_M0_EXCEPTION_REGION);
    excp_info->excp_addr = REG_READ(HP_APM_M0_EXCEPTION_INFO1_REG);
}

/**
 * @brief Interrupt enable for access path(M[0:3])
 *
 * @param hp_m_path HP access path
 * @param enable    Flag for access path interrupt enable/disable
 */
static inline void apm_hp_ll_m_interrupt_enable(apm_ll_hp_access_path_t hp_m_path, bool enable)
{
    if (enable) {
        REG_SET_BIT(HP_APM_INT_EN_REG, BIT(hp_m_path));
    } else {
        REG_CLR_BIT(HP_APM_INT_EN_REG, BIT(hp_m_path));
    }
}

/**
 * @brief HP clock auto gating enable
 *
 * @param enable   Flag for HP clock auto gating enable/disable
 */
static inline void apm_hp_ll_clk_gating_enable(bool enable)
{
    if (enable) {
        REG_SET_BIT(HP_APM_CLOCK_GATE_REG, HP_APM_CLK_EN);
    } else {
        REG_CLR_BIT(HP_APM_CLOCK_GATE_REG, HP_APM_CLK_EN);
    }
}

/**
 * @brief APM/TEE/HP System Reg reset event bypass enable
 *
 * Disable: tee_reg/apm_reg/hp_system_reg will not only be reset by power-reset,
 * but also some reset events.
 * Enable: tee_reg/apm_reg/hp_system_reg will only be reset by power-reset.
 * Some reset events will be bypassed.
 *
 * @param enable   Flag for event bypass enable/disable
 */
static inline void apm_hp_ll_reset_event_enable(bool enable)
{
    if (enable) {
        REG_SET_BIT(PCR_RESET_EVENT_BYPASS_REG, PCR_RESET_EVENT_BYPASS_APM);
    } else {
        REG_CLR_BIT(PCR_RESET_EVENT_BYPASS_REG, PCR_RESET_EVENT_BYPASS_APM);
    }
}
#ifdef __cplusplus
}
#endif
