/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "esp32c3/rom/ets_sys.h"
#include "esp32c3/rom/rtc.h"
#include "esp32c3/rom/uart.h"
#include "soc/rtc.h"
#include "soc/rtc_periph.h"
#include "hal/regi2c_ctrl_ll.h"
#include "esp_hw_log.h"
#include "esp_cpu.h"
#include "sdkconfig.h"
#include "esp_rom_uart.h"

static const char *TAG = "rtc_clk_init";

void rtc_clk_init(rtc_clk_config_t cfg)
{
    rtc_cpu_freq_config_t old_config, new_config;

    /* Set tuning parameters for 8M and 150k clocks.
     * Note: this doesn't attempt to set the clocks to precise frequencies.
     * Instead, we calibrate these clocks against XTAL frequency later, when necessary.
     * - SCK_DCAP value controls tuning of 150k clock.
     *   The higher the value of DCAP is, the lower is the frequency.
     * - CK8M_DFREQ value controls tuning of 8M clock.
     *   CLK_8M_DFREQ constant gives the best temperature characteristics.
     */
    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_SCK_DCAP, cfg.slow_clk_dcap);
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DFREQ, cfg.clk_8m_dfreq);

    /* Configure 150k clock division */
    rtc_clk_divider_set(cfg.clk_rtc_clk_div);

    /* Configure 8M clock division */
    rtc_clk_8m_divider_set(cfg.clk_8m_clk_div);

    /* Reset (disable) i2c internal bus for all regi2c registers */
    regi2c_ctrl_ll_i2c_reset(); // TODO: This should be move out from rtc_clk_init
    /* Enable the internal bus used to configure BBPLL */
    regi2c_ctrl_ll_i2c_bbpll_enable(); // TODO: This should be moved to bbpll_set_config

    soc_xtal_freq_t xtal_freq = cfg.xtal_freq;
    esp_rom_output_tx_wait_idle(0);
    rtc_clk_xtal_freq_update(xtal_freq);
    rtc_clk_apb_freq_update(xtal_freq * MHZ);

    /* Set CPU frequency */
    rtc_clk_cpu_freq_get_config(&old_config);
    uint32_t freq_before = old_config.freq_mhz;
    bool res = rtc_clk_cpu_freq_mhz_to_config(cfg.cpu_freq_mhz, &new_config);
    if (!res) {
        ESP_HW_LOGE(TAG, "invalid CPU frequency value");
        abort();
    }
    rtc_clk_cpu_freq_set_config(&new_config);

    /* Re-calculate the ccount to make time calculation correct. */
    esp_cpu_set_cycle_count( (uint64_t)esp_cpu_get_cycle_count() * cfg.cpu_freq_mhz / freq_before );

    /* Slow & fast clocks setup */
    // We will not power off RC_FAST in bootloader stage even if it is not being used as any
    // cpu / rtc_fast / rtc_slow clock sources, this is because RNG always needs it in the bootloader stage.
    bool need_rc_fast_en = true;
    bool need_rc_fast_d256_en = false;
    if (cfg.slow_clk_src == SOC_RTC_SLOW_CLK_SRC_XTAL32K) {
        rtc_clk_32k_enable(true);
    } else if (cfg.slow_clk_src == SOC_RTC_SLOW_CLK_SRC_RC_FAST_D256) {
        need_rc_fast_d256_en = true;
    }
    rtc_clk_8m_enable(need_rc_fast_en, need_rc_fast_d256_en);
    rtc_clk_fast_src_set(cfg.fast_clk_src);
    rtc_clk_slow_src_set(cfg.slow_clk_src);
}
