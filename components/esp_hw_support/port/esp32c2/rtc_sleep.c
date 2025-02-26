/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdlib.h>
#include "esp_attr.h"
#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/syscon_reg.h"
#include "soc/bb_reg.h"
#include "soc/nrx_reg.h"
#include "soc/fe_reg.h"
#include "soc/timer_group_reg.h"
#include "soc/system_reg.h"
#include "esp32c2/rom/ets_sys.h"
#include "esp32c2/rom/rtc.h"
#include "regi2c_ctrl.h"
#include "soc/regi2c_lp_bias.h"
#include "soc/regi2c_dig_reg.h"

static const DRAM_ATTR rtc_sleep_pu_config_t pu_cfg = RTC_SLEEP_PU_CONFIG_ALL(1);

/**
 * Configure whether certain peripherals are powered down in deep sleep
 * @param cfg power down flags as rtc_sleep_pu_config_t structure
 */
void rtc_sleep_pu(rtc_sleep_pu_config_t cfg)
{
    REG_SET_FIELD(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_LSLP_MEM_FORCE_PU, cfg.dig_fpu);
    REG_SET_FIELD(SYSCON_FRONT_END_MEM_PD_REG, SYSCON_DC_MEM_FORCE_PU, cfg.fe_fpu);
    REG_SET_FIELD(SYSCON_FRONT_END_MEM_PD_REG, SYSCON_PBUS_MEM_FORCE_PU, cfg.fe_fpu);
    REG_SET_FIELD(SYSCON_FRONT_END_MEM_PD_REG, SYSCON_AGC_MEM_FORCE_PU, cfg.fe_fpu);
    REG_SET_FIELD(BBPD_CTRL, BB_FFT_FORCE_PU, cfg.bb_fpu);
    REG_SET_FIELD(BBPD_CTRL, BB_DC_EST_FORCE_PU, cfg.bb_fpu);
    REG_SET_FIELD(NRXPD_CTRL, NRX_RX_ROT_FORCE_PU, cfg.nrx_fpu);
    REG_SET_FIELD(NRXPD_CTRL, NRX_VIT_FORCE_PU, cfg.nrx_fpu);
    REG_SET_FIELD(NRXPD_CTRL, NRX_DEMAP_FORCE_PU, cfg.nrx_fpu);
    REG_SET_FIELD(FE_GEN_CTRL, FE_IQ_EST_FORCE_PU, cfg.fe_fpu);
    REG_SET_FIELD(FE2_TX_INTERP_CTRL, FE2_TX_INF_FORCE_PU, cfg.fe_fpu);
    if (cfg.sram_fpu) {
        REG_SET_FIELD(SYSCON_MEM_POWER_UP_REG, SYSCON_SRAM_POWER_UP, SYSCON_SRAM_POWER_UP);
    } else {
        REG_SET_FIELD(SYSCON_MEM_POWER_UP_REG, SYSCON_SRAM_POWER_UP, 0);
    }
    if (cfg.rom_ram_fpu) {
        REG_SET_FIELD(SYSCON_MEM_POWER_UP_REG, SYSCON_ROM_POWER_UP, SYSCON_ROM_POWER_UP);
    } else {
        REG_SET_FIELD(SYSCON_MEM_POWER_UP_REG, SYSCON_ROM_POWER_UP, 0);
    }
}

