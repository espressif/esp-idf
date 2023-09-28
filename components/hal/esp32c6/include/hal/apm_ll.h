/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "soc/pcr_reg.h"
#include "soc/tee_reg.h"
#include "soc/lp_tee_reg.h"
#include "soc/lp_apm0_reg.h"
#include "soc/hp_apm_reg.h"
#include "soc/lp_apm_reg.h"
#include "soc/interrupts.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

#define APM_LL_CTRL_EXCEPTION_ID        0x0000001FU
#define APM_LL_CTRL_EXCEPTION_ID_S      18
#define APM_LL_CTRL_EXCEPTION_ID_V      0x0000001FU
#define APM_LL_CTRL_EXCEPTION_MODE      0x00000003U
#define APM_LL_CTRL_EXCEPTION_MODE_S    16
#define APM_LL_CTRL_EXCEPTION_MODE_V    0x00000003U
#define APM_LL_CTRL_EXCEPTION_REGION    0x0000FFFFU
#define APM_LL_CTRL_EXCEPTION_REGION_S  0
#define APM_LL_CTRL_EXCEPTION_REGION_V  0x0000FFFFU

#define APM_LL_HP_MAX_REGION_NUM 15
#define APM_LL_LP_MAX_REGION_NUM 3
#define APM_LL_MASTER_MAX        32

#define  LP_APM0_MAX_ACCESS_PATH 0x1
#define  HP_APM_MAX_ACCESS_PATH  0x4
#define  LP_APM_MAX_ACCESS_PATH  0x2

#define APM_CTRL_REGION_FILTER_EN_REG(apm_ctrl) \
    ({\
        (LP_APM0_CTRL == apm_ctrl) ? (LP_APM0_REGION_FILTER_EN_REG) :     \
        ((HP_APM_CTRL == apm_ctrl) ? (HP_APM_REGION_FILTER_EN_REG) :      \
        ((LP_APM_CTRL == apm_ctrl) ? (LP_APM_REGION_FILTER_EN_REG) : 0)); \
    })

#define TEE_LL_MODE_CTRL_REG(master_id) (TEE_M0_MODE_CTRL_REG + 4 * (master_id))

#define APM_LL_REGION_ADDR_START_REG(apm_ctrl, regn_num) \
    ({\
        (LP_APM0_CTRL == apm_ctrl) ? (LP_APM0_REGION0_ADDR_START_REG + 0xC * (regn_num)) :     \
        ((HP_APM_CTRL == apm_ctrl) ? (HP_APM_REGION0_ADDR_START_REG + 0xC * (regn_num)) :      \
        ((LP_APM_CTRL == apm_ctrl) ? (LP_APM_REGION0_ADDR_START_REG + 0xC * (regn_num)) : 0)); \
    })

#define APM_LL_REGION_ADDR_END_REG(apm_ctrl, regn_num) \
    ({\
        (LP_APM0_CTRL == apm_ctrl) ? (LP_APM0_REGION0_ADDR_END_REG + 0xC * (regn_num)) :     \
        ((HP_APM_CTRL == apm_ctrl) ? (HP_APM_REGION0_ADDR_END_REG + 0xC * (regn_num)) :      \
        ((LP_APM_CTRL == apm_ctrl) ? (LP_APM_REGION0_ADDR_END_REG + 0xC * (regn_num)) : 0)); \
    })

#define APM_LL_REGION_ADDR_ATTR_REG(apm_ctrl, regn_num) \
    ({\
        (LP_APM0_CTRL == apm_ctrl) ? (LP_APM0_REGION0_PMS_ATTR_REG + 0xC * (regn_num)) :     \
        ((HP_APM_CTRL == apm_ctrl) ? (HP_APM_REGION0_PMS_ATTR_REG + 0xC * (regn_num)) :      \
        ((LP_APM_CTRL == apm_ctrl) ? (LP_APM_REGION0_PMS_ATTR_REG + 0xC * (regn_num)) : 0)); \
    })

