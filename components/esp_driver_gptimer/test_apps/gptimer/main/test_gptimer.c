/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "driver/gptimer.h"
#include "soc/soc_caps.h"
#include "esp_attr.h"

#if CONFIG_GPTIMER_ISR_IRAM_SAFE
#define TEST_ALARM_CALLBACK_ATTR IRAM_ATTR
#else
#define TEST_ALARM_CALLBACK_ATTR
#endif // CONFIG_GPTIMER_ISR_IRAM_SAFE

TEST_CASE("gptimer_set_get_raw_count", "[gptimer]")
{
    gptimer_config_t config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1 * 1000 * 1000,
    };
    gptimer_handle_t timers[SOC_TIMER_GROUP_TOTAL_TIMERS];
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ESP_OK(gptimer_new_timer(&config, &timers[i]));
    }

    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, gptimer_new_timer(&config, &timers[0]));
    unsigned long long get_value = 0;
    printf("check gptimer initial count value\r\n");
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ESP_OK(gptimer_get_raw_count(timers[i], &get_value));
        TEST_ASSERT_EQUAL(0, get_value);
    }
    unsigned long long set_values[] = {100, 500, 666};
    for (size_t j = 0; j < sizeof(set_values) / sizeof(set_values[0]); j++) {
        for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
            printf("set raw count to %llu for gptimer %d\r\n", set_values[j], i);
            TEST_ESP_OK(gptimer_set_raw_count(timers[i], set_values[j]));
        }
        vTaskDelay(pdMS_TO_TICKS(10));
        for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
            TEST_ESP_OK(gptimer_get_raw_count(timers[i], &get_value));
            printf("get raw count of gptimer %d: %llu\r\n", i, get_value);
            TEST_ASSERT_EQUAL(set_values[j], get_value);
        }
    }

    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ESP_OK(gptimer_del_timer(timers[i]));
    }
}

TEST_CASE("gptimer_wallclock_with_various_clock_sources", "[gptimer]")
{
    gptimer_clock_source_t test_clk_srcs[] = SOC_GPTIMER_CLKS;
    uint32_t timer_resolution_hz[SOC_TIMER_GROUP_TOTAL_TIMERS];

    // test with various clock sources
    for (size_t i = 0; i < sizeof(test_clk_srcs) / sizeof(test_clk_srcs[0]); i++) {
        gptimer_config_t timer_config = {
            .clk_src = test_clk_srcs[i],
            .direction = GPTIMER_COUNT_UP,
            .resolution_hz = 1 * 1000 * 1000,
        };
        gptimer_handle_t timers[SOC_TIMER_GROUP_TOTAL_TIMERS];
        for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
            TEST_ESP_OK(gptimer_new_timer(&timer_config, &timers[i]));
            TEST_ESP_OK(gptimer_get_resolution(timers[i], &timer_resolution_hz[i]));
        }
        // start timer before enable should fail
        TEST_ESP_ERR(ESP_ERR_INVALID_STATE, gptimer_start(timers[0]));
        printf("enable timers\r\n");
        for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
            TEST_ESP_OK(gptimer_enable(timers[i]));
        }
        printf("start timers\r\n");
        for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
            TEST_ESP_OK(gptimer_start(timers[i]));
        }
        esp_rom_delay_us(20 * 1000); // 20ms = 20_000 ticks
        uint64_t value = 0;
        for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
            TEST_ESP_OK(gptimer_get_raw_count(timers[i], &value));
            // convert the raw count to us
            value = value * 1000000 / timer_resolution_hz[i];
            TEST_ASSERT_UINT_WITHIN(200, 20000, value);
        }
        printf("stop timers\r\n");
        for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
            TEST_ESP_OK(gptimer_stop(timers[i]));
        }
        printf("check whether timers have stopped\r\n");
        esp_rom_delay_us(20 * 1000);
        for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
            TEST_ESP_OK(gptimer_get_raw_count(timers[i], &value));
            printf("get raw count of gptimer %d: %llu\r\n", i, value);
            // convert the raw count to us
            value = value * 1000000 / timer_resolution_hz[i];
            TEST_ASSERT_UINT_WITHIN(200, 20000, value);
        }
        printf("restart timers\r\n");
        for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
            TEST_ESP_OK(gptimer_start(timers[i]));
        }
        esp_rom_delay_us(20 * 1000);
        printf("stop timers again\r\n");
        for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
            TEST_ESP_OK(gptimer_stop(timers[i]));
        }
        printf("check whether timers have stopped\r\n");
        esp_rom_delay_us(20 * 1000);
        for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
            TEST_ESP_OK(gptimer_get_raw_count(timers[i], &value));
            printf("get raw count of gptimer %d: %llu\r\n", i, value);
            // convert the raw count to us
            value = value * 1000000 / timer_resolution_hz[i];
            TEST_ASSERT_UINT_WITHIN(400, 40000, value);
        }
        printf("disable timers\r\n");
        for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
            TEST_ESP_OK(gptimer_disable(timers[i]));
        }
        printf("delete timers\r\n");
        for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
            TEST_ESP_OK(gptimer_del_timer(timers[i]));
        }
    }
}