void rtc_sleep_get_default_config(uint32_t sleep_flags, rtc_sleep_config_t *out_config)
{
    *out_config = (rtc_sleep_config_t) {
        .lslp_mem_inf_fpu = 1,
        .int_8m_pd_en = ((sleep_flags) & RTC_SLEEP_PD_INT_8M) ? 1 : 0,
        .deep_slp = ((sleep_flags) & RTC_SLEEP_PD_DIG) ? 1 : 0,
        .wdt_flashboot_mod_en = 0,
        .vddsdio_pd_en = ((sleep_flags) & RTC_SLEEP_PD_VDDSDIO) ? 1 : 0,
        .xtal_fpu = ((sleep_flags) & RTC_SLEEP_PD_XTAL) ? 0 : 1,
        .deep_slp_reject = 1,
        .light_slp_reject = 1,
    };
    if (sleep_flags & RTC_SLEEP_PD_DIG) {
        assert(sleep_flags & RTC_SLEEP_PD_XTAL);
        if (!(sleep_flags & RTC_SLEEP_PD_INT_8M)) {
            /*
             * dbg_att_slp need to set to 0: rtc voltage is about 0.98v
             * support all features:
             * - 8MD256 as RTC slow clock src
             * - RTC IO as input
             */
            out_config->rtc_regulator_fpu = 1;
            out_config->dbg_atten_slp = RTC_CNTL_DBG_ATTEN_DEEPSLEEP_NODROP;
            out_config->rtc_dbias_slp = 0;
        } else if (sleep_flags & RTC_SLEEP_NO_ULTRA_LOW) {
            /*
             * Default mode
             * rtc voltage in sleep need stable and not less than 0.7v
             * support features:
             * - RTC IO as input
             */
            out_config->rtc_regulator_fpu = 1;
            out_config->dbg_atten_slp = RTC_CNTL_DBG_ATTEN_DEEPSLEEP_DEFAULT;
            out_config->rtc_dbias_slp = RTC_CNTL_RTC_DBIAS_DEEPSLEEP_0V7;
        } else {
            /*
             * rtc regulator not opened and rtc voltage is about 0.66v (ultra low power):
             * not support features:
             * - 8MD256 as RTC slow clock src
             * - RTC IO as input
             */
            out_config->rtc_regulator_fpu = 0;
            out_config->dbg_atten_slp = RTC_CNTL_DBG_ATTEN_DEEPSLEEP_DEFAULT;
            out_config->rtc_dbias_slp = 0;
        }
    } else {
        out_config->rtc_regulator_fpu = 1;
        // rtc & digital voltage from high to low
        if ((sleep_flags & RTC_SLEEP_DIG_USE_8M) || !(sleep_flags & RTC_SLEEP_PD_XTAL)) {
            /*
             * digital voltage need to be >= 1.1v
             * Support all features:
             * - XTAL
             * - RC 8M used by digital system
             * - 8MD256 as RTC slow clock src (only need dbg_atten_slp set to 0)
             */
            out_config->dbg_atten_slp = RTC_CNTL_DBG_ATTEN_LIGHTSLEEP_NODROP;
            out_config->dig_dbias_slp = RTC_CNTL_DBIAS_1V10;
            out_config->rtc_dbias_slp = RTC_CNTL_DBIAS_1V10;
        } else if (!(sleep_flags & RTC_SLEEP_PD_INT_8M)){
            /*
             * dbg_att_slp need to set to 0: digital voltage is about 0.64v & rtc voltage is 0.98v
             * Support features:
             * - 8MD256 as RTC slow clock src
            */
            out_config->dbg_atten_slp = RTC_CNTL_DBG_ATTEN_LIGHTSLEEP_NODROP;
            out_config->dig_dbias_slp = 0;
            out_config->rtc_dbias_slp = 0;
        } else {
            /*
             * digital voltage not less than 0.6v.
             * not support features:
             * - XTAL
             * - RC 8M used by digital system
             * - 8MD256 as RTC slow clock src
            */
            out_config->dbg_atten_slp = RTC_CNTL_DBG_ATTEN_LIGHTSLEEP_DEFAULT;
            out_config->dig_dbias_slp = RTC_CNTL_DIG_DBIAS_LIGHTSLEEP_0V6;
            out_config->rtc_dbias_slp = RTC_CNTL_RTC_DBIAS_LIGHTSLEEP_0V6;
        }
    }
    if (!(sleep_flags & RTC_SLEEP_PD_XTAL)) {
        out_config->bias_sleep_slp = RTC_CNTL_BIASSLP_SLEEP_ON;
        out_config->pd_cur_slp = RTC_CNTL_PD_CUR_SLEEP_ON;
    } else {
        out_config->bias_sleep_slp = RTC_CNTL_BIASSLP_SLEEP_DEFAULT;
        out_config->pd_cur_slp = RTC_CNTL_PD_CUR_SLEEP_DEFAULT;
    }
}

