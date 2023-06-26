/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/dport_reg.h"
#include "soc/gpio_reg.h"
#include "soc/spi_mem_reg.h"
#include "soc/extmem_reg.h"
#include "soc/regi2c_ulp.h"
#include "hal/efuse_hal.h"
#include "hal/efuse_ll.h"
#include "regi2c_ctrl.h"
#include "esp_hw_log.h"
#ifndef BOOTLOADER_BUILD
#include "esp_private/sar_periph_ctrl.h"
#endif

__attribute__((unused)) static const char *TAG = "rtc_init";

static void set_ocode_by_efuse(int calib_version);
static void calibrate_ocode(void);

void rtc_init(rtc_config_t cfg)
{
    /**
     * When run rtc_init, it maybe deep sleep reset. Since we power down modem in deep sleep, after wakeup
     * from deep sleep, these fields are changed and not reset. We will access two BB regs(BBPD_CTRL and
     * NRXPD_CTRL) in rtc_sleep_pu. If PD modem and no iso, CPU will stuck when access these two BB regs
     * and finally triggle RTC WDT. So need to clear modem Force PD.
     *
     * No worry about the power consumption, Because modem Force PD will be set at the end of this function.
     */
    CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_WIFI_FORCE_PD);

    CLEAR_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_PVTMON_PU);
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_PLL_BUF_WAIT, cfg.pll_wait);
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_CK8M_WAIT, cfg.ck8m_wait);

    /* Moved from rtc sleep to rtc init to save sleep function running time */
    // set shortest possible sleep time limit
    REG_SET_FIELD(RTC_CNTL_TIMER5_REG, RTC_CNTL_MIN_SLP_VAL, RTC_CNTL_MIN_SLP_VAL_MIN);

    /* This power domian removed
    * set rom&ram timer
    * REG_SET_FIELD(RTC_CNTL_TIMER3_REG, RTC_CNTL_ROM_RAM_POWERUP_TIMER, ROM_RAM_POWERUP_CYCLES);
    * REG_SET_FIELD(RTC_CNTL_TIMER3_REG, RTC_CNTL_ROM_RAM_WAIT_TIMER, ROM_RAM_WAIT_CYCLES);
    */
    // set wifi timer
    rtc_init_config_t rtc_init_cfg = RTC_INIT_CONFIG_DEFAULT();
    REG_SET_FIELD(RTC_CNTL_TIMER3_REG, RTC_CNTL_WIFI_POWERUP_TIMER, rtc_init_cfg.wifi_powerup_cycles);
    REG_SET_FIELD(RTC_CNTL_TIMER3_REG, RTC_CNTL_WIFI_WAIT_TIMER, rtc_init_cfg.wifi_wait_cycles);
    // set rtc peri timer
    REG_SET_FIELD(RTC_CNTL_TIMER4_REG, RTC_CNTL_POWERUP_TIMER, rtc_init_cfg.rtc_powerup_cycles);
    REG_SET_FIELD(RTC_CNTL_TIMER4_REG, RTC_CNTL_WAIT_TIMER, rtc_init_cfg.rtc_wait_cycles);
    // set digital wrap timer
    REG_SET_FIELD(RTC_CNTL_TIMER4_REG, RTC_CNTL_DG_WRAP_POWERUP_TIMER, rtc_init_cfg.dg_wrap_powerup_cycles);
    REG_SET_FIELD(RTC_CNTL_TIMER4_REG, RTC_CNTL_DG_WRAP_WAIT_TIMER, rtc_init_cfg.dg_wrap_wait_cycles);
    // set rtc memory timer
    REG_SET_FIELD(RTC_CNTL_TIMER5_REG, RTC_CNTL_RTCMEM_POWERUP_TIMER, rtc_init_cfg.rtc_mem_powerup_cycles);
    REG_SET_FIELD(RTC_CNTL_TIMER5_REG, RTC_CNTL_RTCMEM_WAIT_TIMER, rtc_init_cfg.rtc_mem_wait_cycles);

    SET_PERI_REG_MASK(RTC_CNTL_BIAS_CONF_REG,
                      RTC_CNTL_DEC_HEARTBEAT_WIDTH | RTC_CNTL_INC_HEARTBEAT_PERIOD);

    /* Reset RTC bias to default value (needed if waking up from deep sleep) */
    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DBIAS_WAK, RTC_CNTL_DBIAS_1V10);
    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DBIAS_SLP, RTC_CNTL_DBIAS_1V10);
    /* Set the wait time to the default value. */
    REG_SET_FIELD(RTC_CNTL_TIMER2_REG, RTC_CNTL_ULPCP_TOUCH_START_WAIT, RTC_CNTL_ULPCP_TOUCH_START_WAIT_DEFAULT);

    if (cfg.clkctl_init) {
        //clear CMMU clock force on
        CLEAR_PERI_REG_MASK(EXTMEM_PRO_CACHE_MMU_POWER_CTRL_REG, EXTMEM_PRO_CACHE_MMU_MEM_FORCE_ON);
        //clear rom clock force on
        REG_SET_FIELD(DPORT_ROM_CTRL_0_REG, DPORT_ROM_FO, 0);
        //clear sram clock force on
        REG_SET_FIELD(DPORT_SRAM_CTRL_0_REG, DPORT_SRAM_FO, 0);
        //clear tag clock force on
        CLEAR_PERI_REG_MASK(EXTMEM_PRO_DCACHE_TAG_POWER_CTRL_REG, EXTMEM_PRO_DCACHE_TAG_MEM_FORCE_ON);
        CLEAR_PERI_REG_MASK(EXTMEM_PRO_ICACHE_TAG_POWER_CTRL_REG, EXTMEM_PRO_ICACHE_TAG_MEM_FORCE_ON);
        //clear register clock force on
        CLEAR_PERI_REG_MASK(SPI_MEM_CLOCK_GATE_REG(0), SPI_MEM_CLK_EN);
        CLEAR_PERI_REG_MASK(SPI_MEM_CLOCK_GATE_REG(1), SPI_MEM_CLK_EN);
    }
    if (cfg.pwrctl_init) {
        CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_FORCE_PU);
        //cancel xtal force pu if no need to force power up
        //cannot cancel xtal force pu if pll is force power on
        if (!(cfg.xtal_fpu | cfg.bbpll_fpu)) {
            CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_XTL_FORCE_PU);
        } else {
            SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_XTL_FORCE_PU);
        }
        // CLEAR APLL close
        CLEAR_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_PLLA_FORCE_PU);
        SET_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_PLLA_FORCE_PD);

        //cancel bbpll force pu if setting no force power up
        if (!cfg.bbpll_fpu) {
            CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BBPLL_FORCE_PU);
            CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BBPLL_I2C_FORCE_PU);
            CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BB_I2C_FORCE_PU);
        } else {
            SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BBPLL_FORCE_PU);
            SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BBPLL_I2C_FORCE_PU);
            SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BB_I2C_FORCE_PU);
        }
        //cancel RTC REG force PU
        CLEAR_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_REG, RTC_CNTL_REGULATOR_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_REG, RTC_CNTL_DBOOST_FORCE_PU);

        //combine two rtc memory options
        CLEAR_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_MEM_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_MEM_FORCE_NOISO);

        if (cfg.rtc_dboost_fpd) {
            SET_PERI_REG_MASK(RTC_CNTL_REG, RTC_CNTL_DBOOST_FORCE_PD);
        } else {
            CLEAR_PERI_REG_MASK(RTC_CNTL_REG, RTC_CNTL_DBOOST_FORCE_PD);
        }
        //cancel sar i2c pd force
        CLEAR_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG,RTC_CNTL_SAR_I2C_FORCE_PD);
        //cancel digital pu force
        CLEAR_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_MEM_FORCE_PU);

        /* If this mask is enabled, all soc memories cannot enter power down mode */
        /* We should control soc memory power down mode from RTC, so we will not touch this register any more */
        CLEAR_PERI_REG_MASK(DPORT_MEM_PD_MASK_REG, DPORT_LSLP_MEM_PD_MASK);
        /* If this pd_cfg is set to 1, all memory won't enter low power mode during light sleep */
        /* If this pd_cfg is set to 0, all memory will enter low power mode during light sleep */
        rtc_sleep_pd_config_t pd_cfg = RTC_SLEEP_PD_CONFIG_ALL(0);
        rtc_sleep_pd(pd_cfg);

        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_DG_WRAP_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_WIFI_FORCE_PU);
        // ROM_RAM power domain is removed
        // CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_CPU_ROM_RAM_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_WRAP_FORCE_NOISO);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_WIFI_FORCE_NOISO);
        // CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_CPU_ROM_RAM_FORCE_NOISO);
        CLEAR_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_FORCE_NOISO);
        //cancel digital PADS force no iso
        if (cfg.cpu_waiti_clk_gate) {
            CLEAR_PERI_REG_MASK(DPORT_CPU_PER_CONF_REG, DPORT_CPU_WAIT_MODE_FORCE_ON);
        } else {
            SET_PERI_REG_MASK(DPORT_CPU_PER_CONF_REG, DPORT_CPU_WAIT_MODE_FORCE_ON);
        }
        /*if DPORT_CPU_WAIT_MODE_FORCE_ON == 0 , the cpu clk will be closed when cpu enter WAITI mode*/

        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_PAD_FORCE_UNHOLD);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_PAD_FORCE_NOISO);
    }
    /* force power down wifi and bt power domain */
    SET_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_WIFI_FORCE_ISO);
    SET_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_WIFI_FORCE_PD);

