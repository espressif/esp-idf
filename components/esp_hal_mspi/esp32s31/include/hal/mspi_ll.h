/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The ll is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

/**
 * Background
 *
 * This file is for the MSPI related, but not Flash driver related registers, these registers:
 * - may influence both Flash and PSRAM
 * - not related or directly related to Flash controller driver
 *
 * Some hints for naming convention:
 * - For MSPI timing tuning related registers, the LL should start with `mspi_timing_ll_`
 * - For others, the LL should start with `mspi_ll_`
 */

#pragma once

#include <sys/param.h>
#include <stdbool.h>
#include "esp_bit_defs.h"
#include "hal/assert.h"
#include "soc/soc.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "soc/spi_mem_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/iomux_mspi_pin_reg.h"
#include "soc/iomux_mspi_pin_struct.h"
#include "soc/spi_mem_struct.h"
#include "soc/spi_mem_s_struct.h"
#include "soc/spi_mem_c_reg.h"
#include "soc/spi1_mem_c_reg.h"
#include "soc/clk_tree_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MSPI_LL_PERIPH_NUM                            4
#define MSPI_TIMING_LL_MSPI_ID_0                      0
#define MSPI_TIMING_LL_MSPI_ID_1                      1

// PSRAM frequency should be constrained by AXI frequency to avoid FIFO underflow.
#define MSPI_TIMING_LL_PSRAM_FREQ_AXI_CONSTRAINED     1

#define MSPI_LL_AXI_DISABLE_SUPPORTED                 1
#define MSPI_LL_INTR_EVENT_SUPPORTED                  1

/**
 * MSPI DQS ID
 */
typedef enum {
    MSPI_LL_DQS_ID_0,
} mspi_ll_dqs_id_t;

/**
 * MSPI DQS Phase
 */
typedef enum {
    MSPI_LL_DQS_PHASE_67_5,
    MSPI_LL_DQS_PHASE_78_75,
    MSPI_LL_DQS_PHASE_90,
    MSPI_LL_DQS_PHASE_101_25,
    MSPI_LL_DQS_PHASE_MAX,
} mspi_ll_dqs_phase_t;

/**
 * MSPI Delayline
 *
 * @note The Sequence of these enums should not be changed
 */
typedef enum {
    MSPI_LL_PIN_D = 0,
    MSPI_LL_PIN_Q,
    MSPI_LL_PIN_WP,
    MSPI_LL_PIN_HD,
    MSPI_LL_PIN_D4,
    MSPI_LL_PIN_D5,
    MSPI_LL_PIN_D6,
    MSPI_LL_PIN_D7,
    MSPI_LL_PIN_DQS0,
    MSPI_LL_PIN_CLK,
    MSPI_LL_PIN_CS,
    MSPI_LL_PIN_MAX,
} mspi_ll_pin_t;

/*---------------------------------------------------------------
                    PSRAM tuning
---------------------------------------------------------------*/
/**
 * Set all MSPI DQS phase
 *
 * @param dqs_id  DQS ID
 * @param phase   Phase
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_dqs_phase(mspi_ll_dqs_id_t dqs_id, mspi_ll_dqs_phase_t phase)
{
    HAL_ASSERT(dqs_id < 1);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQS_0_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQS_0_PHASE, phase);
}

/**
 * Set all MSPI delayline
 *
 * @param pin        Pin
 * @param delayline  Delayline
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_delayline(mspi_ll_pin_t pin, uint8_t delayline)
{
    HAL_ASSERT(pin < MSPI_LL_PIN_MAX);

    switch (pin) {
    case MSPI_LL_PIN_DQS0:
        MSPI_IOMUX.psram_pin_group.dqs0.reg_psram_dqs_delay_90 = delayline;
        MSPI_IOMUX.psram_pin_group.dqs0.reg_psram_dqs_delay_270 = delayline;
        break;
    case MSPI_LL_PIN_D:
    case MSPI_LL_PIN_Q:
    case MSPI_LL_PIN_WP:
    case MSPI_LL_PIN_HD:
    case MSPI_LL_PIN_D4:
    case MSPI_LL_PIN_D5:
    case MSPI_LL_PIN_D6:
    case MSPI_LL_PIN_D7:
        MSPI_IOMUX.psram_pin_group.pin_group0[pin].reg_psram_pin_dlc = delayline;
        break;
    case MSPI_LL_PIN_CLK:
        MSPI_IOMUX.psram_pin_group.pin_clk.reg_psram_pin_dlc = delayline;
        break;
    case MSPI_LL_PIN_CS:
        MSPI_IOMUX.psram_pin_group.pin_cs.reg_psram_pin_dlc = delayline;
        break;
    default:
        HAL_ASSERT(false);
        break;
    }
}

/**
 * Enable DQS
 *
 * @param en  Enable/disable
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_enable_dqs(bool en)
{
    if (en) {
        REG_SET_BIT(IOMUX_MSPI_PIN_PSRAM_DQS_0_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQS_0_XPD);
    } else {
        REG_CLR_BIT(IOMUX_MSPI_PIN_PSRAM_DQS_0_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQS_0_XPD);
    }
}

/**
 * Set all MSPI pin drive
 *
 * @param drv  Pin drive
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_pin_drv_set(uint8_t drv)
{
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_D_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_D_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_Q_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_Q_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_WP_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_WP_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_HOLD_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_HOLD_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ4_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ4_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ5_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ5_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ6_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ6_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ7_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ7_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQS_0_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQS_0_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_CK_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_CK_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_CS_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_CS_DRV, drv);
}

/*---------------------------------------------------------------
                    Flash tuning
---------------------------------------------------------------*/
/*
 * @brief Select FLASH clock source
 *
 * @param mspi_id      mspi_id
 * @param clk_src      clock source, see valid sources in type `soc_periph_flash_clk_src_t`
 */
