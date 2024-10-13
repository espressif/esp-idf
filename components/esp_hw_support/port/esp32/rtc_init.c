/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/rtc_periph.h"
#include "soc/dport_reg.h"
#include "hal/efuse_ll.h"
#include "soc/gpio_periph.h"
#ifndef BOOTLOADER_BUILD
#include "esp_private/sar_periph_ctrl.h"
#endif


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

    CLEAR_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_PVTMON_PU | RTC_CNTL_TXRF_I2C_PU |
            RTC_CNTL_RFRX_PBUS_PU | RTC_CNTL_CKGEN_I2C_PU | RTC_CNTL_PLL_I2C_PU);

    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_PLL_BUF_WAIT, cfg.pll_wait);
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_XTL_BUF_WAIT, cfg.xtal_wait);
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_CK8M_WAIT, cfg.ck8m_wait);

    REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_DBG_ATTEN, RTC_CNTL_DBG_ATTEN_DEFAULT);
    SET_PERI_REG_MASK(RTC_CNTL_BIAS_CONF_REG,
            RTC_CNTL_DEC_HEARTBEAT_WIDTH | RTC_CNTL_INC_HEARTBEAT_PERIOD);

    /* Reset RTC bias to default value (needed if waking up from deep sleep) */
    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DBIAS_WAK, RTC_CNTL_DBIAS_1V10);
    REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DBIAS_SLP, RTC_CNTL_DBIAS_1V10);

    if (cfg.clkctl_init) {
        //clear CMMU clock force on
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PRO_CACHE_CTRL1_REG, DPORT_PRO_CMMU_FORCE_ON);
        DPORT_CLEAR_PERI_REG_MASK(DPORT_APP_CACHE_CTRL1_REG, DPORT_APP_CMMU_FORCE_ON);
        //clear rom clock force on
        DPORT_SET_PERI_REG_BITS(DPORT_ROM_FO_CTRL_REG, DPORT_SHARE_ROM_FO, 0, DPORT_SHARE_ROM_FO_S);
        DPORT_CLEAR_PERI_REG_MASK(DPORT_ROM_FO_CTRL_REG, DPORT_APP_ROM_FO);
        DPORT_CLEAR_PERI_REG_MASK(DPORT_ROM_FO_CTRL_REG, DPORT_PRO_ROM_FO);
        //clear sram clock force on
        DPORT_CLEAR_PERI_REG_MASK(DPORT_SRAM_FO_CTRL_0_REG, DPORT_SRAM_FO_0);
        DPORT_CLEAR_PERI_REG_MASK(DPORT_SRAM_FO_CTRL_1_REG, DPORT_SRAM_FO_1);
        //clear tag clock force on
        DPORT_CLEAR_PERI_REG_MASK(DPORT_TAG_FO_CTRL_REG, DPORT_APP_CACHE_TAG_FORCE_ON);
        DPORT_CLEAR_PERI_REG_MASK(DPORT_TAG_FO_CTRL_REG, DPORT_PRO_CACHE_TAG_FORCE_ON);
    }

    if (cfg.pwrctl_init) {
        CLEAR_PERI_REG_MASK(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_FORCE_PU);
        //cancel xtal force pu
        CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_XTL_FORCE_PU);
        //cancel BIAS force pu
        CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BIAS_CORE_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BIAS_I2C_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BIAS_FORCE_NOSLEEP);
        // bias follow 8M
        SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BIAS_CORE_FOLW_8M);
        SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BIAS_I2C_FOLW_8M);
        SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BIAS_SLEEP_FOLW_8M);
        // CLEAR APLL close
        CLEAR_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_PLLA_FORCE_PU);
        SET_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_PLLA_FORCE_PD);
        CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BBPLL_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BBPLL_I2C_FORCE_PU);
        //cancel RTC REG force PU
        CLEAR_PERI_REG_MASK(RTC_CNTL_REG, RTC_CNTL_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_REG, RTC_CNTL_DBOOST_FORCE_PU);
        if (cfg.rtc_dboost_fpd) {
            SET_PERI_REG_MASK(RTC_CNTL_REG, RTC_CNTL_DBOOST_FORCE_PD);
        } else {
            CLEAR_PERI_REG_MASK(RTC_CNTL_REG, RTC_CNTL_DBOOST_FORCE_PD);
        }
        //cancel digital pu force
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_LSLP_MEM_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_DG_WRAP_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_WIFI_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_CPU_ROM_RAM_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_MEM_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_PWC_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_WRAP_FORCE_NOISO);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_WIFI_FORCE_NOISO);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_CPU_ROM_RAM_FORCE_NOISO);
        CLEAR_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_MEM_FORCE_NOISO);
        CLEAR_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_FORCE_NOISO);
        //cancel digital PADS force no iso
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_PAD_FORCE_UNHOLD);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_PAD_FORCE_NOISO);
    }
    /* force power down modem(wifi and btdm) power domain */
    SET_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_WIFI_FORCE_ISO);
    SET_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_WIFI_FORCE_PD);

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
    }
    if (efuse_ll_get_sdio_force()) {
        // Get configuration from EFUSE
        result.force = 0;
        result.enable = efuse_ll_get_xpd_sdio();
        result.tieh = efuse_ll_get_sdio_tieh();
        //DREFH/M/L eFuse are used for EFUSE_ADC_VREF instead. Therefore tuning
        //will only be available on older chips that don't have EFUSE_ADC_VREF
        if(efuse_ll_get_blk3_part_reserve() == 0){
            //BLK3_PART_RESERVE indicates the presence of EFUSE_ADC_VREF
            // in this case, DREFH/M/L are also set from EFUSE
            result.drefh = efuse_ll_get_sdio_drefh();
            result.drefm = efuse_ll_get_sdio_drefm();
            result.drefl = efuse_ll_get_sdio_drefl();
        }
        return result;
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
