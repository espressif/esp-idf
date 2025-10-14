/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "soc/apm_defs.h"
#include "soc/tee_reg.h"
#include "soc/lp_tee_reg.h"
#include "soc/hp_apm_reg.h"
#include "soc/hp_apm_struct.h"
#include "soc/lp_apm0_reg.h"
#include "soc/lp_apm0_struct.h"
#include "soc/lp_apm_reg.h"
#include "soc/lp_apm_struct.h"

#include "soc/pcr_reg.h"
#include "soc/interrupts.h"
#include "hal/apm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Helper macros for calculating pms attr field position for given security mode */
#define APM_REGION_PMS_SHIFT(mode)      (4U * ((mode) - 1))
#define APM_REGION_PMS_MASK(mode)       (0x07U << APM_REGION_PMS_SHIFT(mode))
#define APM_REGION_PMS_FIELD(mode, pms) ((pms) << APM_REGION_PMS_SHIFT(mode))

/**
 * @brief Set security mode for a specific master in HP-TEE
 *
 * @param id Master ID
 * @param mode Security mode to set
 */
static inline void apm_ll_hp_tee_set_master_sec_mode(apm_master_id_t id, apm_security_mode_t mode)
{
    REG_WRITE(TEE_M0_MODE_CTRL_REG + APM_TEE_MODE_CTRL_OFFSET * id, mode);
}

/**
 * @brief Enable/disable clock gating for HP-TEE
 *
 * @param enable True to enable, false to disable
 */
static inline void apm_ll_hp_tee_enable_clk_gating(bool enable)
{
    if (enable) {
        REG_CLR_BIT(TEE_CLOCK_GATE_REG, TEE_CLK_EN);
    } else {
        REG_SET_BIT(TEE_CLOCK_GATE_REG, TEE_CLK_EN);
    }
}

/**
 * @brief Set security mode for a specific master in LP-TEE
 *
 * @param id Master ID (unused)
 * @param mode Security mode to set
 */
static inline void apm_ll_lp_tee_set_master_sec_mode(apm_master_id_t id, apm_security_mode_t mode)
{
    (void)id;
    REG_WRITE(LP_TEE_M0_MODE_CTRL_REG, mode);
}

/**
 * @brief Enable/disable clock gating for LP-TEE
 *
 * @param enable True to enable, false to disable
 */
static inline void apm_ll_lp_tee_enable_clk_gating(bool enable)
{
    if (enable) {
        REG_CLR_BIT(LP_TEE_CLOCK_GATE_REG, LP_TEE_CLK_EN);
    } else {
        REG_SET_BIT(LP_TEE_CLOCK_GATE_REG, LP_TEE_CLK_EN);
    }
}

/**
 * @brief Enable/disable forced HP memory access for LP-TEE
 *
 * @param enable True to enable, false to disable
 */
static inline void apm_ll_lp_tee_enable_force_hp_mem_access(bool enable)
{
    if (enable) {
        REG_SET_BIT(LP_TEE_FORCE_ACC_HP_REG, LP_TEE_FORCE_ACC_HPMEM_EN);
    } else {
        REG_CLR_BIT(LP_TEE_FORCE_ACC_HP_REG, LP_TEE_FORCE_ACC_HPMEM_EN);
    }
}

/**
 * @brief Enable/disable all controller filters in HP-APM
 *
 * @param enable True to enable, false to disable
 */
static inline void apm_ll_hp_apm_enable_ctrl_filter_all(bool enable)
{
    REG_WRITE(HP_APM_FUNC_CTRL_REG, enable ? UINT32_MAX : 0);
}

/**
 * @brief Enable/disable region filter in HP-APM
 *
 * @param regn_num Region number
 * @param enable True to enable, false to disable
 */
static inline void apm_ll_hp_apm_enable_region_filter(uint32_t regn_num, bool enable)
{
    if (enable) {
        REG_SET_BIT(HP_APM_REGION_FILTER_EN_REG, BIT(regn_num));
    } else {
        REG_CLR_BIT(HP_APM_REGION_FILTER_EN_REG, BIT(regn_num));
    }
}

/**
 * @brief Set region start address in HP-APM
 *
 * @param regn_num Region number
 * @param addr Start address
 */
static inline void apm_ll_hp_apm_set_region_start_addr(uint32_t regn_num, uint32_t addr)
{
    REG_WRITE(HP_APM_REGION0_ADDR_START_REG + APM_REGION_ADDR_OFFSET * regn_num, addr);
}

