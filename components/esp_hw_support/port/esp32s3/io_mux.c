/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "esp_private/io_mux.h"
#include "hal/rtc_io_ll.h"

#define RTCIO_RCC_ATOMIC() \
    for (int _rc_cnt = 1; \
    _rc_cnt ? (portENTER_CRITICAL(&rtc_spinlock), 1) : 0;     \
    portEXIT_CRITICAL(&rtc_spinlock), _rc_cnt--)

esp_err_t io_mux_set_clock_source(soc_module_clk_t clk_src)
{
    // IO MUX clock source is not selectable
    return ESP_OK;
}

extern portMUX_TYPE rtc_spinlock;
static portMUX_TYPE s_io_mux_spinlock = portMUX_INITIALIZER_UNLOCKED;

static rtc_io_status_t s_rtc_io_status = {
    .rtc_io_enabled_cnt = { 0 },
    .rtc_io_using_mask = 0
};

void io_mux_enable_lp_io_clock(gpio_num_t gpio_num, bool enable)
{
    assert(gpio_num != GPIO_NUM_NC);
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

void io_mux_force_disable_lp_io_clock(gpio_num_t gpio_num)
{
    assert(gpio_num != GPIO_NUM_NC);
    portENTER_CRITICAL(&s_io_mux_spinlock);
    s_rtc_io_status.rtc_io_enabled_cnt[gpio_num] = 0;
    s_rtc_io_status.rtc_io_using_mask &= ~(1ULL << gpio_num);
    if (s_rtc_io_status.rtc_io_using_mask == 0) {
        RTCIO_RCC_ATOMIC() {
            rtcio_ll_enable_io_clock(false);
        }
    }
    portEXIT_CRITICAL(&s_io_mux_spinlock);
}
