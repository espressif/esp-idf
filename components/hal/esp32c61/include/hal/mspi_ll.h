/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
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
#include "hal/misc.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MSPI_TIMING_LL_MSPI_ID_0                      0
#define MSPI_TIMING_LL_MSPI_ID_1                      1
#define MSPI_LL_CORE_CLOCK_80_MHZ                     80
#define MSPI_LL_CORE_CLOCK_120_MHZ                    120
#define MSPI_TIMING_LL_CORE_CLOCK_MHZ_DEFAULT         MSPI_LL_CORE_CLOCK_80_MHZ

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
    case FLASH_CLK_SRC_SPLL:
        PCR.mspi_clk_conf.mspi_func_clk_sel = 2;
        break;
    default:
        HAL_ASSERT(false);
    }
}

/**
 * @brief Set MSPI_FAST_CLK's high-speed divider (valid when SOC_ROOT clock source is PLL)
 *
 * @param mspi_id       SPI0 / SPI1
 * @param core_clk_mhz  core clock mhz
 */
static inline __attribute__((always_inline)) void mspi_timing_ll_set_core_clock(uint8_t mspi_id, uint32_t core_clk_mhz)
{
    HAL_ASSERT(mspi_id == 0);
    uint32_t divider = 0;
    switch (core_clk_mhz) {
        case 80:
            divider = 6;
            break;
        case 120:
            divider = 4;
            break;
        default:
            HAL_ASSERT(false);
    }

    HAL_FORCE_MODIFY_U32_REG_FIELD(PCR.mspi_clk_conf, mspi_fast_div_num, divider - 1);
}

/**
 * @brief Enable the mspi core clock
 *
 * @param mspi_id       SPI0 / SPI1
 * @param enable        enable the core clock
 */
static inline __attribute__((always_inline)) void mspi_timing_ll_enable_core_clock(uint8_t mspi_id, bool enable)
{
    PCR.mspi_conf.mspi_clk_en = enable;
}


#ifdef __cplusplus
}
#endif
