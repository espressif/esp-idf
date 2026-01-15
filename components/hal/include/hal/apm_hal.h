/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "esp_err.h"
#include "soc/soc_caps.h"
#if SOC_APM_SUPPORTED
#include "hal/apm_ll.h"
#include "hal/apm_types.h"

#if SOC_IS(ESP32P4)

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
 * @brief Helper macro to create a region entry configuration
 *
 */
#define APM_HAL_REGION_ENTRY_BASE(PATH,NUM, START, END, PMS) \
    .path_id         = (PATH),                          \
    .regn_num        = (NUM),                           \
    .regn_start_addr = ((START) & ~0x03U),              \
    .regn_end_addr   = (((END) - 1U) & ~0x03U),         \
    .regn_pms        = (PMS),                           \
    .filter_en       = true


#if SOC_APM_SUPPORT_CTRL_CFG_LOCK
#define APM_HAL_REGION_ENTRY(PATH, NUM, START, END, PMS, LOCK) \
    { APM_HAL_REGION_ENTRY_BASE(PATH, NUM, START, END, PMS), .lock_en = LOCK }
#else
#define APM_HAL_REGION_ENTRY(PATH, NUM, START, END, PMS) \
    { APM_HAL_REGION_ENTRY_BASE(PATH, NUM, START, END, PMS) }
#endif

/**
 * @brief Helper macro to create a security mode configuration
 *
 */
#define APM_HAL_SEC_MODE_CFG(CTRL_MOD, MODE, REGNS)        \
    {                                                      \
        .ctrl_mod   = (CTRL_MOD),                          \
        .mode       = (MODE),                              \
        .regn_count = sizeof(REGNS) / sizeof((REGNS)[0]),  \
        .regions    = (REGNS),                             \
    }

/**
 * @brief APM controller info structure
 */
typedef struct {
    apm_ctrl_module_t ctrl_mod;         /*!< APM controller module */
    apm_ctrl_access_path_t path;        /*!< Access path */
} apm_hal_ctrl_info_t;

/**
 * @brief APM region configuration structure
 */
typedef struct {
    apm_ctrl_access_path_t path_id;     /*!< Path identifier */
    uint32_t regn_num;                  /*!< Region number */
    uint32_t regn_start_addr;           /*!< Region start address */
    uint32_t regn_end_addr;             /*!< Region end address */
    uint32_t regn_pms;                  /*!< Region permissions */
    bool filter_en;                     /*!< Filter enable flag */
#if SOC_APM_SUPPORT_CTRL_CFG_LOCK
    bool lock_en;                       /*!< Lock enable flag */
#endif
} apm_hal_ctrl_region_cfg_t;

/**
 * @brief APM security mode configuration structure
 */
typedef struct {
    apm_ctrl_module_t ctrl_mod;           /*!< APM controller module */
    apm_security_mode_t mode;             /*!< Security mode */
    uint32_t regn_count;                  /*!< Number of regions */
    apm_hal_ctrl_region_cfg_t *regions;   /*!< Array of region configurations */
} apm_hal_ctrl_sec_mode_cfg_t;

/**
 * @brief Set security mode for specified masters
 *
 * @param master_mask Mask of masters to configure
 * @param mode Security mode to set
 */
void apm_hal_set_master_sec_mode(uint32_t master_mask, apm_security_mode_t mode);

/**
 * @brief Set security mode for all masters
 *
 * @param mode Security mode to set
 */
void apm_hal_set_master_sec_mode_all(apm_security_mode_t mode);

#if SOC_APM_SUPPORT_CTRL_CFG_LOCK
/**
 * @brief Lock security mode for specified masters
 *
 * @param master_mask Mask of masters to configure
 */
void apm_hal_lock_master_sec_mode(uint32_t master_mask);

/**
 * @brief Lock security mode for all masters
 *
 */
void apm_hal_lock_master_sec_mode_all(void);
#endif

#if SOC_APM_SUPPORT_TEE_PERI_ACCESS_CTRL
/**
 * @brief Set access permissions for the specified peripherals in the given TEE ctrl module
 *
 * @param ctrl_mod TEE ctrl module
 * @param periph_mask Peripheral mask
 * @param mode Security mode
 * @param pms Access permissions
 */
void apm_hal_tee_set_peri_access(apm_tee_ctrl_module_t ctrl_mod, uint64_t periph_mask, apm_security_mode_t mode, apm_perm_t pms);

/**
 * @brief Set access permissions for all peripherals in the given TEE ctrl module
 *
 * @param ctrl_mod TEE ctrl module
 * @param mode Security mode
 * @param pms Access permissions
 */
void apm_hal_tee_set_peri_access_all(apm_tee_ctrl_module_t ctrl_mod, apm_security_mode_t mode, apm_perm_t pms);
#endif

/**
 * @brief Enable/disable TEE clock gating for a APM controller module
 *
 * @param ctrl_mod TEE controller module
 * @param enable True to enable, false to disable
 */
void apm_hal_tee_enable_clk_gating(apm_tee_ctrl_module_t ctrl_mod, bool enable);

/**
 * @brief Enable/disable controller filter for specific path
 *
 * @param ctrl_mod APM controller module
 * @param path Access path
 * @param enable True to enable, false to disable
 */
void apm_hal_enable_ctrl_filter(apm_ctrl_module_t ctrl_mod, apm_ctrl_access_path_t path, bool enable);

/**
 * @brief Enable/disable all controller filters
 *
 * @param enable True to enable, false to disable
 */
