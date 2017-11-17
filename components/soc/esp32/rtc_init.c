// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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
#include "soc/dport_reg.h"
#include "soc/efuse_reg.h"
#include "soc/gpio_reg.h"


void rtc_init(rtc_config_t cfg)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_PVTMON_PU);

    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_PLL_BUF_WAIT, cfg.pll_wait);
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_XTL_BUF_WAIT, cfg.xtal_wait);
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_CK8M_WAIT, cfg.ck8m_wait);

    REG_SET_FIELD(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_DBG_ATTEN, 0x3);
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
}

rtc_vddsdio_config_t rtc_vddsdio_get_config()
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
    uint32_t efuse_reg = REG_READ(EFUSE_BLK0_RDATA4_REG);
    if (efuse_reg & EFUSE_RD_SDIO_FORCE) {
        // Get configuration from EFUSE
        result.force = 0;
        result.enable = (efuse_reg & EFUSE_RD_XPD_SDIO_REG_M) >> EFUSE_RD_XPD_SDIO_REG_S;
        result.tieh = (efuse_reg & EFUSE_RD_SDIO_TIEH_M) >> EFUSE_RD_SDIO_TIEH_S;
        // in this case, DREFH/M/L are also set from EFUSE
        result.drefh = (efuse_reg & EFUSE_RD_SDIO_DREFH_M) >> EFUSE_RD_SDIO_DREFH_S;
        result.drefm = (efuse_reg & EFUSE_RD_SDIO_DREFM_M) >> EFUSE_RD_SDIO_DREFM_S;
        result.drefl = (efuse_reg & EFUSE_RD_SDIO_DREFL_M) >> EFUSE_RD_SDIO_DREFL_S;
        return result;
    }

    // Otherwise, VDD_SDIO is controlled by bootstrapping pin
    uint32_t strap_reg = REG_READ(GPIO_STRAP_REG);
    result.force = 0;
    result.tieh = (strap_reg & BIT(5)) ? 0 : 1;
    result.enable = result.tieh == 0; // only power on the regulator if VDD=1.8
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
