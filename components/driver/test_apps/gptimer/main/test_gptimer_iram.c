/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "unity.h"
#include "unity_test_utils.h"
#include "esp_attr.h"
#include "driver/gptimer.h"

static bool IRAM_ATTR on_gptimer_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    uint32_t *alarm_counts = (uint32_t *)user_ctx;
    (*alarm_counts)++;
    return false;
}

static void IRAM_ATTR test_delay_post_cache_disable(void *args)
{
    esp_rom_delay_us(1000);
}

TEST_CASE("gptimer_interrupt_iram_safe", "[gptimer]")
{
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1 * 1000 * 1000, // 1MHz, 1 tick = 1us
    };
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &gptimer));
    gptimer_event_callbacks_t cbs = {
        .on_alarm = on_gptimer_alarm_cb,
    };
    uint32_t alarm_counts = 0;
    TEST_ESP_OK(gptimer_register_event_callbacks(gptimer, &cbs, &alarm_counts));
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 100, // 100us per alarm event
        .flags.auto_reload_on_alarm = true,
    };
    TEST_ESP_OK(gptimer_set_alarm_action(gptimer, &alarm_config));
    TEST_ESP_OK(gptimer_enable(gptimer));
    TEST_ESP_OK(gptimer_start(gptimer));

    vTaskDelay(pdMS_TO_TICKS(10));

    printf("disable flash cache and check the alarm events are still in working\r\n");
    for (int i = 0; i < 10; i++) {
        unity_utils_run_cache_disable_stub(test_delay_post_cache_disable, NULL);
    }
    printf("alarm counts: %"PRIu32"\r\n", alarm_counts);
    TEST_ASSERT_GREATER_THAN(150, alarm_counts);

    // delete gptimer
    TEST_ESP_OK(gptimer_stop(gptimer));
    TEST_ESP_OK(gptimer_disable(gptimer));
    TEST_ESP_OK(gptimer_del_timer(gptimer));
}
