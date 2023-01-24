/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "hal/apm_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Region configuration data.
 */
typedef struct {
    apm_ll_secure_mode_t sec_mode;
    uint32_t regn_num;
    uint32_t regn_start_addr;
    uint32_t regn_end_addr;
    uint32_t regn_pms;
} apm_hp_hal_region_config_data_t;

/**
 * @brief Secure mode(TEE/REE[0:2] configuration data.
 */
typedef struct {
    apm_ll_secure_mode_t sec_mode;             /* Secure mode to be configured TEE/REE[0:2]. */
    uint32_t master_ids;                       /* Bit mask for masters to be part of this secure mode. */
    apm_hp_hal_region_config_data_t *pms_data; /* Region configuration data. */
} apm_hp_secure_mode_config_t;

/**
 * @brief Set secure mode
 *
 * @param master_id APM master ID
 * @param sec_mode Secure mode
 */
void apm_tee_hal_set_master_secure_mode(apm_ll_master_id_t master_id, apm_ll_secure_mode_t sec_mode);

/**
 * @brief TEE controller clock auto gating enable
 *
 * @param enable Flag for HP clock auto gating enable/disable
 */
void apm_tee_hal_clk_gating_enable(bool enable);

/**
 * @brief enable/disable HP Region access permission filter
 *
 * @param regn_num Memory Region number
 * @param enable Flag for Region access filter enable/disable
 */
void apm_hp_hal_region_filter_enable(uint32_t regn_num, bool enable);

/**
 * @brief enable/disable HP access path(M[0:3])
 *
 * @param hp_m_path HP access path
 * @param enable    Flag for HP M path filter enable/disable
 */
void apm_hp_hal_m_filter_enable(apm_ll_hp_access_path_t hp_m_path, bool enable);

/**
 * @brief Region configuration
 *
 * @param pms_data Region configuration data
 */
void apm_hp_hal_region_config(const apm_hp_hal_region_config_data_t *pms_data);

/**
 * @brief Get HP access path(M[0:3]) exception status
 *
 * @param hp_m_path HP access path
 */
uint8_t apm_hp_hal_m_exception_status(apm_ll_hp_access_path_t hp_m_path);

/**
 * @brief Clear HP access path(M[0:3]) exception
 *
 * @param hp_m_path HP access path
 */
void apm_hp_hal_m_exception_clear(apm_ll_hp_access_path_t hp_m_path);

/**
 * @brief Get HP access path(M[0:3]) exception information
 *
 * @param excp_info Exception related information like addr,
 * region, sec_mode and master id
 */
void apm_hp_hal_get_m_exception_info(apm_hp_m_exception_info_t *excp_info);

/**
 * @brief Interrupt enable for access path(M[0:3])
 *
 * @param hp_m_path HP access path
 * @param enable    Flag for access path interrupt enable/disable
 */
void apm_hp_hal_m_interrupt_enable(apm_ll_hp_access_path_t hp_m_path, bool enable);

/**
 * @brief HP clock auto gating enable
 *
 * @param enable   Flag for HP clock auto gating enable/disable
 */
void apm_hp_hal_clk_gating_enable(bool enable);

/**
 * @brief TEE/REE execution environment configuration.
 *
 * This API will be called from TEE mode initialization code which is
 * responsible to setup TEE/REE execution environment.
 * It includes, allocation of all bus masters, memory ranges and other
 * peripherals to the given secure mode.
 * All this information should be passed by the TEE mode initialization code.
 */
void apm_hp_hal_master_sec_mode_config(apm_hp_secure_mode_config_t *sec_mode_data);

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
void apm_hp_hal_reset_event_enable(bool enable);

#ifdef __cplusplus
}
#endif
