/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <string.h>
#include <sys/lock.h>
#include <sys/param.h>

#include "esp_attr.h"
#include "esp_sleep.h"
#include "esp_cpu.h"

#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/soc_caps.h"
#include "hal/uart_ll.h"
#include "hal/clk_tree_ll.h"

#if SOC_LP_TIMER_SUPPORTED
#include "hal/lp_timer_ll.h"
#include "hal/lp_timer_hal.h"
#else
#include "hal/rtc_cntl_ll.h"
#endif

#if SOC_PMU_SUPPORTED
#include "hal/pmu_ll.h"
#endif

#include "sdkconfig.h"
#include "esp_rom_uart.h"
#include "esp_rom_sys.h"

#ifdef CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C6
#include "esp32c6/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rom/rtc.h"
#endif

void RTC_IRAM_ATTR esp_wake_stub_sleep(esp_deep_sleep_wake_stub_fn_t new_stub)
{

#if CONFIG_IDF_TARGET_ESP32
    // Since the app core of esp32 does not support access to RTC_FAST_MEMORY,
    // `esp_set_deep_sleep_wake_stub` is not declared in RTC_FAST_MEMORY,
    // so we cannot call it here
    REG_WRITE(RTC_ENTRY_ADDR_REG, (uint32_t)new_stub);
#else
    esp_set_deep_sleep_wake_stub(new_stub);
#endif

#if SOC_PM_SUPPORT_DEEPSLEEP_CHECK_STUB_ONLY
    esp_set_deep_sleep_wake_stub_default_entry();
#else
    set_rtc_memory_crc();
#endif // SOC_PM_SUPPORT_DEEPSLEEP_CHECK_STUB_MEM

    // Go to sleep.
#if SOC_PMU_SUPPORTED
    pmu_ll_hp_clear_wakeup_intr_status(&PMU);
    pmu_ll_hp_clear_reject_intr_status(&PMU);
    pmu_ll_hp_clear_reject_cause(&PMU);
    pmu_ll_hp_set_sleep_enable(&PMU);
#else
    rtc_cntl_ll_sleep_enable();
#endif

    // A few CPU cycles may be necessary for the sleep to start...
#if __XTENSA__
    xt_utils_wait_for_intr();
#else
    rv_utils_wait_for_intr();
#endif // __XTENSA__
    // never reaches here.
}

void RTC_IRAM_ATTR esp_wake_stub_uart_tx_wait_idle(uint8_t uart_no)
{
    while (!uart_ll_is_tx_idle(UART_LL_GET_HW(uart_no))) {};
}

void RTC_IRAM_ATTR esp_wake_stub_set_wakeup_time(uint64_t time_in_us)
{
    uint64_t rtc_count_delta = time_in_us * (1 << RTC_CLK_CAL_FRACT) / clk_ll_rtc_slow_load_cal();
#if SOC_LP_TIMER_SUPPORTED
    lp_timer_ll_counter_snapshot(&LP_TIMER);
    uint32_t lo = lp_timer_ll_get_counter_value_low(&LP_TIMER, 0);
    uint32_t hi = lp_timer_ll_get_counter_value_high(&LP_TIMER, 0);
    uint64_t rtc_curr_count = (uint64_t)hi << 32 | lo;

    lp_timer_ll_clear_alarm_intr_status(&LP_TIMER);
    lp_timer_ll_set_alarm_target(&LP_TIMER, 0, rtc_curr_count + rtc_count_delta);
    lp_timer_ll_set_target_enable(&LP_TIMER, 0, true);
#else
    uint64_t rtc_curr_count = rtc_cntl_ll_get_rtc_time();
    rtc_cntl_ll_set_wakeup_timer(rtc_curr_count + rtc_count_delta);
#endif

}

uint32_t RTC_IRAM_ATTR esp_wake_stub_get_wakeup_cause(void)
{
#if SOC_PMU_SUPPORTED
    return pmu_ll_hp_get_wakeup_cause(&PMU);
#else
    return rtc_cntl_ll_get_wakeup_cause();
#endif
}
