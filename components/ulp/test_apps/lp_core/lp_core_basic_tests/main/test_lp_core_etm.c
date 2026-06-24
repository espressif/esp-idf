/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "lp_core_test_app_counter.h"
#include "ulp_lp_core.h"
#include "test_shared.h"
#include "unity.h"
#include "test_utils.h"
#include "esp_log.h"
#include "driver/gptimer.h"
#include "lp_core_etm.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern const uint8_t lp_core_main_counter_bin_start[] asm("_binary_lp_core_test_app_counter_bin_start");
extern const uint8_t lp_core_main_counter_bin_end[]   asm("_binary_lp_core_test_app_counter_bin_end");

static void load_and_start_lp_core_firmware(ulp_lp_core_cfg_t* cfg, const uint8_t* firmware_start, const uint8_t* firmware_end)
{
    TEST_ASSERT(ulp_lp_core_load_binary(firmware_start,
                                        (firmware_end - firmware_start)) == ESP_OK);

    TEST_ASSERT(ulp_lp_core_run(cfg) == ESP_OK);

}

#define STOP_TEST_ITERATIONS 50
#define TIMER_PERIOD_MS 10

static int timer_cb_count;

static bool on_gptimer_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    timer_cb_count++;
    if (timer_cb_count >= STOP_TEST_ITERATIONS) {
        gptimer_stop(timer);
    }
    return false;
}

/**
 * @brief Test connects ULP wakeup source to a GP timer alarm ETM event.
 *        At every wakeup the ULP program increments a counter and in the
 *        end we check that the ULP woke-up the expected number of times.
 */
TEST_CASE("LP core can be woken up by ETM event", "[ulp]")
{
    // GPTimer alarm ---> ETM channel A ---> ULP wake-up
    printf("allocate etm channel\r\n");
    esp_etm_channel_config_t etm_config = {};
    esp_etm_channel_handle_t etm_channel_a;
    TEST_ESP_OK(esp_etm_new_channel(&etm_config, &etm_channel_a));

    esp_etm_task_handle_t lp_core_task = NULL;

    lp_core_etm_task_config_t lp_core_task_config = {
        .task_type = LP_CORE_TASK_WAKEUP_CPU,
    };
    TEST_ESP_OK(lp_core_new_etm_task(&lp_core_task_config, &lp_core_task));

    printf("create a gptimer\r\n");
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1 * 1000 * 1000, // 1MHz, 1 tick = 1us
    };
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &gptimer));

    printf("get gptimer etm event handle\r\n");
    esp_etm_event_handle_t gptimer_event = NULL;
    gptimer_etm_event_config_t gptimer_etm_event_conf = {
        .event_type = GPTIMER_ETM_EVENT_ALARM_MATCH,
    };
    TEST_ESP_OK(gptimer_new_etm_event(gptimer, &gptimer_etm_event_conf, &gptimer_event));

    printf("connect event and task to the channel\r\n");
    TEST_ESP_OK(esp_etm_channel_connect(etm_channel_a, gptimer_event, lp_core_task));

    printf("enable etm channel\r\n");
    TEST_ESP_OK(esp_etm_channel_enable(etm_channel_a));

    printf("set timer alarm action\r\n");
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = TIMER_PERIOD_MS * 1000, // 10ms per alarm event
        .flags.auto_reload_on_alarm = true,
    };
    TEST_ESP_OK(gptimer_set_alarm_action(gptimer, &alarm_config));

    printf("register alarm callback\r\n");
    gptimer_event_callbacks_t cbs = {
        .on_alarm = on_gptimer_alarm_cb,
    };
    TEST_ESP_OK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));

    /* Load ULP firmware and start the coprocessor */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_ETM,
#if ESP_ROM_HAS_LP_ROM
        .skip_lp_rom_boot = true,
#endif //ESP_ROM_HAS_LP_ROM
    };

    load_and_start_lp_core_firmware(&cfg, lp_core_main_counter_bin_start, lp_core_main_counter_bin_end);

    printf("enable and start timer\r\n");
    TEST_ESP_OK(gptimer_enable(gptimer));
    TEST_ESP_OK(gptimer_start(gptimer));

    // Wait for more than the expected time for the test to complete
    // To ensure that the ULP ran exactly as many times as we expected
    vTaskDelay((TIMER_PERIOD_MS * STOP_TEST_ITERATIONS * 2) / portTICK_PERIOD_MS);

    TEST_ASSERT_EQUAL(STOP_TEST_ITERATIONS, timer_cb_count);
    TEST_ASSERT_EQUAL(STOP_TEST_ITERATIONS, ulp_counter);

    TEST_ESP_OK(gptimer_disable(gptimer));
    TEST_ESP_OK(gptimer_del_timer(gptimer));

    TEST_ESP_OK(esp_etm_del_task(lp_core_task));
    TEST_ESP_OK(esp_etm_del_event(gptimer_event));
    TEST_ESP_OK(esp_etm_channel_disable(etm_channel_a));
    TEST_ESP_OK(esp_etm_del_channel(etm_channel_a));
}
