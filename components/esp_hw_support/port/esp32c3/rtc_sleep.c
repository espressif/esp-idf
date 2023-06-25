/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
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
#include "soc/i2s_reg.h"
#include "soc/bb_reg.h"
#include "soc/nrx_reg.h"
#include "soc/fe_reg.h"
#include "soc/timer_group_reg.h"
#include "soc/system_reg.h"
#include "soc/chip_revision.h"
#include "esp32c3/rom/ets_sys.h"
#include "esp32c3/rom/rtc.h"
#include "regi2c_ctrl.h"
#include "soc/regi2c_dig_reg.h"
#include "soc/regi2c_lp_bias.h"
#include "hal/efuse_hal.h"
#if CONFIG_ESP_SLEEP_SYSTIMER_STALL_WORKAROUND
#include "soc/systimer_reg.h"
#endif

static const DRAM_ATTR  rtc_sleep_pu_config_t pu_cfg = RTC_SLEEP_PU_CONFIG_ALL(1);

/**
 * Configure whether certain peripherals are powered down in deep sleep
 * @param cfg power down flags as rtc_sleep_pu_config_t structure
 */
void rtc_sleep_pu(rtc_sleep_pu_config_t cfg)
{
    REG_SET_FIELD(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_LSLP_MEM_FORCE_PU, cfg.dig_fpu);
    REG_SET_FIELD(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_FASTMEM_FORCE_LPU, cfg.rtc_fpu);
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
        .lslp_mem_inf_fpu = 0,
        .rtc_mem_inf_follow_cpu = (sleep_flags & RTC_SLEEP_PD_RTC_MEM_FOLLOW_CPU) ? 1 : 0,
        .rtc_fastmem_pd_en = (sleep_flags & RTC_SLEEP_PD_RTC_FAST_MEM) ? 1 : 0,
        .rtc_slowmem_pd_en = (sleep_flags & RTC_SLEEP_PD_RTC_SLOW_MEM) ? 1 : 0,
        .rtc_peri_pd_en = (sleep_flags & RTC_SLEEP_PD_RTC_PERIPH) ? 1 : 0,
        .wifi_pd_en = (sleep_flags & RTC_SLEEP_PD_WIFI) ? 1 : 0,
        .bt_pd_en = (sleep_flags & RTC_SLEEP_PD_BT) ? 1 : 0,
        .cpu_pd_en = (sleep_flags & RTC_SLEEP_PD_CPU) ? 1 : 0,
        .int_8m_pd_en = (sleep_flags & RTC_SLEEP_PD_INT_8M) ? 1 : 0,
        .dig_peri_pd_en = (sleep_flags & RTC_SLEEP_PD_DIG_PERIPH) ? 1 : 0,
        .deep_slp = (sleep_flags & RTC_SLEEP_PD_DIG) ? 1 : 0,
        .wdt_flashboot_mod_en = 0,
        .vddsdio_pd_en = (sleep_flags & RTC_SLEEP_PD_VDDSDIO) ? 1 : 0,
        .xtal_fpu = (sleep_flags & RTC_SLEEP_PD_XTAL) ? 0 : 1,
        .deep_slp_reject = 1,
        .light_slp_reject = 1
    };

    if (sleep_flags & RTC_SLEEP_PD_DIG) {
        assert(sleep_flags & RTC_SLEEP_PD_XTAL);
        bool eco2_workaround = false;
    #if CONFIG_ESP32C3_REV_MIN_FULL < 3
        if (!ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 3)) {
            eco2_workaround = true; /* workaround for deep sleep issue in high temp on ECO2 and below */
        }
    #endif
        if (!(sleep_flags & RTC_SLEEP_PD_INT_8M)) {
            /*
             * dbg_att_slp need to set to 0: rtc voltage is about 0.83v
             * support all features:
             * - 8MD256 as RTC slow clock src
             * - RTC memory under high temperature
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
             * - RTC memory under high temperature
             * - RTC IO as input
             */
            out_config->rtc_regulator_fpu = 1;
            out_config->dbg_atten_slp = eco2_workaround ? RTC_CNTL_DBG_ATTEN_DEEPSLEEP_NODROP: RTC_CNTL_DBG_ATTEN_DEEPSLEEP_DEFAULT;
            out_config->rtc_dbias_slp = RTC_CNTL_RTC_DBIAS_DEEPSLEEP_0V7;
        } else {
            /*
             * rtc regulator not opened and rtc voltage is about 0.66v (ultra low power):
             * not support features:
             * - RTC IO as input
             * - RTC memory under high temperature
             */
            out_config->rtc_regulator_fpu = 0;
            out_config->dbg_atten_slp = eco2_workaround ? RTC_CNTL_DBG_ATTEN_DEEPSLEEP_NODROP: RTC_CNTL_DBG_ATTEN_DEEPSLEEP_DEFAULT;
            out_config->rtc_dbias_slp = 0;  /* not used */
        }
    } else {
        out_config->rtc_regulator_fpu = 1;
        // rtc & digital voltage from high to low
        if ((sleep_flags & RTC_SLEEP_DIG_USE_8M) || !(sleep_flags & RTC_SLEEP_PD_XTAL)) {
            /*
             * digital voltage need to be >= 1.1v
             * if 8MD256 select as RTC slow clock src, only need dbg_atten_slp set to 0
             * Support all features:
             * - XTAL
             * - RC 8M used by digital system
             * - 8MD256 as RTC slow clock src
             */
            out_config->dbg_atten_slp = RTC_CNTL_DBG_ATTEN_LIGHTSLEEP_NODROP;
            out_config->dig_dbias_slp = RTC_CNTL_DBIAS_1V10;
            out_config->rtc_dbias_slp = RTC_CNTL_DBIAS_1V10;
        } else if (!(sleep_flags & RTC_SLEEP_PD_INT_8M)){
            /*
             * dbg_att_slp need to set to 0: digital voltage is about 0.67v & rtc vol is about 0.83v
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
    if (cfg.wifi_pd_en) {
        REG_CLR_BIT(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_WIFI_FORCE_NOISO | RTC_CNTL_WIFI_FORCE_ISO);
        REG_CLR_BIT(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_WIFI_FORCE_PU);
        SET_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_WIFI_PD_EN);
    } else {
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_WIFI_PD_EN);
    }
    if (cfg.bt_pd_en) {
        REG_CLR_BIT(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_BT_FORCE_NOISO | RTC_CNTL_BT_FORCE_ISO);
        REG_CLR_BIT(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_BT_FORCE_PU);
        SET_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_BT_PD_EN);
    } else {
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_BT_PD_EN);
    }
    if (cfg.cpu_pd_en) {
        SET_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_CPU_TOP_PD_EN);
    } else {
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_CPU_TOP_PD_EN);
    }
    if (cfg.dig_peri_pd_en) {
        SET_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_DG_PERI_PD_EN);
    } else {
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_DG_PERI_PD_EN);
    }
    REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_DBG_ATTEN_MONITOR, RTC_CNTL_DBG_ATTEN_MONITOR_DEFAULT);
    REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_BIAS_SLEEP_MONITOR, RTC_CNTL_BIASSLP_MONITOR_DEFAULT);
    REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_PD_CUR_MONITOR, RTC_CNTL_PD_CUR_MONITOR_DEFAULT);

    assert(!cfg.pd_cur_slp || cfg.bias_sleep_slp);
    REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_EXT_RTC_DREG_SLEEP, cfg.rtc_dbias_slp);
    REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_EXT_DIG_DREG_SLEEP, cfg.dig_dbias_slp);
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
    /* mem force pu */
    SET_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_LSLP_MEM_FORCE_PU);
    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_REGULATOR_FORCE_PU, cfg.rtc_regulator_fpu);
    if (!cfg.int_8m_pd_en) {
        SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_FORCE_PU);
        SET_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_FORCE_NOGATING);
    } else {
        CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_FORCE_NOGATING);
    }

    /* enable VDDSDIO control by state machine */
    REG_CLR_BIT(RTC_CNTL_SDIO_CONF_REG, RTC_CNTL_SDIO_FORCE);
    REG_SET_FIELD(RTC_CNTL_SDIO_CONF_REG, RTC_CNTL_SDIO_PD_EN, cfg.vddsdio_pd_en);

    REG_SET_FIELD(RTC_CNTL_SLP_REJECT_CONF_REG, RTC_CNTL_DEEP_SLP_REJECT_EN, cfg.deep_slp_reject);
    REG_SET_FIELD(RTC_CNTL_SLP_REJECT_CONF_REG, RTC_CNTL_LIGHT_SLP_REJECT_EN, cfg.light_slp_reject);

    REG_SET_FIELD(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_XTL_FORCE_PU, cfg.xtal_fpu);
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_XTAL_GLOBAL_FORCE_NOGATING, cfg.xtal_fpu);
}