#define APM_LL_APM_CTRL_EXCP_STATUS_REG(apm_ctrl, apm_m_path) \
    ({\
        (LP_APM0_CTRL == apm_ctrl) ? (LP_APM0_M0_STATUS_REG + 0x10 * (apm_m_path)) :     \
        ((HP_APM_CTRL == apm_ctrl) ? (HP_APM_M0_STATUS_REG + 0x10 * (apm_m_path)) :      \
        ((LP_APM_CTRL == apm_ctrl) ? (LP_APM_M0_STATUS_REG + 0x10 * (apm_m_path)) : 0)); \
    })

#define APM_CTRL_M_REGION_STATUS_CLR    (BIT(0))
#define APM_LL_APM_CTRL_EXCP_CLR_REG(apm_ctrl, apm_m_path) \
    ({\
        (LP_APM0_CTRL == apm_ctrl) ? (LP_APM0_M0_STATUS_CLR_REG + 0x10 * (apm_m_path)) :     \
        ((HP_APM_CTRL == apm_ctrl) ? (HP_APM_M0_STATUS_CLR_REG + 0x10 * (apm_m_path)) :      \
        ((LP_APM_CTRL == apm_ctrl) ? (LP_APM_M0_STATUS_CLR_REG + 0x10 * (apm_m_path)) : 0)); \
    })

#define APM_LL_TEE_EXCP_INFO0_REG(apm_ctrl, apm_m_path) \
    ({\
        (LP_APM0_CTRL == apm_ctrl) ? (LP_APM0_M0_EXCEPTION_INFO0_REG + 0x10 * (apm_m_path)) :     \
        ((HP_APM_CTRL == apm_ctrl) ? (HP_APM_M0_EXCEPTION_INFO0_REG + 0x10 * (apm_m_path)) :      \
        ((LP_APM_CTRL == apm_ctrl) ? (LP_APM_M0_EXCEPTION_INFO0_REG + 0x10 * (apm_m_path)) : 0)); \
    })

#define APM_LL_APM_CTRL_EXCP_STATUS_REG(apm_ctrl, apm_m_path) \
    ({\
        (LP_APM0_CTRL == apm_ctrl) ? (LP_APM0_M0_STATUS_REG + 0x10 * (apm_m_path)) :     \
        ((HP_APM_CTRL == apm_ctrl) ? (HP_APM_M0_STATUS_REG + 0x10 * (apm_m_path)) :      \
        ((LP_APM_CTRL == apm_ctrl) ? (LP_APM_M0_STATUS_REG + 0x10 * (apm_m_path)) : 0)); \
    })

#define APM_LL_TEE_EXCP_INFO1_REG(apm_ctrl, apm_m_path) \
    ({\
        (LP_APM0_CTRL == apm_ctrl) ? (LP_APM0_M0_EXCEPTION_INFO1_REG + 0x10 * (apm_m_path)) :     \
        ((HP_APM_CTRL == apm_ctrl) ? (HP_APM_M0_EXCEPTION_INFO1_REG + 0x10 * (apm_m_path)) :      \
        ((LP_APM_CTRL == apm_ctrl) ? (LP_APM_M0_EXCEPTION_INFO1_REG + 0x10 * (apm_m_path)) : 0)); \
    })

#define APM_LL_SEC_MODE_REGION_ATTR(sec_mode, regn_pms) ((regn_pms) << (4 * (sec_mode - 1)))
#define APM_LL_SEC_MODE_REGION_ATTR_V  0x00000003U
#define APM_LL_SEC_MODE_REGION_ATTR_M(sec_mode) (APM_LL_SEC_MODE_REGION_ATTR_V << (4 * (sec_mode - 1)))

#define APM_LL_APM_CTRL_INT_EN_REG(apm_ctrl) \
    ({\
        (LP_APM0_CTRL == apm_ctrl) ? (LP_APM0_INT_EN_REG) :     \
        ((HP_APM_CTRL == apm_ctrl) ? (HP_APM_INT_EN_REG) :      \
        ((LP_APM_CTRL == apm_ctrl) ? (LP_APM_INT_EN_REG) : 0)); \
    })

