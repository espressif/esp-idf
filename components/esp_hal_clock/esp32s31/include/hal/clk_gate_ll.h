/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_attr.h"
#include "soc/soc.h"
#include "soc/reset_reasons.h"
#include "soc/lp_clkrst_struct.h"
#include "soc/lp_clkrst_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enable or disable the clock gate for rtc fast to lp periph
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_rtc_fast_to_lp_periph_en(bool enable)
{
    LP_CLKRST.root_clk_conf.fosc_clk_force_on = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_rtc_fast_to_lp_periph_en(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _clk_gate_ll_rtc_fast_to_lp_periph_en(__VA_ARGS__); \
    } while(0)

#ifdef __cplusplus
}
#endif
