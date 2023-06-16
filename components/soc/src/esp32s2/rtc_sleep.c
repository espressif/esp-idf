// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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
#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/apb_ctrl_reg.h"
#include "soc/dport_reg.h"
#include "soc/rtc.h"
#include "soc/i2s_reg.h"
#include "soc/timer_group_reg.h"
#include "soc/bb_reg.h"
#include "soc/nrx_reg.h"
#include "soc/fe_reg.h"
#include "soc/rtc.h"
#include "esp32s2/rom/ets_sys.h"
#include "esp32s2/rom/rtc.h"

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
        SET_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_WIFI_PD_EN);
    } else {
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_WIFI_PD_EN);
    }
    SET_PERI_REG_MASK(RTC_CNTL_REG, RTC_CNTL_REGULATOR_FORCE_PU);
    REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_DBG_ATTEN_MONITOR, RTC_CNTL_DBG_ATTEN_MONITOR_DEFAULT);
    REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_BIAS_SLEEP_MONITOR, RTC_CNTL_BIASSLP_MONITOR_DEFAULT);
    REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_BIAS_SLEEP_DEEP_SLP, RTC_CNTL_BIASSLP_SLEEP_DEFAULT);
    REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_PD_CUR_MONITOR, RTC_CNTL_PD_CUR_MONITOR_DEFAULT);
    REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_PD_CUR_DEEP_SLP, RTC_CNTL_PD_CUR_SLEEP_DEFAULT);
    if (cfg.deep_slp) {
        REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_DBG_ATTEN_DEEP_SLP, RTC_CNTL_DBG_ATTEN_DEEPSLEEP_DEFAULT);
        SET_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_DG_WRAP_PD_EN);
        CLEAR_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG,
                            RTC_CNTL_CKGEN_I2C_PU | RTC_CNTL_PLL_I2C_PU |
                            RTC_CNTL_RFRX_PBUS_PU | RTC_CNTL_TXRF_I2C_PU);

        extern bool esp_phy_is_initialized(void);
        if (esp_phy_is_initialized()){
            extern void phy_close_rf(void);
            phy_close_rf();
        }
    } else {
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_DG_WRAP_PD_EN);
        REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_DBG_ATTEN_DEEP_SLP, RTC_CNTL_DBG_ATTEN_LIGHTSLEEP_DEFAULT);
    }

    /* enable VDDSDIO control by state machine */
    REG_CLR_BIT(RTC_CNTL_SDIO_CONF_REG, RTC_CNTL_SDIO_FORCE);
    REG_SET_FIELD(RTC_CNTL_SDIO_CONF_REG, RTC_CNTL_SDIO_PD_EN, cfg.vddsdio_pd_en);

    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DBIAS_SLP, cfg.rtc_dbias_slp);
    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DBIAS_WAK, cfg.rtc_dbias_wak);
    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, cfg.dig_dbias_wak);
    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_SLP, cfg.dig_dbias_slp);

    REG_SET_FIELD(RTC_CNTL_SLP_REJECT_CONF_REG, RTC_CNTL_DEEP_SLP_REJECT_EN, cfg.deep_slp_reject);
    REG_SET_FIELD(RTC_CNTL_SLP_REJECT_CONF_REG, RTC_CNTL_LIGHT_SLP_REJECT_EN, cfg.light_slp_reject);

    /* Set wait cycle for touch or COCPU after deep sleep and light sleep. */
    REG_SET_FIELD(RTC_CNTL_TIMER2_REG, RTC_CNTL_ULPCP_TOUCH_START_WAIT, RTC_CNTL_ULPCP_TOUCH_START_WAIT_IN_SLEEP);
}

void rtc_sleep_set_wakeup_time(uint64_t t)
{
    WRITE_PERI_REG(RTC_CNTL_SLP_TIMER0_REG, t & UINT32_MAX);
    WRITE_PERI_REG(RTC_CNTL_SLP_TIMER1_REG, t >> 32);
}

/* Read back 'reject' status when waking from light or deep sleep */
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