/**
 * @note Delta of the timer count after the triggering of the alarm. Delta must be sufficient large to account for the latency
 * between the alarm triggering and the execution of the callback that actually stops the gptimer.
 */
#if CONFIG_PM_ENABLE
#define GPTIMER_STOP_ON_ALARM_COUNT_DELTA  150
#else
#define GPTIMER_STOP_ON_ALARM_COUNT_DELTA  40
#endif // CONFIG_PM_ENABLE

TEST_ALARM_CALLBACK_ATTR static bool test_gptimer_alarm_stop_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    TaskHandle_t task_handle = (TaskHandle_t)user_data;
    BaseType_t high_task_wakeup;
    gptimer_stop(timer);
    esp_rom_printf("count=%lld @alarm\n", edata->count_value);
    vTaskNotifyGiveFromISR(task_handle, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

TEST_CASE("gptimer_stop_on_alarm", "[gptimer]")
{
    TaskHandle_t task_handle =  xTaskGetCurrentTaskHandle();

    gptimer_config_t timer_config = {
        .resolution_hz = 1 * 1000 * 1000,
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
    };
    gptimer_handle_t timers[SOC_TIMER_GROUP_TOTAL_TIMERS];
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ESP_OK(gptimer_new_timer(&timer_config, &timers[i]));
    }

    gptimer_event_callbacks_t cbs = {
        .on_alarm = test_gptimer_alarm_stop_callback,
    };
    gptimer_alarm_config_t alarm_config = {};

    printf("start timers\r\n");
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        alarm_config.alarm_count = 100000 * (i + 1);
        TEST_ESP_OK(gptimer_set_alarm_action(timers[i], &alarm_config));
        TEST_ESP_OK(gptimer_register_event_callbacks(timers[i], &cbs, task_handle));
        TEST_ESP_OK(gptimer_enable(timers[i]));
        TEST_ESP_OK(gptimer_start(timers[i]));
        printf("alarm value for gptimer %d: %llu\r\n", i, alarm_config.alarm_count);
    }
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
    }

    printf("check whether the timers have stopped in the ISR\r\n");
    vTaskDelay(pdMS_TO_TICKS(20));
    unsigned long long value = 0;
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ESP_OK(gptimer_get_raw_count(timers[i], &value));
        printf("get raw count of gptimer %d: %llu\r\n", i, value);
        TEST_ASSERT_UINT_WITHIN(GPTIMER_STOP_ON_ALARM_COUNT_DELTA, 100000 * (i + 1), value);
    }

    printf("restart timers\r\n");
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        alarm_config.alarm_count = 100000 * (i + 1);
        // reset counter value to zero
        TEST_ESP_OK(gptimer_set_raw_count(timers[i], 0));
        TEST_ESP_OK(gptimer_start(timers[i]));
    }
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
    }
    printf("check whether the timers have stopped in the ISR\r\n");
    vTaskDelay(pdMS_TO_TICKS(20));
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ESP_OK(gptimer_get_raw_count(timers[i], &value));
        printf("get raw count of gptimer %d: %llu\r\n", i, value);
        TEST_ASSERT_UINT_WITHIN(GPTIMER_STOP_ON_ALARM_COUNT_DELTA, 100000 * (i + 1), value);
    }

    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ESP_OK(gptimer_disable(timers[i]));
        TEST_ESP_OK(gptimer_del_timer(timers[i]));
    }
}

/**
 * @note Delta of the timer count after the triggering of the alarm. Delta must be sufficient large to account for the latency
 * between the alarm triggering and the capturing of the counter's value in the subsequent ISR.
 */
#if CONFIG_PM_ENABLE
#define GPTIMER_AUTO_RELOAD_ON_ALARM_COUNT_DELTA  200
#else
#define GPTIMER_AUTO_RELOAD_ON_ALARM_COUNT_DELTA  20
#endif // CONFIG_PM_ENABLE