__attribute__((always_inline))
static inline void _mspi_timing_ll_set_flash_clk_src(uint32_t mspi_id, soc_periph_flash_clk_src_t clk_src)
{
    HAL_ASSERT(mspi_id == MSPI_TIMING_LL_MSPI_ID_0);
    uint32_t clk_val = 0;
    switch (clk_src) {
    case FLASH_CLK_SRC_XTAL:
        clk_val = 0;
        break;
    case FLASH_CLK_SRC_SPLL:
        clk_val = 1;
        break;
    case FLASH_CLK_SRC_CPLL:
        clk_val = 2;
        break;
    default:
        HAL_ASSERT(false);
        break;
    }

    HP_SYS_CLKRST.flash_ctrl0.reg_flash_sys_clk_en = 1;
    HP_SYS_CLKRST.flash_ctrl0.reg_flash_pll_clk_en = 1;
    HP_SYS_CLKRST.flash_ctrl0.reg_flash_clk_src_sel = clk_val;
}

/*---------------------------------------------------------------
                    Misc
---------------------------------------------------------------*/
/**
 * @brief Enable/Disable MSPI controller interrupt
 *
 * @param mspi_id     mspi_id
 * @param intr_mask   interrupt mask
 * @param enable      enable / disable
 */
__attribute__((always_inline))
static inline void mspi_ll_enable_intr(uint8_t spi_num, uint32_t intr_mask, bool enable)
{
    if (enable) {
        SPIMEM0.mem_int_ena.val |= intr_mask;
    } else {
        SPIMEM0.mem_int_ena.val &= ~intr_mask;
    }
}

/**
 * @brief Clear MSPI controller interrupt
 *
 * @param mspi_id     mspi_id
 * @param intr_mask   interrupt mask
 */
__attribute__((always_inline))
static inline void mspi_ll_clear_intr(uint8_t spi_num, uint32_t intr_mask)
{
    SPIMEM0.mem_int_clr.val = intr_mask;
}

/**
 * @brief Get MSPI controller interrupt raw
 *
 * @param mspi_id     mspi_id
 */
__attribute__((always_inline))
static inline uint32_t mspi_ll_get_intr_raw(uint8_t spi_num)
{
    return SPIMEM0.mem_int_raw.val;
}

/**
 * Enable AXI access to flash
 *
 * @param spi_num  SPI0 / SPI1
 * @param enable   Enable / Disable
 */
__attribute__((always_inline))
static inline void mspi_ll_flash_enable_axi_access(uint8_t spi_num, bool enable)
{
    SPIMEM0.mem_cache_fctrl.close_axi_inf_en = !enable;
}

/**
 * Enable AXI access to PSRAM
 *
 * @param spi_num  SPI0 / SPI1
 * @param enable   Enable / Disable
 */
__attribute__((always_inline))
static inline void mspi_ll_psram_enable_axi_access(uint8_t spi_num, bool enable)
{
    SPIMEM2.mem_cache_fctrl.close_axi_inf_en = !enable;
}

#ifdef __cplusplus
}
#endif