#define APM_CTRL_CLK_EN    (BIT(0))
#define APM_LL_APM_CTRL_CLOCK_GATE_REG(apm_ctrl) \
    ({\
        (LP_APM0_CTRL == apm_ctrl) ? (LP_APM0_CLOCK_GATE_REG) :     \
        ((HP_APM_CTRL == apm_ctrl) ? (HP_APM_CLOCK_GATE_REG) :      \
        ((LP_APM_CTRL == apm_ctrl) ? (LP_APM_CLOCK_GATE_REG) : 0)); \
    })

#define APM_LL_APM_CTRL_FUNC_CTRL_REG(apm_ctrl) \
    ({\
        (LP_APM0_CTRL == apm_ctrl) ? (LP_APM0_FUNC_CTRL_REG) :     \
        ((HP_APM_CTRL == apm_ctrl) ? (HP_APM_FUNC_CTRL_REG) :      \
        ((LP_APM_CTRL == apm_ctrl) ? (LP_APM_FUNC_CTRL_REG) : 0)); \
    })

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
 * @brief APM Controller
 */
typedef enum {
    LP_APM0_CTRL = 0,
    HP_APM_CTRL  = 1,
    LP_APM_CTRL  = 2,
} apm_ll_apm_ctrl_t;

/**
 * @brief APM Secure Mode
 */
typedef enum {
    APM_LL_SECURE_MODE_TEE  = 0, /* Trusted execution environment mode */
    APM_LL_SECURE_MODE_REE0 = 1, /* Rich execution environment mode0 */
    APM_LL_SECURE_MODE_REE1 = 2, /* Rich execution environment mode1 */
    APM_LL_SECURE_MODE_REE2 = 3, /* Rich execution environment mode2 */
} apm_ll_secure_mode_t;

/**
 * @brief APM Ctrl access path
 */
typedef enum {
    APM_CTRL_ACCESS_PATH_M0 = 0x0,
    APM_CTRL_ACCESS_PATH_M1 = 0x1,
    APM_CTRL_ACCESS_PATH_M2 = 0x2,
    APM_CTRL_ACCESS_PATH_M3 = 0x3,
} apm_ll_ctrl_access_path_t;

/**
 * @brief APM Ctrl path.
 */
typedef struct {
    apm_ll_apm_ctrl_t apm_ctrl;           /* APM Ctrl: LP APM0/HP APM/LP APM. */
    apm_ll_ctrl_access_path_t apm_m_path; /* APM Ctrl access path M[0:n]. */
} apm_ctrl_path_t;

/**
 * @brief APM exception information
 */
typedef struct {
    apm_ctrl_path_t apm_path;
    uint8_t  excp_regn;
    uint8_t  excp_mode;
    uint8_t  excp_id;
    uint8_t  excp_type;
    uint32_t excp_addr;
} apm_ctrl_exception_info_t;

/**
 * @brief Set secure mode
 *
 * @param apm_ctrl APM Ctrl (LP_APM0/HP_APM/LP_APM)
 * @param master_id APM master ID
 * @param sec_mode Secure mode
 */
