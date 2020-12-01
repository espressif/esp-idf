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
#include "sdkconfig.h"
#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/efuse_periph.h"
#include "soc/gpio_reg.h"
#include "soc/spi_mem_reg.h"
#include "soc/extmem_reg.h"
#include "soc/system_reg.h"
#include "i2c_rtc_clk.h"

void rtc_init(rtc_config_t cfg)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_PVTMON_PU);
    rtc_clk_set_xtal_wait();
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_PLL_BUF_WAIT, cfg.pll_wait);
    REG_SET_FIELD(RTC_CNTL_TIMER1_REG, RTC_CNTL_CK8M_WAIT, cfg.ck8m_wait);

    /* Moved from rtc sleep to rtc init to save sleep function running time */
    // set shortest possible sleep time limit
    //REG_SET_FIELD(RTC_CNTL_TIMER5_REG, RTC_CNTL_MIN_SLP_VAL, RTC_CNTL_MIN_SLP_VAL_MIN);

    /* This power domian removed
    * set rom&ram timer
    * REG_SET_FIELD(RTC_CNTL_TIMER3_REG, RTC_CNTL_ROM_RAM_POWERUP_TIMER, ROM_RAM_POWERUP_CYCLES);
    * REG_SET_FIELD(RTC_CNTL_TIMER3_REG, RTC_CNTL_ROM_RAM_WAIT_TIMER, ROM_RAM_WAIT_CYCLES);
    */
    // set wifi timer
    rtc_init_config_t rtc_init_cfg = RTC_INIT_CONFIG_DEFAULT();
    REG_SET_FIELD(RTC_CNTL_TIMER3_REG, RTC_CNTL_WIFI_POWERUP_TIMER, rtc_init_cfg.wifi_powerup_cycles);
    REG_SET_FIELD(RTC_CNTL_TIMER3_REG, RTC_CNTL_WIFI_WAIT_TIMER, rtc_init_cfg.wifi_wait_cycles);
    // set rtc peri timer
    //REG_SET_FIELD(RTC_CNTL_TIMER4_REG, RTC_CNTL_POWERUP_TIMER, rtc_init_cfg.rtc_powerup_cycles);
    //REG_SET_FIELD(RTC_CNTL_TIMER4_REG, RTC_CNTL_WAIT_TIMER, rtc_init_cfg.rtc_wait_cycles);
    // set digital wrap timer
    REG_SET_FIELD(RTC_CNTL_TIMER4_REG, RTC_CNTL_DG_WRAP_POWERUP_TIMER, rtc_init_cfg.dg_wrap_powerup_cycles);
    REG_SET_FIELD(RTC_CNTL_TIMER4_REG, RTC_CNTL_DG_WRAP_WAIT_TIMER, rtc_init_cfg.dg_wrap_wait_cycles);
    // set rtc memory timer
    //REG_SET_FIELD(RTC_CNTL_TIMER5_REG, RTC_CNTL_RTCMEM_POWERUP_TIMER, rtc_init_cfg.rtc_mem_powerup_cycles);
    //REG_SET_FIELD(RTC_CNTL_TIMER5_REG, RTC_CNTL_RTCMEM_WAIT_TIMER, rtc_init_cfg.rtc_mem_wait_cycles);

    //SET_PERI_REG_MASK(RTC_CNTL_BIAS_CONF_REG, RTC_CNTL_INC_HEARTBEAT_PERIOD);

    /* Reset RTC bias to default value (needed if waking up from deep sleep) */
    //REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DBIAS_WAK, RTC_CNTL_DBIAS_1V10);
    //REG_SET_FIELD(RTC_CNTL_REG, RTC_CNTL_DBIAS_SLP, RTC_CNTL_DBIAS_1V10);


    if (cfg.clkctl_init) {
        //clear CMMU clock force on
        CLEAR_PERI_REG_MASK(EXTMEM_CACHE_MMU_POWER_CTRL_REG, EXTMEM_CACHE_MMU_MEM_FORCE_ON);
        //clear rom clock force on
        //REG_SET_FIELD(SYSTEM_ROM_CTRL_0_REG, SYSTEM_ROM_IRAM0_CLKGATE_FORCE_ON, 0);
        //clear sram clock force on
        //REG_SET_FIELD(SYSTEM_SRAM_CTRL_0_REG, SYSTEM_SRAM_CLKGATE_FORCE_ON, 0);
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
        // CLEAR APLL close
        CLEAR_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_PLLA_FORCE_PU);
        SET_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_PLLA_FORCE_PD);

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
        //cancel RTC REG force PU
        //CLEAR_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_REG, RTC_CNTL_REGULATOR_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_REG, RTC_CNTL_DBOOST_FORCE_PU);

        //combine two rtc memory options
        //CLEAR_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_MEM_FORCE_PU);
        //CLEAR_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_MEM_FORCE_NOISO);

        if (cfg.rtc_dboost_fpd) {
            SET_PERI_REG_MASK(RTC_CNTL_REG, RTC_CNTL_DBOOST_FORCE_PD);
        } else {
            CLEAR_PERI_REG_MASK(RTC_CNTL_REG, RTC_CNTL_DBOOST_FORCE_PD);
        }

        //cancel sar i2c pd force
        //CLEAR_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_SAR_I2C_FORCE_PD);
        //cancel digital pu force
        //CLEAR_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_MEM_FORCE_PU);
        //cannel i2c_reset_protect pd force
        //CLEAR_PERI_REG_MASK(RTC_CNTL_ANA_CONF_REG,RTC_CNTL_I2C_RESET_POR_FORCE_PD);

        /* If this mask is enabled, all soc memories cannot enter power down mode */
        /* We should control soc memory power down mode from RTC, so we will not touch this register any more */
        //CLEAR_PERI_REG_MASK(SYSTEM_MEM_PD_MASK_REG, SYSTEM_LSLP_MEM_PD_MASK);
        /* If this pd_cfg is set to 1, all memory won't enter low power mode during light sleep */
        /* If this pd_cfg is set to 0, all memory will enter low power mode during light sleep */
        rtc_sleep_pd_config_t pd_cfg = RTC_SLEEP_PD_CONFIG_ALL(0);
        rtc_sleep_pd(pd_cfg);

        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_DG_WRAP_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_WIFI_FORCE_PU);
        // ROM_RAM power domain is removed
        // CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PWC_REG, RTC_CNTL_CPU_ROM_RAM_FORCE_PU);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_WRAP_FORCE_NOISO);
        CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_WIFI_FORCE_NOISO);
        // CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_CPU_ROM_RAM_FORCE_NOISO);
        //CLEAR_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_FORCE_NOISO);
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