void rtc_sleep_low_init(uint32_t slowclk_period)
{
    // set 5 PWC state machine times to fit in main state machine time
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_PLL_BUF_WAIT, RTC_CNTL_PLL_BUF_WAIT_SLP_CYCLES);
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_XTL_BUF_WAIT, rtc_time_us_to_slowclk(RTC_CNTL_XTL_BUF_WAIT_SLP_US, slowclk_period));
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_CK8M_WAIT, RTC_CNTL_CK8M_WAIT_SLP_CYCLES);
}

#if CONFIG_ESP_SLEEP_SYSTIMER_STALL_WORKAROUND
void rtc_sleep_systimer_enable(bool en)
{
    if (en) {
        REG_SET_BIT(SYSTIMER_CONF_REG, SYSTIMER_TIMER_UNIT1_WORK_EN);
    } else {
        REG_CLR_BIT(SYSTIMER_CONF_REG, SYSTIMER_TIMER_UNIT1_WORK_EN);
    }
}
#endif

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
       functions to do this (as registers will spill to stack, corrupting the CRC).

       Instead, load all the values we need into registers then use register ops only to calculate
       the CRC value, write it to the RTC CRC value register, and immediately go into deep sleep.
     */

    /* Values used to set the SYSTEM_RTC_FASTMEM_CONFIG_REG value */
    const unsigned CRC_START_ADDR = 0;
    const unsigned CRC_LEN = 0x7ff;

    asm volatile(
                 /* Start CRC calculation */
                 "sw %1, 0(%0)\n" // set RTC_MEM_CRC_ADDR & RTC_MEM_CRC_LEN
                 "or t0, %1, %2\n"
                 "sw t0, 0(%0)\n" // set RTC_MEM_CRC_START

                 /* Wait for the CRC calculation to finish */
                 ".Lwaitcrc:\n"
                 "fence\n"
                 "lw t0, 0(%0)\n"
                 "li t1, "STR(SYSTEM_RTC_MEM_CRC_FINISH)"\n"
                 "and t0, t0, t1\n"
                 "beqz t0, .Lwaitcrc\n"
                 "not %2, %2\n" // %2 -> ~DPORT_RTC_MEM_CRC_START
                 "and t0, t0, %2\n"
                 "sw t0, 0(%0)\n"  // clear RTC_MEM_CRC_START
                 "fence\n"
                 "not %2, %2\n" // %2 -> DPORT_RTC_MEM_CRC_START, probably unnecessary but gcc assumes inputs unchanged

                 /* Store the calculated value in RTC_MEM_CRC_REG */
                 "lw t0, 0(%3)\n"
                 "sw t0, 0(%4)\n"
                 "fence\n"

                 /* Set register bit to go into deep sleep */
                 "lw t0, 0(%5)\n"
                 "or   t0, t0, %6\n"
                 "sw t0, 0(%5)\n"
                 "fence\n"

                 /* Wait for sleep reject interrupt (never finishes if successful) */
                 ".Lwaitsleep:"
                 "fence\n"
                 "lw t0, 0(%7)\n"
                 "and t0, t0, %8\n"
                 "beqz t0, .Lwaitsleep\n"

                 :
                 :
                   "r" (SYSTEM_RTC_FASTMEM_CONFIG_REG), // %0
                   "r" ( (CRC_START_ADDR << SYSTEM_RTC_MEM_CRC_START_S)
                         | (CRC_LEN << SYSTEM_RTC_MEM_CRC_LEN_S)), // %1
                   "r" (SYSTEM_RTC_MEM_CRC_START), // %2
                   "r" (SYSTEM_RTC_FASTMEM_CRC_REG), // %3
                   "r" (RTC_MEMORY_CRC_REG), // %4
                   "r" (RTC_CNTL_STATE0_REG), // %5
                   "r" (RTC_CNTL_SLEEP_EN), // %6
                   "r" (RTC_CNTL_INT_RAW_REG), // %7
                   "r" (RTC_CNTL_SLP_REJECT_INT_RAW | RTC_CNTL_SLP_WAKEUP_INT_RAW) // %8
                 : "t0", "t1" // working registers
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
        rtc_sleep_pu(pu_cfg);
    }
    return reject;
}
