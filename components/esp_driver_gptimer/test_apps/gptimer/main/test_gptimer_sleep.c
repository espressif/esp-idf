/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
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
#include "esp_sleep.h"
#include "esp_private/sleep_cpu.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/esp_pmu.h"
#if !SOC_LIGHT_SLEEP_SUPPORTED
#include "esp_private/gptimer.h"
#include "esp_private/sleep_retention.h"
#include "hal/timer_ll.h"
#include "hal/wdt_hal.h"
#endif

#if CONFIG_GPTIMER_ISR_IRAM_SAFE
#define TEST_ALARM_CALLBACK_ATTR IRAM_ATTR
#else
#define TEST_ALARM_CALLBACK_ATTR
#endif // CONFIG_GPTIMER_ISR_IRAM_SAFE

static TEST_ALARM_CALLBACK_ATTR bool test_gptimer_alarm_stop_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    TaskHandle_t task_handle = (TaskHandle_t)user_data;
    BaseType_t high_task_wakeup;
    gptimer_stop(timer);
    vTaskNotifyGiveFromISR(task_handle, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

/**
 * @brief This function abstracts the behavior of performing the Backup-Reset-Restore process on the specified
 *        Timer group and is used as a helper function to test the retention function of the driver.
 *        If light-sleep feature is supported, this function will enter and exit a real light sleep or PD_TOP
 *        light sleep. Otherwise, it will trigger retention by software and reset the timer module to simulate
 *        a light-sleep in/out process to verify the driver's support for GPTimer sleep retention.
 *
 * @param timer Timer handle to be reset, created by `gptimer_new_timer()`
 * @param back_up_before_sleep Whether to back up GPTimer registers before sleep
 */
static void test_gptimer_survival_after_sleep_helper(gptimer_handle_t timer, bool back_up_before_sleep)
{
#if SOC_LIGHT_SLEEP_SUPPORTED
    esp_sleep_context_t sleep_ctx;
    esp_sleep_set_sleep_context(&sleep_ctx);
    printf("go to light sleep for 2 seconds\r\n");
#if ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(true));
#endif
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(2 * 1000 * 1000));
    TEST_ESP_OK(esp_light_sleep_start());

    printf("Waked up! Let's see if GPTimer driver can still work...\r\n");
#if ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(false));
#endif

    printf("check if the sleep happened as expected\r\n");
    TEST_ASSERT_EQUAL(0, sleep_ctx.sleep_request_result);
#if SOC_TIMER_SUPPORT_SLEEP_RETENTION && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
    if (back_up_before_sleep) {
        // Verify that when GPTimer retention is configured and sleep is requested,
        // the TOP power domain should be allowed to power down.
        TEST_ASSERT_EQUAL(PMU_SLEEP_PD_TOP, sleep_ctx.sleep_flags & PMU_SLEEP_PD_TOP);
    } else {
        // Verify that when GPTimer retention is not configured and sleep is requested, the TOP power
        // domain should not be allowed to power down to ensure the peripheral context is not lost.
        TEST_ASSERT_EQUAL(0, sleep_ctx.sleep_flags & PMU_SLEEP_PD_TOP);
    }
#endif
    esp_sleep_set_sleep_context(NULL);
#elif SOC_TIMER_SUPPORT_SLEEP_RETENTION && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
    if (back_up_before_sleep) {
        printf("Back up the timer group context in use and then reset it\r\n");
        sleep_retention_do_extra_retention(true);

        int group_id;
        gptimer_get_group_id(timer, &group_id);
        _timer_ll_reset_register(group_id);

        printf("Reset done! Let's restore its context and see if its driver can still work...\r\n");
        sleep_retention_do_extra_retention(false);
    }
#endif
}

/**
 * @brief Test the GPTimer driver can still work after light sleep
 *
 * @param back_up_before_sleep Whether to back up GPTimer registers before sleep
 */
static void test_gptimer_sleep_retention(bool back_up_before_sleep)
{
    TaskHandle_t task_handle =  xTaskGetCurrentTaskHandle();
    gptimer_config_t timer_config = {
        .resolution_hz = 10000, // 10KHz, 1 tick = 0.1ms
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .flags.backup_before_sleep = back_up_before_sleep,
    };
    gptimer_handle_t timer = NULL;
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &timer));
    gptimer_event_callbacks_t cbs = {
        .on_alarm = test_gptimer_alarm_stop_callback,
    };
    TEST_ESP_OK(gptimer_register_event_callbacks(timer, &cbs, task_handle));

    gptimer_alarm_config_t alarm_config = {
        .alarm_count = 10000, // alarm period = 1s
        .reload_count = 5000,
        .flags.auto_reload_on_alarm = true,
    };
    TEST_ESP_OK(gptimer_set_alarm_action(timer, &alarm_config));

    TEST_ESP_OK(gptimer_enable(timer));
    TEST_ESP_OK(gptimer_start(timer));

    /// counting from 0 to 10000, it's about 1 second
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1100)));
    uint64_t count_value_before_sleep = 0;
    TEST_ESP_OK(gptimer_get_raw_count(timer, &count_value_before_sleep));
    printf("count value before sleep: %llu\n", count_value_before_sleep);
    // the count value should near the reload value
    TEST_ASSERT_INT_WITHIN(1, 5000, count_value_before_sleep);

    // Note: don't enable the gptimer before going to sleep, ensure no power management lock is acquired by it
    TEST_ESP_OK(gptimer_disable(timer));

    test_gptimer_survival_after_sleep_helper(timer, back_up_before_sleep);

    uint64_t count_value_after_sleep = 0;
    TEST_ESP_OK(gptimer_get_raw_count(timer, &count_value_after_sleep));
    printf("count value after sleep wakeup: %llu\n", count_value_after_sleep);
    TEST_ASSERT_EQUAL(count_value_before_sleep, count_value_after_sleep);

    // re-enable the timer and start it
    TEST_ESP_OK(gptimer_enable(timer));
    TEST_ESP_OK(gptimer_start(timer));

    /// this time, the timer should count from 5000 to 10000, it's about 0.5 second
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(600)));

    TEST_ESP_OK(gptimer_get_raw_count(timer, &count_value_after_sleep));
    printf("gptimer count value: %llu\n", count_value_after_sleep);
    // the count value should near the reload value
    TEST_ASSERT_INT_WITHIN(1, 5000, count_value_after_sleep);

    TEST_ESP_OK(gptimer_disable(timer));
    TEST_ESP_OK(gptimer_del_timer(timer));
}

TEST_CASE("gptimer can work after light sleep", "[gptimer]")
{
    test_gptimer_sleep_retention(false);
#if SOC_TIMER_SUPPORT_SLEEP_RETENTION
    test_gptimer_sleep_retention(true);
#endif
}