TEST_ALARM_CALLBACK_ATTR static bool test_gptimer_alarm_reload_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    TaskHandle_t task_handle = (TaskHandle_t)user_data;
    BaseType_t high_task_wakeup;
    esp_rom_printf("alarm isr count=%llu\r\n", edata->count_value);
    // check if the count value has been reloaded
    TEST_ASSERT_UINT_WITHIN(GPTIMER_AUTO_RELOAD_ON_ALARM_COUNT_DELTA, 100, edata->count_value);
    vTaskNotifyGiveFromISR(task_handle, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

TEST_CASE("gptimer_auto_reload_on_alarm", "[gptimer]")
{
    TaskHandle_t task_handle =  xTaskGetCurrentTaskHandle();

    gptimer_config_t timer_config = {
        .resolution_hz = 1 * 1000 * 1000,
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
    };
    gptimer_handle_t timers[SOC_TIMER_GROUP_TOTAL_TIMERS];
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ESP_OK(gptimer_new_timer(&timer_config, &timers[i]));
    }

    gptimer_event_callbacks_t cbs = {
        .on_alarm = test_gptimer_alarm_reload_callback,
    };
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 100,
        .alarm_count = 100000,
        .flags.auto_reload_on_alarm = true,
    };

    printf("start timers\r\n");
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ESP_OK(gptimer_set_alarm_action(timers[i], &alarm_config));
        TEST_ESP_OK(gptimer_register_event_callbacks(timers[i], &cbs, task_handle));
        TEST_ESP_OK(gptimer_enable(timers[i]));
        TEST_ESP_OK(gptimer_start(timers[i]));
        TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
        TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
        TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
        // delete should fail if timer is not disabled
        TEST_ESP_ERR(ESP_ERR_INVALID_STATE, gptimer_del_timer(timers[i]));
        TEST_ESP_OK(gptimer_stop(timers[i]));
    }

    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ESP_OK(gptimer_disable(timers[i]));
        TEST_ESP_OK(gptimer_del_timer(timers[i]));
    }
}

TEST_ALARM_CALLBACK_ATTR static bool test_gptimer_alarm_normal_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    TaskHandle_t task_handle = (TaskHandle_t)user_data;
    BaseType_t high_task_wakeup;
    esp_rom_printf("alarm isr count=%llu\r\n", edata->count_value);
    // check the count value at alarm event
    vTaskNotifyGiveFromISR(task_handle, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

/**
 * @note Delta of the timer count after the triggering of the alarm. Delta must be sufficient large to account for the latency
 * between the alarm triggering and the capturing of the counter's value in the subsequent ISR.
 * Also should account for the inaccuracy of the systick during DFS.
*/
#if CONFIG_PM_ENABLE
#define GPTIMER_ONE_SHOT_ALARM_COUNT_DELTA 15000
#else
#define GPTIMER_ONE_SHOT_ALARM_COUNT_DELTA 1000
#endif // CONFIG_PM_ENABLE

TEST_CASE("gptimer_one_shot_alarm", "[gptimer]")
{
    TaskHandle_t task_handle =  xTaskGetCurrentTaskHandle();

    gptimer_config_t timer_config = {
        .resolution_hz = 1 * 1000 * 1000,
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
    };
    gptimer_handle_t timers[SOC_TIMER_GROUP_TOTAL_TIMERS];
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        timer_config.intr_priority = i % 3 + 1; // test different priorities
        TEST_ESP_OK(gptimer_new_timer(&timer_config, &timers[i]));
    }

    gptimer_event_callbacks_t cbs = {
        .on_alarm = test_gptimer_alarm_normal_callback,
    };
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 100000, // 100ms
    };

    printf("start timers\r\n");
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ESP_OK(gptimer_set_alarm_action(timers[i], &alarm_config));
        TEST_ESP_OK(gptimer_register_event_callbacks(timers[i], &cbs, task_handle));
        TEST_ESP_OK(gptimer_enable(timers[i]));
        TEST_ESP_OK(gptimer_start(timers[i]));
        TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
        // no alarm event should trigger again, as auto-reload is not enabled and alarm value hasn't changed in the isr
        TEST_ASSERT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
        // the alarm is stopped, but the counter should still work
        uint64_t value = 0;
        TEST_ESP_OK(gptimer_get_raw_count(timers[i], &value));
        TEST_ASSERT_UINT_WITHIN(GPTIMER_ONE_SHOT_ALARM_COUNT_DELTA, 1100000, value); // 1100000 = 100ms alarm + 1s delay
        TEST_ESP_OK(gptimer_stop(timers[i]));
    }

    printf("restart timers\r\n");
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ESP_OK(gptimer_start(timers[i]));
        // alarm should be triggered immediately as the counter value has across the target alarm value already
        TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, 0));
        TEST_ESP_OK(gptimer_stop(timers[i]));
    }

    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ESP_OK(gptimer_disable(timers[i]));
        TEST_ESP_OK(gptimer_del_timer(timers[i]));
    }
}

