/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "hal/assert.h"
#include "soc/dma_pms_reg.h"
#include "soc/hp2lp_peri_pms_reg.h"
#include "soc/hp_peri_pms_reg.h"
#include "soc/lp2hp_peri_pms_reg.h"
#include "soc/lp_peri_pms_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Master Secure Mode
 */
typedef enum {
    APM_LL_SECURE_MODE_TEE = 0, /* Trusted execution environment mode (Machine mode). */
    APM_LL_SECURE_MODE_REE = 1,  /* Rich execution environment mode (User mode). */
    APM_LL_SECURE_MODE_INV = 2,  /* Invalid mode. */
} apm_ll_secure_mode_t;

/**
 * @brief Bus Masters.
 */
typedef enum {
    APM_LL_MASTER_LPCPU  = 0,
    APM_LL_MASTER_HPCPU0,
    APM_LL_MASTER_HPCPU1,
    APM_LL_MASTER_DMA,
} apm_ll_master_id_t;

/**
 * @brief APM Controller
 */
typedef enum {
    LP_APM_CTRL    = 0,
    HP2LP_APM_CTRL,
    HP_APM_CTRL,
    LP2HP_APM_CTRL,
    DMA_APM_CTRL,
    MAX_APM_CTRL,
} apm_ll_apm_ctrl_t;

/**
 * @brief HP CPU Peripherals.
 */
typedef enum {
    PMS_COREn_XM_PSRAM_ALLOW = 0,
    PMS_COREn_XM_FLASH_ALLOW,
    PMS_COREn_XM_L2MEM_ALLOW,
    PMS_COREn_XM_L2ROM_ALLOW,
    PMS_COREn_XM_TRACE0_ALLOW = 6,
    PMS_COREn_XM_TRACE1_ALLOW,
    PMS_COREn_XM_CPU_BUS_MON_ALLOW,
    PMS_COREn_XM_L2MEM_MON_ALLOW,
    PMS_COREn_XM_TCM_MON_ALLOW,
    PMS_COREn_XM_CACHE_ALLOW,

    PMS_COREn_XM_HP_USBOTG_ALLOW = 32,
    PMS_COREn_XM_HP_USBOTG11_ALLOW,
    PMS_COREn_XM_HP_USBOTG11_WRAP_ALLOW,
    PMS_COREn_XM_HP_GDMA_ALLOW,
    PMS_COREn_XM_HP_SDMMC_ALLOW = 37,
    PMS_COREn_XM_HP_AHB_PDMA_ALLOW,
    PMS_COREn_XM_HP_JPEG_ALLOW,
    PMS_COREn_XM_HP_PPA_ALLOW,
    PMS_COREn_XM_HP_DMA2D_ALLOW,
    PMS_COREn_XM_HP_KEY_MANAGER_ALLOW,
    PMS_COREn_XM_HP_AXI_PDMA_ALLOW,
    PMS_COREn_XM_HP_FLASH_ALLOW,
    PMS_COREn_XM_HP_PSRAM_ALLOW,
    PMS_COREn_XM_HP_CRYPTO_ALLOW,
    PMS_COREn_XM_HP_GMAC_ALLOW,
    PMS_COREn_XM_HP_USB_PHY_ALLOW,
    PMS_COREn_XM_HP_CSI_HOST_ALLOW = 50,
    PMS_COREn_XM_HP_DSI_HOST_ALLOW,
    PMS_COREn_XM_HP_ISP_ALLOW,
    PMS_COREn_XM_HP_H264_CORE_ALLOW,
    PMS_COREn_XM_HP_RMT_ALLOW,
    PMS_COREn_XM_HP_BITSCRAMBLER_ALLOW,
    PMS_COREn_XM_HP_AXI_ICM_ALLOW,
    PMS_COREn_XM_HP_PERI_PMS_ALLOW,
    PMS_COREn_XM_LP2HP_PERI_PMS_ALLOW,
    PMS_COREn_XM_DMA_PMS_ALLOW,
    PMS_COREn_XM_HP_H264_DMA2D_ALLOW = 60,

    PMS_COREn_XM_HP_MCPWM0_ALLOW = 64,
    PMS_COREn_XM_HP_MCPWM1_ALLOW,
    PMS_COREn_XM_HP_TIMER_GROUP0_ALLOW,
    PMS_COREn_XM_HP_TIMER_GROUP1_ALLOW,
    PMS_COREn_XM_HP_I2C0_ALLOW,
    PMS_COREn_XM_HP_I2C1_ALLOW,
    PMS_COREn_XM_HP_I2S0_ALLOW,
    PMS_COREn_XM_HP_I2S1_ALLOW,
    PMS_COREn_XM_HP_I2S2_ALLOW,
    PMS_COREn_XM_HP_PCNT_ALLOW,
    PMS_COREn_XM_HP_UART0_ALLOW,
    PMS_COREn_XM_HP_UART1_ALLOW,
    PMS_COREn_XM_HP_UART2_ALLOW,
    PMS_COREn_XM_HP_UART3_ALLOW,
    PMS_COREn_XM_HP_UART4_ALLOW,
    PMS_COREn_XM_HP_PARLIO_ALLOW269,
    PMS_COREn_XM_HP_GPSPI2_ALLOW270,
    PMS_COREn_XM_HP_GPSPI3_ALLOW271,
    PMS_COREn_XM_HP_USBDEVICE_ALLOW,
    PMS_COREn_XM_HP_LEDC_ALLOW,
    PMS_COREn_XM_HP_ETM_ALLOW = 85,
    PMS_COREn_XM_HP_INTRMTX_ALLOW,
    PMS_COREn_XM_HP_TWAI0_ALLOW,
    PMS_COREn_XM_HP_TWAI1_ALLOW,
    PMS_COREn_XM_HP_TWAI2_ALLOW,
    PMS_COREn_XM_HP_I3C_MST_ALLOW,
    PMS_COREn_XM_HP_I3C_SLV_ALLOW,
    PMS_COREn_XM_HP_LCDCAM_ALLOW,
    PMS_COREn_XM_HP_ADC_ALLOW = 94,
    PMS_COREn_XM_HP_UHCI_ALLOW,

    PMS_COREn_XM_HP_GPIO_ALLOW = 96,
    PMS_COREn_XM_HP_IOMUX_ALLOW,
    PMS_COREn_XM_HP_SYSTIMER_ALLOW,
    PMS_COREn_XM_HP_SYS_REG_ALLOW,
    PMS_COREn_XM_HP_CLKRST_ALLOW,
    PMS_COREn_XM_HP_PERI_MAX,
} apm_ll_hp_peri_t;

