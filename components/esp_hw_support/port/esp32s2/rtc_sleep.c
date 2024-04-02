/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/syscon_reg.h"
#include "soc/dport_reg.h"
#include "soc/i2s_reg.h"
#include "soc/timer_group_reg.h"
#include "soc/bb_reg.h"
#include "soc/nrx_reg.h"
#include "soc/fe_reg.h"
#include "esp32s2/rom/ets_sys.h"
#include "esp32s2/rom/rtc.h"
#include "hal/rtc_cntl_ll.h"

/**
 * Configure whether certain peripherals are powered down in deep sleep
 * @param cfg power down flags as rtc_sleep_pd_config_t structure
 */
void rtc_sleep_pd(rtc_sleep_pd_config_t cfg)
{
    REG_SET_FIELD(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_LSLP_MEM_FORCE_PU, cfg.dig_fpu);
    REG_SET_FIELD(RTC_CNTL_PWC_REG, RTC_CNTL_FASTMEM_FORCE_LPU, cfg.rtc_fpu);
    REG_SET_FIELD(RTC_CNTL_PWC_REG, RTC_CNTL_SLOWMEM_FORCE_LPU, cfg.rtc_fpu);
    REG_SET_FIELD(I2S_PD_CONF_REG(0), I2S_PLC_MEM_FORCE_PU, cfg.i2s_fpu);
    REG_SET_FIELD(I2S_PD_CONF_REG(0), I2S_FIFO_FORCE_PU, cfg.i2s_fpu);
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
}

