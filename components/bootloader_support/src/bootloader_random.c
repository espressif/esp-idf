// Copyright 2010-2016 Espressif Systems (Shanghai) PTE LTD
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
#include "bootloader_random.h"
#include "soc/cpu.h"
#include "soc/wdev_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "soc/syscon_reg.h"
#include "soc/dport_reg.h"
#include "soc/i2s_reg.h"
#include "esp_log.h"

#ifndef BOOTLOADER_BUILD
#include "esp_system.h"
#endif

const char *TAG = "boot_rng";

void bootloader_fill_random(void *buffer, size_t length)
{
    uint8_t *buffer_bytes = (uint8_t *)buffer;
    uint32_t random;
#ifdef BOOTLOADER_BUILD
    uint32_t start, now;
#endif

    for (int i = 0; i < length; i++) {
        if (i == 0 || i % 4 == 0) { /* redundant check is for a compiler warning */
#ifdef BOOTLOADER_BUILD
            /* in bootloader with ADC feeding HWRNG, we accumulate 1
               bit of entropy per 40 APB cycles (==80 CPU cycles.)

               To avoid reading the entire RNG hardware state out
               as-is, we repeatedly read the RNG register and XOR all
               values.
            */
            random = REG_READ(WDEV_RND_REG);
            RSR(CCOUNT, start);
            do {
                random ^= REG_READ(WDEV_RND_REG);
                RSR(CCOUNT, now);
            } while(now - start < 80*32*2); /* extra factor of 2 is precautionary */
#else
            random = esp_random();
#endif
        }

        buffer_bytes[i] = random >> ((i % 4) * 8);
    }
}

void bootloader_random_enable(void)
{
    /* Ensure the hardware RNG is enabled following a soft reset.  This should always be the case already (this clock is
       never disabled while the CPU is running), this is a "belts and braces" type check.
     */
    DPORT_SET_PERI_REG_MASK(DPORT_WIFI_CLK_EN_REG, DPORT_WIFI_CLK_RNG_EN);

    /* Enable SAR ADC in test mode to feed ADC readings of the 1.1V
       reference via I2S into the RNG entropy input.

       Note: I2S requires the PLL to be running, so the call to rtc_set_cpu_freq(CPU_80M)
       in early bootloader startup must have been made.
    */
    SET_PERI_REG_BITS(RTC_CNTL_TEST_MUX_REG, RTC_CNTL_DTEST_RTC, 2, RTC_CNTL_DTEST_RTC_S);
    SET_PERI_REG_MASK(RTC_CNTL_TEST_MUX_REG, RTC_CNTL_ENT_RTC);
    SET_PERI_REG_MASK(SENS_SAR_START_FORCE_REG, SENS_SAR2_EN_TEST);

    DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_I2S0_CLK_EN);
    CLEAR_PERI_REG_MASK(SENS_SAR_START_FORCE_REG, SENS_ULP_CP_FORCE_START_TOP);
    CLEAR_PERI_REG_MASK(SENS_SAR_START_FORCE_REG, SENS_ULP_CP_START_TOP);
    // Test pattern configuration byte 0xAD:
    //--[7:4] channel_sel: 10-->en_test
    //--[3:2] bit_width  : 3-->12bit
    //--[1:0] atten      : 1-->3dB attenuation
    WRITE_PERI_REG(SYSCON_SARADC_SAR2_PATT_TAB1_REG, 0xADADADAD);
    WRITE_PERI_REG(SYSCON_SARADC_SAR2_PATT_TAB2_REG, 0xADADADAD);
    WRITE_PERI_REG(SYSCON_SARADC_SAR2_PATT_TAB3_REG, 0xADADADAD);
    WRITE_PERI_REG(SYSCON_SARADC_SAR2_PATT_TAB4_REG, 0xADADADAD);

    SET_PERI_REG_BITS(SENS_SAR_MEAS_WAIT2_REG, SENS_FORCE_XPD_SAR, 3, SENS_FORCE_XPD_SAR_S);
    SET_PERI_REG_MASK(SENS_SAR_READ_CTRL_REG, SENS_SAR1_DIG_FORCE);
    SET_PERI_REG_MASK(SENS_SAR_READ_CTRL2_REG, SENS_SAR2_DIG_FORCE);
    SET_PERI_REG_MASK(SYSCON_SARADC_CTRL_REG, SYSCON_SARADC_SAR2_MUX);
    SET_PERI_REG_BITS(SYSCON_SARADC_CTRL_REG, SYSCON_SARADC_SAR_CLK_DIV, 4, SYSCON_SARADC_SAR_CLK_DIV_S);

    SET_PERI_REG_BITS(SYSCON_SARADC_FSM_REG, SYSCON_SARADC_RSTB_WAIT, 8, SYSCON_SARADC_RSTB_WAIT_S); /* was 1 */
    SET_PERI_REG_BITS(SYSCON_SARADC_FSM_REG, SYSCON_SARADC_START_WAIT, 10, SYSCON_SARADC_START_WAIT_S);
    SET_PERI_REG_BITS(SYSCON_SARADC_CTRL_REG, SYSCON_SARADC_WORK_MODE, 0, SYSCON_SARADC_WORK_MODE_S);
    SET_PERI_REG_MASK(SYSCON_SARADC_CTRL_REG, SYSCON_SARADC_SAR_SEL);
    CLEAR_PERI_REG_MASK(SYSCON_SARADC_CTRL_REG, SYSCON_SARADC_DATA_SAR_SEL);

    SET_PERI_REG_BITS(I2S_SAMPLE_RATE_CONF_REG(0), I2S_RX_BCK_DIV_NUM, 20, I2S_RX_BCK_DIV_NUM_S);

    SET_PERI_REG_MASK(SYSCON_SARADC_CTRL_REG,SYSCON_SARADC_DATA_TO_I2S);

    CLEAR_PERI_REG_MASK(I2S_CONF2_REG(0), I2S_CAMERA_EN);
    SET_PERI_REG_MASK(I2S_CONF2_REG(0), I2S_LCD_EN);
    SET_PERI_REG_MASK(I2S_CONF2_REG(0), I2S_DATA_ENABLE);
    SET_PERI_REG_MASK(I2S_CONF2_REG(0), I2S_DATA_ENABLE_TEST_EN);
    SET_PERI_REG_MASK(I2S_CONF_REG(0), I2S_RX_START);
}