TEST_ALARM_CALLBACK_ATTR static bool test_gptimer_alarm_update_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    TaskHandle_t task_handle = (TaskHandle_t)user_data;
    BaseType_t high_task_wakeup;
    esp_rom_printf("alarm isr count=%llu\r\n", edata->count_value);
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = edata->count_value + 100000, // alarm in next 100ms again
    };
    gptimer_set_alarm_action(timer, &alarm_config);
    vTaskNotifyGiveFromISR(task_handle, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

TEST_CASE("gptimer_update_alarm_dynamically", "[gptimer]")
{
    TaskHandle_t task_handle =  xTaskGetCurrentTaskHandle();

    gptimer_config_t timer_config = {
        .resolution_hz = 1 * 1000 * 1000,
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
    };
    gptimer_handle_t timers[SOC_TIMER_GROUP_TOTAL_TIMERS];
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ESP_OK(gptimer_new_timer(&timer_config, &timers[i]));
    }

    gptimer_event_callbacks_t cbs = {
        .on_alarm = test_gptimer_alarm_update_callback,
    };
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = 100000, // initial alarm count, 100ms
    };
    printf("start timers\r\n");
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ESP_OK(gptimer_set_alarm_action(timers[i], &alarm_config));
        TEST_ESP_OK(gptimer_register_event_callbacks(timers[i], &cbs, task_handle));
        TEST_ESP_OK(gptimer_enable(timers[i]));
        TEST_ESP_OK(gptimer_start(timers[i]));
        // check the alarm event for multiple times
        TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(500)));
        TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(500)));
        TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(500)));
        TEST_ESP_OK(gptimer_stop(timers[i]));
        // check there won't be more interrupts triggered than expected
        TEST_ASSERT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(500)));
    }

    printf("restart timers\r\n");
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ESP_OK(gptimer_start(timers[i]));
        // check the alarm event for multiple times
        TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(500)));
        TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(500)));
        TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(500)));
        TEST_ESP_OK(gptimer_stop(timers[i]));
        // check there won't be more interrupts triggered than expected
        TEST_ASSERT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(500)));
    }

    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ESP_OK(gptimer_disable(timers[i]));
        TEST_ESP_OK(gptimer_del_timer(timers[i]));
    }
}

/**
 * @noteDelta of the timer count after the triggering of the alarm. Delta must be sufficient large to account for the latency
 * between the alarm triggering and the capturing of the counter's value in the subsequent ISR.
 */
#if CONFIG_PM_ENABLE
#define GPTIMER_COUNT_DOWN_RELOAD_DELTA  200
#else
#define GPTIMER_COUNT_DOWN_RELOAD_DELTA  30
#endif // CONFIG_PM_ENABLE

TEST_ALARM_CALLBACK_ATTR static bool test_gptimer_count_down_reload_alarm_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    TaskHandle_t task_handle = (TaskHandle_t)user_data;
    BaseType_t high_task_wakeup;
    esp_rom_printf("alarm isr count=%llu\r\n", edata->count_value);
    // check if the count value has been reloaded
    TEST_ASSERT_UINT_WITHIN(GPTIMER_COUNT_DOWN_RELOAD_DELTA, 200000, edata->count_value);
    vTaskNotifyGiveFromISR(task_handle, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

TEST_CASE("gptimer_count_down_reload", "[gptimer]")
{
    TaskHandle_t task_handle =  xTaskGetCurrentTaskHandle();

    gptimer_config_t timer_config = {
        .resolution_hz = 1 * 1000 * 1000,
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_DOWN,
    };
    gptimer_handle_t timers[SOC_TIMER_GROUP_TOTAL_TIMERS];
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ESP_OK(gptimer_new_timer(&timer_config, &timers[i]));
        TEST_ESP_OK(gptimer_set_raw_count(timers[i], 200000));
    }

    gptimer_event_callbacks_t cbs = {
        .on_alarm = test_gptimer_count_down_reload_alarm_callback,
    };
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 200000, // 200ms
        .alarm_count = 0,
        .flags.auto_reload_on_alarm = true,
    };

    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ESP_OK(gptimer_set_alarm_action(timers[i], &alarm_config));
        TEST_ESP_OK(gptimer_register_event_callbacks(timers[i], &cbs, task_handle));
        TEST_ESP_OK(gptimer_enable(timers[i]));
        TEST_ESP_OK(gptimer_start(timers[i]));
        // check twice, as it's a period event
        TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
        TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
        TEST_ESP_OK(gptimer_stop(timers[i]));
    }

    printf("restart gptimer with previous configuration\r\n");
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ESP_OK(gptimer_start(timers[i]));
        // check twice, as it's a period event
        TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
        TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
        TEST_ESP_OK(gptimer_stop(timers[i]));
    }

    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ESP_OK(gptimer_disable(timers[i]));
        TEST_ESP_OK(gptimer_del_timer(timers[i]));
    }
}

