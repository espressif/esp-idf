/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "unity.h"
#include "unity_test_utils.h"
#include "esp_attr.h"
#include "driver/gptimer.h"
#include "driver/gpio_etm.h"
#include "driver/gpio.h"

#if CONFIG_GPTIMER_ISR_IRAM_SAFE
#define TEST_ALARM_CALLBACK_ATTR IRAM_ATTR
#else
#define TEST_ALARM_CALLBACK_ATTR
#endif // CONFIG_GPTIMER_ISR_IRAM_SAFE

TEST_ALARM_CALLBACK_ATTR
static bool on_gptimer_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    return false;
}

TEST_CASE("gptimer_etm_alarm_event_with_interrupt_enabled", "[gptimer][etm]")
{
    const uint32_t output_gpio = 1;
    // GPTimer alarm ---> ETM channel A ---> GPIO toggle
    printf("allocate etm channel\r\n");
    esp_etm_channel_config_t etm_config = {};
    esp_etm_channel_handle_t etm_channel_a;
    TEST_ESP_OK(esp_etm_new_channel(&etm_config, &etm_channel_a));

    printf("allocate GPIO etm task\r\n");
    esp_etm_task_handle_t gpio_task = NULL;
    gpio_etm_task_config_t gpio_task_config = {
        .action = GPIO_ETM_TASK_ACTION_TOG,
    };
    TEST_ESP_OK(gpio_new_etm_task(&gpio_task_config, &gpio_task));
    // set gpio number for the gpio etm primitives
    TEST_ESP_OK(gpio_etm_task_add_gpio(gpio_task, output_gpio));

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
    };
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &gptimer));

    printf("get gptimer etm event handle\r\n");
    esp_etm_event_handle_t gptimer_event = NULL;
    gptimer_etm_event_config_t gptimer_etm_event_conf = {
        .event_type = GPTIMER_ETM_EVENT_ALARM_MATCH,
    };
    TEST_ESP_OK(gptimer_new_etm_event(gptimer, &gptimer_etm_event_conf, &gptimer_event));

    printf("connect event and task to the channel\r\n");
    TEST_ESP_OK(esp_etm_channel_connect(etm_channel_a, gptimer_event, gpio_task));

    printf("enable etm channel\r\n");
    TEST_ESP_OK(esp_etm_channel_enable(etm_channel_a));

    printf("set timer alarm action\r\n");
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 100, // 100us per alarm event
        .flags.auto_reload_on_alarm = true,
    };
    TEST_ESP_OK(gptimer_set_alarm_action(gptimer, &alarm_config));

    printf("register alarm callback\r\n");
    gptimer_event_callbacks_t cbs = {
        .on_alarm = on_gptimer_alarm_cb,
    };
    TEST_ESP_OK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));

    printf("enable and start timer\r\n");
    TEST_ESP_OK(gptimer_enable(gptimer));
    TEST_ESP_OK(gptimer_start(gptimer));

    // delay sometime for us to view the waveform, should see a 5KHz square waveform
    vTaskDelay(pdMS_TO_TICKS(1000));

    // delete gptimer
    TEST_ESP_OK(gptimer_stop(gptimer));
    TEST_ESP_OK(gptimer_disable(gptimer));
    TEST_ESP_OK(gptimer_del_timer(gptimer));

    // delete etm primitives
    TEST_ESP_OK(gpio_etm_task_rm_gpio(gpio_task, output_gpio));
    TEST_ESP_OK(esp_etm_del_task(gpio_task));
    TEST_ESP_OK(esp_etm_del_event(gptimer_event));
    TEST_ESP_OK(esp_etm_channel_disable(etm_channel_a));
    TEST_ESP_OK(esp_etm_del_channel(etm_channel_a));
}

