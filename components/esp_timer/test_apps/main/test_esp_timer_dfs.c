/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "esp_timer.h"
#include "unity.h"
#include "esp_rom_sys.h"
#include "esp_sleep.h"
#include "esp_pm.h"
#include "esp_log.h"
#include "soc/soc_caps.h"
#include "soc/rtc.h"
#include "esp_rtc_time.h"
#include "esp_private/esp_clk.h"

#define ALARM_PERIOD_MS 100
#define ALARM_TIMES     200 // 200*100ms = 20s

static const char* TAG = "ESP_TIMER with DFS";

static uint32_t s_current_alarm = 0;
static uint64_t s_alarm_records[ALARM_TIMES + 1] = {0};

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C5
static uint32_t supported_freq[] = {10, 20, 40, 80, 160, 240};
#elif CONFIG_IDF_TARGET_ESP32C2
static uint32_t supported_freq[] = {10, 20, 40, 80, 120};
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32C61
static uint32_t supported_freq[] = {10, 20, 40, 80, 160};
#elif CONFIG_IDF_TARGET_ESP32H2 || CONFIG_IDF_TARGET_ESP32H21
static uint32_t supported_freq[] = {8, 16, 32, 48, 64, 96};
#elif CONFIG_IDF_TARGET_ESP32H4
static uint32_t supported_freq[] = {8, 16, 24, 32};
#elif CONFIG_IDF_TARGET_ESP32P4
static uint32_t supported_freq[] = {10, 20, 40, 90, 180, 360};
#endif

static SemaphoreHandle_t s_alarm_finished;

static IRAM_ATTR void periodic_timer_callback(void* arg)
{
    s_alarm_records[s_current_alarm] = esp_rtc_get_time_us();
    BaseType_t yield;
    if (s_current_alarm == ALARM_TIMES) {
        xSemaphoreGiveFromISR(s_alarm_finished, &yield);
    }
    s_current_alarm++;
    if (yield) {
        portYIELD_FROM_ISR();
    }
}

static int64_t measuring_periodic_timer_accuracy(uint64_t alarm_records[])
{
    int64_t sum_jitter_us = 0;
    int64_t max_jitter_us = 0;
    int64_t jitter_array[ALARM_TIMES] = {0};

    for (int i = 1; i <= ALARM_TIMES; ++i) {
        int64_t jitter_us = (int64_t)alarm_records[i] - (int64_t)alarm_records[i - 1] - ALARM_PERIOD_MS * 1000;
        jitter_array[i - 1] = jitter_us;
        if (llabs(jitter_us) > llabs(max_jitter_us)) {
            max_jitter_us = jitter_us;
        }
        sum_jitter_us += jitter_us;
    }
    int64_t avg_jitter_us = sum_jitter_us / ALARM_TIMES;

    // Calculates the sum of squares of standard deviations
    int64_t sum_sq_diff = 0;
    for (int i = 0; i < ALARM_TIMES; ++i) {
        sum_sq_diff += (jitter_array[i] - avg_jitter_us) * (jitter_array[i] - avg_jitter_us);
    }

    double variance = sum_sq_diff / ALARM_TIMES;
    double stddev = sqrt(variance);

    printf("Average jitter us:  %"PRIi64"\n", avg_jitter_us);
    printf("Max jitter us:      %"PRIi64"\n", max_jitter_us);
    printf("Standard Deviation: %.3f\n", stddev);
    printf("Drift Percentage:   %.3f%%\n", (double)avg_jitter_us / (ALARM_PERIOD_MS * 10));

    // Reset measurement
    s_current_alarm = 0;
    bzero(s_alarm_records, sizeof(s_alarm_records));
    return avg_jitter_us;
}

static int64_t test_periodic_timer_accuracy_on_dfs(esp_timer_handle_t timer)
{
    // Calibrate slow clock.
#if !CONFIG_ESP_SYSTEM_RTC_EXT_XTAL
    esp_clk_slowclk_cal_set(rtc_clk_cal(RTC_CAL_RTC_MUX, 8192));
#endif

    ESP_ERROR_CHECK(esp_timer_start_periodic(timer, ALARM_PERIOD_MS * 1000));

    s_alarm_finished = xSemaphoreCreateBinary();
    // Each FreeRTOS tick will perform a min_freq_mhz->max_freq_mhz -> min_freq_mhz frequency switch
    xSemaphoreTake(s_alarm_finished, portMAX_DELAY);
    ESP_ERROR_CHECK(esp_timer_stop(timer));
    int64_t avg_jitter_us = measuring_periodic_timer_accuracy(s_alarm_records);
    vSemaphoreDelete(s_alarm_finished);
    return avg_jitter_us;
}

