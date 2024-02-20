/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include "esp_err.h"
#include "esp_log.h"
#include "ulp_common.h"
#include "esp_private/esp_clk.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_periph.h"

#if CONFIG_IDF_TARGET_ESP32
#include "soc/sens_reg.h"
#define ULP_FSM_PREPARE_SLEEP_CYCLES 2    /*!< Cycles spent by FSM preparing ULP for sleep */
#define ULP_FSM_WAKEUP_SLEEP_CYCLES  2    /*!< Cycles spent by FSM waking up ULP from sleep */
#endif

esp_err_t ulp_set_wakeup_period(size_t period_index, uint32_t period_us)
{
    if (period_index > 4) {
        return ESP_ERR_INVALID_ARG;
    }

    uint64_t period_us_64 = period_us;

#if CONFIG_IDF_TARGET_ESP32
    uint64_t period_cycles = (period_us_64 << RTC_CLK_CAL_FRACT) / esp_clk_slowclk_cal_get();
    uint64_t min_sleep_period_cycles = ULP_FSM_PREPARE_SLEEP_CYCLES
                                       + ULP_FSM_WAKEUP_SLEEP_CYCLES
                                       + REG_GET_FIELD(RTC_CNTL_TIMER2_REG, RTC_CNTL_ULPCP_TOUCH_START_WAIT);
    if (period_cycles < min_sleep_period_cycles) {
        period_cycles = min_sleep_period_cycles;
        ESP_LOGW("ulp", "Sleep period clipped to minimum of %"PRIu32" cycles", (uint32_t) min_sleep_period_cycles);
    } else {
        period_cycles -= min_sleep_period_cycles;
    }
    REG_SET_FIELD(SENS_ULP_CP_SLEEP_CYC0_REG + period_index * sizeof(uint32_t),
                  SENS_SLEEP_CYCLES_S0, (uint32_t) period_cycles);
#elif defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
    soc_rtc_slow_clk_src_t slow_clk_src = rtc_clk_slow_src_get();
    rtc_cal_sel_t cal_clk = RTC_CAL_RTC_MUX;
    if (slow_clk_src == SOC_RTC_SLOW_CLK_SRC_XTAL32K) {
        cal_clk = RTC_CAL_32K_XTAL;
    } else if (slow_clk_src == SOC_RTC_SLOW_CLK_SRC_RC_FAST_D256) {
        cal_clk  = RTC_CAL_8MD256;
    }
    uint32_t slow_clk_period = rtc_clk_cal(cal_clk, 100);
    uint64_t period_cycles = rtc_time_us_to_slowclk(period_us_64, slow_clk_period);

    REG_SET_FIELD(RTC_CNTL_ULP_CP_TIMER_1_REG, RTC_CNTL_ULP_CP_TIMER_SLP_CYCLE, ((uint32_t)period_cycles));
#endif
    return ESP_OK;
}

void ulp_timer_stop(void)
{
#if CONFIG_IDF_TARGET_ESP32
    CLEAR_PERI_REG_MASK(RTC_CNTL_STATE0_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN);
#else
    CLEAR_PERI_REG_MASK(RTC_CNTL_ULP_CP_TIMER_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN);
#endif
}

void ulp_timer_resume(void)
{
#if CONFIG_IDF_TARGET_ESP32
    SET_PERI_REG_MASK(RTC_CNTL_STATE0_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN);
#else
    SET_PERI_REG_MASK(RTC_CNTL_ULP_CP_TIMER_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN);
#endif
}
