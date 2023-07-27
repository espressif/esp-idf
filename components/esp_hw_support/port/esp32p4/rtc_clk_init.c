/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "esp32p4/rom/ets_sys.h"
#include "esp32p4/rom/rtc.h"
#include "esp32p4/rom/uart.h"
#include "soc/rtc.h"
#include "esp_cpu.h"
#include "regi2c_ctrl.h"
#include "soc/lp_clkrst_reg.h"
#include "soc/regi2c_dig_reg.h"
#include "esp_hw_log.h"
#include "sdkconfig.h"
#include "esp_rom_uart.h"
#include "esp_private/esp_pmu.h"
#include "hal/clk_tree_ll.h"

static const char *TAG = "rtc_clk_init";

#if SOC_PMU_SUPPORTED
/**
 * Initialize the ICG map of some modem clock domains in the PMU_ACTIVE state
 *
 * A pre-initialization interface is used to initialize the ICG map of the
 * MODEM_APB, I2C_MST and LP_APB clock domains in the PMU_ACTIVE state, and
 * disable the clock gating of these clock domains in the PMU_ACTIVE state,
 * because the system clock source (PLL) in the system boot up process needs
 * to use the i2c master peripheral.
 *
 * ICG map of all modem clock domains under different power states (PMU_ACTIVE,
 * PMU_MODEM and PMU_SLEEP) will be initialized in esp_perip_clk_init().
 */
static void rtc_clk_modem_clock_domain_active_state_icg_map_preinit(void)
{
    /* Configure modem ICG code in PMU_ACTIVE state */
    pmu_ll_hp_set_icg_modem(&PMU, PMU_MODE_HP_ACTIVE, PMU_HP_ICG_MODEM_CODE_ACTIVE);

    /* Disable clock gating for MODEM_APB, I2C_MST and LP_APB clock domains in PMU_ACTIVE state */
    modem_syscon_ll_set_modem_apb_icg_bitmap(&MODEM_SYSCON, BIT(PMU_HP_ICG_MODEM_CODE_ACTIVE));
    modem_lpcon_ll_set_i2c_master_icg_bitmap(&MODEM_LPCON, BIT(PMU_HP_ICG_MODEM_CODE_ACTIVE));
    modem_lpcon_ll_set_lp_apb_icg_bitmap(&MODEM_LPCON, BIT(PMU_HP_ICG_MODEM_CODE_ACTIVE));

    /* Software trigger force update modem ICG code and ICG switch */
    pmu_ll_imm_update_dig_icg_modem_code(&PMU, true);
    pmu_ll_imm_update_dig_icg_switch(&PMU, true);
}
#endif  //#if SOC_PMU_SUPPORTED

void rtc_clk_init(rtc_clk_config_t cfg)
{
    rtc_cpu_freq_config_t old_config, new_config;

#if SOC_PMU_SUPPORTED
    rtc_clk_modem_clock_domain_active_state_icg_map_preinit();
#endif  //#if SOC_PMU_SUPPORTED

    /* Set tuning parameters for RC_FAST, RC_SLOW, and RC32K clocks.
     * Note: this doesn't attempt to set the clocks to precise frequencies.
     * Instead, we calibrate these clocks against XTAL frequency later, when necessary.
     * - SCK_DCAP value controls tuning of RC_SLOW clock.
     *   The higher the value of DCAP is, the lower is the frequency.
     * - CK8M_DFREQ value controls tuning of RC_FAST clock.
     *   CLK_8M_DFREQ constant gives the best temperature characteristics.
     * - RC32K_DFREQ value controls tuning of RC32K clock.
     */
    REG_SET_FIELD(LP_CLKRST_FOSC_CNTL_REG, LP_CLKRST_FOSC_DFREQ, cfg.clk_8m_dfreq);
    REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_SCK_DCAP, cfg.slow_clk_dcap);
    REG_SET_FIELD(LP_CLKRST_RC32K_CNTL_REG, LP_CLKRST_RC32K_DFREQ, cfg.rc32k_dfreq);

    rtc_xtal_freq_t xtal_freq = cfg.xtal_freq;
    esp_rom_uart_tx_wait_idle(0);
    rtc_clk_xtal_freq_update(xtal_freq);

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
    if (cfg.slow_clk_src == SOC_RTC_SLOW_CLK_SRC_XTAL32K) {
        rtc_clk_32k_enable(true);
    } else if (cfg.slow_clk_src == SOC_RTC_SLOW_CLK_SRC_OSC_SLOW) {
        rtc_clk_32k_enable_external();
    } else if (cfg.slow_clk_src == SOC_RTC_SLOW_CLK_SRC_RC32K) {
       rtc_clk_rc32k_enable(true);
    }
    rtc_clk_8m_enable(need_rc_fast_en);
    rtc_clk_fast_src_set(cfg.fast_clk_src);
    rtc_clk_slow_src_set(cfg.slow_clk_src);
}
