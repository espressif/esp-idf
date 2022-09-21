/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/dport_reg.h"
#include "soc/i2s_periph.h"
#include "soc/timer_periph.h"
#include "soc/bb_reg.h"
#include "soc/nrx_reg.h"
#include "soc/fe_reg.h"
#include "esp32/rom/ets_sys.h"
#include "esp32/rom/rtc.h"
#include "hal/rtc_cntl_ll.h"
#include "esp_rom_sys.h"

#define MHZ (1000000)

#define ROM_RAM_POWERUP_CYCLES   RTC_CNTL_OTHER_BLOCKS_POWERUP_CYCLES
#define ROM_RAM_WAIT_CYCLES      RTC_CNTL_OTHER_BLOCKS_WAIT_CYCLES

#define WIFI_POWERUP_CYCLES      RTC_CNTL_OTHER_BLOCKS_POWERUP_CYCLES
#define WIFI_WAIT_CYCLES         RTC_CNTL_OTHER_BLOCKS_WAIT_CYCLES

#define RTC_POWERUP_CYCLES       RTC_CNTL_OTHER_BLOCKS_POWERUP_CYCLES
#define RTC_WAIT_CYCLES          RTC_CNTL_OTHER_BLOCKS_WAIT_CYCLES

#define DG_WRAP_POWERUP_CYCLES   RTC_CNTL_OTHER_BLOCKS_POWERUP_CYCLES
#define DG_WRAP_WAIT_CYCLES      RTC_CNTL_OTHER_BLOCKS_WAIT_CYCLES

#define RTC_MEM_POWERUP_CYCLES   RTC_CNTL_OTHER_BLOCKS_POWERUP_CYCLES
#define RTC_MEM_WAIT_CYCLES      RTC_CNTL_OTHER_BLOCKS_WAIT_CYCLES

/**
 * @brief Power down flags for rtc_sleep_pd function
 */
typedef struct {
    uint32_t dig_pd : 1;    //!< Set to 1 to power down digital part in sleep
    uint32_t rtc_pd : 1;    //!< Set to 1 to power down RTC memories in sleep
    uint32_t cpu_pd : 1;    //!< Set to 1 to power down digital memories and CPU in sleep
    uint32_t i2s_pd : 1;    //!< Set to 1 to power down I2S in sleep
    uint32_t bb_pd : 1;     //!< Set to 1 to power down WiFi in sleep
    uint32_t nrx_pd : 1;    //!< Set to 1 to power down WiFi in sleep
    uint32_t fe_pd : 1;     //!< Set to 1 to power down WiFi in sleep
} rtc_sleep_pd_config_t;

/**
 * Initializer for rtc_sleep_pd_config_t which sets all flags to the same value
 */
#define RTC_SLEEP_PD_CONFIG_ALL(val) {\
    .dig_pd = (val), \
    .rtc_pd = (val), \
    .cpu_pd = (val), \
    .i2s_pd = (val), \
    .bb_pd = (val), \
    .nrx_pd = (val), \
    .fe_pd = (val), \
}

/**
 * Configure whether certain peripherals are powered down in deep sleep
 * @param cfg power down flags as rtc_sleep_pd_config_t structure
 */
static void rtc_sleep_pd(rtc_sleep_pd_config_t cfg)
{
    REG_SET_FIELD(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_LSLP_MEM_FORCE_PU, ~cfg.dig_pd);
    REG_SET_FIELD(RTC_CNTL_PWC_REG, RTC_CNTL_SLOWMEM_FORCE_LPU, ~cfg.rtc_pd);
    REG_SET_FIELD(RTC_CNTL_PWC_REG, RTC_CNTL_FASTMEM_FORCE_LPU, ~cfg.rtc_pd);
    DPORT_REG_SET_FIELD(DPORT_MEM_PD_MASK_REG, DPORT_LSLP_MEM_PD_MASK, ~cfg.cpu_pd);
    REG_SET_FIELD(I2S_PD_CONF_REG(0), I2S_PLC_MEM_FORCE_PU, ~cfg.i2s_pd);
    REG_SET_FIELD(I2S_PD_CONF_REG(0), I2S_FIFO_FORCE_PU, ~cfg.i2s_pd);
    REG_SET_FIELD(BBPD_CTRL, BB_FFT_FORCE_PU, ~cfg.bb_pd);
    REG_SET_FIELD(BBPD_CTRL, BB_DC_EST_FORCE_PU, ~cfg.bb_pd);
    REG_SET_FIELD(NRXPD_CTRL, NRX_RX_ROT_FORCE_PU, ~cfg.nrx_pd);
    REG_SET_FIELD(NRXPD_CTRL, NRX_VIT_FORCE_PU, ~cfg.nrx_pd);
    REG_SET_FIELD(NRXPD_CTRL, NRX_DEMAP_FORCE_PU, ~cfg.nrx_pd);
    REG_SET_FIELD(FE_GEN_CTRL, FE_IQ_EST_FORCE_PU, ~cfg.fe_pd);
    REG_SET_FIELD(FE2_TX_INTERP_CTRL, FE2_TX_INF_FORCE_PU, ~cfg.fe_pd);
}

