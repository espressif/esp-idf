/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <string.h>
#include "hal/twai_types.h"
#include "soc/pcr_reg.h"
#include "soc/pcr_struct.h"

#define TWAI_LL_GET(_attr) TWAI_LL_ ## _attr
#define TWAI_LL_SUPPORT(_feat) TWAI_LL_SUPPORT_ ## _feat

#define TWAI_LL_SUPPORT_TIMESTAMP 1
#define TWAI_LL_SUPPORT_RX_STATUS 1

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable the bus clock and module clock for twai module
 *
 * @param twai_id Hardware ID
 * @param enable true to enable, false to disable
 */
static inline void twai_ll_enable_bus_clock(uint8_t twai_id, bool enable)
{
    PCR.twai0_conf.twai0_clk_en = enable;
}

/**
 * @brief Reset the twai module
 *
 * @param twai_id Hardware ID
 */
static inline void twai_ll_reset_register(uint8_t twai_id)
{
    PCR.twai0_conf.twai0_rst_en = 1;
    PCR.twai0_conf.twai0_rst_en = 0;
}

/**
 * @brief Set clock source for TWAI module
 *
 * @param twai_id Hardware ID
 * @param clk_src Clock source
 */
static inline void twai_ll_set_clock_source(uint8_t twai_id, twai_clock_source_t clk_src)
{
    switch (clk_src) {
    case TWAI_CLK_SRC_XTAL:
        PCR.twai0_func_clk_conf.twai0_func_clk_sel = 0;
        break;
    case TWAI_CLK_SRC_PLL_F96M:
        PCR.twai0_func_clk_conf.twai0_func_clk_sel = 2;
        break;
    // We do not plan to support the TWAI_CLK_SRC_RC_FAST clock source,
    // as the accuracy of this clock does not meet the requirements for the baud rate
    // case TWAI_CLK_SRC_RC_FAST:
    //     PCR.twai0_func_clk_conf.twai0_func_clk_sel = 1;
    //     break;
    default:
        HAL_ASSERT(false);
    }
}

/**
 * @brief Enable TWAI module clock source
 *
 * @param twai_id Hardware ID
 * @param enable true to enable, false to disable
 */
static inline void twai_ll_enable_clock(uint8_t twai_id, bool enable)
{
    PCR.twai0_func_clk_conf.twai0_func_clk_en = enable;
    if (enable) {
        while (!PCR.twai0_conf.twai0_ready);
    }
}

#ifdef __cplusplus
}
#endif
