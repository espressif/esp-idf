/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "sdkconfig.h"
#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/gpio_reg.h"
#include "soc/spi_mem_reg.h"
#include "soc/extmem_reg.h"
#include "soc/system_reg.h"
#include "hal/efuse_hal.h"
#include "hal/efuse_ll.h"
#include "regi2c_ctrl.h"
#include "soc/regi2c_dig_reg.h"
#include "soc/regi2c_lp_bias.h"
#include "esp_hw_log.h"
#ifndef BOOTLOADER_BUILD
#include "esp_private/sar_periph_ctrl.h"
#endif

static const char *TAG = "rtc_init";

static void set_ocode_by_efuse(int calib_version);
static void calibrate_ocode(void);
static void set_rtc_dig_dbias(void);

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
    CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_WIFI_FORCE_ISO);

    REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_XPD_DIG_REG, 0);
    REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_XPD_RTC_REG, 0);

    CLEAR_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_PVTMON_PU);
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_PLL_BUF_WAIT, cfg.pll_wait);
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_CK8M_WAIT, cfg.ck8m_wait);
    REG_SET_FIELD(RTC_CNTL_TIMER5_REG, RTC_CNTL_MIN_SLP_VAL, RTC_CNTL_MIN_SLP_VAL_MIN);

    // set default powerup & wait time
    rtc_init_config_t rtc_init_cfg = RTC_INIT_CONFIG_DEFAULT();
    REG_SET_FIELD(RTC_CNTL_TIMER3_REG, RTC_CNTL_WIFI_POWERUP_TIMER, rtc_init_cfg.wifi_powerup_cycles);
    REG_SET_FIELD(RTC_CNTL_TIMER3_REG, RTC_CNTL_WIFI_WAIT_TIMER, rtc_init_cfg.wifi_wait_cycles);
    REG_SET_FIELD(RTC_CNTL_TIMER3_REG, RTC_CNTL_BT_POWERUP_TIMER, rtc_init_cfg.bt_powerup_cycles);
    REG_SET_FIELD(RTC_CNTL_TIMER3_REG, RTC_CNTL_BT_WAIT_TIMER, rtc_init_cfg.bt_wait_cycles);
    REG_SET_FIELD(RTC_CNTL_TIMER4_REG, RTC_CNTL_CPU_TOP_POWERUP_TIMER, rtc_init_cfg.cpu_top_powerup_cycles);
    REG_SET_FIELD(RTC_CNTL_TIMER4_REG, RTC_CNTL_CPU_TOP_WAIT_TIMER, rtc_init_cfg.cpu_top_wait_cycles);
    REG_SET_FIELD(RTC_CNTL_TIMER4_REG, RTC_CNTL_DG_WRAP_POWERUP_TIMER, rtc_init_cfg.dg_wrap_powerup_cycles);
    REG_SET_FIELD(RTC_CNTL_TIMER4_REG, RTC_CNTL_DG_WRAP_WAIT_TIMER, rtc_init_cfg.dg_wrap_wait_cycles);
    REG_SET_FIELD(RTC_CNTL_TIMER6_REG, RTC_CNTL_DG_PERI_POWERUP_TIMER, rtc_init_cfg.dg_peri_powerup_cycles);
    REG_SET_FIELD(RTC_CNTL_TIMER6_REG, RTC_CNTL_DG_PERI_WAIT_TIMER, rtc_init_cfg.dg_peri_wait_cycles);

    if (cfg.cali_ocode) {
        uint32_t rtc_calib_version = efuse_ll_get_blk_version_major(); // IDF-5366
        if (rtc_calib_version == 1) {
            set_ocode_by_efuse(rtc_calib_version);
        } else {
            calibrate_ocode();
        }
    }

    set_rtc_dig_dbias();

    if (cfg.clkctl_init) {
        //clear CMMU clock force on
        CLEAR_PERI_REG_MASK(EXTMEM_CACHE_MMU_POWER_CTRL_REG, EXTMEM_CACHE_MMU_MEM_FORCE_ON);
        //clear tag clock force on
        CLEAR_PERI_REG_MASK(EXTMEM_ICACHE_TAG_POWER_CTRL_REG, EXTMEM_ICACHE_TAG_MEM_FORCE_ON);
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
        // force pd APLL
        CLEAR_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_PLLA_FORCE_PU);
        SET_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_PLLA_FORCE_PD);

        //open sar_i2c protect function to avoid sar_i2c reset when rtc_ldo is low.
        CLEAR_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_I2C_RESET_POR_FORCE_PD);

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

        CLEAR_PERI_REG_MASK(RTC_CNTL_REG, RTC_CNTL_REGULATOR_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_REG, RTC_CNTL_DBOOST_FORCE_PU);

        if (cfg.rtc_dboost_fpd) {
            SET_PERI_REG_MASK(RTC_CNTL_REG, RTC_CNTL_DBOOST_FORCE_PD);
        } else {
            CLEAR_PERI_REG_MASK(RTC_CNTL_REG, RTC_CNTL_DBOOST_FORCE_PD);
        }

        //clear i2c_reset_protect pd force, need tested in low temperature.
        //CLEAR_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG,RTC_CNTL_I2C_RESET_POR_FORCE_PD);

        /* If this mask is enabled, all soc memories cannot enter power down mode */
        /* We should control soc memory power down mode from RTC, so we will not touch this register any more */
        CLEAR_PERI_REG_MASK(SYSTEM_MEM_PD_MASK_REG, SYSTEM_LSLP_MEM_PD_MASK);

        /* If this pd_cfg is set to 1, all memory won't enter low power mode during light sleep */
        /* If this pd_cfg is set to 0, all memory will enter low power mode during light sleep */
        rtc_sleep_pu_config_t pu_cfg = RTC_SLEEP_PU_CONFIG_ALL(0);
        rtc_sleep_pu(pu_cfg);

        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_DG_WRAP_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_WIFI_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_BT_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_CPU_TOP_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_DG_PERI_FORCE_PU);

        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_WRAP_FORCE_NOISO);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_WIFI_FORCE_NOISO);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_BT_FORCE_NOISO);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_CPU_TOP_FORCE_NOISO);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_PERI_FORCE_NOISO);
        //cancel digital PADS force no iso
        if (cfg.cpu_waiti_clk_gate) {
            CLEAR_PERI_REG_MASK(SYSTEM_CPU_PER_CONF_REG, SYSTEM_CPU_WAIT_MODE_FORCE_ON);
        } else {
            SET_PERI_REG_MASK(SYSTEM_CPU_PER_CONF_REG, SYSTEM_CPU_WAIT_MODE_FORCE_ON);
        }
        /*if SYSTEM_CPU_WAIT_MODE_FORCE_ON == 0 , the cpu clk will be closed when cpu enter WAITI mode*/
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_PAD_FORCE_UNHOLD);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_PAD_FORCE_NOISO);
    }
    /* force power down wifi and bt power domain */
    SET_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_WIFI_FORCE_ISO);
    SET_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_WIFI_FORCE_PD);
    /* force power down bt power domain */
    SET_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_BT_FORCE_ISO);
    SET_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_BT_FORCE_PD);

    REG_WRITE(RTC_CNTL_INT_ENA_REG, 0);
    REG_WRITE(RTC_CNTL_INT_CLR_REG, UINT32_MAX);
    REGI2C_WRITE_MASK(I2C_ULP, I2C_ULP_IR_FORCE_XPD_CK, 1);

