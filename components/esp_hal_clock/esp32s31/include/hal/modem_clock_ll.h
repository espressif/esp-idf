/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_attr.h"
#include "soc/hp_sys_clkrst_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Check if the modem PLL clock gate is enabled on HP_SYS_CLKRST.modem_conf.
 */
FORCE_INLINE_ATTR bool modem_clock_ll_pll_clk_is_enabled(void)
{
    return HP_SYS_CLKRST.modem_conf.modem_pll_clk_en;
}

/**
 * Check if the modem root clock source is PLL.
 */
FORCE_INLINE_ATTR bool modem_clock_ll_source_is_pll(void)
{
    return HP_SYS_CLKRST.modem_conf.modem_clk_source_sel;
}

/**
 * Ungate modem bus/root clocks and release modem reset on modem_conf.
 * Sets modem_apb_clk_en=1, modem_clk_en=1, modem_rst_en=0 in one write; other
 * fields preserved. Use after SW_CPU_RESET where modem_conf may retain a gated
 * or held-in-reset state.
 */
FORCE_INLINE_ATTR void _modem_clock_ll_ungate(void)
{
    hp_sys_clkrst_modem_conf_reg_t conf;
    conf.val = HP_SYS_CLKRST.modem_conf.val;
    conf.modem_apb_clk_en = 1;
    conf.modem_clk_en = 1;
    conf.modem_rst_en = 0;
    HP_SYS_CLKRST.modem_conf.val = conf.val;
}

/**
 * Select modem root clock source (XTAL <-> SoC PLL) on HP_SYS_CLKRST.modem_conf.
 *
 * When switching the mux, only one of the source clocks may be enabled; having
 * both (or neither) enabled during the switch can glitch the modem clock.
 * Hence the two-step write: first enable the target source and disable the
 * other while keeping the old mux select, then flip modem_clk_source_sel.
 * Other modem_conf fields (e.g. modem_apb_clk_en / modem_rst_en) are preserved.
 *
 * @param enable true: select PLL source; false: select XTAL source
 */
FORCE_INLINE_ATTR void _modem_clock_ll_enable_pll_clock_source(bool enable)
{
    hp_sys_clkrst_modem_conf_reg_t conf;
    conf.val = HP_SYS_CLKRST.modem_conf.val;
    if (enable) {
        conf.modem_pll_clk_en = 1;
        conf.modem_xtal_clk_en = 0;
        HP_SYS_CLKRST.modem_conf.val = conf.val;
        conf.modem_clk_source_sel = 1;
        HP_SYS_CLKRST.modem_conf.val = conf.val;
        conf.modem_xtal_clk_en = 1;
        HP_SYS_CLKRST.modem_conf.val = conf.val;
    } else {
        conf.modem_xtal_clk_en = 1;
        conf.modem_pll_clk_en = 0;
        HP_SYS_CLKRST.modem_conf.val = conf.val;
        conf.modem_clk_source_sel = 0;
        HP_SYS_CLKRST.modem_conf.val = conf.val;
    }
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define modem_clock_ll_enable_pll_clock_source(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _modem_clock_ll_enable_pll_clock_source(__VA_ARGS__); \
    } while (0)

#ifdef __cplusplus
}
#endif
