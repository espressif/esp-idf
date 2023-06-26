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

static void set_ocode_by_efuse(int ocode_scheme_ver);
static void calibrate_ocode(void);
static void set_rtc_dig_dbias(void);

void rtc_init(rtc_config_t cfg)
{
    REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_XPD_DIG_REG, 0);
    REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_XPD_RTC_REG, 0);

    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_PLL_BUF_WAIT, cfg.pll_wait);
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_CK8M_WAIT, cfg.ck8m_wait);
    REG_SET_FIELD(RTC_CNTL_TIMER5_REG, RTC_CNTL_MIN_SLP_VAL, RTC_CNTL_MIN_SLP_VAL_MIN);

    if (cfg.cali_ocode) {
        uint8_t blk_version_minor = efuse_ll_get_blk_version_minor();
        uint8_t blk_version_major = efuse_ll_get_blk_version_major();
        bool ignore_major = efuse_ll_get_disable_blk_version_major();

        uint8_t ocode_scheme_ver = 0;
        if(blk_version_major > 0 && !ignore_major) {
            ESP_HW_LOGE(TAG, "Invalid blk_version_major.\n");
            abort();
        }
        if((blk_version_major > 0) || (blk_version_major == 0 && blk_version_minor >= 1)) {
            ocode_scheme_ver = 1;
        }

        if (ocode_scheme_ver == 1) {
            set_ocode_by_efuse(ocode_scheme_ver);
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
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_WRAP_FORCE_NOISO);

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
    REG_WRITE(RTC_CNTL_INT_ENA_REG, 0);
    REG_WRITE(RTC_CNTL_INT_CLR_REG, UINT32_MAX);
    REGI2C_WRITE_MASK(I2C_ULP, I2C_ULP_IR_FORCE_XPD_CK, 1);

#ifndef BOOTLOADER_BUILD
    //initialise SAR related peripheral register settings
    sar_periph_ctrl_init();
#endif
}

