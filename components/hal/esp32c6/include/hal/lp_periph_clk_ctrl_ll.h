/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdlib.h>
#include "soc/periph_defs.h"
#include "soc/clk_tree_defs.h"
#include "soc/soc.h"
#include "soc/lpperi_reg.h"
#include "soc/lp_clkrst_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

static uint32_t lp_periph_ll_get_clk_en_mask(lp_periph_module_t lp_periph)
{
    switch (lp_periph) {
    case LP_PERIPH_I2C0_MODULE:
        return LPPERI_LP_EXT_I2C_CK_EN;
    case LP_PERIPH_UART0_MODULE:
        return LPPERI_LP_UART_CK_EN;
    default:
        // Unsupported LP peripherals
        abort();
    }
}

static uint32_t lp_periph_ll_get_rst_en_mask(lp_periph_module_t lp_periph)
{
    switch (lp_periph) {
    case LP_PERIPH_I2C0_MODULE:
        return LPPERI_LP_EXT_I2C_RESET_EN;
    case LP_PERIPH_UART0_MODULE:
        return LPPERI_LP_UART_RESET_EN;
    default:
        // Unsupported LP peripherals
        abort();
    }
}

static inline void lp_periph_ll_enable_clk_clear_rst(lp_periph_module_t lp_periph)
{
    SET_PERI_REG_MASK(LPPERI_CLK_EN_REG, lp_periph_ll_get_clk_en_mask(lp_periph));
    CLEAR_PERI_REG_MASK(LPPERI_RESET_EN_REG, lp_periph_ll_get_rst_en_mask(lp_periph));
}

static inline void lp_periph_ll_disable_clk_set_rst(lp_periph_module_t lp_periph)
{
    CLEAR_PERI_REG_MASK(LPPERI_CLK_EN_REG, lp_periph_ll_get_clk_en_mask(lp_periph));
    SET_PERI_REG_MASK(LPPERI_RESET_EN_REG, lp_periph_ll_get_rst_en_mask(lp_periph));
}

static inline void lp_periph_ll_reset(lp_periph_module_t lp_periph)
{
    uint32_t bit_mask =  lp_periph_ll_get_rst_en_mask(lp_periph);
    SET_PERI_REG_MASK(LPPERI_RESET_EN_REG, bit_mask);
    CLEAR_PERI_REG_MASK(LPPERI_RESET_EN_REG, bit_mask);
}

static inline void lp_periph_ll_set_clk_src(lp_periph_module_t lp_periph, soc_module_clk_t clk_src)
{
    uint32_t val;
    switch (lp_periph) {
    case LP_PERIPH_I2C0_MODULE:
        switch (clk_src) {
        case LP_I2C_SCLK_LP_FAST:
            val = 0;
            break;
        case LP_I2C_SCLK_XTAL_D2:
            val = 1;
            break;
        default:
            // Invalid LP_I2C clock source
            abort();
        }
        REG_SET_FIELD(LP_CLKRST_LPPERI_REG, LP_CLKRST_LP_I2C_CLK_SEL, val);
        break;
    case LP_PERIPH_UART0_MODULE:
        switch (clk_src) {
        case LP_UART_SCLK_LP_FAST:
            val = 0;
            break;
        case LP_UART_SCLK_XTAL_D2:
            val = 1;
            break;
        default:
            // Invalid LP_UART clock source
            abort();
        }
        REG_SET_FIELD(LP_CLKRST_LPPERI_REG, LP_CLKRST_LP_UART_CLK_SEL, val);
        break;
    default:
        // Unsupported LP peripherals
        abort();
    }
}

#ifdef __cplusplus
}
#endif