#if !CONFIG_IDF_ENV_FPGA
    if (cfg.cali_ocode) {
        uint32_t rtc_calib_version = efuse_ll_get_blk_version_minor(); // IDF-5366
        if (rtc_calib_version == 2) {
            set_ocode_by_efuse(rtc_calib_version);
        } else {
            calibrate_ocode();
        }
    }
#endif // !CONFIG_IDF_ENV_FPGA

    REG_WRITE(RTC_CNTL_INT_ENA_REG, 0);
    REG_WRITE(RTC_CNTL_INT_CLR_REG, UINT32_MAX);

#ifndef BOOTLOADER_BUILD
    //initialise SAR related peripheral register settings
    sar_periph_ctrl_init();
#endif
}

rtc_vddsdio_config_t rtc_vddsdio_get_config(void)
{
    rtc_vddsdio_config_t result;
    uint32_t sdio_conf_reg = REG_READ(RTC_CNTL_SDIO_CONF_REG);
    result.drefh = (sdio_conf_reg & RTC_CNTL_DREFH_SDIO_M) >> RTC_CNTL_DREFH_SDIO_S;
    result.drefm = (sdio_conf_reg & RTC_CNTL_DREFM_SDIO_M) >> RTC_CNTL_DREFM_SDIO_S;
    result.drefl = (sdio_conf_reg & RTC_CNTL_DREFL_SDIO_M) >> RTC_CNTL_DREFL_SDIO_S;
    if (sdio_conf_reg & RTC_CNTL_SDIO_FORCE) {
        // Get configuration from RTC
        result.force = 1;
        result.enable = (sdio_conf_reg & RTC_CNTL_XPD_SDIO_REG_M) >> RTC_CNTL_XPD_SDIO_REG_S;
        result.tieh = (sdio_conf_reg & RTC_CNTL_SDIO_TIEH_M) >> RTC_CNTL_SDIO_TIEH_S;
        return result;
    } else {
        result.force = 0;
    }
#if 0 // ToDo: re-enable the commented codes
    if (efuse_ll_get_sdio_force()) {
        result.enable = efuse_ll_get_sdio_xpd();
        result.tieh = efuse_ll_get_sdio_tieh();
        result.drefm = efuse_ll_get_sdio_drefm();
        result.drefl = efuse_ll_get_sdio_drefl();
        result.drefh = efuse_ll_get_sdio_drefh();
        return result;
    }
#endif
    // Otherwise, VDD_SDIO is controlled by bootstrapping pin
    uint32_t strap_reg = REG_READ(GPIO_STRAP_REG);
    result.tieh = (strap_reg & BIT(5)) ? RTC_VDDSDIO_TIEH_1_8V : RTC_VDDSDIO_TIEH_3_3V;
    result.enable = 1;
    return result;
}