#ifndef BOOTLOADER_BUILD
    //initialise SAR related peripheral register settings
    sar_periph_ctrl_init();
#endif
}


static void set_ocode_by_efuse(int calib_version)
{
    assert(calib_version == 1);
    uint32_t ocode = efuse_ll_get_ocode();
    REGI2C_WRITE_MASK(I2C_ULP, I2C_ULP_EXT_CODE, ocode);
    REGI2C_WRITE_MASK(I2C_ULP, I2C_ULP_IR_FORCE_CODE, 1);
}

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
    while (1) {
        odone_flag = REGI2C_READ_MASK(I2C_ULP, I2C_ULP_O_DONE_FLAG);
        bg_odone_flag = REGI2C_READ_MASK(I2C_ULP, I2C_ULP_BG_O_DONE_FLAG);
        cycle1 = rtc_time_get();
        if (odone_flag && bg_odone_flag) {
            break;
        }
        if (cycle1 >= timeout_cycle) {
            ESP_HW_LOGW(TAG, "o_code calibration fail\n");
            break;
        }
    }
    rtc_clk_cpu_freq_set_config(&old_config);
}

static uint32_t get_dig_dbias_by_efuse(uint8_t chip_version)
{
    assert(chip_version >= 3);
    return efuse_ll_get_dig_dbias_hvt();
}