TEST_ALARM_CALLBACK_ATTR static bool test_gptimer_overflow_reload_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    TaskHandle_t task_handle = (TaskHandle_t)user_data;
    BaseType_t high_task_wakeup;
    // Note: esp_rom_printf can't print value with 64 bit length, so the following print result is meaningless, but as an incidator for test that the alarm has fired
    esp_rom_printf("alarm isr count=%llu\r\n", edata->count_value);
    vTaskNotifyGiveFromISR(task_handle, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

TEST_CASE("gptimer_overflow", "[gptimer]")
{
    TaskHandle_t task_handle =  xTaskGetCurrentTaskHandle();

    gptimer_config_t timer_config = {
        .resolution_hz = 1 * 1000 * 1000,
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
    };
    gptimer_handle_t timers[SOC_TIMER_GROUP_TOTAL_TIMERS];
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ESP_OK(gptimer_new_timer(&timer_config, &timers[i]));
    }
#if SOC_TIMER_GROUP_COUNTER_BIT_WIDTH == 64
    uint64_t reload_at = UINT64_MAX - 100000;
#else
    uint64_t reload_at = (1ULL << SOC_TIMER_GROUP_COUNTER_BIT_WIDTH) - 100000;
#endif
    gptimer_event_callbacks_t cbs = {
        .on_alarm = test_gptimer_overflow_reload_callback,
    };
    gptimer_alarm_config_t alarm_config = {
        .reload_count = reload_at,
        .alarm_count = 100000, // 100ms
        .flags.auto_reload_on_alarm = true,
    };
    // The counter should start from [COUNTER_MAX-100000] and overflows to [0] and continue, then reached to alarm value [100000], reloaded to [COUNTER_MAX-100000] automatically
    // thus the period should be 200ms
    printf("start timers\r\n");
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ESP_OK(gptimer_set_alarm_action(timers[i], &alarm_config));
        TEST_ESP_OK(gptimer_register_event_callbacks(timers[i], &cbs, task_handle));
        // we start from the reload value
        TEST_ESP_OK(gptimer_set_raw_count(timers[i], reload_at));
        TEST_ESP_OK(gptimer_enable(timers[i]));
        TEST_ESP_OK(gptimer_start(timers[i]));
        TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(400)));
        TEST_ESP_OK(gptimer_stop(timers[i]));
    }

    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ESP_OK(gptimer_disable(timers[i]));
        TEST_ESP_OK(gptimer_del_timer(timers[i]));
    }
}

TEST_ALARM_CALLBACK_ATTR static bool test_gptimer_alarm_late_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    bool *alarm_fired = (bool *)user_data;
    *alarm_fired = true;
    esp_rom_printf("alarm isr count=%llu\r\n", edata->count_value);
    return false;
}

TEST_CASE("gptimer_trig_alarm_with_old_count", "[gptimer]")
{
    printf("install gptimer driver\r\n");
    gptimer_config_t timer_config = {
        .resolution_hz = 10 * 1000, // 10KHz, 1 tick = 0.1ms
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
    };
    gptimer_handle_t timer;
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &timer));

    printf("register alarm callback\r\n");
    bool alarm_fired = false;
    gptimer_event_callbacks_t cbs = {
        .on_alarm = test_gptimer_alarm_late_callback,
    };
    TEST_ESP_OK(gptimer_register_event_callbacks(timer, &cbs, &alarm_fired));
    TEST_ESP_OK(gptimer_enable(timer));
    TEST_ESP_OK(gptimer_start(timer));

    printf("let the timer go for sometime\r\n");
    vTaskDelay(pdMS_TO_TICKS(1000));

    printf("set alarm config with a very early count value\r\n");
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 10, // 1ms < current count, so the alarm should fire immediately
    };
    TEST_ESP_OK(gptimer_set_alarm_action(timer, &alarm_config));

    vTaskDelay(pdMS_TO_TICKS(100));
    // check it's fired
    TEST_ASSERT_TRUE(alarm_fired);

    TEST_ESP_OK(gptimer_stop(timer));
    TEST_ESP_OK(gptimer_disable(timer));
    TEST_ESP_OK(gptimer_del_timer(timer));
}