void rtc_sleep_get_default_config(uint32_t sleep_flags, rtc_sleep_config_t *out_config)
{
    *out_config = (rtc_sleep_config_t) {
        .lslp_mem_inf_fpu = 0,
        .rtc_mem_inf_follow_cpu = (sleep_flags & RTC_SLEEP_PD_RTC_MEM_FOLLOW_CPU) ? 1 : 0,
        .rtc_fastmem_pd_en = (sleep_flags & RTC_SLEEP_PD_RTC_FAST_MEM) ? 1 : 0,
        .rtc_slowmem_pd_en = (sleep_flags & RTC_SLEEP_PD_RTC_SLOW_MEM) ? 1 : 0,
        .rtc_peri_pd_en = (sleep_flags & RTC_SLEEP_PD_RTC_PERIPH) ? 1 : 0,
        .wifi_pd_en = (sleep_flags & RTC_SLEEP_PD_WIFI) ? 1 : 0,
        .int_8m_pd_en = (sleep_flags & RTC_SLEEP_PD_INT_8M) ? 1 : 0,
        .deep_slp = (sleep_flags & RTC_SLEEP_PD_DIG) ? 1 : 0,
        .wdt_flashboot_mod_en = 0,
        .vddsdio_pd_en = (sleep_flags & RTC_SLEEP_PD_VDDSDIO) ? 1 : 0,
        .xtal_fpu = (sleep_flags & RTC_SLEEP_PD_XTAL) ? 0 : 1,
        .deep_slp_reject = 1,
        .light_slp_reject = 1
    };

    if (sleep_flags & RTC_SLEEP_PD_DIG) {
        assert(sleep_flags & RTC_SLEEP_PD_XTAL);
        out_config->dig_dbias_slp = 0;  //not used
        //rtc voltage from high to low
        if ((sleep_flags & RTC_SLEEP_USE_ADC_TESEN_MONITOR) || !(sleep_flags & RTC_SLEEP_PD_INT_8M)) {
            /*
             * rtc voltage in sleep mode >= 0.9v
             * if 8MD256 select as RTC slow clock src, only need dbg_atten_slp set to 0
             * Support all features:
             * - ADC/Temperature sensor in monitor mode (ULP) (also need pd_cur_monitor = 0)
             * - 8MD256 as RTC slow clock src
             * - RTC IO as input
             * - RTC Memory at high temperature
             * - ULP
             * - Touch sensor
             */
            out_config->rtc_regulator_fpu = 1;
            out_config->dbg_atten_slp = RTC_CNTL_DBG_ATTEN_DEEPSLEEP_NODROP;
            out_config->rtc_dbias_slp = RTC_CNTL_DBIAS_0V90;
        } else if (sleep_flags & RTC_SLEEP_NO_ULTRA_LOW) {
            /*
             * rtc voltage in sleep mode >= 0.7v (default mode)
             * Support features:
             * - RTC IO as input
             * - RTC Memory at high temperature
             * - ULP
             * - Touch sensor
             */
            out_config->rtc_regulator_fpu = 1;
            out_config->dbg_atten_slp = RTC_CNTL_DBG_ATTEN_DEEPSLEEP_DEFAULT;
            out_config->rtc_dbias_slp = RTC_CNTL_RTC_DBIAS_DEEPSLEEP_0V7;
        } else {
            /*
             * rtc regulator not opened and rtc voltage is about 0.66v (ultra low power)
             * Support features:
             * - ULP
             * - Touch sensor
             */
            out_config->rtc_regulator_fpu = 0;
            out_config->dbg_atten_slp = RTC_CNTL_DBG_ATTEN_DEEPSLEEP_DEFAULT;
            out_config->rtc_dbias_slp = 0;
        }
    } else {
        out_config->rtc_regulator_fpu = 1;
        // rtc & digital voltage from high to low
        if ((sleep_flags & RTC_SLEEP_DIG_USE_8M) || !(sleep_flags & RTC_SLEEP_PD_XTAL) || !(sleep_flags & RTC_SLEEP_PD_INT_8M)) {
            /*
             * digital voltage need to be >= 1.1v
             * if 8MD256 select as RTC slow clock src, only need dbg_atten_slp set to 0
             * rtc voltage need to near digital voltage to keep system stable
             * Support all features:
             * - XTAL
             * - RC 8M used by digital system
             * - 8MD256 as RTC slow clock src (only need dbg_atten_slp set to 0)
             * - ADC/Temperature sensor in monitor mode (ULP) (also need pd_cur_monitor = 0)
             * - ULP
             * - Touch sensor
             */
            out_config->dbg_atten_slp = RTC_CNTL_DBG_ATTEN_LIGHTSLEEP_NODROP;
            out_config->dig_dbias_slp = RTC_CNTL_DBIAS_1V10;
            out_config->rtc_dbias_slp = RTC_CNTL_DBIAS_1V10;
        } else if (sleep_flags & RTC_SLEEP_USE_ADC_TESEN_MONITOR) {
            /*
             * rtc voltage need to be >= 0.9v
             * digital voltage need to near rtc voltage to make system stable and low current
             * Support features:
             * - ADC/Temperature sensor in monitor mode (ULP) (also need pd_cur_monitor = 0)
             * - ULP
             * - Touch sensor
            */
            out_config->dbg_atten_slp = RTC_CNTL_DBG_ATTEN_LIGHTSLEEP_DEFAULT;
            out_config->dig_dbias_slp = RTC_CNTL_DIG_DBIAS_LIGHTSLEEP_0V9;
            out_config->rtc_dbias_slp = RTC_CNTL_RTC_DBIAS_LIGHTSLEEP_0V9;
        } else {
            /*
             * digital voltage not less than 0.75v.
             * rtc voltage need to near digital voltage to keep system stable
             * Support features:
             * - ULP
             * - Touch sensor
            */
            out_config->dbg_atten_slp = RTC_CNTL_DBG_ATTEN_LIGHTSLEEP_DEFAULT;
            out_config->dig_dbias_slp = RTC_CNTL_DIG_DBIAS_LIGHTSLEEP_0V75;
            out_config->rtc_dbias_slp = RTC_CNTL_RTC_DBIAS_LIGHTSLEEP_0V75;
        }
    }
    if (!(sleep_flags & RTC_SLEEP_PD_XTAL)) {
        out_config->bias_sleep_monitor = RTC_CNTL_BIASSLP_MONITOR_ON;
        out_config->pd_cur_monitor = RTC_CNTL_PD_CUR_MONITOR_ON;
        out_config->bias_sleep_slp = RTC_CNTL_BIASSLP_SLEEP_ON;
        out_config->pd_cur_slp = RTC_CNTL_PD_CUR_SLEEP_ON;
    } else {
        out_config->bias_sleep_monitor = RTC_CNTL_BIASSLP_MONITOR_DEFAULT;
        out_config->pd_cur_monitor = (sleep_flags & RTC_SLEEP_USE_ADC_TESEN_MONITOR)?
                                    RTC_CNTL_PD_CUR_MONITOR_ON : RTC_CNTL_PD_CUR_MONITOR_DEFAULT;
        out_config->bias_sleep_slp = RTC_CNTL_BIASSLP_SLEEP_DEFAULT;
        out_config->pd_cur_slp = RTC_CNTL_PD_CUR_SLEEP_DEFAULT;
    }
}