uint32_t get_rtc_dbias_by_efuse(uint8_t chip_version, uint32_t dig_dbias)
{
    assert(chip_version >= 3);
    uint32_t rtc_dbias = 0;
    signed int k_rtc_ldo = efuse_ll_get_k_rtc_ldo();
    signed int k_dig_ldo = efuse_ll_get_k_dig_ldo();
    signed int v_rtc_bias20 = efuse_ll_get_v_rtc_dbias20();
    signed int v_dig_bias20 = efuse_ll_get_v_dig_dbias20();
    k_rtc_ldo =  ((k_rtc_ldo & BIT(6)) != 0)? -(k_rtc_ldo & 0x3f): k_rtc_ldo;
    k_dig_ldo =  ((k_dig_ldo & BIT(6)) != 0)? -(k_dig_ldo & 0x3f): (uint8_t)k_dig_ldo;
    v_rtc_bias20 =  ((v_rtc_bias20 & BIT(7)) != 0)? -(v_rtc_bias20 & 0x7f): (uint8_t)v_rtc_bias20;
    v_dig_bias20 =  ((v_dig_bias20 & BIT(7)) != 0)? -(v_dig_bias20 & 0x7f): (uint8_t)v_dig_bias20;

    uint32_t v_rtc_dbias20_real_mul10000 = V_RTC_MID_MUL10000 + v_rtc_bias20 * 10000 / 500;
    uint32_t v_dig_dbias20_real_mul10000 = V_DIG_MID_MUL10000 + v_dig_bias20 * 10000 / 500;
    signed int k_rtc_ldo_real_mul10000 = K_RTC_MID_MUL10000 + k_rtc_ldo;
    signed int k_dig_ldo_real_mul10000 = K_DIG_MID_MUL10000 + k_dig_ldo;
    uint32_t v_dig_nearest_1v15_mul10000 = v_dig_dbias20_real_mul10000 + k_dig_ldo_real_mul10000 * (dig_dbias - 20);
    uint32_t v_rtc_nearest_1v15_mul10000 = 0;
    for (rtc_dbias = 15; rtc_dbias < 31; rtc_dbias++) {
        v_rtc_nearest_1v15_mul10000 = v_rtc_dbias20_real_mul10000 + k_rtc_ldo_real_mul10000 * (rtc_dbias - 20);
        if (v_rtc_nearest_1v15_mul10000 >= v_dig_nearest_1v15_mul10000 - 250)
            break;
    }
    return rtc_dbias;
}

static void set_rtc_dig_dbias()
{
    /*
    1. a reasonable dig_dbias which by scanning pvt to make 160 CPU run successful stored in efuse;
    2. also we store some value in efuse, include:
        k_rtc_ldo (slope of rtc voltage & rtc_dbias);
        k_dig_ldo (slope of digital voltage & digital_dbias);
        v_rtc_bias20 (rtc voltage when rtc dbais is 20);
        v_dig_bias20 (digital voltage when digital dbais is 20).
    3. a reasonable rtc_dbias can be calculated by a certion formula.
    */
    uint32_t rtc_dbias = 28, dig_dbias = 28;
    unsigned chip_version = efuse_hal_chip_revision();
    if (chip_version >= 3) {
        dig_dbias = get_dig_dbias_by_efuse(chip_version);
        if (dig_dbias != 0) {
            if (dig_dbias + 4 > 31) {
                dig_dbias = 31;
            } else {
                dig_dbias += 4;
            }
            rtc_dbias = get_rtc_dbias_by_efuse(chip_version, dig_dbias); // already burn dig_dbias in efuse
        } else {
            dig_dbias = 28;
            ESP_HW_LOGD(TAG, "not burn core voltage in efuse or burn wrong voltage value in chip version: 0%d\n", chip_version);
        }
    }
    else {
        ESP_HW_LOGD(TAG, "chip_version is less than 3, not burn core voltage in efuse\n");
    }
    REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_EXT_RTC_DREG, rtc_dbias);
    REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_EXT_DIG_DREG, dig_dbias);
}
