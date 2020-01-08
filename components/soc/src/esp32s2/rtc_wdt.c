// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

#include "soc/rtc_wdt.h"
#include "soc/rtc.h"
#include "soc/efuse_periph.h"


bool rtc_wdt_get_protect_status(void)
{
    return READ_PERI_REG(RTC_CNTL_WDTWPROTECT_REG) != RTC_CNTL_WDT_WKEY_VALUE;
}

void rtc_wdt_protect_off(void)
{
    WRITE_PERI_REG(RTC_CNTL_WDTWPROTECT_REG, RTC_CNTL_WDT_WKEY_VALUE);
}

void rtc_wdt_protect_on(void)
{
    WRITE_PERI_REG(RTC_CNTL_WDTWPROTECT_REG, 0);
}


void rtc_wdt_enable(void)
{
    REG_SET_BIT(RTC_CNTL_WDTFEED_REG, RTC_CNTL_WDT_FEED);
    SET_PERI_REG_MASK(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_EN | RTC_CNTL_WDT_PAUSE_IN_SLP);
}

void rtc_wdt_flashboot_mode_enable(void)
{
    REG_SET_BIT(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_FLASHBOOT_MOD_EN);
}

void rtc_wdt_disable(void)
{
    bool protect = rtc_wdt_get_protect_status();
    if (protect) {
        rtc_wdt_protect_off();
    }
    REG_SET_BIT(RTC_CNTL_WDTFEED_REG, RTC_CNTL_WDT_FEED);
    rtc_wdt_set_stage(RTC_WDT_STAGE0, RTC_WDT_STAGE_ACTION_OFF);
    rtc_wdt_set_stage(RTC_WDT_STAGE1, RTC_WDT_STAGE_ACTION_OFF);
    rtc_wdt_set_stage(RTC_WDT_STAGE2, RTC_WDT_STAGE_ACTION_OFF);
    rtc_wdt_set_stage(RTC_WDT_STAGE3, RTC_WDT_STAGE_ACTION_OFF);
    REG_CLR_BIT(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_FLASHBOOT_MOD_EN);
    REG_CLR_BIT(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_EN);
    if (protect) {
        rtc_wdt_protect_on();
    }
}

void rtc_wdt_feed(void)
{
    bool protect = rtc_wdt_get_protect_status();
    if (protect) {
        rtc_wdt_protect_off();
    }
    REG_SET_BIT(RTC_CNTL_WDTFEED_REG, RTC_CNTL_WDT_FEED);
    if (protect) {
        rtc_wdt_protect_on();
    }
}

static uint32_t get_addr_reg(rtc_wdt_stage_t stage)
{
    uint32_t reg;
    if (stage == RTC_WDT_STAGE0) {
        reg = RTC_CNTL_WDTCONFIG1_REG;
    } else if (stage == RTC_WDT_STAGE1) {
        reg = RTC_CNTL_WDTCONFIG2_REG;
    } else if (stage == RTC_WDT_STAGE2) {
        reg = RTC_CNTL_WDTCONFIG3_REG;
    } else {
    	reg = RTC_CNTL_WDTCONFIG4_REG;
    }
    return reg;
}

esp_err_t rtc_wdt_set_time(rtc_wdt_stage_t stage, unsigned int timeout_ms)
{
    if (stage > 3) {
        return ESP_ERR_INVALID_ARG;
    }
    uint32_t timeout = (uint32_t) ((uint64_t) rtc_clk_slow_freq_get_hz() * timeout_ms / 1000);
    if (stage == RTC_WDT_STAGE0) {
        timeout = timeout >> (1 + REG_GET_FIELD(EFUSE_RD_REPEAT_DATA1_REG, EFUSE_WDT_DELAY_SEL));
    }
    WRITE_PERI_REG(get_addr_reg(stage), timeout);
    return ESP_OK;
}

esp_err_t rtc_wdt_get_timeout(rtc_wdt_stage_t stage, unsigned int* timeout_ms)
{
    if (stage > 3) {
        return ESP_ERR_INVALID_ARG;
    }
    uint32_t time_tick;
    time_tick = READ_PERI_REG(get_addr_reg(stage));
    *timeout_ms = time_tick * 1000 / rtc_clk_slow_freq_get_hz();

    return ESP_OK;
}

esp_err_t rtc_wdt_set_stage(rtc_wdt_stage_t stage, rtc_wdt_stage_action_t stage_sel)
{
    if (stage > 3 || stage_sel > 4) {
        return ESP_ERR_INVALID_ARG;
    }
    if (stage == RTC_WDT_STAGE0) {
        REG_SET_FIELD(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_STG0, stage_sel);
    } else if (stage == RTC_WDT_STAGE1) {
        REG_SET_FIELD(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_STG1, stage_sel);
    } else if (stage == RTC_WDT_STAGE2) {
        REG_SET_FIELD(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_STG2, stage_sel);
    } else {
        REG_SET_FIELD(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_STG3, stage_sel);
    }

    return ESP_OK;
}

esp_err_t rtc_wdt_set_length_of_reset_signal(rtc_wdt_reset_sig_t reset_src, rtc_wdt_length_sig_t reset_signal_length)
{
    if (reset_src > 1 || reset_signal_length > 7) {
        return ESP_ERR_INVALID_ARG;
    }
    if (reset_src == 0) {
        REG_SET_FIELD(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_SYS_RESET_LENGTH, reset_signal_length);
    } else {
        REG_SET_FIELD(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_CPU_RESET_LENGTH, reset_signal_length);
    }

    return ESP_OK;
}

bool rtc_wdt_is_on(void)
{
    return (REG_GET_BIT(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_EN) != 0) || (REG_GET_BIT(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_FLASHBOOT_MOD_EN) != 0);
}