void rtc_sleep_init(rtc_sleep_config_t cfg)
{
    if (cfg.lslp_mem_inf_fpu) {
        rtc_sleep_pd_config_t pd_cfg = RTC_SLEEP_PD_CONFIG_ALL(1);
        rtc_sleep_pd(pd_cfg);
    }

    if (cfg.rtc_mem_inf_follow_cpu) {
        SET_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_MEM_FOLW_CPU);
    } else {
        CLEAR_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_MEM_FOLW_CPU);
    }

    if (cfg.rtc_fastmem_pd_en) {
        SET_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_FASTMEM_PD_EN);
        CLEAR_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_FASTMEM_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_FASTMEM_FORCE_NOISO);
    } else {
        CLEAR_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_FASTMEM_PD_EN);
        SET_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_FASTMEM_FORCE_PU);
        SET_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_FASTMEM_FORCE_NOISO);
    }

    if (cfg.rtc_slowmem_pd_en) {
        SET_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_SLOWMEM_PD_EN);
        CLEAR_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_SLOWMEM_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_SLOWMEM_FORCE_NOISO);
    } else {
        CLEAR_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_SLOWMEM_PD_EN);
        SET_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_SLOWMEM_FORCE_PU);
        SET_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_SLOWMEM_FORCE_NOISO);
    }

    if (cfg.rtc_peri_pd_en) {
        SET_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_PD_EN);
    } else {
        CLEAR_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_PD_EN);
    }

    if (cfg.wifi_pd_en) {
        REG_CLR_BIT(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_WIFI_FORCE_NOISO);
        REG_CLR_BIT(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_WIFI_FORCE_PU);
        SET_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_WIFI_PD_EN);
    } else {
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_WIFI_PD_EN);
    }

    assert(!cfg.pd_cur_monitor || cfg.bias_sleep_monitor);
    assert(!cfg.pd_cur_slp || cfg.bias_sleep_slp);

    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DBIAS_SLP, cfg.rtc_dbias_slp);
    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_SLP, cfg.dig_dbias_slp);

    REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_DBG_ATTEN_MONITOR, RTC_CNTL_DBG_ATTEN_MONITOR_DEFAULT);
    REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_BIAS_SLEEP_MONITOR, cfg.bias_sleep_monitor);
    REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_BIAS_SLEEP_DEEP_SLP, cfg.bias_sleep_slp);
    REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_PD_CUR_MONITOR, cfg.pd_cur_monitor);
    REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_PD_CUR_DEEP_SLP, cfg.pd_cur_slp);
    REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_DBG_ATTEN_DEEP_SLP, cfg.dbg_atten_slp);

    if (cfg.deep_slp) {
        SET_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_DG_WRAP_PD_EN);
        CLEAR_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG,
                            RTC_CNTL_CKGEN_I2C_PU | RTC_CNTL_PLL_I2C_PU |
                            RTC_CNTL_RFRX_PBUS_PU | RTC_CNTL_TXRF_I2C_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BB_I2C_FORCE_PU);
    } else {
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_DG_WRAP_PD_EN);
    }

    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_REGULATOR_FORCE_PU, cfg.rtc_regulator_fpu);
    if (!cfg.int_8m_pd_en) {
        REG_SET_BIT(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_FORCE_PU);
    } else {
        REG_CLR_BIT(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_FORCE_PU);
    }

    /* enable VDDSDIO control by state machine */
    REG_CLR_BIT(RTC_CNTL_SDIO_CONF_REG, RTC_CNTL_SDIO_FORCE);
    REG_SET_FIELD(RTC_CNTL_SDIO_CONF_REG, RTC_CNTL_SDIO_PD_EN, cfg.vddsdio_pd_en);

    REG_SET_FIELD(RTC_CNTL_SLP_REJECT_CONF_REG, RTC_CNTL_DEEP_SLP_REJECT_EN, cfg.deep_slp_reject);
    REG_SET_FIELD(RTC_CNTL_SLP_REJECT_CONF_REG, RTC_CNTL_LIGHT_SLP_REJECT_EN, cfg.light_slp_reject);
    /* Set wait cycle for touch or COCPU after deep sleep and light sleep. */
    REG_SET_FIELD(RTC_CNTL_TIMER2_REG, RTC_CNTL_ULPCP_TOUCH_START_WAIT, RTC_CNTL_ULPCP_TOUCH_START_WAIT_IN_SLEEP);

    REG_SET_FIELD(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_XTL_FORCE_PU, cfg.xtal_fpu);
}