TEST_CASE("gptimer_etm_alarm_event_without_interrupt", "[gptimer][etm]")
{
    const uint32_t output_gpio = 1;
    // GPTimer alarm ---> ETM channel A ---> GPIO toggle
    // GPTimer alarm ---> ETM channel B ---> GPTimer alarm re-enable
    printf("allocate etm channel\r\n");
    esp_etm_channel_config_t etm_config = {};
    esp_etm_channel_handle_t etm_channel_a, etm_channel_b;
    TEST_ESP_OK(esp_etm_new_channel(&etm_config, &etm_channel_a));
    TEST_ESP_OK(esp_etm_new_channel(&etm_config, &etm_channel_b));

    printf("allocate GPIO etm task\r\n");
    esp_etm_task_handle_t gpio_task = NULL;
    gpio_etm_task_config_t gpio_task_config = {
        .action = GPIO_ETM_TASK_ACTION_TOG,
    };
    TEST_ESP_OK(gpio_new_etm_task(&gpio_task_config, &gpio_task));
    // set gpio number for the gpio etm primitives
    TEST_ESP_OK(gpio_etm_task_add_gpio(gpio_task, output_gpio));

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

    printf("connect event and task to the channel\r\n");
    TEST_ESP_OK(esp_etm_channel_connect(etm_channel_a, gptimer_event, gpio_task));
    TEST_ESP_OK(esp_etm_channel_connect(etm_channel_b, gptimer_event, gptimer_task));

    printf("enable etm channel\r\n");
    TEST_ESP_OK(esp_etm_channel_enable(etm_channel_a));
    TEST_ESP_OK(esp_etm_channel_enable(etm_channel_b));

    printf("set timer alarm action\r\n");
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 100, // 100us per alarm event
        .flags.auto_reload_on_alarm = true,
    };
    TEST_ESP_OK(gptimer_set_alarm_action(gptimer, &alarm_config));

    printf("enable and start timer\r\n");
    TEST_ESP_OK(gptimer_enable(gptimer));
    TEST_ESP_OK(gptimer_start(gptimer));

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