void rtc_sleep_get_default_config(uint32_t sleep_flags, rtc_sleep_config_t *out_config)
{
    *out_config = (rtc_sleep_config_t) {
        .lslp_mem_inf_fpu = 0,
        .rtc_mem_inf_fpu = 0,
        .rtc_mem_inf_follow_cpu = ((sleep_flags) & RTC_SLEEP_PD_RTC_MEM_FOLLOW_CPU) ? 1 : 0,
        .rtc_fastmem_pd_en = ((sleep_flags) & RTC_SLEEP_PD_RTC_FAST_MEM) ? 1 : 0,
        .rtc_slowmem_pd_en = ((sleep_flags) & RTC_SLEEP_PD_RTC_SLOW_MEM) ? 1 : 0,
        .rtc_peri_pd_en = ((sleep_flags) & RTC_SLEEP_PD_RTC_PERIPH) ? 1 : 0,
        .modem_pd_en = (sleep_flags & RTC_SLEEP_PD_MODEM) ? 1 : 0,
        .int_8m_pd_en = ((sleep_flags) & RTC_SLEEP_PD_INT_8M) ? 1 : 0,
        .rom_mem_pd_en = 0,
        .deep_slp = ((sleep_flags) & RTC_SLEEP_PD_DIG) ? 1 : 0,
        .wdt_flashboot_mod_en = 0,
        .lslp_meminf_pd = 1,
        .vddsdio_pd_en = ((sleep_flags) & RTC_SLEEP_PD_VDDSDIO) ? 1 : 0,
        .xtal_fpu = ((sleep_flags) & RTC_SLEEP_PD_XTAL) ? 0 : 1,
        .deep_slp_reject = 1,
        .light_slp_reject = 1,
    };

    if ((sleep_flags) & RTC_SLEEP_PD_DIG) {
        out_config->dig_dbias_wak = RTC_CNTL_DBIAS_1V10;
        out_config->dig_dbias_slp = RTC_CNTL_DBIAS_0V90;
        out_config->rtc_dbias_wak = RTC_CNTL_DBIAS_1V10;
        out_config->rtc_dbias_slp = RTC_CNTL_DBIAS_0V90;
        out_config->dbg_atten_slp = !(sleep_flags & RTC_SLEEP_PD_INT_8M) ? RTC_CNTL_DBG_ATTEN_NODROP : RTC_CNTL_DBG_ATTEN_DEFAULT;
    } else {
        out_config->dig_dbias_wak = RTC_CNTL_DBIAS_1V10;
        out_config->dig_dbias_slp = !((sleep_flags) & RTC_SLEEP_PD_INT_8M) ? RTC_CNTL_DBIAS_1V10 : RTC_CNTL_DBIAS_0V90;
        out_config->rtc_dbias_wak = RTC_CNTL_DBIAS_1V10;
        out_config->rtc_dbias_slp = !((sleep_flags) & RTC_SLEEP_PD_INT_8M) ? RTC_CNTL_DBIAS_1V10 : RTC_CNTL_DBIAS_0V90;
        out_config->dbg_atten_slp = RTC_CNTL_DBG_ATTEN_NODROP;
    }
}