void rtc_sleep_low_init(uint32_t slowclk_period)
{
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_PLL_BUF_WAIT, RTC_CNTL_PLL_BUF_WAIT_SLP_CYCLES);
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_CK8M_WAIT, RTC_CNTL_CK8M_WAIT_SLP_CYCLES);
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_XTL_BUF_WAIT, rtc_time_us_to_slowclk(RTC_CNTL_XTL_BUF_WAIT_SLP_US, slowclk_period));
}

/* Read back 'reject' status when waking from light or deep sleep */
static uint32_t rtc_sleep_finish(uint32_t lslp_mem_inf_fpu);

uint32_t rtc_sleep_start(uint32_t wakeup_opt, uint32_t reject_opt, uint32_t lslp_mem_inf_fpu)
{
    REG_SET_FIELD(RTC_CNTL_WAKEUP_STATE_REG, RTC_CNTL_WAKEUP_ENA, wakeup_opt);
    REG_SET_FIELD(RTC_CNTL_SLP_REJECT_CONF_REG, RTC_CNTL_SLEEP_REJECT_ENA, reject_opt);
    if (reject_opt != 0) {
        REG_SET_BIT(RTC_CNTL_SLP_REJECT_CONF_REG, RTC_CNTL_LIGHT_SLP_REJECT_EN);
    }

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

#define STR2(X) #X
#define STR(X) STR2(X)

uint32_t rtc_deep_sleep_start(uint32_t wakeup_opt, uint32_t reject_opt)
{
    REG_SET_FIELD(RTC_CNTL_WAKEUP_STATE_REG, RTC_CNTL_WAKEUP_ENA, wakeup_opt);
    WRITE_PERI_REG(RTC_CNTL_SLP_REJECT_CONF_REG, reject_opt);

    SET_PERI_REG_MASK(RTC_CNTL_INT_CLR_REG,
                      RTC_CNTL_SLP_REJECT_INT_CLR | RTC_CNTL_SLP_WAKEUP_INT_CLR);

    /* Calculate RTC Fast Memory CRC (for wake stub) & go to deep sleep

       Because we may be running from RTC memory as stack, we can't easily call any
       functions to do this (as registers may spill to stack, corrupting the CRC).

       Instead, load all the values we need into registers (triggering any stack spills)
       then use register ops only to calculate the CRC value, write it to the RTC CRC value
       register, and immediately go into deep sleep.
     */

    /* Values used to set the DPORT_RTC_FASTMEM_CONFIG_REG value */
    const unsigned CRC_START_ADDR = 0;
    const unsigned CRC_LEN = 0x7ff;

    asm volatile(
                 "movi a2, 0\n" // trigger a stack spill on working register if needed

                 /* Start CRC calculation */
                 "s32i %1, %0, 0\n" // set RTC_MEM_CRC_ADDR & RTC_MEM_CRC_LEN
                 "or a2, %1, %2\n"
                 "s32i a2, %0, 0\n" // set RTC_MEM_CRC_START

                 /* Wait for the CRC calculation to finish */
                 ".Lwaitcrc:\n"
                 "memw\n"
                 "l32i a2, %0, 0\n"
                 "bbci a2, "STR(DPORT_RTC_MEM_CRC_FINISH_S)", .Lwaitcrc\n"
                 "xor %2, %2, %2\n" // %2 -> ~DPORT_RTC_MEM_CRC_START
                 "and a2, a2, %2\n"
                 "s32i a2, %0, 0\n"  // clear RTC_MEM_CRC_START
                 "memw\n"
                 "xor %2, %2, %2\n" // %2 -> DPORT_RTC_MEM_CRC_START, probably unnecessary but gcc assumes inputs unchanged

                 /* Store the calculated value in RTC_MEM_CRC_REG */
                 "l32i a2, %3, 0\n"
                 "s32i a2, %4, 0\n"
                 "memw\n"

                 /* Set register bit to go into deep sleep */
                 "l32i a2, %5, 0\n"
                 "or   a2, a2, %6\n"
                 "s32i a2, %5, 0\n"
                 "memw\n"

                 /* Wait for sleep reject interrupt (never finishes if successful) */
                 ".Lwaitsleep:"
                 "memw\n"
                 "l32i a2, %7, 0\n"
                 "and a2, a2, %8\n"
                 "beqz a2, .Lwaitsleep\n"

                 :
                 : /* Note, at -O0 this is the limit of available registers in this function */
                   "r" (DPORT_RTC_FASTMEM_CONFIG_REG), // %0
                   "r" ( (CRC_START_ADDR << DPORT_RTC_MEM_CRC_START_S)
                         | (CRC_LEN << DPORT_RTC_MEM_CRC_LEN_S)), // %1
                   "r" (DPORT_RTC_MEM_CRC_START), // %2
                   "r" (DPORT_RTC_FASTMEM_CRC_REG), // %3
                   "r" (RTC_MEMORY_CRC_REG), // %4
                   "r" (RTC_CNTL_STATE0_REG), // %5
                   "r" (RTC_CNTL_SLEEP_EN), // %6
                   "r" (RTC_CNTL_INT_RAW_REG), // %7
                   "r" (RTC_CNTL_SLP_REJECT_INT_RAW | RTC_CNTL_SLP_WAKEUP_INT_RAW) // %8
                 : "a2" // working register
                 );

    return rtc_sleep_finish(0);
}

static uint32_t rtc_sleep_finish(uint32_t lslp_mem_inf_fpu)
{
    /* In deep sleep mode, we never get here */
    uint32_t reject = REG_GET_FIELD(RTC_CNTL_INT_RAW_REG, RTC_CNTL_SLP_REJECT_INT_RAW);
    SET_PERI_REG_MASK(RTC_CNTL_INT_CLR_REG,
                      RTC_CNTL_SLP_REJECT_INT_CLR | RTC_CNTL_SLP_WAKEUP_INT_CLR);

    /* restore config if it is a light sleep */
    if (lslp_mem_inf_fpu) {
        rtc_sleep_pd_config_t pd_cfg = RTC_SLEEP_PD_CONFIG_ALL(0);
        rtc_sleep_pd(pd_cfg);
    }

    /* Recover default wait cycle for touch or COCPU after wakeup. */
    REG_SET_FIELD(RTC_CNTL_TIMER2_REG, RTC_CNTL_ULPCP_TOUCH_START_WAIT, RTC_CNTL_ULPCP_TOUCH_START_WAIT_DEFAULT);

    return reject;
}
