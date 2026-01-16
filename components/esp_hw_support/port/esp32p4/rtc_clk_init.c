/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "esp32p4/rom/ets_sys.h"
#include "esp32p4/rom/rtc.h"
#include "soc/rtc.h"
#include "esp_cpu.h"
#include "regi2c_ctrl.h"
#include "soc/regi2c_dig_reg.h"
#include "soc/regi2c_bias.h"
#include "soc/lp_clkrst_reg.h"
#include "soc/lp_system_reg.h"
#include "soc/pmu_reg.h"
#include "soc/chip_revision.h"
#include "esp_hw_log.h"
#include "sdkconfig.h"
#include "esp_rom_serial_output.h"
#include "esp_private/esp_pmu.h"
#include "hal/clk_tree_ll.h"
#include "hal/efuse_hal.h"

ESP_HW_LOG_ATTR_TAG(TAG, "rtc_clk_init");

void rtc_clk_init(rtc_clk_config_t cfg)
{
    rtc_cpu_freq_config_t old_config, new_config;

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

    REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_FORCE_RTC_DREG, 1);
    REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_FORCE_DIG_DREG, 1);
    REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_XPD_RTC_REG, 0);
    REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_XPD_DIG_REG, 0);

    REGI2C_WRITE_MASK(I2C_BIAS, I2C_BIAS_OR_FORCE_XPD_CK, 0);
    REGI2C_WRITE_MASK(I2C_BIAS, I2C_BIAS_OR_FORCE_XPD_REF_OUT_BUF, 0);
    REGI2C_WRITE_MASK(I2C_BIAS, I2C_BIAS_OR_FORCE_XPD_IPH, 0);
    REGI2C_WRITE_MASK(I2C_BIAS, I2C_BIAS_OR_FORCE_XPD_VGATE_BUF, 0);

    uint32_t hp_dbias = get_act_hp_dbias();
    uint32_t lp_dbias = get_act_lp_dbias();
    pmu_ll_hp_set_regulator_xpd(&PMU, PMU_MODE_HP_ACTIVE, true);
    pmu_ll_hp_set_regulator_dbias(&PMU, PMU_MODE_HP_ACTIVE, hp_dbias);
    pmu_ll_lp_set_regulator_dbias(&PMU, PMU_MODE_LP_ACTIVE, lp_dbias);

    uint32_t pvt_hp_dcmvset = GET_PERI_REG_BITS2(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_HP_DBIAS_VOL_V, PMU_HP_DBIAS_VOL_S);
    uint32_t hp_dcmvset =  HP_CALI_ACTIVE_DCM_VSET_DEFAULT;
    if (pvt_hp_dcmvset > hp_dcmvset) {
        hp_dcmvset = pvt_hp_dcmvset;
    }
    // Switch to DCDC
#if CONFIG_ESP32P4_REV_MIN_301
    unsigned chip_version = efuse_hal_chip_revision();
    if (ESP_CHIP_REV_ABOVE(chip_version, 301)) {
        SET_PERI_REG_MASK(PMU_DCM_CTRL_REG, PMU_DCDC_FB_RES_FORCE_PD);
    }
#endif
    pmu_ll_set_dcdc_en(&PMU, true);
    pmu_ll_set_dcdc_switch_force_power_down(&PMU, false);
    pmu_ll_hp_set_dcm_vset(&PMU, PMU_MODE_HP_ACTIVE, hp_dcmvset);
    SET_PERI_REG_MASK(PMU_HP_ACTIVE_HP_REGULATOR0_REG, PMU_DIG_REGULATOR0_DBIAS_SEL); // Hand over control of dbias to pmu
    esp_rom_delay_us(1000);
#if CONFIG_ESP32P4_REV_MIN_301
    if (ESP_CHIP_REV_ABOVE(chip_version, 301)) {
        REG_SET_FIELD(LP_SYSTEM_REG_SYS_CTRL_REG, LP_SYSTEM_REG_LP_FIB_SEL, 0xEF);// lp_fib_sel bit4 set to 0: select dig_fib_reg instead of ana_fib_reg
        CLEAR_PERI_REG_MASK(PMU_DCM_CTRL_REG, PMU_DCDC_FB_RES_FORCE_PD);
        esp_rom_delay_us(10);
    }
#endif
    pmu_ll_hp_set_regulator_xpd(&PMU, PMU_MODE_HP_ACTIVE, false);

    soc_xtal_freq_t xtal_freq = cfg.xtal_freq;
    esp_rom_output_tx_wait_idle(0);
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
    } else if (cfg.slow_clk_src == SOC_RTC_SLOW_CLK_SRC_RC32K) {
       rtc_clk_rc32k_enable(true);
    }
    rtc_clk_8m_enable(need_rc_fast_en);
    rtc_clk_fast_src_set(cfg.fast_clk_src);
    rtc_clk_slow_src_set(cfg.slow_clk_src);
}