/**
 * @brief Set region end address in HP-APM
 *
 * @param regn_num Region number
 * @param addr End address
 */
static inline void apm_ll_hp_apm_set_region_end_addr(uint32_t regn_num, uint32_t addr)
{
    REG_WRITE(HP_APM_REGION0_ADDR_END_REG + APM_REGION_ADDR_OFFSET * regn_num, addr);
}

/**
 * @brief Set security mode region attributes in HP-APM
 *
 * @param regn_num Region number
 * @param mode Security mode
 * @param regn_pms Region PMS attributes
 */
static inline void apm_ll_hp_apm_set_sec_mode_region_attr(uint32_t regn_num, apm_security_mode_t mode, uint32_t regn_pms)
{
    uint32_t reg = HP_APM_REGION0_PMS_ATTR_REG + APM_REGION_PMS_ATTR_OFFSET * regn_num;
    uint32_t val = REG_READ(reg);
    val &= ~APM_REGION_PMS_MASK(mode);
    val |= APM_REGION_PMS_FIELD(mode, regn_pms);
    REG_WRITE(reg, val);
}

/**
 * @brief Get exception data (regn, master, security mode) from HP-APM
 *
 * @param path Access path
 * @return Exception data
 */
static inline uint32_t apm_ll_hp_apm_get_excp_data(apm_ctrl_access_path_t path)
{
    return REG_READ(HP_APM_M0_EXCEPTION_INFO0_REG + APM_EXCP_INFO_OFFSET * path);
}

/**
 * @brief Get exception status from HP-APM
 *
 * @param path Access path
 * @return Exception type
 */
static inline uint32_t apm_ll_hp_apm_get_excp_type(apm_ctrl_access_path_t path)
{
    return REG_READ(HP_APM_M0_STATUS_REG + APM_EXCP_INFO_OFFSET * path);
}

/**
 * @brief Get exception address from HP-APM
 *
 * @param path Access path
 * @return Exception address
 */
static inline uint32_t apm_ll_hp_apm_get_excp_addr(apm_ctrl_access_path_t path)
{
    return REG_READ(HP_APM_M0_EXCEPTION_INFO1_REG + APM_EXCP_INFO_OFFSET * path);
}

/**
 * @brief Get exception information from HP-APM
 *
 * @param path Access path
 * @param info Pointer to store exception information
 */
static inline void apm_ll_hp_apm_get_excp_info(apm_ctrl_access_path_t path, apm_ctrl_exception_info_t *info)
{
    hp_apm_m0_exception_info0_reg_t reg;
    reg.val = apm_ll_hp_apm_get_excp_data(path);
    info->regn = reg.m0_exception_region;
    info->mode = reg.m0_exception_mode;
    info->id   = reg.m0_exception_id;

    info->type = apm_ll_hp_apm_get_excp_type(path);
    info->addr = apm_ll_hp_apm_get_excp_addr(path);
}

/**
 * @brief Clear controller exception status in HP-APM
 *
 * @param path Access path
 */
static inline void apm_ll_hp_apm_clear_ctrl_excp_status(apm_ctrl_access_path_t path)
{
    REG_SET_BIT(HP_APM_M0_STATUS_CLR_REG + APM_EXCP_INFO_OFFSET * path, APM_EXCP_STATUS_CLR_BIT);
}

/**
 * @brief Enable/disable controller interrupt in HP-APM
 *
 * @param path Access path
 * @param enable True to enable, false to disable
 */
static inline void apm_ll_hp_apm_enable_ctrl_intr(apm_ctrl_access_path_t path, bool enable)
{
    if (enable) {
        REG_SET_BIT(HP_APM_INT_EN_REG, BIT(path));
    } else {
        REG_CLR_BIT(HP_APM_INT_EN_REG, BIT(path));
    }
}

/**
 * @brief Get controller interrupt source number from HP-APM
 *
 * @param path Access path
 * @return Interrupt source number
 */
static inline int apm_ll_hp_apm_get_ctrl_intr_src(apm_ctrl_access_path_t path)
{
    return ETS_HP_APM_M0_INTR_SOURCE + path;
}

/**
 * @brief Enable/disable controller clock gating in HP-APM
 *
 * @param enable True to enable, false to disable
 */
static inline void apm_ll_hp_apm_enable_ctrl_clk_gating(bool enable)
{
    if (enable) {
        REG_CLR_BIT(HP_APM_CLOCK_GATE_REG, HP_APM_CLK_EN);
    } else {
        REG_SET_BIT(HP_APM_CLOCK_GATE_REG, HP_APM_CLK_EN);
    }
}

