/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include "soc/soc_caps.h"
#if SOC_APM_SUPPORTED
#include "hal/apm_ll.h"

/**
 * @brief Region configuration data.
 */
typedef struct {
    uint32_t regn_num;             /* Address Region number cover by this configuration data. */
    uint32_t regn_start_addr;      /* Address Region start address. */
    uint32_t regn_end_addr;        /* Address Region end address. */
    uint32_t regn_pms;             /* Access Permission for Master in different secure mode. */
    bool     filter_enable;        /* Address Region Filter enable/disable. */
    apm_ll_apm_ctrl_t apm_ctrl;    /* APM Ctrl: LP APM0/HP APM/LP APM. */
    apm_ll_secure_mode_t sec_mode; /* Master secure mode: TEE/REE[0-2].*/
} apm_ctrl_region_config_data_t;

/**
 * @brief Secure mode(TEE/REE[0:2] configuration data.
 */
typedef struct {
    apm_ll_apm_ctrl_t apm_ctrl;              /* APM Ctrl: LP APM0/HP APM/LP APM. */
    apm_ll_secure_mode_t sec_mode;           /* Secure mode to be configured TEE/REE[0:2]. */
    uint8_t  apm_m_cnt;                      /* Access path M count. */
    uint32_t regn_count;                     /* Access Ctrl region count. */
    uint32_t master_ids;                     /* Bit mask for masters to be part of this secure mode. */
    apm_ctrl_region_config_data_t *pms_data; /* Region configuration data. */
} apm_ctrl_secure_mode_config_t;

/**
 * @brief Set secure mode
 *
 * @param apm_ctrl APM Ctrl to be configured
 * @param master_id APM master ID
 * @param sec_mode Secure mode
 */
void apm_tee_hal_set_master_secure_mode(apm_ll_apm_ctrl_t apm_ctrl, apm_ll_master_id_t master_id,
                                        apm_ll_secure_mode_t sec_mode);

/**
 * @brief TEE controller clock auto gating enable
 *
 * @param enable Flag for HP clock auto gating enable/disable
 */
void apm_tee_hal_clk_gating_enable(bool enable);

/**
 * @brief enable/disable APM Ctrl Region access permission filter
 *
 * @param apm_ctrl APM Ctrl to be configured
 * @param regn_num Memory Region number
 * @param enable Flag for Region access filter enable/disable
 */
void apm_hal_apm_ctrl_region_filter_enable(apm_ll_apm_ctrl_t apm_ctrl, uint32_t regn_num, bool enable);

/**
 * @brief enable/disable APM Ctrl access path(M[0:n])
 *
 * @param apm_path   APM controller and access path to be configured
 * @param enable     Flag for M path filter enable/disable
 */
void apm_hal_apm_ctrl_filter_enable(apm_ctrl_path_t *apm_path, bool enable);

/**
 * @brief enable/disable all available APM Ctrl access path(M[0:n])
 *
 * @param enable Flag for M path filter enable/disable
 */
void apm_hal_apm_ctrl_filter_enable_all(bool enable);

/**
 * @brief Region configuration
 *
 * @param pms_data Region configuration data
 */
void apm_hal_apm_ctrl_region_config(const apm_ctrl_region_config_data_t *pms_data);

/**
 * @brief Get APM Ctrl access path(M[0:n]) exception status
 *
 * @param apm_path   APM controller and access path to be configured
 */
uint8_t apm_hal_apm_ctrl_exception_status(apm_ctrl_path_t *apm_path);

/**
 * @brief Clear APM Ctrl access path(M[0:n]) exception
 *
 * @param apm_path   APM controller and access path to be configured
 */
void apm_hal_apm_ctrl_exception_clear(apm_ctrl_path_t *apm_path);

/**
 * @brief Get APM Ctrl access path exception information
 *
 * @param excp_info Exception related information like addr,
 * region, amp_ctrl, apm_m_path, sec_mode and master id
 */
void apm_hal_apm_ctrl_get_exception_info(apm_ctrl_exception_info_t *excp_info);

/**
 * @brief APM Ctrl interrupt enable for access path(M[0:n])
 *
 * @param apm_path   APM controller and access path to be configured
 * @param enable     Flag for access path interrupt enable/disable
 */
void apm_hal_apm_ctrl_interrupt_enable(apm_ctrl_path_t *apm_path, bool enable);

/**
 * @brief APM Ctrl clock auto gating enable
 *
 * @apm_ctrl     APM Ctrl
 * @param enable Flag for HP clock auto gating enable/disable
 */
void apm_hal_apm_ctrl_clk_gating_enable(apm_ll_apm_ctrl_t apm_ctrl, bool enable);

/**
 * @brief TEE/REE execution environment configuration.
 *
 * This API will be called from TEE mode initialization code which is
 * responsible to setup TEE/REE execution environment.
 * It includes, allocation of all bus masters, memory ranges and other
 * peripherals to the given secure mode.
 * All this information should be passed by the TEE mode initialization code.
 *
 * @sec_mode_data APM Ctl configuration data.
 */
void apm_hal_apm_ctrl_master_sec_mode_config(apm_ctrl_secure_mode_config_t *sec_mode_data);

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
void apm_hal_apm_ctrl_reset_event_enable(bool enable);

/**
 * @brief Returns APM Ctrl access path interrupt source number.
 *
 * @param apm_path   APM controller and access path to be configured
 */
esp_err_t apm_hal_apm_ctrl_get_int_src_num(apm_ctrl_path_t *apm_path);

#endif

#ifdef __cplusplus
}
#endif