/**
 * @brief LP CPU Peripherals.
 */
typedef enum {
    PMS_MM_LP_SYSREG_ALLOW = 0,
    PMS_MM_LP_AONCLKRST_ALLOW,
    PMS_MM_LP_TIMER_ALLOW,
    PMS_MM_LP_ANAPERI_ALLOW,
    PMS_MM_LP_PMU_ALLOW,
    PMS_MM_LP_WDT_ALLOW,
    PMS_MM_LP_MAILBOX_ALLOW,
    PMS_MM_LP_PERICLKRST_ALLOW = 8,
    PMS_MM_LP_UART_ALLOW,
    PMS_MM_LP_I2C_ALLOW,
    PMS_MM_LP_SPI_ALLOW,
    PMS_MM_LP_I2CMST_ALLOW,
    PMS_MM_LP_I2S_ALLOW,
    PMS_MM_LP_ADC_ALLOW,
    PMS_MM_LP_TOUCH_ALLOW,
    PMS_MM_LP_IOMUX_ALLOW,
    PMS_MM_LP_INTR_ALLOW,
    PMS_MM_LP_EFUSE_ALLOW,
    PMS_MM_LP_PMS_ALLOW,
    PMS_MM_HP2LP_PMS_ALLOW,
    PMS_MM_LP_TSENS_ALLOW,
    PMS_MM_LP_HUK_ALLOW,
    PMS_HP_COREn_MM_LP_SRAM_ALLOW,
    PMS_LP_MM_PERI_MAX,
} apm_ll_lp_peri_t;

/**
 * @brief LP CPU Peripherals.
 */
typedef enum {
    PMS_DMA_GDMA_CH0 = 0,
    PMS_DMA_GDMA_CH1,
    PMS_DMA_GDMA_CH2,
    PMS_DMA_GDMA_CH3,
    PMS_DMA_AHB_PDMA_ADC,
    PMS_DMA_AHB_PDMA_I2S0,
    PMS_DMA_AHB_PDMA_I2S1,
    PMS_DMA_AHB_PDMA_I2S2,
    PMS_DMA_AHB_PDMA_I3C_MST,
    PMS_DMA_AHB_PDMA_UHCI0,
    PMS_DMA_AHB_PDMA_RMT,
    PMS_DMA_AXI_PDMA_LCDCAM,
    PMS_DMA_AXI_PDMA_GPSPI2,
    PMS_DMA_AXI_PDMA_GPSPI3,
    PMS_DMA_AXI_PDMA_PARLIO,
    PMS_DMA_AXI_PDMA_AES,
    PMS_DMA_AXI_PDMA_SHA,
    PMS_DMA_DMA2D_JPEG,
    PMS_DMA_USB,
    PMS_DMA_GMAC,
    PMS_DMA_SDMMC,
    PMS_DMA_USBOTG11,
    PMS_DMA_TRACE0,
    PMS_DMA_TRACE1,
    PMS_DMA_L2MEM_MON,
    PMS_DMA_TCM_MON,
    PMS_DMA_H264,
    PMS_DMA_DMA2D_PPA,
    PMS_DMA_DMA2D_DUMMY,
    PMS_DMA_AHB_PDMA_DUMMY,
    PMS_DMA_AXI_PDMA_DUMMY,
    PMS_DMA_MAX,
} apm_ll_dma_master_t;

