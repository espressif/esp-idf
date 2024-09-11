/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "esp_private/io_mux.h"
#include "esp_private/periph_ctrl.h"
#include "hal/gpio_ll.h"
#include "hal/rtc_io_ll.h"

#define RTCIO_RCC_ATOMIC()  PERIPH_RCC_ATOMIC()

static portMUX_TYPE s_io_mux_spinlock = portMUX_INITIALIZER_UNLOCKED;
static soc_module_clk_t s_io_mux_clk_src = 0; // by default, the clock source is not set explicitly by any consumer (e.g. SDM, Filter)

#if CONFIG_ULP_COPROC_ENABLED
RTC_DATA_ATTR
#endif
static rtc_io_status_t s_rtc_io_status = {
    .rtc_io_enabled_cnt = { 0 },
    .rtc_io_using_mask = 0
};

esp_err_t io_mux_set_clock_source(soc_module_clk_t clk_src)
{
    bool clk_conflict = false;
    // check is the IO MUX has been set to another clock source
    portENTER_CRITICAL(&s_io_mux_spinlock);
    if (s_io_mux_clk_src != 0 && s_io_mux_clk_src != clk_src) {
        clk_conflict = true;
    } else {
        s_io_mux_clk_src = clk_src;
    }
    portEXIT_CRITICAL(&s_io_mux_spinlock);

    if (clk_conflict) {
        return ESP_ERR_INVALID_STATE;
    }

    gpio_ll_iomux_set_clk_src(clk_src);

    return ESP_OK;
}

void io_mux_enable_lp_io_clock(gpio_num_t gpio_num, bool enable)
{
    portENTER_CRITICAL(&s_io_mux_spinlock);
    if (enable) {
        if (s_rtc_io_status.rtc_io_enabled_cnt[gpio_num] == 0) {
            s_rtc_io_status.rtc_io_using_mask |= (1ULL << gpio_num);
        }
        s_rtc_io_status.rtc_io_enabled_cnt[gpio_num]++;
    } else if (!enable && (s_rtc_io_status.rtc_io_enabled_cnt[gpio_num] > 0)) {
        s_rtc_io_status.rtc_io_enabled_cnt[gpio_num]--;
        if (s_rtc_io_status.rtc_io_enabled_cnt[gpio_num] == 0) {
            s_rtc_io_status.rtc_io_using_mask &= ~(1ULL << gpio_num);
        }
    }
    RTCIO_RCC_ATOMIC() {
        if (s_rtc_io_status.rtc_io_using_mask == 0) {
            rtcio_ll_enable_io_clock(false);
        } else {
            rtcio_ll_enable_io_clock(true);
        }
    }
    portEXIT_CRITICAL(&s_io_mux_spinlock);
}