void apm_hal_enable_ctrl_filter_all(bool enable);

/**
 * @brief Enable/disable region filter
 *
 * @param ctrl_mod APM controller module
 * @param regn_num Region number
 * @param enable True to enable, false to disable
 */
void apm_hal_enable_region_filter(apm_ctrl_module_t ctrl_mod, uint32_t regn_num, bool enable);

/**
 * @brief Set the start address for the given region
 *
 * @param ctrl_mod APM controller module
 * @param regn_num Region number
 * @param addr Address
 */
void apm_hal_set_region_start_addr(apm_ctrl_module_t ctrl_mod, uint32_t regn_num, uint32_t addr);

/**
 * @brief Set the end address for the given region
 *
 * @param ctrl_mod APM controller module
 * @param regn_num Region number
 * @param addr Address
 */
void apm_hal_set_region_end_addr(apm_ctrl_module_t ctrl_mod, uint32_t regn_num, uint32_t addr);

/**
 * @brief Set the permissions for the specified security mode for the given region
 *
 * @param ctrl_mod APM controller module
 * @param regn_num Region number
 * @param mode Security mode
 */
void apm_hal_set_sec_mode_region_attr(apm_ctrl_module_t ctrl_mod, uint32_t regn_num, apm_security_mode_t mode, uint32_t regn_pms);

/**
 * @brief Set region filter configuration
 *
 * @param ctrl_mod APM controller module
 * @param mode Security mode
 * @param regn_cfg Region configuration
 */
void apm_hal_set_region_filter_cfg(apm_ctrl_module_t ctrl_mod, apm_security_mode_t mode, const apm_hal_ctrl_region_cfg_t *regn_cfg);

#if SOC_APM_SUPPORT_CTRL_CFG_LOCK
/**
 * @brief Lock region filter configuration
 *
 * @param ctrl_mod APM controller module
 * @param regn_num Region number
 */
void apm_hal_lock_region_filter_cfg(apm_ctrl_module_t ctrl_mod, uint32_t regn_num);
#endif

/**
 * @brief Set controller security mode configuration
 *
 * @param cfg Security mode configuration
 */
void apm_hal_set_ctrl_sec_mode_cfg(const apm_hal_ctrl_sec_mode_cfg_t *cfg);

/**
 * @brief Get exception type
 *
 * @param ctrl_info Controller information
 * @return Exception type
 */
uint32_t apm_hal_get_exception_type(apm_hal_ctrl_info_t *ctrl_info);

/**
 * @brief Clear exception status
 *
 * @param ctrl_info Controller information
 */
void apm_hal_clear_exception_status(apm_hal_ctrl_info_t *ctrl_info);

/**
 * @brief Get exception information
 *
 * @param ctrl_info Controller information
 * @param excp_info Exception information structure to fill
 */
void apm_hal_get_exception_info(apm_hal_ctrl_info_t *ctrl_info, apm_ctrl_exception_info_t *excp_info);

/**
 * @brief Enable/disable interrupt
 *
 * @param ctrl_info Controller information
 * @param enable True to enable, false to disable
 */
void apm_hal_enable_intr(apm_hal_ctrl_info_t *ctrl_info, bool enable);

/**
 * @brief Get interrupt source number
 *
 * @param ctrl_info Controller information
 * @return Interrupt source number
 */
int apm_hal_get_intr_src_num(apm_hal_ctrl_info_t *ctrl_info);

/**
 * @brief Enable/disable reset event bypass
 *
 * @param enable True to enable, false to disable
 */
void apm_hal_enable_reset_event_bypass(bool enable);

/**
 * @brief Enable/disable controller clock gating
 *
 * @param ctrl_mod APM controller module
 * @param enable True to enable, false to disable
 */
void apm_hal_enable_ctrl_clk_gating(apm_ctrl_module_t ctrl_mod, bool enable);

#endif //SOC_IS(ESP32P4)

#elif SOC_APM_CTRL_FILTER_SUPPORTED //!SOCKconfig_APM_SUPPORTED

#if SOC_IS(ESP32H4)
#include "soc/hp_apm_reg.h"
#define apm_hal_enable_ctrl_filter_all(en) \
    REG_WRITE(HP_APM_FUNC_CTRL_REG, en ? 0xFFFFFFFF : 0);
#elif SOC_IS(ESP32S31)
#include "soc/hp_apm_reg.h"
#include "soc/hp_mem_apm_reg.h"
#define apm_hal_enable_ctrl_filter_all(en) \
    REG_WRITE(HP_APM_FUNC_CTRL_REG, en ? 0xFFFFFFFF : 0); \
    REG_WRITE(HP_MEM_APM_FUNC_CTRL_REG, en ? 0xFFFFFFFF : 0);
#else
#include "soc/hp_apm_reg.h"
#include "soc/lp_apm_reg.h"
#include "soc/lp_apm0_reg.h"

#define apm_hal_enable_ctrl_filter_all(en) \
    REG_WRITE(LP_APM_FUNC_CTRL_REG, en ? 0xFFFFFFFF : 0); \
    REG_WRITE(LP_APM0_FUNC_CTRL_REG, en ? 0xFFFFFFFF : 0); \
    REG_WRITE(HP_APM_FUNC_CTRL_REG, en ? 0xFFFFFFFF : 0);
#endif

#endif //SOC_APM_CTRL_FILTER_SUPPORTED

#ifdef __cplusplus
}
#endif