TEST_CASE("gptimer_auto_reload_by_etm", "[gptimer][etm]")
{
    const uint32_t output_gpio = 1;
    // GPTimer alarm ---> ETM channel A ---> GPIO toggle
    // GPTimer alarm ---> ETM channel B ---> GPTimer alarm re-enable
    // GPTimer alarm ---> ETM channel C ---> GPTimer reload
    printf("allocate etm channel\r\n");
    esp_etm_channel_config_t etm_config = {};
    esp_etm_channel_handle_t etm_channel_a, etm_channel_b, etm_channel_c;
    TEST_ESP_OK(esp_etm_new_channel(&etm_config, &etm_channel_a));
    TEST_ESP_OK(esp_etm_new_channel(&etm_config, &etm_channel_b));
    TEST_ESP_OK(esp_etm_new_channel(&etm_config, &etm_channel_c));

    printf("allocate GPIO etm task\r\n");
    esp_etm_task_handle_t gpio_task = NULL;
    gpio_etm_task_config_t gpio_task_config = {
        .action = GPIO_ETM_TASK_ACTION_TOG,
    };
    TEST_ESP_OK(gpio_new_etm_task(&gpio_task_config, &gpio_task));
    // set gpio number for the gpio etm primitives
    TEST_ESP_OK(gpio_etm_task_add_gpio(gpio_task, output_gpio));

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
    };
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &gptimer));

    printf("get gptimer etm event and task handle\r\n");
    esp_etm_event_handle_t gptimer_event_alarm = NULL;
    gptimer_etm_event_config_t gptimer_etm_event_conf = {
        .event_type = GPTIMER_ETM_EVENT_ALARM_MATCH,
    };
    TEST_ESP_OK(gptimer_new_etm_event(gptimer, &gptimer_etm_event_conf, &gptimer_event_alarm));
    esp_etm_task_handle_t gptimer_task_en_alarm = NULL;
    gptimer_etm_task_config_t gptimer_etm_task_conf = {
        .task_type = GPTIMER_ETM_TASK_EN_ALARM,
    };
    TEST_ESP_OK(gptimer_new_etm_task(gptimer, &gptimer_etm_task_conf, &gptimer_task_en_alarm));
    esp_etm_task_handle_t gptimer_task_reload = NULL;
    gptimer_etm_task_conf.task_type = GPTIMER_ETM_TASK_RELOAD;
    TEST_ESP_OK(gptimer_new_etm_task(gptimer, &gptimer_etm_task_conf, &gptimer_task_reload));

    printf("connect event and task to the channel\r\n");
    TEST_ESP_OK(esp_etm_channel_connect(etm_channel_a, gptimer_event_alarm, gpio_task));
    TEST_ESP_OK(esp_etm_channel_connect(etm_channel_b, gptimer_event_alarm, gptimer_task_en_alarm));
    TEST_ESP_OK(esp_etm_channel_connect(etm_channel_c, gptimer_event_alarm, gptimer_task_reload));

    printf("enable etm channel\r\n");
    TEST_ESP_OK(esp_etm_channel_enable(etm_channel_a));
    TEST_ESP_OK(esp_etm_channel_enable(etm_channel_b));
    TEST_ESP_OK(esp_etm_channel_enable(etm_channel_c));

    printf("dump the etm channel usage\r\n");
    TEST_ESP_OK(esp_etm_dump(stdout));

    printf("set timer alarm action\r\n");
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 100, // 100us per alarm event
        .flags.auto_reload_on_alarm = false, // reload will be done by ETM channel C
    };
    TEST_ESP_OK(gptimer_set_alarm_action(gptimer, &alarm_config));

    printf("enable and start timer\r\n");
    TEST_ESP_OK(gptimer_enable(gptimer));
    TEST_ESP_OK(gptimer_start(gptimer));

    // delay sometime for us to view the waveform, should see a 5KHz square waveform
    vTaskDelay(pdMS_TO_TICKS(1000));

    // delete gptimer
    TEST_ESP_OK(gptimer_stop(gptimer));
    TEST_ESP_OK(gptimer_disable(gptimer));
    TEST_ESP_OK(gptimer_del_timer(gptimer));

    // delete etm primitives
    TEST_ESP_OK(gpio_etm_task_rm_gpio(gpio_task, output_gpio));
    TEST_ESP_OK(esp_etm_del_task(gpio_task));
    TEST_ESP_OK(esp_etm_del_task(gptimer_task_en_alarm));
    TEST_ESP_OK(esp_etm_del_task(gptimer_task_reload));
    TEST_ESP_OK(esp_etm_del_event(gptimer_event_alarm));
    TEST_ESP_OK(esp_etm_channel_disable(etm_channel_a));
    TEST_ESP_OK(esp_etm_channel_disable(etm_channel_b));
    TEST_ESP_OK(esp_etm_channel_disable(etm_channel_c));
    TEST_ESP_OK(esp_etm_del_channel(etm_channel_a));
    TEST_ESP_OK(esp_etm_del_channel(etm_channel_b));
    TEST_ESP_OK(esp_etm_del_channel(etm_channel_c));
}

