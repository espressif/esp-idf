// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdint.h>
#include <stdlib.h>
#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/apb_ctrl_reg.h"
#include "soc/rtc.h"
#include "soc/i2s_reg.h"
#include "soc/bb_reg.h"
#include "soc/nrx_reg.h"
#include "soc/fe_reg.h"
#include "soc/timer_group_reg.h"
#include "soc/system_reg.h"
#include "soc/rtc.h"
#include "esp32c3/rom/ets_sys.h"
#include "esp32c3/rom/rtc.h"
#include "regi2c_ctrl.h"
#include "esp_efuse.h"

/**
 * Configure whether certain peripherals are powered down in deep sleep
 * @param cfg power down flags as rtc_sleep_pu_config_t structure
 */
void rtc_sleep_pu(rtc_sleep_pu_config_t cfg)
{
    REG_SET_FIELD(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_LSLP_MEM_FORCE_PU, cfg.dig_fpu);
    REG_SET_FIELD(RTC_CNTL_PWC_REG, RTC_CNTL_FASTMEM_FORCE_LPU, cfg.rtc_fpu);
    REG_SET_FIELD(APB_CTRL_FRONT_END_MEM_PD_REG, APB_CTRL_DC_MEM_FORCE_PU, cfg.fe_fpu);
    REG_SET_FIELD(APB_CTRL_FRONT_END_MEM_PD_REG, APB_CTRL_PBUS_MEM_FORCE_PU, cfg.fe_fpu);
    REG_SET_FIELD(APB_CTRL_FRONT_END_MEM_PD_REG, APB_CTRL_AGC_MEM_FORCE_PU, cfg.fe_fpu);
    REG_SET_FIELD(BBPD_CTRL, BB_FFT_FORCE_PU, cfg.bb_fpu);
    REG_SET_FIELD(BBPD_CTRL, BB_DC_EST_FORCE_PU, cfg.bb_fpu);
    REG_SET_FIELD(NRXPD_CTRL, NRX_RX_ROT_FORCE_PU, cfg.nrx_fpu);
    REG_SET_FIELD(NRXPD_CTRL, NRX_VIT_FORCE_PU, cfg.nrx_fpu);
    REG_SET_FIELD(NRXPD_CTRL, NRX_DEMAP_FORCE_PU, cfg.nrx_fpu);
    REG_SET_FIELD(FE_GEN_CTRL, FE_IQ_EST_FORCE_PU, cfg.fe_fpu);
    REG_SET_FIELD(FE2_TX_INTERP_CTRL, FE2_TX_INF_FORCE_PU, cfg.fe_fpu);
    if (cfg.sram_fpu) {
        REG_SET_FIELD(APB_CTRL_MEM_POWER_UP_REG, APB_CTRL_SRAM_POWER_UP, APB_CTRL_SRAM_POWER_UP);
    } else {
        REG_SET_FIELD(APB_CTRL_MEM_POWER_UP_REG, APB_CTRL_SRAM_POWER_UP, 0);
    }
    if (cfg.rom_ram_fpu) {
        REG_SET_FIELD(APB_CTRL_MEM_POWER_UP_REG, APB_CTRL_ROM_POWER_UP, APB_CTRL_ROM_POWER_UP);
    } else {
        REG_SET_FIELD(APB_CTRL_MEM_POWER_UP_REG, APB_CTRL_ROM_POWER_UP, 0);
    }
}