static inline void apm_tee_ll_set_master_secure_mode(apm_ll_apm_ctrl_t apm_ctrl, apm_ll_master_id_t master_id,
                                                     apm_ll_secure_mode_t sec_mode)
{
    if (apm_ctrl == HP_APM_CTRL) {
        REG_WRITE(TEE_LL_MODE_CTRL_REG(master_id), sec_mode);
    } else if ((apm_ctrl == LP_APM0_CTRL) || (apm_ctrl == LP_APM_CTRL)) {
        REG_WRITE(LP_TEE_M0_MODE_CTRL_REG, sec_mode);
    }
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
 * @brief enable/disable APM Ctrl Region access permission filter
 *
 * @param apm_ctrl APM Ctrl (LP_APM0/HP_APM/LP_APM)
 * @param regn_num Memory Region number
 * @param enable Flag for Region access filter enable/disable
 */
static inline void apm_ll_apm_ctrl_region_filter_enable(apm_ll_apm_ctrl_t apm_ctrl,
                                                        uint32_t regn_num, bool enable)
{
    if (enable) {
        REG_SET_BIT(APM_CTRL_REGION_FILTER_EN_REG(apm_ctrl), BIT(regn_num));
    } else {
        REG_CLR_BIT(APM_CTRL_REGION_FILTER_EN_REG(apm_ctrl), BIT(regn_num));
    }
}

/**
 * @brief enable/disable APM Ctrl access path(M[0:n])
 *
 * @param apm_ctrl   APM Ctrl (LP_APM0/HP_APM/LP_APM)
 * @param apm_m_path APM Ctrl access path
 * @param enable     Flag for LP APM0 M path filter enable/disable
 */
static inline void apm_ll_apm_ctrl_filter_enable(apm_ll_apm_ctrl_t apm_ctrl,
                                                 apm_ll_ctrl_access_path_t apm_m_path, bool enable)
{
    HAL_ASSERT(((apm_ctrl == LP_APM0_CTRL) && (apm_m_path < LP_APM0_MAX_ACCESS_PATH)) ||
               ((apm_ctrl == HP_APM_CTRL) && (apm_m_path < HP_APM_MAX_ACCESS_PATH)) ||
               ((apm_ctrl == LP_APM_CTRL) && (apm_m_path < LP_APM_MAX_ACCESS_PATH))
              );

    if (enable) {
        REG_SET_BIT(APM_LL_APM_CTRL_FUNC_CTRL_REG(apm_ctrl), BIT(apm_m_path));
    } else {
        REG_CLR_BIT(APM_LL_APM_CTRL_FUNC_CTRL_REG(apm_ctrl), BIT(apm_m_path));
    }
}

/**
 * @brief APM Ctrl Region start address configuration
 *
 * @param apm_ctrl APM Ctrl (LP_APM0/HP_APM/LP_APM)
 * @param regn_num Region number to be configured
 * @param addr     Region start address
 */
static inline void apm_ll_apm_ctrl_set_region_start_address(apm_ll_apm_ctrl_t apm_ctrl,
                                                            uint32_t regn_num, uint32_t addr)
{
    HAL_ASSERT((((apm_ctrl == LP_APM0_CTRL) || (apm_ctrl == LP_APM_CTRL)) && (regn_num <= APM_LL_LP_MAX_REGION_NUM)) ||
               ((apm_ctrl == HP_APM_CTRL) && (regn_num <= APM_LL_HP_MAX_REGION_NUM))
              );

    REG_WRITE(APM_LL_REGION_ADDR_START_REG(apm_ctrl, regn_num), addr);
}

/**
 * @brief APM Ctrl Region end address configuration
 *
 * @param apm_ctrl APM Ctrl (LP_APM0/HP_APM/LP_APM)
 * @param regn_num Region number to be configured
 * @param addr     Region end address
 */
static inline void apm_ll_apm_ctrl_set_region_end_address(apm_ll_apm_ctrl_t apm_ctrl,
                                                          uint32_t regn_num, uint32_t addr)
{
    HAL_ASSERT((((apm_ctrl == LP_APM0_CTRL) || (apm_ctrl == LP_APM_CTRL)) && (regn_num <= APM_LL_LP_MAX_REGION_NUM)) ||
               ((apm_ctrl == HP_APM_CTRL) && (regn_num <= APM_LL_HP_MAX_REGION_NUM))
              );

    REG_WRITE(APM_LL_REGION_ADDR_END_REG(apm_ctrl, regn_num), addr);
}

/**
 * @brief HP Region pms attributes configuration
 *
 * @param apm_ctrl APM Ctrl (LP_APM0/HP_APM/LP_APM)
 * @param regn_num Region number to be configured
 * @param sec_mode Secure mode of the Master
 * @param regn_pms XWR permissions for the given secure mode and Region number
 */
static inline void apm_ll_apm_ctrl_sec_mode_region_attr_config(apm_ll_apm_ctrl_t apm_ctrl,
                                                               uint32_t regn_num, apm_ll_secure_mode_t sec_mode, uint32_t regn_pms)
{
    HAL_ASSERT((((apm_ctrl == LP_APM0_CTRL) || (apm_ctrl == LP_APM_CTRL)) && (regn_num <= APM_LL_LP_MAX_REGION_NUM)) ||
               ((apm_ctrl == HP_APM_CTRL) && (regn_num <= APM_LL_HP_MAX_REGION_NUM))
              );

    uint32_t val = 0;
    val = REG_READ(APM_LL_REGION_ADDR_ATTR_REG(apm_ctrl, regn_num));
    val &= ~APM_LL_SEC_MODE_REGION_ATTR_M(sec_mode);
    val |= APM_LL_SEC_MODE_REGION_ATTR(sec_mode, regn_pms);
    REG_WRITE(APM_LL_REGION_ADDR_ATTR_REG(apm_ctrl, regn_num), val);
}

/**
 * @brief Get APM Ctrl access path(M[0:n]) exception status
 *
 * @param apm_ctrl   APM Ctrl (LP_APM0/HP_APM/LP_APM)
 * @param apm_m_path APM Ctrl access path
 */
static inline uint8_t apm_ll_apm_ctrl_exception_status(apm_ll_apm_ctrl_t apm_ctrl,
                                                       apm_ll_ctrl_access_path_t apm_m_path)
{
    HAL_ASSERT(((apm_ctrl == LP_APM0_CTRL) && (apm_m_path < LP_APM0_MAX_ACCESS_PATH)) ||
               ((apm_ctrl == HP_APM_CTRL) && (apm_m_path < HP_APM_MAX_ACCESS_PATH)) ||
               ((apm_ctrl == LP_APM_CTRL) && (apm_m_path < LP_APM_MAX_ACCESS_PATH))
              );

    return REG_READ(APM_LL_APM_CTRL_EXCP_STATUS_REG(apm_ctrl, apm_m_path));
}

/**
 * @brief Clear APM Ctrl access path(M[0:n]) exception
 *
 * @param apm_ctrl   APM Ctrl (LP_APM0/HP_APM/LP_APM)
 * @param amp_m_path APM Ctrl access path
 */
static inline void apm_ll_apm_ctrl_exception_clear(apm_ll_apm_ctrl_t apm_ctrl,
                                                   apm_ll_ctrl_access_path_t apm_m_path)
{
    HAL_ASSERT(((apm_ctrl == LP_APM0_CTRL) && (apm_m_path < LP_APM0_MAX_ACCESS_PATH)) ||
               ((apm_ctrl == HP_APM_CTRL) && (apm_m_path < HP_APM_MAX_ACCESS_PATH)) ||
               ((apm_ctrl == LP_APM_CTRL) && (apm_m_path < LP_APM_MAX_ACCESS_PATH))
              );

    REG_SET_BIT(APM_LL_APM_CTRL_EXCP_CLR_REG(apm_ctrl, apm_m_path),
                APM_CTRL_M_REGION_STATUS_CLR);
}

/**
 * @brief Get APM Ctrl access path(M[0:n]) exception information
 *
 * @param excp_info  Exception related information like addr,
 * region, apm_ctrl, apm_m_path, sec_mode and master id
 */
static inline void apm_ll_apm_ctrl_get_exception_info(apm_ctrl_exception_info_t *excp_info)
{
    HAL_ASSERT(((excp_info->apm_path.apm_ctrl == LP_APM0_CTRL) && (excp_info->apm_path.apm_m_path < LP_APM0_MAX_ACCESS_PATH)) ||
               ((excp_info->apm_path.apm_ctrl == HP_APM_CTRL) && (excp_info->apm_path.apm_m_path < HP_APM_MAX_ACCESS_PATH)) ||
               ((excp_info->apm_path.apm_ctrl == LP_APM_CTRL) && (excp_info->apm_path.apm_m_path < LP_APM_MAX_ACCESS_PATH))
              );

    excp_info->excp_id = REG_GET_FIELD(APM_LL_TEE_EXCP_INFO0_REG(excp_info->apm_path.apm_ctrl, excp_info->apm_path.apm_m_path),
                                       APM_LL_CTRL_EXCEPTION_ID);
    excp_info->excp_mode = REG_GET_FIELD(APM_LL_TEE_EXCP_INFO0_REG(excp_info->apm_path.apm_ctrl, excp_info->apm_path.apm_m_path),
                                         APM_LL_CTRL_EXCEPTION_MODE);
    excp_info->excp_regn = REG_GET_FIELD(APM_LL_TEE_EXCP_INFO0_REG(excp_info->apm_path.apm_ctrl, excp_info->apm_path.apm_m_path),
                                         APM_LL_CTRL_EXCEPTION_REGION);
    excp_info->excp_type = apm_ll_apm_ctrl_exception_status(excp_info->apm_path.apm_ctrl, excp_info->apm_path.apm_m_path);
    excp_info->excp_addr = REG_READ(APM_LL_TEE_EXCP_INFO1_REG(excp_info->apm_path.apm_ctrl, excp_info->apm_path.apm_m_path));
}

/**
 * @brief Interrupt enable for APM Ctrl at access path(M[0:n])
 *
 * @param apm_ctrl   APM Ctrl (LP_APM0/HP_APM/LP_APM)
 * @param apm_m_path APM Ctrl access patch(M[0:n])
 * @param enable     Flag for access path interrupt enable/disable
 */
static inline void apm_ll_apm_ctrl_interrupt_enable(apm_ll_apm_ctrl_t apm_ctrl,
                                                    apm_ll_ctrl_access_path_t apm_m_path, bool enable)
{
    HAL_ASSERT(((apm_ctrl == LP_APM0_CTRL) && (apm_m_path < LP_APM0_MAX_ACCESS_PATH)) ||
               ((apm_ctrl == HP_APM_CTRL) && (apm_m_path < HP_APM_MAX_ACCESS_PATH)) ||
               ((apm_ctrl == LP_APM_CTRL) && (apm_m_path < LP_APM_MAX_ACCESS_PATH))
              );

    if (enable) {
        REG_SET_BIT(APM_LL_APM_CTRL_INT_EN_REG(apm_ctrl), BIT(apm_m_path));
    } else {
        REG_CLR_BIT(APM_LL_APM_CTRL_INT_EN_REG(apm_ctrl), BIT(apm_m_path));
    }
}

/**
 * @brief APM Ctrl clock auto gating enable
 *
 * @param apm_ctrl  APM Ctrl (LP_APM0/HP_APM/LP_APM)
 * @param enable    Flag for HP clock auto gating enable/disable
 */
static inline void apm_ll_apm_ctrl_clk_gating_enable(apm_ll_apm_ctrl_t apm_ctrl, bool enable)
{
    if (enable) {
        REG_SET_BIT(APM_LL_APM_CTRL_CLOCK_GATE_REG(apm_ctrl), APM_CTRL_CLK_EN);
    } else {
        REG_CLR_BIT(APM_LL_APM_CTRL_CLOCK_GATE_REG(apm_ctrl), APM_CTRL_CLK_EN);
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
static inline void apm_ll_apm_ctrl_reset_event_enable(bool enable)
{
    if (enable) {
        REG_SET_BIT(PCR_RESET_EVENT_BYPASS_REG, PCR_RESET_EVENT_BYPASS_APM);
    } else {
        REG_CLR_BIT(PCR_RESET_EVENT_BYPASS_REG, PCR_RESET_EVENT_BYPASS_APM);
    }
}

/**
 * @brief Return APM Ctrl interrupt source number.
 *
 * @param apm_ctrl   APM Ctrl (LP_APM0/HP_APM/LP_APM)
 * @param apm_m_path APM Ctrl access patch(M[0:n])
 */
static inline esp_err_t apm_ll_apm_ctrl_get_int_src_num(apm_ll_apm_ctrl_t apm_ctrl, apm_ll_ctrl_access_path_t apm_m_path)
{
    HAL_ASSERT(((apm_ctrl == LP_APM0_CTRL) && (apm_m_path < LP_APM0_MAX_ACCESS_PATH)) ||
               ((apm_ctrl == HP_APM_CTRL) && (apm_m_path < HP_APM_MAX_ACCESS_PATH)) ||
               ((apm_ctrl == LP_APM_CTRL) && (apm_m_path < LP_APM_MAX_ACCESS_PATH))
              );

    switch (apm_ctrl) {
    case LP_APM0_CTRL :
        return (ETS_LP_APM0_INTR_SOURCE);
    case HP_APM_CTRL :
        return (ETS_HP_APM_M0_INTR_SOURCE + apm_m_path);
    case LP_APM_CTRL :
        return (ETS_LP_APM_M0_INTR_SOURCE + apm_m_path);
    }

    return -1;
}

#ifdef __cplusplus
}
#endif
