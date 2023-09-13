/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/param.h>
#include "esp_timer.h"
#include "unity.h"
#include "esp_rom_sys.h"
#include "esp_sleep.h"

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32P4) // TODO IDF-7528

static void timer_cb1(void *arg)
{
    ++*((int*) arg);
}

TEST_CASE("Test the periodic timer does not handle lost events during light sleep", "[esp_timer][timeout=20]")
{

    int count_calls;
    const esp_timer_create_args_t timer_args = {
            .name = "timer_cb1",
            .arg  = &count_calls,
            .callback = &timer_cb1,
            .skip_unhandled_events = true,
    };
    esp_timer_handle_t periodic_timer;
    esp_timer_create(&timer_args, &periodic_timer);

    int period_cb_ms = 10;
    int interval_ms = 50;
    TEST_ESP_OK(esp_timer_start_periodic(periodic_timer, period_cb_ms * 1000));
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(interval_ms * 1000));
    printf("Run light sleep\n");
    printf("count_calls should be around = %d\n", interval_ms / period_cb_ms);
    for (int i = 0; i < 3; i++) {
        count_calls = 0;
        TEST_ESP_OK(esp_light_sleep_start());
        esp_rom_delay_us(interval_ms * 1000);
        printf("count_calls = %d\n", count_calls);
        TEST_ASSERT_INT_WITHIN(2, interval_ms / period_cb_ms, count_calls);
        TEST_ESP_OK(esp_timer_dump(stdout));
    }
    TEST_ESP_OK(esp_timer_stop(periodic_timer));
    // times_skipped is about 12 (4 from each sleep time).
    TEST_ESP_OK(esp_timer_dump(stdout));
    TEST_ESP_OK(esp_timer_delete(periodic_timer));
}

#endif //#!TEMPORARY_DISABLED_FOR_TARGETS(ESP32P4)