void rtc_sleep_init(rtc_sleep_config_t cfg)
{
    if (cfg.lslp_mem_inf_fpu) {
        rtc_sleep_pu_config_t pu_cfg = RTC_SLEEP_PU_CONFIG_ALL(1);
        rtc_sleep_pu(pu_cfg);
    }
    if (cfg.wifi_pd_en) {
        SET_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_WIFI_PD_EN);
    } else {
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_WIFI_PD_EN);
    }
    if (cfg.bt_pd_en) {
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
    REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_BIAS_SLEEP_DEEP_SLP, RTC_CNTL_BIASSLP_SLEEP_DEFAULT);
    REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_PD_CUR_MONITOR, RTC_CNTL_PD_CUR_MONITOR_DEFAULT);
    REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_PD_CUR_DEEP_SLP, RTC_CNTL_PD_CUR_SLEEP_DEFAULT);
    if (cfg.deep_slp) {
        REGI2C_WRITE_MASK(I2C_ULP, I2C_ULP_IR_FORCE_XPD_CK, 0);
        CLEAR_PERI_REG_MASK(RTC_CNTL_REG, RTC_CNTL_REGULATOR_FORCE_PU);
        unsigned atten_deep_sleep = RTC_CNTL_DBG_ATTEN_DEEPSLEEP_DEFAULT;
    #if CONFIG_ESP32C3_REV_MIN < 3
        if (esp_efuse_get_chip_ver() < 3) {
            atten_deep_sleep = 0; /* workaround for deep sleep issue in high temp on ECO2 and below */
        }
    #endif
        REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_DBG_ATTEN_DEEP_SLP, atten_deep_sleep);
        SET_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_DG_WRAP_PD_EN);
        CLEAR_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG,
                            RTC_CNTL_CKGEN_I2C_PU | RTC_CNTL_PLL_I2C_PU |
                            RTC_CNTL_RFRX_PBUS_PU | RTC_CNTL_TXRF_I2C_PU);
    } else {
        SET_PERI_REG_MASK(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_DG_VDD_DRV_B_SLP_EN);
        REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_DG_VDD_DRV_B_SLP, RTC_CNTL_DG_VDD_DRV_B_SLP_DEFAULT);
        SET_PERI_REG_MASK(RTC_CNTL_REG, RTC_CNTL_REGULATOR_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_DG_WRAP_PD_EN);
        REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_DBG_ATTEN_DEEP_SLP, RTC_CNTL_DBG_ATTEN_LIGHTSLEEP_DEFAULT);
    }

    /* enable VDDSDIO control by state machine */
    REG_CLR_BIT(RTC_CNTL_SDIO_CONF_REG, RTC_CNTL_SDIO_FORCE);
    REG_SET_FIELD(RTC_CNTL_SDIO_CONF_REG, RTC_CNTL_SDIO_PD_EN, cfg.vddsdio_pd_en);

    REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_EXT_RTC_DREG_SLEEP, cfg.rtc_dbias_slp);
    REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_EXT_RTC_DREG, cfg.rtc_dbias_wak);
    REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_EXT_DIG_DREG_SLEEP, cfg.dig_dbias_slp);
    REGI2C_WRITE_MASK(I2C_DIG_REG, I2C_DIG_REG_EXT_DIG_DREG, cfg.dig_dbias_wak);

    REG_SET_FIELD(RTC_CNTL_SLP_REJECT_CONF_REG, RTC_CNTL_DEEP_SLP_REJECT_EN, cfg.deep_slp_reject);
    REG_SET_FIELD(RTC_CNTL_SLP_REJECT_CONF_REG, RTC_CNTL_LIGHT_SLP_REJECT_EN, cfg.light_slp_reject);

    /* gating XTAL clock */
    REG_CLR_BIT(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_XTAL_GLOBAL_FORCE_NOGATING);
}

void rtc_sleep_low_init(uint32_t slowclk_period)
{
    // set 5 PWC state machine times to fit in main state machine time
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_PLL_BUF_WAIT, RTC_CNTL_PLL_BUF_WAIT_SLP_CYCLES);
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_XTL_BUF_WAIT, rtc_time_us_to_slowclk(RTC_CNTL_XTL_BUF_WAIT_SLP_US, slowclk_period));
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_CK8M_WAIT, RTC_CNTL_CK8M_WAIT_SLP_CYCLES);
}

void rtc_sleep_set_wakeup_time(uint64_t t)
{
    WRITE_PERI_REG(RTC_CNTL_SLP_TIMER0_REG, t & UINT32_MAX);
    WRITE_PERI_REG(RTC_CNTL_SLP_TIMER1_REG, t >> 32);
}

static uint32_t rtc_sleep_finish(uint32_t lslp_mem_inf_fpu);

uint32_t rtc_sleep_start(uint32_t wakeup_opt, uint32_t reject_opt, uint32_t lslp_mem_inf_fpu)
{
    REG_SET_FIELD(RTC_CNTL_WAKEUP_STATE_REG, RTC_CNTL_WAKEUP_ENA, wakeup_opt);
    REG_SET_FIELD(RTC_CNTL_SLP_REJECT_CONF_REG, RTC_CNTL_SLEEP_REJECT_ENA, reject_opt);

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
        rtc_sleep_pu_config_t pu_cfg = RTC_SLEEP_PU_CONFIG_ALL(1);
        rtc_sleep_pu(pu_cfg);
    }
    return reject;
}
