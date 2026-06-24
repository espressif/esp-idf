/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_crypto_clk.h"
#include "soc/clk_tree_defs.h"
#include "soc/hp_sys_clkrst_struct.h"
#if !NON_OS_BUILD
#include "esp_private/critical_section.h"
#endif

#if !NON_OS_BUILD
DEFINE_CRIT_SECTION_LOCK_STATIC(s_crypto_common_clk_mux);
#endif
static int s_crypto_common_clk_ref_cnt;

void esp_crypto_common_clk_enable(bool enable)
{
#if !NON_OS_BUILD
    esp_os_enter_critical_safe(&s_crypto_common_clk_mux);
#endif
    if (enable) {
        if (s_crypto_common_clk_ref_cnt++ == 0) {
            HP_SYS_CLKRST.crypto_ctrl0.reg_crypto_sys_clk_en = 1;
            HP_SYS_CLKRST.crypto_ctrl0.reg_crypto_sec_clk_en = 1;
        }
    } else {
        if (s_crypto_common_clk_ref_cnt > 0 && --s_crypto_common_clk_ref_cnt == 0) {
            HP_SYS_CLKRST.crypto_ctrl0.reg_crypto_sec_clk_en = 0;
            HP_SYS_CLKRST.crypto_ctrl0.reg_crypto_sys_clk_en = 0;
        }
    }
#if !NON_OS_BUILD
    esp_os_exit_critical_safe(&s_crypto_common_clk_mux);
#endif
}
