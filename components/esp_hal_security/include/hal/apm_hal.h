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

#if SOC_APM_CTRL_FILTER_SUPPORTED
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
#else
/**
 * @brief HP_PERI_PMS permission configuration structure
 */
typedef struct {
    uint32_t cpu_peri;  /*!< Bitmask for CPU_PERIPH slaves */
    uint32_t hp_peri0;  /*!< Bitmask for HP_PERIPH0 slaves */
    uint64_t hp_peri1;  /*!< Bitmask for HP_PERIPH1 slaves */
} apm_hal_pms_hp_peri_cfg_t;

/**
 * @brief PMS region configuration structure
 */
typedef struct {
    uint32_t regn_num;                      /*!< Region number */
    uint32_t regn_start_addr;               /*!< Region start address */
    uint32_t regn_end_addr;                 /*!< Region end address */
} apm_hal_pms_regn_cfg_t;

/**
 * @brief PMS master configuration structure
 */
typedef struct {
    union {
        struct {
            apm_master_dma_id_t id;         /*!< DMA master ID */
            uint32_t regn_pms_rd;           /*!< Bitmask of regions the master is permitted to read */
            uint32_t regn_pms_wr;           /*!< Bitmask of regions the master is permitted to write */
        } dma;
        struct {
            apm_master_id_t id;             /*!< Master ID */
            apm_security_mode_t mode;       /*!< Security mode */
            uint32_t regn_pms;             /*!< Bitmask of regions the master is permitted to access */
        } lp_peri;
    };
} apm_hal_pms_master_cfg_t;

/**
 * @brief Helper macro to create a PMS region entry configuration
 */
#define APM_HAL_PMS_REGION_ENTRY(NUM, START, END)  \
    {                                              \
        .regn_num        = (NUM),                  \
        .regn_start_addr = (START),                \
        .regn_end_addr   = (END),                  \
    }

/**
 * @brief Helper macro to create a DMA_PMS master entry configuration
 */
#define APM_HAL_DMA_PMS_MASTER_ENTRY(ID, R_PMS, W_PMS) \
    {                                                  \
        .dma = {                                       \
            .id          = (ID),                       \
            .regn_pms_rd = (R_PMS),                    \
            .regn_pms_wr = (W_PMS),                    \
        },                                             \
    }

/**
 * @brief Helper macro to create an LP_PERI_PMS master entry configuration
 */
#define APM_HAL_LP_PERI_PMS_MASTER_ENTRY(ID, MODE, REGN_PMS) \
    {                                                        \
        .lp_peri = {                                         \
            .id       = (ID),                                \
            .mode     = (MODE),                              \
            .regn_pms = (REGN_PMS),                          \
        },                                                   \
    }

/**
 * @brief Set DMA_PMS region address bounds
 *
 * @param regn_num Region number
 * @param start_addr Region start address
 * @param end_addr Region end address
 */
void apm_hal_dma_pms_set_region_bounds(uint32_t regn_num, uint32_t start_addr, uint32_t end_addr);

/**
 * @brief Set DMA_PMS read/write region access mask for a DMA master
 *
 * @param mid DMA master ID
 * @param regn_rd_mask Bitmask of regions the master is permitted to read
 * @param regn_wr_mask Bitmask of regions the master is permitted to write
 */
void apm_hal_dma_pms_set_master_region_attr(apm_master_dma_id_t mid, uint32_t regn_rd_mask, uint32_t regn_wr_mask);

/**
 * @brief Set DMA_PMS region configurations
 *
 * @param regn_count Number of regions to configure
 * @param regn_cfg Array of region configurations
 */
void apm_hal_dma_pms_set_region_cfg(uint32_t regn_count, const apm_hal_pms_regn_cfg_t *regn_cfg);

/**
 * @brief Set DMA_PMS master configurations
 *
 * @param master_count Number of masters to configure
 * @param cfg Array of master configurations
 */
void apm_hal_dma_pms_set_master_cfg(uint32_t master_count, const apm_hal_pms_master_cfg_t *cfg);

/**
 * @brief Set HP2LP_PERI_PMS access permissions for HP CPU
 *
 * @param core_id CPU core ID
 * @param mode Security mode
 * @param enable_mask Bitmask of LP peripherals to enable access to
 */
void apm_hal_hp2lp_peri_pms_set_hpcpu_access(int core_id, apm_security_mode_t mode, uint32_t enable_mask);

