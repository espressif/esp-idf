/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_xt_wdt.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"

#include "esp_log.h"
#include "esp_check.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#if SOC_XT_WDT_SUPPORTED

#include "esp_private/rtc_ctrl.h"
#include "hal/xt_wdt_hal.h"
#include "hal/xt_wdt_ll.h"
#include "soc/rtc.h"

#define RTC_CLK_CAL_CYCLES 500

const static char *TAG = "esp_xt_wdt";

static xt_wdt_hal_context_t s_hal_ctx;

static esp_xt_callback_t s_callback_func;
static void *s_callback_arg;

static portMUX_TYPE s_xt_wdt_lock = portMUX_INITIALIZER_UNLOCKED;

static IRAM_ATTR void rtc_xt_wdt_default_isr_handler(void *arg)
{
    ESP_EARLY_LOGE(TAG, "XTAL32K watchdog timer got triggered");

    portENTER_CRITICAL_ISR(&s_xt_wdt_lock);
    if (s_callback_func) {
        (*s_callback_func)(s_callback_arg);
    }
    portEXIT_CRITICAL_ISR(&s_xt_wdt_lock);
}

esp_err_t esp_xt_wdt_init(const esp_xt_wdt_config_t *cfg)
{
    esp_err_t ret = ESP_OK;

    xt_wdt_hal_config_t hal_config = {
        .timeout = cfg->timeout,
    };

    xt_wdt_hal_init(&s_hal_ctx, &hal_config);

    if (cfg->auto_backup_clk_enable) {
        /* Estimate frequency of internal RTC oscillator */
        uint32_t rtc_clk_frequency_khz = rtc_clk_freq_cal(rtc_clk_cal(RTC_CAL_INTERNAL_OSC, RTC_CLK_CAL_CYCLES)) / 1000;
        ESP_LOGD(TAG, "Calibrating backup clock from rtc clock with frequency %"PRIu32, rtc_clk_frequency_khz);

        xt_wdt_hal_enable_backup_clk(&s_hal_ctx, rtc_clk_frequency_khz);
    }

    ESP_GOTO_ON_ERROR(rtc_isr_register(rtc_xt_wdt_default_isr_handler, NULL, XT_WDT_LL_XTAL32_DEAD_INTR_MASK, 0), err, TAG, "Failed to register isr");

    xt_wdt_hal_enable(&s_hal_ctx, 1);

    return ESP_OK;
err:
    return ret;
}

void esp_xt_wdt_restore_clk(void)
{
    xt_wdt_hal_enable(&s_hal_ctx, false);

    REG_CLR_BIT(RTC_CNTL_EXT_XTL_CONF_REG, RTC_CNTL_XPD_XTAL_32K);
    REG_SET_BIT(RTC_CNTL_EXT_XTL_CONF_REG, RTC_CNTL_XPD_XTAL_32K);

    /* Needs some time after switching to 32khz XTAL before turning on WDT again */
    esp_rom_delay_us(300);

    xt_wdt_hal_enable(&s_hal_ctx, true);
}

void esp_xt_wdt_register_callback(esp_xt_callback_t func, void *arg)
{
    portENTER_CRITICAL(&s_xt_wdt_lock);
    s_callback_func = func;
    s_callback_arg = arg;
    portEXIT_CRITICAL(&s_xt_wdt_lock);
}

#endif //SOC_XT_WDT_SUPPORTED