void bootloader_random_disable(void)
{
    /* Disable i2s clock */
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_I2S0_CLK_EN);


    /* Reset some i2s configuration (possibly redundant as we reset entire
       I2S peripheral further down). */
    CLEAR_PERI_REG_MASK(I2S_CONF2_REG(0), I2S_CAMERA_EN);
    CLEAR_PERI_REG_MASK(I2S_CONF2_REG(0), I2S_LCD_EN);
    CLEAR_PERI_REG_MASK(I2S_CONF2_REG(0), I2S_DATA_ENABLE_TEST_EN);
    CLEAR_PERI_REG_MASK(I2S_CONF2_REG(0), I2S_DATA_ENABLE);
    CLEAR_PERI_REG_MASK(I2S_CONF_REG(0), I2S_RX_START);

    /* Restore SYSCON mode registers */
    CLEAR_PERI_REG_MASK(SENS_SAR_READ_CTRL_REG, SENS_SAR1_DIG_FORCE);
    CLEAR_PERI_REG_MASK(SENS_SAR_READ_CTRL2_REG, SENS_SAR2_DIG_FORCE);

    /* Restore SAR ADC mode */
    CLEAR_PERI_REG_MASK(SENS_SAR_START_FORCE_REG, SENS_SAR2_EN_TEST);
    CLEAR_PERI_REG_MASK(SYSCON_SARADC_CTRL_REG, SYSCON_SARADC_SAR2_MUX
                        | SYSCON_SARADC_SAR_SEL | SYSCON_SARADC_DATA_TO_I2S);
    SET_PERI_REG_BITS(SENS_SAR_MEAS_WAIT2_REG, SENS_FORCE_XPD_SAR, 0, SENS_FORCE_XPD_SAR_S);
    SET_PERI_REG_BITS(SYSCON_SARADC_FSM_REG, SYSCON_SARADC_START_WAIT, 8, SYSCON_SARADC_START_WAIT_S);

    /* Reset i2s peripheral */
    DPORT_SET_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_I2S0_RST);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_I2S0_RST);

    /* Disable pull supply voltage to SAR ADC */
    CLEAR_PERI_REG_MASK(RTC_CNTL_TEST_MUX_REG, RTC_CNTL_ENT_RTC);
    SET_PERI_REG_BITS(RTC_CNTL_TEST_MUX_REG, RTC_CNTL_DTEST_RTC, 0, RTC_CNTL_DTEST_RTC_S);
}