// The results of this test are meaningful only if `CONFIG_ESP_SYSTEM_RTC_EXT_XTAL` is enabled
TEST_CASE("Test the periodic timer timing accuracy when doing DFS", "[esp_timer][manual][ignore]")
{
    esp_pm_config_t pm_config = {
        .light_sleep_enable = false
    };

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &periodic_timer_callback,
        .dispatch_method = ESP_TIMER_ISR,
        .name = "periodic"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));

    for (int min = 0; min < sizeof(supported_freq) / sizeof(uint32_t); min++) {
        for (int max = 0; max < sizeof(supported_freq) / sizeof(uint32_t); max++) {
            if (supported_freq[max] <= supported_freq[min]) {
                continue;
            }
            pm_config.max_freq_mhz = supported_freq[max];
            pm_config.min_freq_mhz = supported_freq[min];
            ESP_LOGI(TAG, "Testing esp_timer accuracy on %d <-> %d  DFS ...", pm_config.min_freq_mhz, pm_config.max_freq_mhz);
            ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
            test_periodic_timer_accuracy_on_dfs(periodic_timer);
        }
    }
}

#if CONFIG_IDF_TARGET_ESP32
int16_t test_lact_compensation_delay = 0;
#define DO_MAGIC_TABLE_MEASUREMENT 0
#if DO_MAGIC_TABLE_MEASUREMENT
IRAM_ATTR int16_t rtc_clk_get_lact_compensation_delay(uint32_t cur_freq, uint32_t cpu_freq_mhz)
{
    (void)cur_freq; (void)cpu_freq_mhz;
    return test_lact_compensation_delay;
}
#endif

// Test Notes:
// 1. The test requires the slow clock source to be selected to `CONFIG_ESP_SYSTEM_RTC_EXT_XTAL`.
// 2. Manually set DO_MAGIC_TABLE_MEASUREMENT to 1 before test.
TEST_CASE("Test DFS lact conpensate magic table ", "[esp_timer][manual][ignore]")
{
    esp_pm_config_t pm_config = {
        .light_sleep_enable = false
    };

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &periodic_timer_callback,
        .dispatch_method = ESP_TIMER_ISR,
        .name = "periodic"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));

    for (int min = 0; min < sizeof(supported_freq) / sizeof(uint32_t); min++) {
        for (int max = 0; max < sizeof(supported_freq) / sizeof(uint32_t); max++) {
            if ((supported_freq[max] <= supported_freq[min]) || (supported_freq[min] >= 80) || (supported_freq[max] <= 40)) {
                continue;
            }

            pm_config.max_freq_mhz = supported_freq[max];
            pm_config.min_freq_mhz = supported_freq[min];
            ESP_LOGI(TAG, "Testing esp_timer accuracy on %d <-> %d  DFS ...", pm_config.min_freq_mhz, pm_config.max_freq_mhz);
            esp_pm_configure(&pm_config);

            int32_t max_delay = 300;
            int32_t min_delay = (pm_config.max_freq_mhz == 240) ? -4000 : 0;
            int32_t test_delay = (max_delay + min_delay) / 2;
            int32_t last_delay = 0;
            int32_t best_delay = 0;
            int64_t min_avg = INT64_MAX;

            do {
                printf("Test delay %ld\n", test_delay);
                test_lact_compensation_delay = test_delay;
                int64_t avg = test_periodic_timer_accuracy_on_dfs(periodic_timer);
                last_delay = test_delay;
                if (avg < 0) {
                    test_delay = (test_delay + max_delay) / 2;
                    min_delay = last_delay;
                } else {
                    test_delay = (test_delay + min_delay) / 2;
                    max_delay = last_delay;
                }

                if (llabs(avg) < llabs(min_avg)) {
                    best_delay = last_delay;
                    min_avg = avg;
                }
            } while (test_delay != last_delay);

            printf("Switch between %d <-> %d\n", pm_config.min_freq_mhz, pm_config.max_freq_mhz);
            printf("Best delay is  %ld\n", best_delay);
            printf("Min average is %lld\n", min_avg);
        }
    }
    ESP_ERROR_CHECK(esp_timer_delete(periodic_timer));
}
#endif
