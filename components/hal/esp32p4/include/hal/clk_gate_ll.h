/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "esp_attr.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "soc/lp_clkrst_struct.h"

/**
 * Enable or disable the clock gate for ref_20m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_20m_clk_en(bool enable)
{
    HP_SYS_CLKRST.ref_clk_ctrl2.reg_ref_20m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_20m_clk_en(...) (void)__DECLARE_RCC_ATOMIC_ENV; _clk_gate_ll_ref_20m_clk_en(__VA_ARGS__)

/**
 * Enable or disable the clock gate for ref_20m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_25m_clk_en(bool enable)
{
    HP_SYS_CLKRST.ref_clk_ctrl1.reg_ref_25m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_25m_clk_en(...) (void)__DECLARE_RCC_ATOMIC_ENV; _clk_gate_ll_ref_25m_clk_en(__VA_ARGS__)

/**
 * Enable or disable the clock gate for ref_20m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_80m_clk_en(bool enable)
{
    HP_SYS_CLKRST.ref_clk_ctrl2.reg_ref_80m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_80m_clk_en(...) (void)__DECLARE_RCC_ATOMIC_ENV; _clk_gate_ll_ref_80m_clk_en(__VA_ARGS__)

/**
 * Enable or disable the clock gate for ref_20m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_160m_clk_en(bool enable)
{
    HP_SYS_CLKRST.ref_clk_ctrl2.reg_ref_160m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_160m_clk_en(...) (void)__DECLARE_RCC_ATOMIC_ENV; _clk_gate_ll_ref_160m_clk_en(__VA_ARGS__)

/**
 * Enable or disable the clock gate for ref_20m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_240m_clk_en(bool enable)
{
    HP_SYS_CLKRST.ref_clk_ctrl1.reg_ref_240m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_240m_clk_en(...) (void)__DECLARE_RCC_ATOMIC_ENV; _clk_gate_ll_ref_240m_clk_en(__VA_ARGS__)

/**
 * Enable or disable the clock gate for xtal to lp periph
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_xtal_to_lp_periph_en(bool enable)
{
    LP_AON_CLKRST.lp_clk_en.xtal_clk_force_on = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_xtal_to_lp_periph_en(...) (void)__DECLARE_RCC_ATOMIC_ENV; _clk_gate_ll_xtal_to_lp_periph_en(__VA_ARGS__)

/**
 * Enable or disable the clock gate for ref_50m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_50m_clk_en(bool enable)
{
    HP_SYS_CLKRST.ref_clk_ctrl1.reg_ref_50m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_50m_clk_en(...) (void)__DECLARE_RCC_ATOMIC_ENV; _clk_gate_ll_ref_50m_clk_en(__VA_ARGS__)


#ifdef __cplusplus
}
#endif
