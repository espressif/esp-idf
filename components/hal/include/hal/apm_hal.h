/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
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

#if CONFIG_IDF_TARGET_ESP32P4

/**
 * @brief DMA configurable region configuration data.
 */
typedef struct {
    apm_ll_dma_master_t dma_master; /* DMA master whose access permission to be configured.*/
    uint32_t            pms_r_mask; /* Read permission mask. */
    uint32_t            pms_w_mask; /* Write permission mask. */
} apm_hal_dma_region_config_data_t;

/**
 * @brief Configure HP peripherals access permission for the HP CPU0/1.
 *
 * @param master_id  HP CPU0/1
 * @param hp_peri    HP peripheral whose access permission to be configured.
 * @param enable     Permission enable/disable
 */
void apm_hal_hp_peri_access_enable(apm_ll_master_id_t master_id, apm_ll_hp_peri_t hp_peri,
                                       apm_ll_secure_mode_t sec_mode, bool enable);

/**
 * @brief Configure LP peripherals access permission for the LP CPU.
 *
 * @param lp_peri    LP peripheral whose access permission to be configured.
 * @param enable     Permission enable/disable
 */
void apm_hal_lp_peri_access_enable(apm_ll_lp_peri_t lp_peri, bool enable);

/**
 * @brief Configure peripherals configurable address ranges.
 *
 * @param regn_num       Configurable address range number.
 * @param regn_low_addr  Configurable address range start address.
 * @param regn_high_addr Configurable address range end address.
 */
void apm_hal_peri_region_config(uint32_t regn_num, uint32_t regn_low_addr,
                                                   uint32_t regn_high_addr);

/**
 * @brief Configure peripherals configurable address ranges.
 *
 * @param master_id  LP CPU and HP CPU0/1
 * @param sec_mode   CPU privilege mode (Machine/User) which corresponds to (TEE/REE).
 * @param regn_num   Configurable address range number.
 * @param regn_pms   Configurable address range permission setting(2-bits field).
 *                   Bit 0: Region 0 permission enable/disable.
 *                   Bit 1: Region 1 permission enable/disable.
 * @return           Configuration performed successfully?
 */
int apm_hal_peri_region_pms(apm_ll_master_id_t master_id, apm_ll_secure_mode_t sec_mode,
                                           uint32_t regn_num, uint32_t regn_pms);

/**
 * @brief Configure APM controller clock gating.
 *
 * @param apm_ctrl APM controller (LP_PERI/HP_PERI/HP_DMA/LP2HP_PERI/HP2LP_PERI).
 * @param enable   Permission enable/disable.
 *                 enable: Enable automatic clock gating.
 *                 disable: Keep the clock always on.
 * @return         Clock gating set successfully?
 */
int apm_hal_apm_ctrl_clk_gating_enable(apm_ll_apm_ctrl_t apm_ctrl, bool enable);

/**
 * @brief Configure DMA configurable address range low address.
 *
 * @param regn_num       Configurable DMA address range number.
 * @param regn_low_addr  Configurable DMA address range start address.
 * @param regn_high_addr Configurable DMA address range end address.
 */
void apm_hal_dma_region_config(uint32_t regn_num, uint32_t regn_low_addr, uint32_t regn_high_addr);

/**
 * @brief Configure DMA configurable address range read permission.
 *
 * @param pms_data DMA configurable region configuration data.
 * @param dma_master DMA master whose access permission to be configured.
 * @param regn_mask  32-bits field, each bit for corresponding DMA configurable address range permission.
 *                   0: Disable read permission.
 *                   1: Enable read permission.
 */
void apm_hal_dma_region_pms(apm_hal_dma_region_config_data_t *pms_data);


#else

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
 * @brief Set all masters to a given secure mode
 *
 * @param sec_mode Secure mode
 */
void apm_tee_hal_set_master_secure_mode_all(apm_ll_secure_mode_t sec_mode);

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
 * @brief Fetch the APM Ctrl access path interrupt source number.
 *
 * @param apm_path   APM controller and access path to be configured
 *
 * @return
 *      - valid interrupt source number on success
 *      - -1: invalid interrupt source
 */
int apm_hal_apm_ctrl_get_int_src_num(apm_ctrl_path_t *apm_path);

#endif //CONFIG_IDF_TARGET_ESP32P4

#elif SOC_APM_CTRL_FILTER_SUPPORTED //!SOC_APM_SUPPORTED

#if CONFIG_IDF_TARGET_ESP32H4
#include "soc/hp_apm_reg.h"
#define apm_hal_apm_ctrl_filter_enable_all(en) \
    REG_WRITE(HP_APM_FUNC_CTRL_REG, en ? 0xFFFFFFFF : 0);
#else
#include "soc/hp_apm_reg.h"
#include "soc/lp_apm_reg.h"
#include "soc/lp_apm0_reg.h"

#define apm_hal_apm_ctrl_filter_enable_all(en) \
    REG_WRITE(LP_APM_FUNC_CTRL_REG, en ? 0xFFFFFFFF : 0); \
    REG_WRITE(LP_APM0_FUNC_CTRL_REG, en ? 0xFFFFFFFF : 0); \
    REG_WRITE(HP_APM_FUNC_CTRL_REG, en ? 0xFFFFFFFF : 0);
#endif

#endif //SOC_APM_CTRL_FILTER_SUPPORTED

#ifdef __cplusplus
}
#endif