#define PMS_PERI_MAX_REGION_NUM    2
#define PMS_DMA_MAX_REGION_NUM     32

#define PMS_COREn_XM_PMS_REGn_REG(master_id, sec_mode, hp_peri) \
    ({\
        (PMS_CORE0_MM_HP_PERI_PMS_REG0_REG + (master_id * 0x20) \
        + (sec_mode * 0x10) + ((hp_peri/32) * 0x4) ); \
    })

#define PMS_PERI_REGION_LOW_REG(regn_num) \
    ({\
        (PMS_PERI_REGION0_LOW_REG + (regn_num * 8)); \
    })

#define PMS_PERI_REGION_HIGH_REG(regn_num) \
    ({\
        (PMS_PERI_REGION0_HIGH_REG + (regn_num * 8)); \
    })

#define PMS_DMA_PMS_R_REG(dma_master) \
    ({\
        (PMS_DMA_GDMA_CH0_R_PMS_REG + (dma_master * 8)); \
    })

#define PMS_DMA_PMS_W_REG(dma_master) \
    ({\
        (PMS_DMA_GDMA_CH0_W_PMS_REG + (dma_master * 8)); \
    })

/**
 * @brief Configure HP peripherals access permission for the HP CPU0/1.
 *
 * @param master_id  HP CPU0/1
 * @param hp_peri    HP peripheral whose access permission to be configured.
 * @param enable     Permission enable/disable
 */
static inline void apm_ll_hp_peri_access_enable(apm_ll_master_id_t master_id, apm_ll_hp_peri_t hp_peri,
                                                apm_ll_secure_mode_t sec_mode, bool enable)
{
    HAL_ASSERT((master_id > APM_LL_MASTER_LPCPU) && (master_id < APM_LL_MASTER_DMA)
               && (hp_peri < PMS_COREn_XM_HP_PERI_MAX) && (sec_mode < APM_LL_SECURE_MODE_INV));

    if (enable) {
        REG_SET_BIT(PMS_COREn_XM_PMS_REGn_REG(master_id, sec_mode, hp_peri), BIT(hp_peri % 32));
    } else {
        REG_CLR_BIT(PMS_COREn_XM_PMS_REGn_REG(master_id, sec_mode, hp_peri), BIT(hp_peri % 32));
    }
}

/**
 * @brief Configure LP peripherals access permission for the LP CPU.
 *
 * @param lp_peri    LP peripheral whose access permission to be configured.
 * @param enable     Permission enable/disable
 */
static inline void apm_ll_lp_peri_access_enable(apm_ll_lp_peri_t lp_peri, bool enable)
{
    HAL_ASSERT(lp_peri < PMS_LP_MM_PERI_MAX);

    if (enable) {
        REG_SET_BIT(PMS_LP_MM_LP_PERI_PMS_REG0_REG, BIT(lp_peri));
    } else {
        REG_CLR_BIT(PMS_LP_MM_LP_PERI_PMS_REG0_REG, BIT(lp_peri));
    }
}

/**
 * @brief Configure peripherals configurable address ranges.
 *
 * @param regn_num       Configurable address range number.
 * @param regn_low_addr  Configurable address range start address.
 * @param regn_high_addr Configurable address range end address.
 */