/**
 * @brief Set LP_PERI_PMS access permissions for LP CPU
 *
 * @param enable_mask Bitmask of LP peripherals to enable access to
 */
void apm_hal_lp_peri_pms_set_lpcpu_access(uint32_t enable_mask);

/**
 * @brief Set HP_PERI PMS access permissions for HP CPU
 *
 * @param core_id CPU core ID
 * @param mode Security mode
 * @param cfg HP peripheral permission configuration
 */
void apm_hal_hp_peri_pms_set_hpcpu_access(int core_id, apm_security_mode_t mode, const apm_hal_pms_hp_peri_cfg_t *cfg);

/**
 * @brief Set LP2HP_PERI PMS access permissions for LP CPU
 *
 * @param cfg HP peripheral permission configuration
 */
void apm_hal_lp2hp_peri_pms_set_lpcpu_access(const apm_hal_pms_hp_peri_cfg_t *cfg);

/**
 * @brief Set LP_PERI_PMS region address bounds
 *
 * @param regn_num Region number
 * @param start_addr Region start address
 * @param end_addr Region end address
 */
void apm_hal_lp_peri_pms_set_region_bounds(uint32_t regn_num, uint32_t start_addr, uint32_t end_addr);

/**
 * @brief Set LP_PERI_PMS region access mask for a master
 *
 * @param mid Master ID
 * @param mode Security mode
 * @param regn_pms_mask Bitmask of regions the master is permitted to access
 */
void apm_hal_lp_peri_pms_set_master_region_attr(apm_master_id_t mid, apm_security_mode_t mode, uint32_t regn_pms_mask);

/**
 * @brief Set LP_PERI_PMS region configurations
 *
 * @param regn_count Number of regions to configure
 * @param regn_cfg Array of region configurations
 */
void apm_hal_lp_peri_pms_set_region_cfg(uint32_t regn_count, const apm_hal_pms_regn_cfg_t *regn_cfg);

/**
 * @brief Set LP_PERI_PMS master configurations
 *
 * @param master_count Number of masters to configure
 * @param master_cfg Array of master configurations
 */
void apm_hal_lp_peri_pms_set_master_cfg(uint32_t master_count, const apm_hal_pms_master_cfg_t *master_cfg);

/**
 * @brief Enable/disable PMS violation interrupt for a controller module
 *
 * @param ctrl_mod APM controller module
 * @param enable True to enable, false to disable
 */
void apm_hal_enable_intr(apm_ctrl_module_t ctrl_mod, bool enable);

/**
 * @brief Clear PMS violation interrupt for a controller module
 *
 * @param ctrl_mod APM controller module
 */
void apm_hal_clear_intr(apm_ctrl_module_t ctrl_mod);

/**
 * @brief Get the combined interrupt status for all PMS controller modules
 *
 * @return Bitmask of modules with a pending interrupt
 */
uint32_t apm_hal_get_intr_status(void);

/**
 * @brief Get PMS violation exception information
 *
 * @param type Exception type identifier
 * @param excp_info Pointer to structure to fill with exception details
 */
void apm_hal_get_exception_info(apm_ctrl_exception_type type, apm_ctrl_exception_info_t *excp_info);

/**
 * @brief Get interrupt source number for a PMS controller module
 *
 * @param ctrl_mod APM controller module
 * @return Interrupt source number
 */
int apm_hal_get_intr_src_num(apm_ctrl_module_t ctrl_mod);

/**
 * @brief Enable/disable clock gating for a PMS controller module
 *
 * @param ctrl_mod APM controller module
 * @param enable True to enable, false to disable
 */
void apm_hal_enable_ctrl_clk_gating(apm_ctrl_module_t ctrl_mod, bool enable);
#endif // SOC_APM_CTRL_FILTER_SUPPORTED

#elif SOC_APM_CTRL_FILTER_SUPPORTED // SOC_APM_CTRL_FILTER_SUPPORTED

#if SOC_IS(ESP32H4)
#include "soc/hp_apm_reg.h"
#define apm_hal_enable_ctrl_filter_all(en) \
    REG_WRITE(HP_APM_FUNC_CTRL_REG, en ? 0xFFFFFFFF : 0);
#elif SOC_IS(ESP32S31)
#include "soc/hp_apm_reg.h"
#include "soc/hp_mem_apm_reg.h"
#include "soc/lp_apm_reg.h"
#define apm_hal_enable_ctrl_filter_all(en) \
    REG_WRITE(LP_APM_FUNC_CTRL_REG, en ? 0xFFFFFFFF : 0); \
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
