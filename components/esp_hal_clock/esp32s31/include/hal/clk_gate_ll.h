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
#include "soc/hp_sys_clkrst_struct.h"
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

/**
 * @brief Set the default clock gate configuration
 */
static inline void periph_ll_clk_gate_set_default(soc_reset_reason_t rst_reason)
{
    if ((rst_reason == RESET_REASON_CHIP_POWER_ON)   || (rst_reason == RESET_REASON_CORE_PMU_PWR_DOWN) ||
            (rst_reason == RESET_REASON_SYS_BROWN_OUT)   || (rst_reason == RESET_REASON_SYS_RWDT) ||
            (rst_reason == RESET_REASON_SYS_SUPER_WDT)   || (rst_reason == RESET_REASON_CORE_SW) ||
            (rst_reason == RESET_REASON_CORE_MWDT0)      || (rst_reason == RESET_REASON_CORE_MWDT1) ||
            (rst_reason == RESET_REASON_CORE_RWDT)       || (rst_reason == RESET_REASON_CORE_PWR_GLITCH) ||
            (rst_reason == RESET_REASON_CORE_EFUSE_CRC) || (rst_reason == RESET_REASON_CORE_USB_JTAG) ||
            (rst_reason == RESET_REASON_CORE_USB_UART)
       ) {
        /*
         * Default lcdcam_lcdcam_ctrl0:
         *   reg_lcdcam_clk_src_sel: 1 (BBPLL 120 MHz path, shared by LCD and CAM)
         *   reg_lcdcam_clk_div_num: 1 (divide by 2 → 60 MHz)
         */
        HP_SYS_CLKRST.lcdcam_lcdcam_ctrl0.reg_lcdcam_clk_src_sel = 1;
        HP_SYS_CLKRST.lcdcam_lcdcam_ctrl0.reg_lcdcam_clk_div_num = 1;
    }
}

#ifdef __cplusplus
}
#endif