TEST_CASE("gptimer_etm_task_capture", "[gptimer][etm]")
{
    const uint32_t input_gpio = 0;
    // GPIO Posedge ---> ETM channel A ---> GPTimer capture
    printf("allocate etm channel\r\n");
    esp_etm_channel_config_t etm_config = {};
    esp_etm_channel_handle_t etm_channel_a;
    TEST_ESP_OK(esp_etm_new_channel(&etm_config, &etm_channel_a));

    printf("allocate GPIO etm event\r\n");
    esp_etm_event_handle_t gpio_event = NULL;
    gpio_etm_event_config_t gpio_event_config = {
        .edge = GPIO_ETM_EVENT_EDGE_POS,
    };
    TEST_ESP_OK(gpio_new_etm_event(&gpio_event_config, &gpio_event));
    // set gpio number for the gpio etm primitives
    TEST_ESP_OK(gpio_etm_event_bind_gpio(gpio_event, input_gpio));

    printf("initialize gpio\r\n");
    gpio_config_t task_gpio_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT_OUTPUT, // we want to simulate the edge signal by software, so it should be input and output
        .pin_bit_mask = 1ULL << input_gpio,
    };
    TEST_ESP_OK(gpio_config(&task_gpio_config));
    TEST_ESP_OK(gpio_set_level(input_gpio, 0));

    printf("create a gptimer\r\n");
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1 * 1000 * 1000, // 1MHz, 1 tick = 1us
    };
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &gptimer));

    printf("get gptimer etm task handle\r\n");
    esp_etm_task_handle_t gptimer_task = NULL;
    gptimer_etm_task_config_t gptimer_etm_task_conf = {
        .task_type = GPTIMER_ETM_TASK_CAPTURE,
    };
    TEST_ESP_OK(gptimer_new_etm_task(gptimer, &gptimer_etm_task_conf, &gptimer_task));

    printf("connect event and task to the channel\r\n");
    TEST_ESP_OK(esp_etm_channel_connect(etm_channel_a, gpio_event, gptimer_task));

    printf("enable etm channel\r\n");
    TEST_ESP_OK(esp_etm_channel_enable(etm_channel_a));

    printf("enable and start gptimer\r\n");
    TEST_ESP_OK(gptimer_enable(gptimer));
    TEST_ESP_OK(gptimer_start(gptimer));

    esp_rom_delay_us(500 * 1000);

    // simulate the edge signal by software
    TEST_ESP_OK(gpio_set_level(input_gpio, 1));
    TEST_ESP_OK(gpio_set_level(input_gpio, 0));

    uint64_t capture_value = 0;
    TEST_ESP_OK(gptimer_get_captured_count(gptimer, &capture_value));
    printf("capture value: %llu\r\n", capture_value);
    // should be around 500us
    TEST_ASSERT_UINT_WITHIN(1000, 500000, capture_value);

    // delete gptimer
    TEST_ESP_OK(gptimer_stop(gptimer));
    TEST_ESP_OK(gptimer_disable(gptimer));
    TEST_ESP_OK(gptimer_del_timer(gptimer));

    // delete etm primitives
    TEST_ESP_OK(esp_etm_del_task(gptimer_task));
    TEST_ESP_OK(esp_etm_del_event(gpio_event));
    TEST_ESP_OK(esp_etm_channel_disable(etm_channel_a));
    TEST_ESP_OK(esp_etm_del_channel(etm_channel_a));
}