static void set_ocode_by_efuse(int ocode_scheme_ver)
{
    assert(ocode_scheme_ver == 1);
    signed int ocode = efuse_ll_get_ocode();

    //recover efuse data
    ocode = ((ocode & BIT(6)) != 0)? -(ocode & 0x3f): ocode;
    ocode = ocode + 100;

    //set ext_ocode
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
    if (slow_clk_src == SOC_RTC_SLOW_CLK_SRC_OSC_SLOW) {
        cal_clk = RTC_CAL_32K_OSC_SLOW;
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

static uint32_t get_dig_dbias_by_efuse(uint8_t dbias_scheme_ver)
{
    assert(dbias_scheme_ver == 1);
    return efuse_ll_get_dig_dbias_hvt();
}

uint32_t get_rtc_dbias_by_efuse(uint8_t dbias_scheme_ver, uint32_t dig_dbias)
{
    assert(dbias_scheme_ver == 1);
    uint32_t rtc_dbias = 31;

    //read efuse data
    signed int dig_slp_dbias2 = efuse_ll_get_dig_ldo_slp_dbias2();
    signed int dig_slp_dbias26 = efuse_ll_get_dig_ldo_slp_dbias26();
    signed int dig_act_dbias26 = efuse_ll_get_dig_ldo_act_dbias26();
    signed int dig_act_step = efuse_ll_get_dig_ldo_act_stepd10();
    signed int rtc_slp_dbias29 = efuse_ll_get_rtc_ldo_slp_dbias29();
    signed int rtc_slp_dbias31 = efuse_ll_get_rtc_ldo_slp_dbias31();
    signed int rtc_act_dbias31 = efuse_ll_get_rtc_ldo_act_dbias31();
    signed int rtc_act_dbias13 = efuse_ll_get_rtc_ldo_act_dbias13();

    //recover dig&rtc parameter
    dig_slp_dbias2 = ((dig_slp_dbias2 & BIT(6)) != 0)? -(dig_slp_dbias2 & 0x3f): dig_slp_dbias2;
    dig_slp_dbias26 = ((dig_slp_dbias26 & BIT(7)) != 0)? -(dig_slp_dbias26 & 0x7f): dig_slp_dbias26;
    dig_act_dbias26 = ((dig_act_dbias26 & BIT(5)) != 0)? -(dig_act_dbias26 & 0x1f): dig_act_dbias26;
    dig_act_step = ((dig_act_step & BIT(3)) != 0)? -(dig_act_step & 0x7): dig_act_step;
    rtc_slp_dbias29 = ((rtc_slp_dbias29 & BIT(8)) != 0)? -(rtc_slp_dbias29 & 0xff): rtc_slp_dbias29;
    rtc_slp_dbias31 = ((rtc_slp_dbias31 & BIT(5)) != 0)? -(rtc_slp_dbias31 & 0x1f): rtc_slp_dbias31;
    rtc_act_dbias31 = ((rtc_act_dbias31 & BIT(5)) != 0)? -(rtc_act_dbias31 & 0x1f): rtc_act_dbias31;
    rtc_act_dbias13 = ((rtc_act_dbias13 & BIT(7)) != 0)? -(rtc_act_dbias13 & 0x7f): rtc_act_dbias13;

    dig_slp_dbias2 = dig_slp_dbias2 + 705;
    dig_slp_dbias26 = dig_slp_dbias26 + dig_slp_dbias2 + 502;
    dig_act_dbias26 = dig_act_dbias26 + dig_slp_dbias26 + 10;
    signed int dig_slp_dbias9 = dig_slp_dbias26 - (dig_slp_dbias26 - dig_slp_dbias2) * 17 / 24;
    signed int dig_act_dbias9 = dig_slp_dbias9 + (dig_act_dbias26 - dig_slp_dbias26) - dig_act_step * 17 / 10;

    rtc_slp_dbias29 = rtc_slp_dbias29 + 1160;
    rtc_slp_dbias31 = rtc_slp_dbias31 + rtc_slp_dbias29 + 37;
    rtc_act_dbias31 = rtc_act_dbias31 + rtc_slp_dbias31 + 8;
    rtc_act_dbias13 = rtc_act_dbias13 + 860;

    //calculate digital LDO volt
    signed int dig_k_act = (dig_act_dbias26 - dig_act_dbias9) / 17;
    signed int dig_b_act = dig_act_dbias26 - dig_k_act * 26;
    uint32_t v_dig_cal = dig_k_act * dig_dbias + dig_b_act;

    //calculate rtc_dbias with dig_volt
    signed int rtc_k_act = (rtc_act_dbias31 - rtc_act_dbias13) / 18;
    signed int rtc_b_act = rtc_act_dbias31 - rtc_k_act * 31;

    uint32_t v_rtc_cal = 0;
    for (rtc_dbias = 15; rtc_dbias < 32; rtc_dbias++) {
        v_rtc_cal = rtc_k_act * rtc_dbias + rtc_b_act;
        if (v_rtc_cal >= v_dig_cal) {
            return rtc_dbias;
        }
    }

    //can't find correct rtc-volt, rtc_dbias can use default value.
    rtc_dbias = 31;
    return rtc_dbias;
}

static void set_rtc_dig_dbias()
{
    /*
    1. a reasonable dig_dbias which by scaning pvt to make 120 CPU run successful stored in efuse;
    2. a reasonable rtc_dbias can be calculated by a certion formula.
    */
    uint32_t rtc_dbias = 31, dig_dbias = 26;
    uint8_t blk_version_minor = efuse_ll_get_blk_version_minor();
    uint8_t blk_version_major = efuse_ll_get_blk_version_major();
    bool ignore_major = efuse_ll_get_disable_blk_version_major();

    uint8_t dbias_scheme_ver = 0;
    if(blk_version_major > 0 && !ignore_major) {
        ESP_HW_LOGE(TAG, "Invalid blk_version_major.\n");
        abort();
    }
    if((blk_version_major > 0) || (blk_version_major == 0 && blk_version_minor >= 1)) {
        dbias_scheme_ver = 1;
    }

    if (dbias_scheme_ver == 1) {
        dig_dbias = get_dig_dbias_by_efuse(dbias_scheme_ver);
        if (dig_dbias != 0) {
            rtc_dbias = get_rtc_dbias_by_efuse(dbias_scheme_ver, dig_dbias); // already burn dig_dbias in efuse
        } else {
            dig_dbias = 26;
            ESP_HW_LOGD(TAG, "not burn core voltage in efuse or burn wrong voltage value. blk_ver: %d.%d\n", blk_version_major, blk_version_minor);
        }
    }
    else {
        ESP_HW_LOGD(TAG, "core voltage not burnt in efuse. blk_ver: %d.%d\n", blk_version_major, blk_version_minor);
    }
    REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_EXT_RTC_DREG, rtc_dbias);
    REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_EXT_DIG_DREG, dig_dbias);
}