/**
 * @brief Enable/disable controller filter for specific path in LP-APM0
 *
 * @param path Access path
 * @param enable True to enable, false to disable
 */
static inline void apm_ll_lp_apm0_enable_ctrl_filter(apm_ctrl_access_path_t path, bool enable)
{
    if (enable) {
        REG_SET_BIT(LP_APM0_FUNC_CTRL_REG, BIT(path));
    } else {
        REG_CLR_BIT(LP_APM0_FUNC_CTRL_REG, BIT(path));
    }
}

/**
 * @brief Enable/disable all controller filters in LP-APM0
 *
 * @param enable True to enable, false to disable
 */
static inline void apm_ll_lp_apm0_enable_ctrl_filter_all(bool enable)
{
    REG_WRITE(LP_APM0_FUNC_CTRL_REG, enable ? UINT32_MAX : 0);
}

/**
 * @brief Enable/disable region filter in LP-APM0
 *
 * @param regn_num Region number
 * @param enable True to enable, false to disable
 */
static inline void apm_ll_lp_apm0_enable_region_filter(uint32_t regn_num, bool enable)
{
    if (enable) {
        REG_SET_BIT(LP_APM0_REGION_FILTER_EN_REG, BIT(regn_num));
    } else {
        REG_CLR_BIT(LP_APM0_REGION_FILTER_EN_REG, BIT(regn_num));
    }
}

/**
 * @brief Set region start address in LP-APM0
 *
 * @param regn_num Region number
 * @param addr Start address
 */
static inline void apm_ll_lp_apm0_set_region_start_addr(uint32_t regn_num, uint32_t addr)
{
    REG_WRITE(LP_APM0_REGION0_ADDR_START_REG + APM_REGION_ADDR_OFFSET * regn_num, addr);
}

/**
 * @brief Set region end address in LP-APM0
 *
 * @param regn_num Region number
 * @param addr End address
 */
static inline void apm_ll_lp_apm0_set_region_end_addr(uint32_t regn_num, uint32_t addr)
{
    REG_WRITE(LP_APM0_REGION0_ADDR_END_REG + APM_REGION_ADDR_OFFSET * regn_num, addr);
}

/**
 * @brief Set security mode region attributes in LP-APM0
 *
 * @param regn_num Region number
 * @param mode Security mode
 * @param regn_pms Region PMS attributes
 */
static inline void apm_ll_lp_apm0_set_sec_mode_region_attr(uint32_t regn_num, apm_security_mode_t mode, uint32_t regn_pms)
{
    uint32_t reg = LP_APM0_REGION0_PMS_ATTR_REG + APM_REGION_PMS_ATTR_OFFSET * regn_num;
    uint32_t val = REG_READ(reg);
    val &= ~APM_REGION_PMS_MASK(mode);
    val |= APM_REGION_PMS_FIELD(mode, regn_pms);
    REG_WRITE(reg, val);
}

/**
 * @brief Get exception data (regn, master, security mode) from LP-APM0
 *
 * @param path Access path
 * @return Exception data
 */
static inline uint32_t apm_ll_lp_apm0_get_excp_data(apm_ctrl_access_path_t path)
{
    (void)path;
    return REG_READ(LP_APM0_M0_EXCEPTION_INFO0_REG);
}

/**
 * @brief Get exception status from LP-APM0
 *
 * @param path Access path
 * @return Exception type
 */
static inline uint32_t apm_ll_lp_apm0_get_excp_type(apm_ctrl_access_path_t path)
{
    (void)path;
    return REG_READ(LP_APM0_M0_STATUS_REG);
}

/**
 * @brief Get exception address from LP-APM0
 *
 * @param path Access path
 * @return Exception address
 */
static inline uint32_t apm_ll_lp_apm0_get_excp_addr(apm_ctrl_access_path_t path)
{
    (void)path;
    return REG_READ(LP_APM0_M0_EXCEPTION_INFO1_REG);
}

/**
 * @brief Get exception information from LP-APM0
 *
 * @param path Access path
 * @param info Pointer to store exception information
 */
static inline void apm_ll_lp_apm0_get_excp_info(apm_ctrl_access_path_t path, apm_ctrl_exception_info_t *info)
{
    lp_apm0_m0_exception_info0_reg_t reg;
    reg.val = apm_ll_lp_apm0_get_excp_data(path);
    info->regn = reg.m0_exception_region;
    info->mode = reg.m0_exception_mode;
    info->id   = reg.m0_exception_id;

    info->type = apm_ll_lp_apm0_get_excp_type(path);
    info->addr = apm_ll_lp_apm0_get_excp_addr(path);
}

