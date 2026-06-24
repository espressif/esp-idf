/*
 * SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "clk_ctrl_os.h"
#include "esp_attr.h"
#include "esp_private/esp_clk_tree_common.h"
#include "soc/clk_tree_defs.h"

bool periph_rtc_dig_clk8m_enable(void)
{
    return esp_clk_tree_enable_src(SOC_MOD_CLK_RC_FAST, true) == ESP_OK;
}

uint32_t periph_rtc_dig_clk8m_get_freq(void)
{
#if SOC_CLK_RC_FAST_SUPPORT_CALIBRATION
    return esp_clk_tree_rc_fast_get_freq_hz(ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT);
#else
    return SOC_CLK_RC_FAST_FREQ_APPROX;
#endif
}

void periph_rtc_dig_clk8m_disable(void)
{
    (void)esp_clk_tree_enable_src(SOC_MOD_CLK_RC_FAST, false);
}

#if SOC_CLK_APLL_SUPPORTED
void periph_rtc_apll_acquire(void)
{
    (void)esp_clk_tree_enable_src(SOC_MOD_CLK_APLL, true);
}

void periph_rtc_apll_release(void)
{
    (void)esp_clk_tree_enable_src(SOC_MOD_CLK_APLL, false);
}

esp_err_t periph_rtc_apll_freq_set(uint32_t expt_freq_hz, uint32_t *real_freq_hz)
{
    return esp_clk_tree_src_set_freq_hz(SOC_MOD_CLK_APLL, expt_freq_hz, real_freq_hz);
}
#endif // SOC_CLK_APLL_SUPPORTED

#if SOC_CLK_MPLL_SUPPORTED
esp_err_t IRAM_ATTR periph_rtc_mpll_acquire(void)
{
    // For IRAM compatibility, we do not use esp_clk_tree_enable_src here
    return esp_clk_tree_mpll_acquire();
}

void periph_rtc_mpll_release(void)
{
    // For IRAM compatibility, we do not use esp_clk_tree_enable_src here
    (void)esp_clk_tree_mpll_release();
}

esp_err_t IRAM_ATTR periph_rtc_mpll_freq_set(uint32_t expt_freq_hz, uint32_t *real_freq_hz)
{
    // For IRAM compatibility, we do not use esp_clk_tree_src_set_freq_hz here
    return esp_clk_tree_mpll_freq_set(expt_freq_hz, real_freq_hz);
}
#endif // SOC_CLK_MPLL_SUPPORTED