static inline void apm_ll_peri_region_config(uint32_t regn_num, uint32_t regn_low_addr,
                                             uint32_t regn_high_addr)
{
    HAL_ASSERT(regn_num < PMS_PERI_MAX_REGION_NUM);

    REG_WRITE(PMS_PERI_REGION_LOW_REG(regn_num), regn_low_addr);
    REG_WRITE(PMS_PERI_REGION_HIGH_REG(regn_num), regn_high_addr);
}

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
static inline int apm_ll_peri_region_pms(apm_ll_master_id_t master_id, apm_ll_secure_mode_t sec_mode,
                                         uint32_t regn_num, uint32_t regn_pms)
{
    HAL_ASSERT((master_id < APM_LL_MASTER_DMA) && (sec_mode < APM_LL_SECURE_MODE_INV));

    regn_pms &= 0x3;

    switch (master_id) {
    case APM_LL_MASTER_LPCPU:
        REG_SET_FIELD(PMS_PERI_REGION_PMS_REG, PMS_LP_CORE_REGION_PMS, regn_pms);
        break;
    case APM_LL_MASTER_HPCPU0:
        if (sec_mode) {
            REG_SET_FIELD(PMS_PERI_REGION_PMS_REG, PMS_HP_CORE0_UM_REGION_PMS, regn_pms);
        } else {
            REG_SET_FIELD(PMS_PERI_REGION_PMS_REG, PMS_HP_CORE0_MM_REGION_PMS, regn_pms);
        }
        break;
    case APM_LL_MASTER_HPCPU1:
        if (sec_mode) {
            REG_SET_FIELD(PMS_PERI_REGION_PMS_REG, PMS_HP_CORE1_UM_REGION_PMS, regn_pms);
        } else {
            REG_SET_FIELD(PMS_PERI_REGION_PMS_REG, PMS_HP_CORE1_MM_REGION_PMS, regn_pms);
        }
        break;
    default:
        return -1;
    }

    return 0;
}

/**
 * @brief Configure APM controller clock gating.
 *
 * @param apm_ctrl APM controller (LP_PERI/HP_PERI/HP_DMA/LP2HP_PERI/HP2LP_PERI).
 * @param enable   Permission enable/disable.
 *                 enable: Enable automatic clock gating.
 *                 disable: Keep the clock always on.
 */
static inline int apm_ll_apm_ctrl_clk_gating_enable(apm_ll_apm_ctrl_t apm_ctrl, bool enable)
{
    uint32_t reg = 0;

    HAL_ASSERT(apm_ctrl < MAX_APM_CTRL);

    switch (apm_ctrl) {
    case LP_APM_CTRL:
        reg = PMS_LP_PERI_PMS_CLK_EN_REG;
        break;
    case HP2LP_APM_CTRL:
        reg = PMS_HP2LP_PERI_PMS_CLK_EN_REG;
        break;
    case HP_APM_CTRL:
        reg = PMS_HP_PERI_PMS_CLK_EN_REG;
        break;
    case LP2HP_APM_CTRL:
        reg = PMS_LP2HP_PERI_PMS_CLK_EN_REG;
        break;
    case DMA_APM_CTRL:
        reg = PMS_DMA_CLK_EN_REG;
        break;
    default:
        return -1;
    }

    if (enable) {
        REG_CLR_BIT(reg, BIT(0));
    } else {
        REG_SET_BIT(reg, BIT(0));
    }

    return 0;
}

/**
 * @brief Configure DMA configurable address range low address.
 *
 * @param regn_num      Configurable DMA address range number.
 * @param regn_low_addr Configurable DMA address range start address.
 */
static inline void apm_ll_dma_region_set_low_address(uint32_t regn_num, uint32_t regn_low_addr)
{
    HAL_ASSERT(regn_num < PMS_DMA_MAX_REGION_NUM);

    REG_WRITE((PMS_DMA_REGION0_LOW_REG + (regn_num * 8)), regn_low_addr);
}

/**
 * @brief Configure DMA configurable address range high address.
 *
 * @param regn_num       Configurable DMA address range number.
 * @param regn_high_addr Configurable DMA address range end address.
 */
static inline void apm_ll_dma_region_set_high_address(uint32_t regn_num, uint32_t regn_high_addr)
{
    HAL_ASSERT(regn_num < PMS_DMA_MAX_REGION_NUM);

    REG_WRITE((PMS_DMA_REGION0_HIGH_REG + (regn_num * 8)), regn_high_addr);
}

/**
 * @brief Configure DMA configurable address range read permission.
 *
 * @param dma_master DMA master whose access permission to be configured.
 * @param regn_mask  32-bits field, each bit for corresponding DMA configurable address range permission.
 *                   0: Disable read permission.
 *                   1: Enable read permission.
 */
static inline void apm_ll_dma_region_r_pms(apm_ll_dma_master_t dma_master, uint32_t regn_mask)
{
    HAL_ASSERT(dma_master < PMS_DMA_MAX);

    REG_WRITE(PMS_DMA_PMS_R_REG(dma_master), regn_mask);
}

/**
 * @brief Configure DMA configurable address range write permission.
 *
 * @param dma_master DMA master whose access permission to be configured.
 * @param regn_mask  32-bits field, each bit for corresponding DMA configurable address range permission.
 *                   0: Disable write permission.
 *                   1: Enable write permission.
 */
static inline void apm_ll_dma_region_w_pms(apm_ll_dma_master_t dma_master, uint32_t regn_mask)
{
    HAL_ASSERT(dma_master < PMS_DMA_MAX);

    REG_WRITE(PMS_DMA_PMS_W_REG(dma_master), regn_mask);
}

#ifdef __cplusplus
}
#endif
