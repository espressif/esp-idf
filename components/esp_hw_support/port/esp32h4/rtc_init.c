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
#include "soc/efuse_periph.h"
#include "soc/gpio_reg.h"
#include "soc/spi_mem_reg.h"
#include "soc/extmem_reg.h"
#include "soc/system_reg.h"
#include "soc/syscon_reg.h"
#include "regi2c_ctrl.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "i2c_pmu.h"
#include "soc/clkrst_reg.h"
#ifndef BOOTLOADER_BUILD
#include "esp_private/sar_periph_ctrl.h"
#endif

void pmu_ctl(void);
void dcdc_ctl(uint32_t mode);
void regulator_slt(regulator_config_t regula_cfg);

void rtc_init(rtc_config_t cfg)
{
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

        if (!cfg.bbpll_fpu) {
            CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BBPLL_FORCE_PU);
            CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BBPLL_I2C_FORCE_PU);
            CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BB_I2C_FORCE_PU);
        } else {
            SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BBPLL_FORCE_PU);
            SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BBPLL_I2C_FORCE_PU);
            SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BB_I2C_FORCE_PU);
        }

#if CONFIG_IDF_TARGET_ESP32H4_BETA_VERSION_2
        CLEAR_PERI_REG_MASK(RTC_CNTL_REGULATOR_REG, RTC_CNTL_REGULATOR_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_REGULATOR_REG, RTC_CNTL_DBOOST_FORCE_PU);
#elif CONFIG_IDF_TARGET_ESP32H4_BETA_VERSION_1
        CLEAR_PERI_REG_MASK(RTC_CNTL_REG, RTC_CNTL_REGULATOR_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_REG, RTC_CNTL_DBOOST_FORCE_PU);
#endif

        // clear i2c_reset_protect pd force, need tested in low temperature.
        CLEAR_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG,RTC_CNTL_I2C_RESET_POR_FORCE_PD);

        /* If this mask is enabled, all soc memories cannot enter power down mode */
        /* We should control soc memory power down mode from RTC, so we will not touch this register any more */
        CLEAR_PERI_REG_MASK(SYSTEM_MEM_PD_MASK_REG, SYSTEM_LSLP_MEM_PD_MASK);

        /* If this pd_cfg is set to 1, all memory won't enter low power mode during light sleep */
        /* If this pd_cfg is set to 0, all memory will enter low power mode during light sleep */
        rtc_sleep_pu_config_t pu_cfg = RTC_SLEEP_PU_CONFIG_ALL(0);
        rtc_sleep_pu(pu_cfg);

        //cancel digital PADS force pu
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_CPU_TOP_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_WIFI_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_FASTMEM_FORCE_LPU); //
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_DG_PERI_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_BT_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_DG_WRAP_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_DG_MEM_FORCE_PU); //
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_LSLP_MEM_FORCE_PU); //

        //cancel digital PADS force no iso
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_WRAP_FORCE_NOISO);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_WIFI_FORCE_NOISO);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_CPU_TOP_FORCE_NOISO);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_PERI_FORCE_NOISO);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_BT_FORCE_NOISO);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_PAD_FORCE_NOISO); //
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_MEM_FORCE_NOISO); //

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
    if (cfg.pmu_ctl) {
        /* pmu init*/
        pmu_ctl();
    }
    /* config dcdc frequency */
    REG_SET_FIELD(RTC_CNTL_DCDC_CTRL0_REG, RTC_CNTL_FSW_DCDC, RTC_CNTL_DCDC_FREQ_DEFAULT);

#ifndef BOOTLOADER_BUILD
    //initialise SAR related peripheral register settings
    sar_periph_ctrl_init();
#endif
}