TEST_CASE("gptimer_start_stop_by_etm_task", "[gptimer][etm]")
{
    const uint32_t input_gpio = 0;
    // GPIO pos edge ---> ETM channel A ---> GPTimer start
    // GPIO neg edge ---> ETM channel B ---> GPTimer stop
    printf("allocate etm channel\r\n");
    esp_etm_channel_config_t etm_config = {};
    esp_etm_channel_handle_t etm_channel_a, etm_channel_b;
    TEST_ESP_OK(esp_etm_new_channel(&etm_config, &etm_channel_a));
    TEST_ESP_OK(esp_etm_new_channel(&etm_config, &etm_channel_b));

    printf("allocate GPIO etm events\r\n");
    esp_etm_event_handle_t gpio_event_pos, gpio_event_neg;
    gpio_etm_event_config_t gpio_event_config = {
        .edge = GPIO_ETM_EVENT_EDGE_POS,
    };
    TEST_ESP_OK(gpio_new_etm_event(&gpio_event_config, &gpio_event_pos));
    gpio_event_config.edge = GPIO_ETM_EVENT_EDGE_NEG;
    TEST_ESP_OK(gpio_new_etm_event(&gpio_event_config, &gpio_event_neg));

    // set gpio number for the gpio etm primitives
    TEST_ESP_OK(gpio_etm_event_bind_gpio(gpio_event_pos, input_gpio));
    TEST_ESP_OK(gpio_etm_event_bind_gpio(gpio_event_neg, input_gpio));

    printf("initialize gpio\r\n");
    gpio_config_t task_gpio_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT_OUTPUT, // we want to simulate the edge signal by software, so it should be input and output
        .pin_bit_mask = 1ULL << input_gpio,
    };
    TEST_ESP_OK(gpio_config(&task_gpio_config));

    // put the gpio into initial state
    TEST_ESP_OK(gpio_set_level(input_gpio, 0));

    printf("create a gptimer\r\n");
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1 * 1000 * 1000, // 1MHz, 1 tick = 1us
    };
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &gptimer));

    printf("get gptimer etm task handle\r\n");
    esp_etm_task_handle_t gptimer_task_start, gptimer_task_stop;
    gptimer_etm_task_config_t gptimer_etm_task_conf = {
        .task_type = GPTIMER_ETM_TASK_START_COUNT,
    };
    TEST_ESP_OK(gptimer_new_etm_task(gptimer, &gptimer_etm_task_conf, &gptimer_task_start));
    gptimer_etm_task_conf.task_type = GPTIMER_ETM_TASK_STOP_COUNT;
    TEST_ESP_OK(gptimer_new_etm_task(gptimer, &gptimer_etm_task_conf, &gptimer_task_stop));

    printf("connect event and task to the channel\r\n");
    TEST_ESP_OK(esp_etm_channel_connect(etm_channel_a, gpio_event_pos, gptimer_task_start));
    TEST_ESP_OK(esp_etm_channel_connect(etm_channel_b, gpio_event_neg, gptimer_task_stop));

    printf("enable etm channel\r\n");
    TEST_ESP_OK(esp_etm_channel_enable(etm_channel_a));
    TEST_ESP_OK(esp_etm_channel_enable(etm_channel_b));

    printf("enable timer\r\n");
    TEST_ESP_OK(gptimer_enable(gptimer));

    // trigger an pos-edge, this should start the gptimer
    TEST_ESP_OK(gpio_set_level(input_gpio, 1));
    esp_rom_delay_us(500 * 1000);
    uint64_t cur_count_val = 0;
    TEST_ESP_OK(gptimer_get_raw_count(gptimer, &cur_count_val));
    printf("cur_count_val: %llu\r\n", cur_count_val);
    TEST_ASSERT_UINT_WITHIN(1000, 500000, cur_count_val);

    // trigger an neg-edge, this should stop the gptimer
    TEST_ESP_OK(gpio_set_level(input_gpio, 0));
    uint64_t count_val_0 = 0;
    TEST_ESP_OK(gptimer_get_raw_count(gptimer, &count_val_0));
    esp_rom_delay_us(500 * 1000);
    uint64_t count_val_1 = 0;
    TEST_ESP_OK(gptimer_get_raw_count(gptimer, &count_val_1));
    TEST_ASSERT_EQUAL(count_val_0, count_val_1);

    // delete gptimer
    TEST_ESP_OK(gptimer_disable(gptimer));
    TEST_ESP_OK(gptimer_del_timer(gptimer));

    // delete etm primitives
    TEST_ESP_OK(esp_etm_del_task(gptimer_task_start));
    TEST_ESP_OK(esp_etm_del_task(gptimer_task_stop));
    TEST_ESP_OK(esp_etm_del_event(gpio_event_pos));
    TEST_ESP_OK(esp_etm_del_event(gpio_event_neg));
    TEST_ESP_OK(esp_etm_channel_disable(etm_channel_a));
    TEST_ESP_OK(esp_etm_channel_disable(etm_channel_b));
    TEST_ESP_OK(esp_etm_del_channel(etm_channel_a));
    TEST_ESP_OK(esp_etm_del_channel(etm_channel_b));
}
