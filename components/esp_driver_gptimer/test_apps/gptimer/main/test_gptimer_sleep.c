/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "driver/gptimer.h"
#include "driver/gpio_etm.h"
#include "driver/gpio.h"
#include "soc/soc_caps.h"
#include "esp_sleep.h"
#include "esp_private/sleep_cpu.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/esp_pmu.h"

static bool test_gptimer_alarm_stop_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    TaskHandle_t task_handle = (TaskHandle_t)user_data;
    BaseType_t high_task_wakeup;
    gptimer_stop(timer);
    vTaskNotifyGiveFromISR(task_handle, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

/**
 * @brief Test the GPTimer driver can still work after light sleep
 *
 * @param allow_pd Whether to allow power down the peripheral in light sleep
 */
static void test_gptimer_sleep_retention(bool allow_pd)
{
    TaskHandle_t task_handle =  xTaskGetCurrentTaskHandle();
    gptimer_config_t timer_config = {
        .resolution_hz = 10000, // 10KHz, 1 tick = 0.1ms
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .flags.allow_pd = allow_pd,
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

    esp_sleep_context_t sleep_ctx;
    esp_sleep_set_sleep_context(&sleep_ctx);
    printf("go to light sleep for 2 seconds\r\n");
#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(true));
#endif
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(2 * 1000 * 1000));
    TEST_ESP_OK(esp_light_sleep_start());

    printf("Waked up! Let's see if GPTimer driver can still work...\r\n");
#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(false));
#endif

    printf("check if the sleep happened as expected\r\n");
    TEST_ASSERT_EQUAL(0, sleep_ctx.sleep_request_result);
#if SOC_TIMER_SUPPORT_SLEEP_RETENTION && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
    // check if the power domain also is powered down
    TEST_ASSERT_EQUAL(allow_pd ? PMU_SLEEP_PD_TOP : 0, (sleep_ctx.sleep_flags) & PMU_SLEEP_PD_TOP);
#endif
    esp_sleep_set_sleep_context(NULL);

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
    TEST_ASSERT_INT_WITHIN(5, 5000, count_value_after_sleep);

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

#if SOC_TIMER_SUPPORT_ETM
/**
 * @brief Test the GPTimer and ETM subsystem can still work after light sleep
 *
 * @param back_up_before_sleep Whether to back up GPTimer registers before sleep
 */
static void test_gptimer_etm_sleep_retention(bool back_up_before_sleep)
{
    const uint32_t output_gpio = 1;
    // GPTimer alarm ---> ETM channel A ---> GPTimer alarm re-enable
    // GPTimer alarm ---> ETM channel B ---> GPIO toggle
    printf("allocate etm channel\r\n");
    esp_etm_channel_config_t etm_config = {
        .flags.allow_pd = back_up_before_sleep,
    };
    esp_etm_channel_handle_t etm_channel_a, etm_channel_b;
    TEST_ESP_OK(esp_etm_new_channel(&etm_config, &etm_channel_a));
    TEST_ESP_OK(esp_etm_new_channel(&etm_config, &etm_channel_b));

    printf("initialize gpio\r\n");
    gpio_config_t task_gpio_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << output_gpio,
    };
    TEST_ESP_OK(gpio_config(&task_gpio_config));

    printf("create a gptimer\r\n");
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1 * 1000 * 1000, // 1MHz, 1 tick = 1us
        .flags.allow_pd = back_up_before_sleep,
    };
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &gptimer));

    printf("get gptimer etm event and task handle\r\n");
    esp_etm_event_handle_t gptimer_event = NULL;
    gptimer_etm_event_config_t gptimer_etm_event_conf = {
        .event_type = GPTIMER_ETM_EVENT_ALARM_MATCH,
    };
    TEST_ESP_OK(gptimer_new_etm_event(gptimer, &gptimer_etm_event_conf, &gptimer_event));
    esp_etm_task_handle_t gptimer_task = NULL;
    gptimer_etm_task_config_t gptimer_etm_task_conf = {
        .task_type = GPTIMER_ETM_TASK_EN_ALARM,
    };
    TEST_ESP_OK(gptimer_new_etm_task(gptimer, &gptimer_etm_task_conf, & gptimer_task));

    printf("connect event and task to the channel a\r\n");
    TEST_ESP_OK(esp_etm_channel_connect(etm_channel_a, gptimer_event, gptimer_task));

    printf("enable etm channels\r\n");
    TEST_ESP_OK(esp_etm_channel_enable(etm_channel_a));
    TEST_ESP_OK(esp_etm_channel_enable(etm_channel_b));

    printf("set timer alarm action\r\n");
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 100, // 100us per alarm event
        .flags.auto_reload_on_alarm = true,
    };
    TEST_ESP_OK(gptimer_set_alarm_action(gptimer, &alarm_config));

    // before going to sleep, ensure the gptimer is not enabled yet, otherwise it will acquire power management lock

    esp_sleep_context_t sleep_ctx;
    esp_sleep_set_sleep_context(&sleep_ctx);
    printf("go to light sleep for 2 seconds\r\n");