void pmu_ctl(void)
{
    pmu_config_t pmu_cfg = PMU_CONFIG_DEFAULT();
    REGI2C_WRITE_MASK(I2C_PMU, I2C_PMU_OR_EN_CONT_CAL, pmu_cfg.or_en_cont_cal);
    REGI2C_WRITE_MASK(I2C_PMU, I2C_PMU_ENX_RTC_DREG, pmu_cfg.enx_rtc_dreg);
    REGI2C_WRITE_MASK(I2C_PMU, I2C_PMU_ENX_DIG_DREG, pmu_cfg.enx_dig_dreg);
    REGI2C_WRITE_MASK(I2C_PMU, I2C_PMU_EN_I2C_RTC_DREG, pmu_cfg.en_i2c_rtc_dreg);
    REGI2C_WRITE_MASK(I2C_PMU, I2C_PMU_EN_I2C_DIG_DREG, pmu_cfg.en_i2c_dig_dreg);
    REGI2C_WRITE_MASK(I2C_PMU, I2C_PMU_EN_I2C_RTC_DREG_SLP, pmu_cfg.en_i2c_rtc_dreg_slp);
    REGI2C_WRITE_MASK(I2C_PMU, I2C_PMU_EN_I2C_DIG_DREG_SLP, pmu_cfg.en_i2c_dig_dreg_slp);
    REGI2C_WRITE_MASK(I2C_PMU, I2C_PMU_OR_XPD_RTC_SLAVE_3P3, pmu_cfg.or_xpd_rtc_slave_3p3);
    REGI2C_WRITE_MASK(I2C_PMU, I2C_PMU_OR_XPD_RTC_REG, pmu_cfg.or_xpd_rtc_reg);
    REGI2C_WRITE_MASK(I2C_PMU, I2C_PMU_OR_XPD_DIG_REG, pmu_cfg.or_xpd_dig_reg);
    REGI2C_WRITE_MASK(I2C_PMU, I2C_PMU_OR_PD_RTC_REG_SLP, pmu_cfg.or_pd_rtc_reg_slp);
    REGI2C_WRITE_MASK(I2C_PMU, I2C_PMU_OR_PD_DIG_REG_SLP, pmu_cfg.or_pd_dig_reg_slp);
    REGI2C_WRITE_MASK(I2C_PMU, I2C_PMU_OR_XPD_DCDC, pmu_cfg.or_xpd_dcdc);
    REGI2C_WRITE_MASK(I2C_PMU, I2C_PMU_OR_DISALBE_DEEP_SLEEP_DCDC, pmu_cfg.or_disalbe_deep_sleep_dcdc);
    REGI2C_WRITE_MASK(I2C_PMU, I2C_PMU_OR_DISALBE_LIGHT_SLEEP_DCDC, pmu_cfg.or_disalbe_light_sleep_dcdc);
    REGI2C_WRITE_MASK(I2C_PMU, I2C_PMU_OR_ENALBE_TRX_MODE_DCDC, pmu_cfg.or_enalbe_trx_mode_dcdc);
    REGI2C_WRITE_MASK(I2C_PMU, I2C_PMU_OR_ENX_REG_DCDC, pmu_cfg.or_enx_reg_dcdc);
    REGI2C_WRITE_MASK(I2C_PMU, I2C_PMU_OR_UNLOCK_DCDC, pmu_cfg.or_unlock_dcdc);
    REGI2C_WRITE_MASK(I2C_PMU, I2C_PMU_OR_FORCE_LOCK_DCDC, pmu_cfg.or_force_lock_dcdc);
    // REGI2C_WRITE_MASK(I2C_PMU, I2C_PMU_OR_ENB_SLOW_CLK, pmu_cfg.or_enb_slow_clk);
    REGI2C_WRITE_MASK(I2C_PMU, I2C_PMU_OR_XPD_TRX, pmu_cfg.or_xpd_trx);
    REGI2C_WRITE_MASK(I2C_PMU, I2C_PMU_OR_EN_RESET_CHIP, pmu_cfg.or_en_reset_chip);
    REGI2C_WRITE_MASK(I2C_PMU, I2C_PMU_OR_FORCE_XPD_REG_SLAVE, pmu_cfg.or_force_xpd_reg_slave);
}

void dslp_osc_pd(void){
     REG_SET_FIELD(RTC_CNTL_RC32K_CTRL_REG,RTC_CNTL_RC32K_XPD, 0);
     REG_SET_FIELD(RTC_CNTL_PLL8M_REG, RTC_CNTL_XPD_PLL8M, 0);
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

    // Otherwise, VDD_SDIO is controlled by bootstrapping pin
    uint32_t strap_reg = REG_READ(GPIO_STRAP_REG);
    result.force = 0;
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