void rtc_vddsdio_set_config(rtc_vddsdio_config_t config)
{
    uint32_t val = 0;
    val |= (config.force << RTC_CNTL_SDIO_FORCE_S);
    val |= (config.enable << RTC_CNTL_XPD_SDIO_REG_S);
    val |= (config.drefh << RTC_CNTL_DREFH_SDIO_S);
    val |= (config.drefm << RTC_CNTL_DREFM_SDIO_S);
    val |= (config.drefl << RTC_CNTL_DREFL_SDIO_S);
    val |= (config.tieh << RTC_CNTL_SDIO_TIEH_S);
    val |= RTC_CNTL_SDIO_PD_EN;
    REG_WRITE(RTC_CNTL_SDIO_CONF_REG, val);
}

static void set_ocode_by_efuse(int calib_version)
{
    assert(calib_version == 2);
    uint32_t ocode = efuse_ll_get_ocode();
    if (ocode >> 6) {
        ocode = 93 - (ocode ^ (1 << 6));
    } else {
        ocode = 93 + ocode;
    }
    REGI2C_WRITE_MASK(I2C_ULP, I2C_ULP_EXT_CODE, ocode);
    REGI2C_WRITE_MASK(I2C_ULP, I2C_ULP_IR_FORCE_CODE, 1);
}

