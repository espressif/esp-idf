/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <freertos/FreeRTOS.h>
#include "soc/clk_ctrl_os.h"
#include "sdkconfig.h"

#define DELAY_RTC_CLK_SWITCH 5

static portMUX_TYPE periph_spinlock = portMUX_INITIALIZER_UNLOCKED;

static uint8_t s_periph_ref_counts = 0;
static uint32_t s_rtc_clk_freq = 0; // Frequency of the 8M/256 clock in Hz

bool periph_rtc_dig_clk8m_enable(void)
{
    portENTER_CRITICAL(&periph_spinlock);
    if (s_periph_ref_counts == 0) {
        rtc_dig_clk8m_enable();
#if CONFIG_IDF_TARGET_ESP32H2
        s_rtc_clk_freq = rtc_clk_freq_cal(rtc_clk_cal(RTC_CAL_RC32K, 100));
#else
        s_rtc_clk_freq = rtc_clk_freq_cal(rtc_clk_cal(RTC_CAL_8MD256, 100));
#endif
        if (s_rtc_clk_freq == 0) {
            portEXIT_CRITICAL(&periph_spinlock);
            return false;
        }
    }
    s_periph_ref_counts++;
    portEXIT_CRITICAL(&periph_spinlock);
    return true;
}

uint32_t periph_rtc_dig_clk8m_get_freq(void)
{
    return s_rtc_clk_freq * 256;
}

void periph_rtc_dig_clk8m_disable(void)
{
    portENTER_CRITICAL(&periph_spinlock);
    assert(s_periph_ref_counts > 0);
    s_periph_ref_counts--;
    if (s_periph_ref_counts == 0) {
        s_rtc_clk_freq = 0;
        rtc_dig_clk8m_disable();
    }
    portEXIT_CRITICAL(&periph_spinlock);
}
