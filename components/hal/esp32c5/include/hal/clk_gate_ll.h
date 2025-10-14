/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
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
#include "soc/pcr_struct.h"

/**
 * Enable or disable the clock gate for ref_12m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_12m_clk_en(bool enable)
{
    PCR.pll_div_clk_en.pll_12m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_12m_clk_en(...) (void)__DECLARE_RCC_ATOMIC_ENV; _clk_gate_ll_ref_12m_clk_en(__VA_ARGS__)

/**
 * Enable or disable the clock gate for ref_20m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_20m_clk_en(bool enable)
{
    PCR.pll_div_clk_en.pll_20m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_20m_clk_en(...) (void)__DECLARE_RCC_ATOMIC_ENV; _clk_gate_ll_ref_20m_clk_en(__VA_ARGS__)

/**
 * Enable or disable the clock gate for ref_40m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_40m_clk_en(bool enable)
{
    PCR.pll_div_clk_en.pll_40m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_40m_clk_en(...) (void)__DECLARE_RCC_ATOMIC_ENV; _clk_gate_ll_ref_40m_clk_en(__VA_ARGS__)

/**
 * Enable or disable the clock gate for ref_48m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_48m_clk_en(bool enable)
{
    PCR.pll_div_clk_en.pll_48m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_48m_clk_en(...) (void)__DECLARE_RCC_ATOMIC_ENV; _clk_gate_ll_ref_48m_clk_en(__VA_ARGS__)

/**
 * Enable or disable the clock gate for ref_60m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_60m_clk_en(bool enable)
{
    PCR.pll_div_clk_en.pll_60m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_60m_clk_en(...) (void)__DECLARE_RCC_ATOMIC_ENV; _clk_gate_ll_ref_60m_clk_en(__VA_ARGS__)

/**
 * Enable or disable the clock gate for ref_80m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_80m_clk_en(bool enable)
{
    PCR.pll_div_clk_en.pll_80m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_80m_clk_en(...) (void)__DECLARE_RCC_ATOMIC_ENV; _clk_gate_ll_ref_80m_clk_en(__VA_ARGS__)

/**
 * Enable or disable the clock gate for ref_120m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_120m_clk_en(bool enable)
{
    PCR.pll_div_clk_en.pll_120m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_120m_clk_en(...) (void)__DECLARE_RCC_ATOMIC_ENV; _clk_gate_ll_ref_120m_clk_en(__VA_ARGS__)

/**
 * Enable or disable the clock gate for ref_160m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_160m_clk_en(bool enable)
{
    PCR.pll_div_clk_en.pll_160m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_160m_clk_en(...) (void)__DECLARE_RCC_ATOMIC_ENV; _clk_gate_ll_ref_160m_clk_en(__VA_ARGS__)

/**
 * Enable or disable the clock gate for ref_240m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_240m_clk_en(bool enable)
{
    PCR.pll_div_clk_en.pll_240m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_240m_clk_en(...) (void)__DECLARE_RCC_ATOMIC_ENV; _clk_gate_ll_ref_240m_clk_en(__VA_ARGS__)

#ifdef __cplusplus
}
#endif