void rtc_sleep_init(rtc_sleep_config_t cfg)
{
    // set shortest possible sleep time limit
    REG_SET_FIELD(RTC_CNTL_TIMER5_REG, RTC_CNTL_MIN_SLP_VAL, RTC_CNTL_MIN_SLP_VAL_MIN);

    // set rom&ram timer
    REG_SET_FIELD(RTC_CNTL_TIMER3_REG, RTC_CNTL_ROM_RAM_POWERUP_TIMER, ROM_RAM_POWERUP_CYCLES);
    REG_SET_FIELD(RTC_CNTL_TIMER3_REG, RTC_CNTL_ROM_RAM_WAIT_TIMER, ROM_RAM_WAIT_CYCLES);
    // set wifi timer
    REG_SET_FIELD(RTC_CNTL_TIMER3_REG, RTC_CNTL_WIFI_POWERUP_TIMER, WIFI_POWERUP_CYCLES);
    REG_SET_FIELD(RTC_CNTL_TIMER3_REG, RTC_CNTL_WIFI_WAIT_TIMER, WIFI_WAIT_CYCLES);
    // set rtc peri timer
    REG_SET_FIELD(RTC_CNTL_TIMER4_REG, RTC_CNTL_POWERUP_TIMER, RTC_POWERUP_CYCLES);
    REG_SET_FIELD(RTC_CNTL_TIMER4_REG, RTC_CNTL_WAIT_TIMER, RTC_WAIT_CYCLES);
    // set digital wrap timer
    REG_SET_FIELD(RTC_CNTL_TIMER4_REG, RTC_CNTL_DG_WRAP_POWERUP_TIMER, DG_WRAP_POWERUP_CYCLES);
    REG_SET_FIELD(RTC_CNTL_TIMER4_REG, RTC_CNTL_DG_WRAP_WAIT_TIMER, DG_WRAP_WAIT_CYCLES);
    // set rtc memory timer
    REG_SET_FIELD(RTC_CNTL_TIMER5_REG, RTC_CNTL_RTCMEM_POWERUP_TIMER, RTC_MEM_POWERUP_CYCLES);
    REG_SET_FIELD(RTC_CNTL_TIMER5_REG, RTC_CNTL_RTCMEM_WAIT_TIMER, RTC_MEM_WAIT_CYCLES);

    REG_SET_FIELD(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_LSLP_MEM_FORCE_PU, cfg.lslp_mem_inf_fpu);

    rtc_sleep_pd_config_t pd_cfg = RTC_SLEEP_PD_CONFIG_ALL(cfg.lslp_meminf_pd);
    rtc_sleep_pd(pd_cfg);

    if (cfg.rtc_mem_inf_fpu) {
        SET_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_MEM_FORCE_PU);
    } else {
        CLEAR_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_MEM_FORCE_PU);
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

    if (cfg.modem_pd_en) {
        REG_CLR_BIT(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_WIFI_FORCE_NOISO | RTC_CNTL_WIFI_FORCE_ISO);
        REG_CLR_BIT(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_WIFI_FORCE_PU);
        SET_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_WIFI_PD_EN);
    } else {
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_WIFI_PD_EN);
    }

    if (cfg.rom_mem_pd_en) {
        SET_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_CPU_ROM_RAM_PD_EN);
    } else {
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_CPU_ROM_RAM_PD_EN);
    }

    if (cfg.deep_slp) {
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG,
                RTC_CNTL_DG_PAD_FORCE_ISO | RTC_CNTL_DG_PAD_FORCE_NOISO);
        SET_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_DG_WRAP_PD_EN);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG,
                RTC_CNTL_DG_WRAP_FORCE_PU | RTC_CNTL_DG_WRAP_FORCE_PD);
        CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BIAS_FORCE_NOSLEEP);

        // Shut down parts of RTC which may have been left enabled by the wireless drivers
        CLEAR_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG,
                RTC_CNTL_CKGEN_I2C_PU | RTC_CNTL_PLL_I2C_PU |
                RTC_CNTL_RFRX_PBUS_PU | RTC_CNTL_TXRF_I2C_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BB_I2C_FORCE_PU);
    } else {
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_DG_WRAP_PD_EN);
    }

    REG_SET_FIELD(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_XTL_FORCE_PU, cfg.xtal_fpu);

    if (!cfg.int_8m_pd_en) {
        REG_SET_BIT(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_FORCE_PU);
    } else {
        REG_CLR_BIT(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_FORCE_PU);
    }

    /* enable VDDSDIO control by state machine */
    REG_CLR_BIT(RTC_CNTL_SDIO_CONF_REG, RTC_CNTL_SDIO_FORCE);
    REG_SET_FIELD(RTC_CNTL_SDIO_CONF_REG, RTC_CNTL_SDIO_PD_EN, cfg.vddsdio_pd_en);

    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DBIAS_SLP, cfg.rtc_dbias_slp);
    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DBIAS_WAK, cfg.rtc_dbias_wak);
    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_WAK, cfg.dig_dbias_wak);
    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DIG_DBIAS_SLP, cfg.dig_dbias_slp);
    REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_DBG_ATTEN, cfg.dbg_atten_slp);

    REG_SET_FIELD(RTC_CNTL_SLP_REJECT_CONF_REG, RTC_CNTL_DEEP_SLP_REJECT_EN, cfg.deep_slp_reject);
    REG_SET_FIELD(RTC_CNTL_SLP_REJECT_CONF_REG, RTC_CNTL_LIGHT_SLP_REJECT_EN, cfg.light_slp_reject);
}

