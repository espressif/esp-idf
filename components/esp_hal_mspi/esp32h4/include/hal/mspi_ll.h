/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc.h"
#include "soc/clk_tree_defs.h"
#include "soc/pcr_struct.h"
#include "soc/spi_mem_struct.h"
#include "hal/misc.h"
#include "hal/assert.h"

#define MSPI_LL_ADDR_INT_SUPPORTED                    1
#define MSPI_LL_PMS_INT_SUPPORTED                     1
#define MSPI_LL_ECC_INT_SUPPORTED                     1
#define MSPI_LL_PERIPH_NUM                            2
#define MSPI_TIMING_LL_MSPI_ID_0                      0
#define MSPI_TIMING_LL_MSPI_ID_1                      1

// PSRAM frequency should be constrained by AXI frequency to avoid FIFO underflow.
#define MSPI_TIMING_LL_PSRAM_FREQ_AXI_CONSTRAINED     1

#define MSPI_LL_EVENT_SLV_ST_END                      (1<<3)
#define MSPI_LL_EVENT_MST_ST_END                      (1<<4)
#define MSPI_LL_EVENT_ECC_ERR                         (1<<5)
#define MSPI_LL_EVENT_PMS_REJECT                      (1<<6)
#define MSPI_LL_EVENT_AXI_RADDR_ERR                   (1<<7)
#define MSPI_LL_EVENT_AXI_WR_FLASH_ERR                (1<<8)
#define MSPI_LL_EVENT_AXI_WADDR_ERR                   (1<<9)
#define MSPI_LL_EVENT_MASK                            (MSPI_LL_EVENT_ECC_ERR | MSPI_LL_EVENT_PMS_REJECT | MSPI_LL_EVENT_AXI_RADDR_ERR | \
                                                       MSPI_LL_EVENT_AXI_WR_FLASH_ERR | MSPI_LL_EVENT_AXI_WADDR_ERR)

#define MSPI_LL_INTR_EVENT_SUPPORTED                  1
#define MSPI_LL_INTR_SHARED                           1

#ifdef __cplusplus
extern "C" {
#endif

/************************** MSPI pll clock configurations **************************/
/*
 * @brief Select FLASH clock source
 *
 * @param mspi_id      mspi_id
 * @param clk_src      clock source, see valid sources in type `soc_periph_flash_clk_src_t`
 */
__attribute__((always_inline))
static inline void _mspi_timing_ll_set_flash_clk_src(uint32_t mspi_id, soc_periph_flash_clk_src_t clk_src)
{
    HAL_ASSERT(mspi_id == 0);
    switch (clk_src) {
    case FLASH_CLK_SRC_XTAL:
        PCR.mspi_clk_conf.mspi_func_clk_sel = 0;
        break;
    case FLASH_CLK_SRC_RC_FAST:
        PCR.mspi_clk_conf.mspi_func_clk_sel = 1;
        break;
    case FLASH_CLK_SRC_REF_F64M:
        PCR.mspi_clk_conf.mspi_func_clk_sel = 2;
        break;
    case FLASH_CLK_SRC_PLL_F48M:
        PCR.mspi_clk_conf.mspi_func_clk_sel = 3;
        break;
    default:
        HAL_ASSERT(false);
    }
}

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
        SPIMEM0.int_ena.val |= intr_mask;
    } else {
        SPIMEM0.int_ena.val &= ~intr_mask;
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
    SPIMEM0.int_clr.val = intr_mask;
}

/**
 * @brief Get MSPI controller interrupt raw
 *
 * @param mspi_id     mspi_id
 */
__attribute__((always_inline))
static inline uint32_t mspi_ll_get_intr_raw(uint8_t spi_num)
{
    return SPIMEM0.int_raw.val;
}

#ifdef __cplusplus
}
#endif