/**
 * @brief Clear controller exception status in LP-APM0
 *
 * @param path Access path
 */
static inline void apm_ll_lp_apm0_clear_ctrl_excp_status(apm_ctrl_access_path_t path)
{
    (void)path;
    REG_SET_BIT(LP_APM0_M0_STATUS_CLR_REG, APM_EXCP_STATUS_CLR_BIT);
}

/**
 * @brief Enable/disable controller interrupt in LP-APM0
 *
 * @param path Access path
 * @param enable True to enable, false to disable
 */
static inline void apm_ll_lp_apm0_enable_ctrl_intr(apm_ctrl_access_path_t path, bool enable)
{
    if (enable) {
        REG_SET_BIT(LP_APM0_INT_EN_REG, BIT(path));
    } else {
        REG_CLR_BIT(LP_APM0_INT_EN_REG, BIT(path));
    }
}

/**
 * @brief Get controller interrupt source number from LP-APM0
 *
 * @param path Access path
 * @return Interrupt source number
 */
static inline int apm_ll_lp_apm0_get_ctrl_intr_src(apm_ctrl_access_path_t path)
{
    (void)path;
    return ETS_LP_APM0_INTR_SOURCE;
}

/**
 * @brief Enable/disable controller clock gating in LP-APM0
 *
 * @param enable True to enable, false to disable
 */
static inline void apm_ll_lp_apm0_enable_ctrl_clk_gating(bool enable)
{
    if (enable) {
        REG_CLR_BIT(LP_APM0_CLOCK_GATE_REG, LP_APM0_CLK_EN);
    } else {
        REG_SET_BIT(LP_APM0_CLOCK_GATE_REG, LP_APM0_CLK_EN);
    }

}

/**
 * @brief Enable/disable controller filter for specific path in HP-APM
 *
 * @param path Access path
 * @param enable True to enable, false to disable
 */
static inline void apm_ll_hp_apm_enable_ctrl_filter(apm_ctrl_access_path_t path, bool enable)
{
    if (enable) {
        REG_SET_BIT(HP_APM_FUNC_CTRL_REG, BIT(path));
    } else {
        REG_CLR_BIT(HP_APM_FUNC_CTRL_REG, BIT(path));
    }
}

/**
 * @brief Enable/disable controller filter for specific path in LP-APM
 *
 * @param path Access path
 * @param enable True to enable, false to disable
 */
static inline void apm_ll_lp_apm_enable_ctrl_filter(apm_ctrl_access_path_t path, bool enable)
{
    if (enable) {
        REG_SET_BIT(LP_APM_FUNC_CTRL_REG, BIT(path));
    } else {
        REG_CLR_BIT(LP_APM_FUNC_CTRL_REG, BIT(path));
    }
}

/**
 * @brief Enable/disable all controller filters in LP-APM
 *
 * @param enable True to enable, false to disable
 */
static inline void apm_ll_lp_apm_enable_ctrl_filter_all(bool enable)
{
    REG_WRITE(LP_APM_FUNC_CTRL_REG, enable ? UINT32_MAX : 0);
}

/**
 * @brief Enable/disable region filter in LP-APM
 *
 * @param regn_num Region number
 * @param enable True to enable, false to disable
 */
static inline void apm_ll_lp_apm_enable_region_filter(uint32_t regn_num, bool enable)
{
    if (enable) {
        REG_SET_BIT(LP_APM_REGION_FILTER_EN_REG, BIT(regn_num));
    } else {
        REG_CLR_BIT(LP_APM_REGION_FILTER_EN_REG, BIT(regn_num));
    }
}

/**
 * @brief Set region start address in LP-APM
 *
 * @param regn_num Region number
 * @param addr Start address
 */
static inline void apm_ll_lp_apm_set_region_start_addr(uint32_t regn_num, uint32_t addr)
{
    REG_WRITE(LP_APM_REGION0_ADDR_START_REG + APM_REGION_ADDR_OFFSET * regn_num, addr);
}

/**
 * @brief Set region end address in LP-APM
 *
 * @param regn_num Region number
 * @param addr End address
 */
static inline void apm_ll_lp_apm_set_region_end_addr(uint32_t regn_num, uint32_t addr)
{
    REG_WRITE(LP_APM_REGION0_ADDR_END_REG + APM_REGION_ADDR_OFFSET * regn_num, addr);
}

