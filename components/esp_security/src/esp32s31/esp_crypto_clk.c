/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_crypto_clk.h"
#include "soc/clk_tree_defs.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "hal/clk_gate_ll.h"
#include "esp_private/esp_clk_tree_common.h"
#if !NON_OS_BUILD
#include "esp_private/critical_section.h"
#endif

#if !NON_OS_BUILD
DEFINE_CRIT_SECTION_LOCK_STATIC(s_crypto_common_clk_mux);
#define CRYPTO_CLK_LOCK()   esp_os_enter_critical_safe(&s_crypto_common_clk_mux)
#define CRYPTO_CLK_UNLOCK() esp_os_exit_critical_safe(&s_crypto_common_clk_mux)
#else
#define CRYPTO_CLK_LOCK()
#define CRYPTO_CLK_UNLOCK()
#endif

static void esp_crypto_pll_f240m_enable(bool enable)
{
#if !NON_OS_BUILD
    esp_clk_tree_enable_src(SOC_MOD_CLK_PLL_F240M, enable);
#else
    /* Bootloader: BBPLL is already on; no esp_clk_tree in NON_OS. */
    _clk_gate_ll_ref_240m_clk_en(enable);
#endif
}

FORCE_INLINE_ATTR void esp_crypto_periph_clk_enable(bool enable)
{
    HP_SYS_CLKRST.crypto_ctrl0.reg_crypto_sys_clk_en = enable;
    HP_SYS_CLKRST.crypto_ctrl0.reg_crypto_sec_clk_en = enable;
}

#if CONFIG_ESP_CRYPTO_CLK_ON_DEMAND

static int s_crypto_common_clk_ref_cnt;

void esp_crypto_common_clk_enable(bool enable)
{
    CRYPTO_CLK_LOCK();
    if (enable) {
        if (s_crypto_common_clk_ref_cnt++ == 0) {
            /* Parent: PLL_F240M (see esp_crypto_clk_init() REG_CRYPTO_CLK_SRC_SEL). */
            esp_crypto_pll_f240m_enable(true);
            esp_crypto_periph_clk_enable(true);
        }
    } else if (s_crypto_common_clk_ref_cnt > 0 && --s_crypto_common_clk_ref_cnt == 0) {
        esp_crypto_periph_clk_enable(false);
        esp_crypto_pll_f240m_enable(false);
    }
    CRYPTO_CLK_UNLOCK();
}

#else /* !CONFIG_ESP_CRYPTO_CLK_ON_DEMAND */

static bool s_crypto_clk_always_on_done;

static void esp_crypto_clk_always_on(void)
{
    CRYPTO_CLK_LOCK();
    if (!s_crypto_clk_always_on_done) {
        esp_crypto_pll_f240m_enable(true);
        esp_crypto_periph_clk_enable(true);
        s_crypto_clk_always_on_done = true;
    }
    CRYPTO_CLK_UNLOCK();
}

void esp_crypto_common_clk_enable(bool enable)
{
    /* Keep clocks always on: enable once, ignore disable. */
    if (enable) {
        esp_crypto_clk_always_on();
    }
}

#endif /* CONFIG_ESP_CRYPTO_CLK_ON_DEMAND */