#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(true));
#endif
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(2 * 1000 * 1000));
    TEST_ESP_OK(esp_light_sleep_start());

    printf("Waked up! Let's see if GPTimer and ETM can still work...\r\n");
#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(false));
#endif

    printf("check if the sleep happened as expected\r\n");
    TEST_ASSERT_EQUAL(0, sleep_ctx.sleep_request_result);
#if SOC_TIMER_SUPPORT_SLEEP_RETENTION && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
    // check if the power domain also is powered down
    TEST_ASSERT_EQUAL(back_up_before_sleep ? PMU_SLEEP_PD_TOP : 0, (sleep_ctx.sleep_flags) & PMU_SLEEP_PD_TOP);
#endif
    esp_sleep_set_sleep_context(NULL);

    printf("enable and start timer\r\n");
    TEST_ESP_OK(gptimer_enable(gptimer));
    TEST_ESP_OK(gptimer_start(gptimer));

    printf("allocate GPIO etm task\r\n");
    esp_etm_task_handle_t gpio_task = NULL;
    gpio_etm_task_config_t gpio_task_config = {
        .action = GPIO_ETM_TASK_ACTION_TOG,
    };
    TEST_ESP_OK(gpio_new_etm_task(&gpio_task_config, &gpio_task));
    // set gpio number for the gpio etm primitives
    TEST_ESP_OK(gpio_etm_task_add_gpio(gpio_task, output_gpio));
    printf("connect event and task to the channel b\r\n");
    TEST_ESP_OK(esp_etm_channel_connect(etm_channel_b, gptimer_event, gpio_task));

    // delay sometime for us to view the waveform, should see a 5KHz square waveform
    vTaskDelay(pdMS_TO_TICKS(1000));

    // delete gptimer
    TEST_ESP_OK(gptimer_stop(gptimer));
    TEST_ESP_OK(gptimer_disable(gptimer));
    TEST_ESP_OK(gptimer_del_timer(gptimer));

    // delete etm primitives
    TEST_ESP_OK(gpio_etm_task_rm_gpio(gpio_task, output_gpio));
    TEST_ESP_OK(esp_etm_del_task(gpio_task));
    TEST_ESP_OK(esp_etm_del_task(gptimer_task));
    TEST_ESP_OK(esp_etm_del_event(gptimer_event));
    TEST_ESP_OK(esp_etm_channel_disable(etm_channel_a));
    TEST_ESP_OK(esp_etm_channel_disable(etm_channel_b));
    TEST_ESP_OK(esp_etm_del_channel(etm_channel_a));
    TEST_ESP_OK(esp_etm_del_channel(etm_channel_b));
}

TEST_CASE("gptimer and ETM can work after light sleep", "[gptimer]")
{
    test_gptimer_etm_sleep_retention(false);
#if SOC_TIMER_SUPPORT_SLEEP_RETENTION && SOC_ETM_SUPPORT_SLEEP_RETENTION
    test_gptimer_etm_sleep_retention(true);
#endif
}

#endif // SOC_TIMER_SUPPORT_ETM