/**
 * TODO IDF-4141, this seems influence flash,
 */
static void calibrate_ocode(void)
{
    /*
    Bandgap output voltage is not precise when calibrate o-code by hardware sometimes, so need software o-code calibration (must turn off PLL).
    Method:
    1. read current cpu config, save in old_config;
    2. switch cpu to xtal because PLL will be closed when o-code calibration;
    3. begin o-code calibration;
    4. wait o-code calibration done flag(odone_flag & bg_odone_flag) or timeout;
    5. set cpu to old-config.
    */
    soc_rtc_slow_clk_src_t slow_clk_src = rtc_clk_slow_src_get();
    rtc_cal_sel_t cal_clk = RTC_CAL_RTC_MUX;
    if (slow_clk_src == SOC_RTC_SLOW_CLK_SRC_XTAL32K) {
        cal_clk = RTC_CAL_32K_XTAL;
    } else if (slow_clk_src == SOC_RTC_SLOW_CLK_SRC_RC_FAST_D256) {
        cal_clk  = RTC_CAL_8MD256;
    }

    uint64_t max_delay_time_us = 10000;
    uint32_t slow_clk_period = rtc_clk_cal(cal_clk, 100);
    uint64_t max_delay_cycle = rtc_time_us_to_slowclk(max_delay_time_us, slow_clk_period);
    uint64_t cycle0 = rtc_time_get();
    uint64_t timeout_cycle = cycle0 + max_delay_cycle;
    uint64_t cycle1 = 0;

    rtc_cpu_freq_config_t old_config;
    rtc_clk_cpu_freq_get_config(&old_config);
    rtc_clk_cpu_freq_set_xtal();

    REGI2C_WRITE_MASK(I2C_ULP, I2C_ULP_IR_RESETB, 0);
    REGI2C_WRITE_MASK(I2C_ULP, I2C_ULP_IR_RESETB, 1);
    bool odone_flag = 0;
    bool bg_odone_flag = 0;
    while(1) {
        odone_flag = REGI2C_READ_MASK(I2C_ULP, I2C_ULP_O_DONE_FLAG);
        bg_odone_flag = REGI2C_READ_MASK(I2C_ULP, I2C_ULP_BG_O_DONE_FLAG);
        cycle1 = rtc_time_get();
        if (odone_flag && bg_odone_flag)
            break;
        if (cycle1 >= timeout_cycle) {
            ESP_HW_LOGW(TAG, "o_code calibration fail");
            break;
        }
    }
    rtc_clk_cpu_freq_set_config(&old_config);
}