void rtc_sleep_init(rtc_sleep_config_t cfg)
{
    if (cfg.lslp_mem_inf_fpu) {
        rtc_sleep_pu(pu_cfg);
    }

    assert(!cfg.pd_cur_slp || cfg.bias_sleep_slp);
    REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_EXT_RTC_DREG_SLEEP, cfg.rtc_dbias_slp);
    REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_EXT_DIG_DREG_SLEEP, cfg.dig_dbias_slp);

    REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_DBG_ATTEN_MONITOR, RTC_CNTL_DBG_ATTEN_MONITOR_DEFAULT);
    REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_BIAS_SLEEP_MONITOR, RTC_CNTL_BIASSLP_MONITOR_DEFAULT);
    REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_PD_CUR_MONITOR, RTC_CNTL_PD_CUR_MONITOR_DEFAULT);
    REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_DBG_ATTEN_DEEP_SLP, cfg.dbg_atten_slp);
    REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_BIAS_SLEEP_DEEP_SLP, cfg.bias_sleep_slp);
    REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_PD_CUR_DEEP_SLP, cfg.pd_cur_slp);

    if (cfg.deep_slp) {
        REGI2C_WRITE_MASK(I2C_ULP, I2C_ULP_IR_FORCE_XPD_CK, 0);
        SET_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_DG_WRAP_PD_EN);
        CLEAR_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG,
                            RTC_CNTL_CKGEN_I2C_PU | RTC_CNTL_PLL_I2C_PU |
                            RTC_CNTL_RFRX_PBUS_PU | RTC_CNTL_TXRF_I2C_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BB_I2C_FORCE_PU);
    } else {
        SET_PERI_REG_MASK(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_DG_VDD_DRV_B_SLP_EN);
        REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_DG_VDD_DRV_B_SLP, RTC_CNTL_DG_VDD_DRV_B_SLP_DEFAULT);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_DG_WRAP_PD_EN);
    }
    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_REGULATOR_FORCE_PU, cfg.rtc_regulator_fpu);

    if (!cfg.int_8m_pd_en) {
        SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_FORCE_PU);
        SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_FORCE_NOGATING);
    } else {
        CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_FORCE_NOGATING);
    }

    /* enable VDDSDIO control by state machine */
    REG_CLR_BIT(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_VDD_SPI_PWR_FORCE);
    REG_SET_FIELD(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_VDD_SPI_PD_EN, cfg.vddsdio_pd_en);

    REG_SET_FIELD(RTC_CNTL_SLP_REJECT_CONF_REG, RTC_CNTL_DEEP_SLP_REJECT_EN, cfg.deep_slp_reject);
    REG_SET_FIELD(RTC_CNTL_SLP_REJECT_CONF_REG, RTC_CNTL_LIGHT_SLP_REJECT_EN, cfg.light_slp_reject);

    REG_SET_FIELD(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_XTL_FORCE_PU, cfg.xtal_fpu);
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_XTAL_GLOBAL_FORCE_NOGATING, cfg.xtal_fpu);
}

void rtc_sleep_low_init(uint32_t slowclk_period, bool dslp)
{
    // set 5 PWC state machine times to fit in main state machine time
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_PLL_BUF_WAIT, dslp ? RTC_CNTL_PLL_BUF_WAIT_DEFAULT : RTC_CNTL_PLL_BUF_WAIT_SLP_CYCLES);
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_XTL_BUF_WAIT, dslp ? RTC_CNTL_XTL_BUF_WAIT_DEFAULT : rtc_time_us_to_slowclk(RTC_CNTL_XTL_BUF_WAIT_SLP_US, slowclk_period));
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_CK8M_WAIT, dslp ? RTC_CNTL_CK8M_WAIT_DEFAULT : RTC_CNTL_CK8M_WAIT_SLP_CYCLES);
}

static uint32_t rtc_sleep_finish(uint32_t lslp_mem_inf_fpu);

uint32_t rtc_sleep_start(uint32_t wakeup_opt, uint32_t reject_opt, uint32_t lslp_mem_inf_fpu)
{
    REG_SET_FIELD(RTC_CNTL_WAKEUP_STATE_REG, RTC_CNTL_WAKEUP_ENA, wakeup_opt);
    REG_SET_FIELD(RTC_CNTL_SLP_REJECT_CONF_REG, RTC_CNTL_SLEEP_REJECT_ENA, reject_opt);

    SET_PERI_REG_MASK(RTC_CNTL_INT_CLR_REG,
                      RTC_CNTL_SLP_REJECT_INT_CLR | RTC_CNTL_SLP_WAKEUP_INT_CLR);

    /* Start entry into sleep mode */
    SET_PERI_REG_MASK(RTC_CNTL_STATE0_REG, RTC_CNTL_SLEEP_EN);

    while (GET_PERI_REG_MASK(RTC_CNTL_INT_RAW_REG,
                             RTC_CNTL_SLP_REJECT_INT_RAW | RTC_CNTL_SLP_WAKEUP_INT_RAW) == 0) {
        ;
    }

    return rtc_sleep_finish(lslp_mem_inf_fpu);
}

static uint32_t rtc_sleep_finish(uint32_t lslp_mem_inf_fpu)
{
    /* In deep sleep mode, we never get here */
    uint32_t reject = REG_GET_FIELD(RTC_CNTL_INT_RAW_REG, RTC_CNTL_SLP_REJECT_INT_RAW);
    SET_PERI_REG_MASK(RTC_CNTL_INT_CLR_REG,
                      RTC_CNTL_SLP_REJECT_INT_CLR | RTC_CNTL_SLP_WAKEUP_INT_CLR);

    /* restore config if it is a light sleep */
    if (lslp_mem_inf_fpu) {
        rtc_sleep_pu(pu_cfg);
    }
    return reject;
}
