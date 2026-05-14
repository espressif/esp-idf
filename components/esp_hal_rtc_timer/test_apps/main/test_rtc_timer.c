/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "unity.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"
#include "esp_private/esp_clk.h"
#include "hal/rtc_timer_hal.h"
#include "hal/rtc_timer_ll.h"
#include "soc/rtc.h"
#include "soc/soc_caps.h"
#include "soc/rtc_cntl_periph.h"

#if SOC_RTC_TIMER_SUPPORTED

#define RTC_TIMER_SAMPLE_DELAY_US             50000
#define RTC_TIMER_CONVERSION_MIN_TOLERANCE_US 5000
#define RTC_TIMER_ALARM_DELAY_US              4000
#define RTC_TIMER_ALARM_TIMEOUT_US            100000

static uint64_t max_u64(uint64_t lhs, uint64_t rhs)
{
    return (lhs > rhs) ? lhs : rhs;
}

static uint64_t abs_diff_u64(uint64_t lhs, uint64_t rhs)
{
    return (lhs > rhs) ? (lhs - rhs) : (rhs - lhs);
}

static uint32_t get_rtc_timer_period(void)
{
    uint32_t period = esp_clk_slowclk_cal_get();

    TEST_ASSERT_NOT_EQUAL_UINT32(0, period);
    return period;
}

static void assert_counter_elapsed_matches_time(uint8_t timer_id)
{
    const uint32_t period = get_rtc_timer_period();
    const int64_t start_host_us = esp_timer_get_time();
    const uint64_t start_cycles = rtc_timer_hal_get_cycle_count(timer_id);

    esp_rom_delay_us(RTC_TIMER_SAMPLE_DELAY_US);

    const uint64_t end_cycles = rtc_timer_hal_get_cycle_count(timer_id);
    const int64_t end_host_us = esp_timer_get_time();
    const uint64_t host_elapsed_us = end_host_us - start_host_us;
    const uint64_t timer_elapsed_us = rtc_time_slowclk_to_us(end_cycles - start_cycles, period);
    const uint64_t tolerance_us = max_u64(host_elapsed_us / 4, RTC_TIMER_CONVERSION_MIN_TOLERANCE_US);
    const uint64_t delta_us = abs_diff_u64(timer_elapsed_us, host_elapsed_us);
    char message[160];

    printf("timer%" PRIu8 " counter %" PRIu64 " -> %" PRIu64 " cycles, host=%" PRIu64 "us timer=%" PRIu64 "us tol=%" PRIu64 "us\n",
           timer_id, start_cycles, end_cycles, host_elapsed_us, timer_elapsed_us, tolerance_us);

    snprintf(message, sizeof(message), "timer%" PRIu8 " counter did not increase: %" PRIu64 " -> %" PRIu64,
             timer_id, start_cycles, end_cycles);
    TEST_ASSERT_TRUE_MESSAGE(end_cycles > start_cycles, message);

    snprintf(message, sizeof(message), "timer%" PRIu8 " elapsed mismatch: host=%" PRIu64 "us timer=%" PRIu64 "us delta=%" PRIu64 "us tol=%" PRIu64 "us",
             timer_id, host_elapsed_us, timer_elapsed_us, delta_us, tolerance_us);
    TEST_ASSERT_TRUE_MESSAGE(delta_us <= tolerance_us, message);
}

TEST_CASE("rtc timer counter can be read and converted to elapsed time", "[rtc_timer]")
{
    assert_counter_elapsed_matches_time(0);
}

#if SOC_RTC_TIMER_V2 || SOC_RTC_TIMER_V3
static uint32_t get_alarm_intr_mask(uint8_t timer_id)
{
    if (timer_id == 0) {
#if SOC_RTC_TIMER_V3
        return RTC_TIMER_SOC_WAKEUP_INT_RAW;
#else
        return LP_TIMER_SOC_WAKEUP_INT_RAW;
#endif
    }

#if defined(RTC_TIMER_MAIN_TIMER_LP_INT_RAW)
    return RTC_TIMER_MAIN_TIMER_LP_INT_RAW;
#elif defined(LP_TIMER_MAIN_TIMER_LP_INT_RAW)
    return LP_TIMER_MAIN_TIMER_LP_INT_RAW;
#else
    return 0;
#endif
}

static void assert_alarm_interrupt_round_trip(uint8_t timer_id)
{
    const uint32_t period = get_rtc_timer_period();
    uint64_t delay_ticks = rtc_time_us_to_slowclk(RTC_TIMER_ALARM_DELAY_US, period);
    const uint32_t raw_mask = get_alarm_intr_mask(timer_id);

    rtc_timer_ll_set_target_enable(&LP_TIMER, timer_id, false);
    rtc_timer_ll_alarm_intr_enable(&LP_TIMER, timer_id, false);
    rtc_timer_ll_clear_alarm_intr_status(&LP_TIMER, timer_id);
    TEST_ASSERT_EQUAL_HEX32(0, rtc_timer_ll_get_intr_raw(&LP_TIMER, timer_id) & raw_mask);

    const uint64_t start_cycles = rtc_timer_hal_get_cycle_count(timer_id);
    rtc_timer_hal_set_wakeup_time(timer_id, start_cycles + delay_ticks);

    bool fired = false;
    const int64_t deadline = esp_timer_get_time() + RTC_TIMER_ALARM_TIMEOUT_US;
    while (esp_timer_get_time() < deadline) {
        if (rtc_timer_ll_get_intr_raw(&LP_TIMER, timer_id) & raw_mask) {
            fired = true;
            break;
        }
    }

    printf("timer%" PRIu8 " alarm raw=0x%" PRIx32 " mask=0x%" PRIx32 "\n",
           timer_id, rtc_timer_ll_get_intr_raw(&LP_TIMER, timer_id), raw_mask);
    TEST_ASSERT_TRUE_MESSAGE(fired, "RTC timer alarm did not raise its raw interrupt status");

    rtc_timer_ll_set_target_enable(&LP_TIMER, timer_id, false);
    rtc_timer_ll_alarm_intr_enable(&LP_TIMER, timer_id, false);
    rtc_timer_ll_clear_alarm_intr_status(&LP_TIMER, timer_id);
    TEST_ASSERT_EQUAL_HEX32(0, rtc_timer_ll_get_intr_raw(&LP_TIMER, timer_id) & raw_mask);
}

TEST_CASE("rtc timer alarm interrupts can be raised and cleared", "[rtc_timer]")
{
    assert_alarm_interrupt_round_trip(0);
    if (get_alarm_intr_mask(1) != 0) {
        assert_alarm_interrupt_round_trip(1);
    }
}
#endif  // SOC_RTC_TIMER_V2 || SOC_RTC_TIMER_V3

#endif  // SOC_RTC_TIMER_SUPPORTED