/**
 * @brief Set security mode region attributes in LP-APM
 *
 * @param regn_num Region number
 * @param mode Security mode
 * @param regn_pms Region PMS attributes
 */
static inline void apm_ll_lp_apm_set_sec_mode_region_attr(uint32_t regn_num, apm_security_mode_t mode, uint32_t regn_pms)
{
    uint32_t reg = LP_APM_REGION0_PMS_ATTR_REG + APM_REGION_PMS_ATTR_OFFSET * regn_num;
    uint32_t val = REG_READ(reg);
    val &= ~APM_REGION_PMS_MASK(mode);
    val |= APM_REGION_PMS_FIELD(mode, regn_pms);
    REG_WRITE(reg, val);
}

/**
 * @brief Get exception data (regn, master, security mode) from LP-APM
 *
 * @param path Access path
 * @return Exception data
 */
static inline uint32_t apm_ll_lp_apm_get_excp_data(apm_ctrl_access_path_t path)
{
    return REG_READ(LP_APM_M0_EXCEPTION_INFO0_REG + APM_EXCP_INFO_OFFSET * path);
}

/**
 * @brief Get exception status from LP-APM
 *
 * @param path Access path
 * @return Exception type
 */
static inline uint32_t apm_ll_lp_apm_get_excp_type(apm_ctrl_access_path_t path)
{
    return REG_READ(LP_APM_M0_STATUS_REG + APM_EXCP_INFO_OFFSET * path);
}

/**
 * @brief Get exception address from LP-APM
 *
 * @param path Access path
 * @return Exception address
 */
static inline uint32_t apm_ll_lp_apm_get_excp_addr(apm_ctrl_access_path_t path)
{
    return REG_READ(LP_APM_M0_EXCEPTION_INFO1_REG + APM_EXCP_INFO_OFFSET * path);
}

/**
 * @brief Get exception information from LP-APM
 *
 * @param path Access path
 * @param info Pointer to store exception information
 */
static inline void apm_ll_lp_apm_get_excp_info(apm_ctrl_access_path_t path, apm_ctrl_exception_info_t *info)
{
    lp_apm_m0_exception_info0_reg_t reg;
    reg.val = apm_ll_lp_apm_get_excp_data(path);
    info->regn = reg.m0_exception_region;
    info->mode = reg.m0_exception_mode;
    info->id   = reg.m0_exception_id;

    info->type = apm_ll_lp_apm_get_excp_type(path);
    info->addr = apm_ll_lp_apm_get_excp_addr(path);
}

/**
 * @brief Clear controller exception status in LP-APM
 *
 * @param path Access path
 */
static inline void apm_ll_lp_apm_clear_ctrl_excp_status(apm_ctrl_access_path_t path)
{
    REG_SET_BIT(LP_APM_M0_STATUS_CLR_REG + APM_EXCP_INFO_OFFSET * path, APM_EXCP_STATUS_CLR_BIT);
}

/**
 * @brief Enable/disable controller interrupt in LP-APM
 *
 * @param path Access path
 * @param enable True to enable, false to disable
 */
static inline void apm_ll_lp_apm_enable_ctrl_intr(apm_ctrl_access_path_t path, bool enable)
{
    if (enable) {
        REG_SET_BIT(LP_APM_INT_EN_REG, BIT(path));
    } else {
        REG_CLR_BIT(LP_APM_INT_EN_REG, BIT(path));
    }
}

/**
 * @brief Enable/disable controller clock gating in LP-APM
 *
 * @param enable True to enable, false to disable
 */
static inline void apm_ll_lp_apm_enable_ctrl_clk_gating(bool enable)
{
    if (enable) {
        REG_CLR_BIT(LP_APM_CLOCK_GATE_REG, LP_APM_CLK_EN);
    } else {
        REG_SET_BIT(LP_APM_CLOCK_GATE_REG, LP_APM_CLK_EN);
    }
}

/**
 * @brief Get controller interrupt source number from LP-APM
 *
 * @param path Access path
 * @return Interrupt source number
 */
static inline int apm_ll_lp_apm_get_ctrl_intr_src(apm_ctrl_access_path_t path)
{
    return ETS_LP_APM_M0_INTR_SOURCE + path;
}

/**
 * @brief Enable/disable APM reset event bypass
 *
 * @param enable True to enable, false to disable
 */
static inline void apm_ll_enable_reset_event_bypass(bool enable)
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