void rtc_sleep_low_init(uint32_t slowclk_period)
{
    // set 5 PWC state machine times to fit in main state machine time
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_PLL_BUF_WAIT, RTC_CNTL_PLL_BUF_WAIT_SLP_CYCLES);
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_XTL_BUF_WAIT, rtc_time_us_to_slowclk(RTC_CNTL_XTL_BUF_WAIT_SLP_US, slowclk_period));
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_CK8M_WAIT, RTC_CNTL_CK8M_WAIT_SLP_CYCLES);
}

/* Read back 'reject' status when waking from light or deep sleep */
static uint32_t rtc_sleep_finish(void);

uint32_t rtc_sleep_start(uint32_t wakeup_opt, uint32_t reject_opt)
{
    REG_SET_FIELD(RTC_CNTL_WAKEUP_STATE_REG, RTC_CNTL_WAKEUP_ENA, wakeup_opt);
    WRITE_PERI_REG(RTC_CNTL_SLP_REJECT_CONF_REG, reject_opt);

    SET_PERI_REG_MASK(RTC_CNTL_INT_CLR_REG,
            RTC_CNTL_SLP_REJECT_INT_CLR | RTC_CNTL_SLP_WAKEUP_INT_CLR);

    /* Start entry into sleep mode */
    SET_PERI_REG_MASK(RTC_CNTL_STATE0_REG, RTC_CNTL_SLEEP_EN);

    while (GET_PERI_REG_MASK(RTC_CNTL_INT_RAW_REG,
                             RTC_CNTL_SLP_REJECT_INT_RAW | RTC_CNTL_SLP_WAKEUP_INT_RAW) == 0) {
        ;
    }

    return rtc_sleep_finish();
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

    /* Values used to set the RTC_MEM_CONFG value */
    const unsigned CRC_START_ADDR = 0;
    const unsigned CRC_LEN = 0x7ff;
    const unsigned RTC_MEM_PID = 1;

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
                 "bbci a2, "STR(RTC_MEM_CRC_FINISH_S)", .Lwaitcrc\n"
                 "and a2, a2, %3\n" // clear RTC_MEM_CRC_START
                 "s32i a2, %0, 0\n"
                 "memw\n"

                 /* Store the calculated value in RTC_MEM_CRC_REG */
                 "l32i a2, %4, 0\n"
                 "s32i a2, %5, 0\n"
                 "memw\n"

                 /* Set register bit to go into deep sleep */
                 "l32i a2, %6, 0\n"
                 "or   a2, a2, %7\n"
                 "s32i a2, %6, 0\n"
                 "memw\n"

                 /* Set wait cycle for touch or COCPU after deep sleep. */
                 ".Lwaitsleep:"
                 "memw\n"
                 "l32i a2, %8, 0\n"
                 "and a2, a2, %9\n"
                 "beqz a2, .Lwaitsleep\n"

                 :
                 : "r" (RTC_MEM_CONF), // %0
                   "r" ( (CRC_START_ADDR << RTC_MEM_CRC_ADDR_S)
                         | (CRC_LEN << RTC_MEM_CRC_LEN_S)
                         | (RTC_MEM_PID << RTC_MEM_PID_CONF_S) ), // %1
                   "r" (RTC_MEM_CRC_START), // %2
                   "r" (~RTC_MEM_CRC_START), // %3
                   "r" (RTC_MEM_CRC_RES), // %4
                   "r" (RTC_MEMORY_CRC_REG), // %5
                   "r" (RTC_CNTL_STATE0_REG), // %6
                   "r" (RTC_CNTL_SLEEP_EN), // %7
                   "r" (RTC_CNTL_INT_RAW_REG), // %8
                   "r" (RTC_CNTL_SLP_REJECT_INT_RAW | RTC_CNTL_SLP_WAKEUP_INT_RAW) // %9
                 : "a2" // working register
                 );

    return rtc_sleep_finish();
}

static uint32_t rtc_sleep_finish(void)
{
    /* In deep sleep mode, we never get here */
    uint32_t reject = REG_GET_FIELD(RTC_CNTL_INT_RAW_REG, RTC_CNTL_SLP_REJECT_INT_RAW);
    SET_PERI_REG_MASK(RTC_CNTL_INT_CLR_REG,
            RTC_CNTL_SLP_REJECT_INT_CLR | RTC_CNTL_SLP_WAKEUP_INT_CLR);

    /* restore DBG_ATTEN to the default value */
    REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_DBG_ATTEN, RTC_CNTL_DBG_ATTEN_DEFAULT);
    return reject;
}
