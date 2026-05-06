/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "esp_attr.h"
#include "esp_etm.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "driver/gpio_etm.h"
#include "driver/gptimer_etm.h"
#include "test_ana_cmpr.h"

#define TEST_TIME_US        5000

static gptimer_handle_t test_ana_cmpr_gptimer_init(void)
{
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1 * 1000 * 1000, // 1MHz, 1 tick = 1us
    };
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &gptimer));
    TEST_ESP_OK(gptimer_set_raw_count(gptimer, 0));
    TEST_ESP_OK(gptimer_enable(gptimer));
    return gptimer;
}

static void test_ana_cmpr_gptimer_deinit(gptimer_handle_t gptimer)
{
    TEST_ESP_OK(gptimer_disable(gptimer));
    TEST_ESP_OK(gptimer_del_timer(gptimer));
}

static ana_cmpr_handle_t test_ana_cmpr_init(void)
{
    ana_cmpr_handle_t cmpr = NULL;

    ana_cmpr_config_t config = {
        .unit = TEST_ANA_CMPR_UNIT_ID,
        .clk_src = ANA_CMPR_CLK_SRC_DEFAULT,
        .ref_src = ANA_CMPR_REF_SRC_INTERNAL,
        .cross_type = ANA_CMPR_CROSS_ANY,
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
        .src_chan0_gpio = ana_cmpr_periph[TEST_ANA_CMPR_UNIT_ID].pad_gpios[0],
        .resample_limit = 3,
#endif
    };
    TEST_ESP_OK(ana_cmpr_new_unit(&config, &cmpr));

    ana_cmpr_internal_ref_config_t ref_cfg = {
        .ref_volt = ANA_CMPR_REF_VOLT_50_PCT_VDD,
    };
    TEST_ESP_OK(ana_cmpr_set_internal_reference(cmpr, &ref_cfg));

    ana_cmpr_debounce_config_t dbc_cfg = {
        .wait_us = 10,
    };
    TEST_ESP_OK(ana_cmpr_set_debounce(cmpr, &dbc_cfg));

#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    ana_cmpr_scan_config_t scan_cfg = {
        .scan_mode = ANA_CMPR_SCAN_MODE_FULL,
        .poll_period_us = 2,
    };
    TEST_ESP_OK(ana_cmpr_set_scan_config(cmpr, &scan_cfg));
#endif

    TEST_ESP_OK(ana_cmpr_enable(cmpr));

    return cmpr;
}

static void test_ana_cmpr_deinit(ana_cmpr_handle_t cmpr)
{
    TEST_ESP_OK(ana_cmpr_disable(cmpr));
    TEST_ESP_OK(ana_cmpr_del_unit(cmpr));
}

typedef struct {
    esp_etm_event_handle_t cmpr_pos_evt;
    esp_etm_event_handle_t cmpr_neg_evt;
    esp_etm_task_handle_t gptimer_start_task;
    esp_etm_task_handle_t gptimer_stop_task;
    esp_etm_channel_handle_t etm_pos_handle;
    esp_etm_channel_handle_t etm_neg_handle;
} test_ana_cmpr_etm_handles_t;

/*
 * ETM chain v1: ana_cmpr cross events drive gptimer start/stop tasks.
 *
 * The comparator input is toggled by software in the test body. ETM only forwards
 * the comparator crossing events to the timer control tasks.
 *
 *   src_gpio falling edge
 *          |
 *          v
 *   ana_cmpr NEG_CROSS event --ETM--> GPTIMER_ETM_TASK_START_COUNT
 *
 *   src_gpio rising edge
 *          |
 *          v
 *   ana_cmpr POS_CROSS event --ETM--> GPTIMER_ETM_TASK_STOP_COUNT
 *
 * Result: gptimer counts the low pulse width between the negative and positive
 * comparator crossings.
 */
static test_ana_cmpr_etm_handles_t test_ana_cmpr_init_etm_chain_v1(ana_cmpr_handle_t cmpr, gptimer_handle_t gptimer)
{
    test_ana_cmpr_etm_handles_t etm_handles = {};

    /* Allocate the analog comparator positive & negative cross events */
    ana_cmpr_etm_event_config_t evt_cfg = {
        .event_type = ANA_CMPR_EVENT_POS_CROSS,
    };
    TEST_ESP_OK(ana_cmpr_new_etm_event(cmpr, &evt_cfg, &etm_handles.cmpr_pos_evt));
    evt_cfg.event_type = ANA_CMPR_EVENT_NEG_CROSS;
    TEST_ESP_OK(ana_cmpr_new_etm_event(cmpr, &evt_cfg, &etm_handles.cmpr_neg_evt));

    /* Allocate the GPTimer tasks */
    gptimer_etm_task_config_t gptimer_etm_task_conf = {
        .task_type = GPTIMER_ETM_TASK_START_COUNT,
    };
    TEST_ESP_OK(gptimer_new_etm_task(gptimer, &gptimer_etm_task_conf, &etm_handles.gptimer_start_task));
    gptimer_etm_task_conf.task_type = GPTIMER_ETM_TASK_STOP_COUNT;
    TEST_ESP_OK(gptimer_new_etm_task(gptimer, &gptimer_etm_task_conf, &etm_handles.gptimer_stop_task));

    /* Allocate the Event Task Matrix channels */
    esp_etm_channel_config_t etm_cfg = {};
    TEST_ESP_OK(esp_etm_new_channel(&etm_cfg, &etm_handles.etm_pos_handle));
    TEST_ESP_OK(esp_etm_new_channel(&etm_cfg, &etm_handles.etm_neg_handle));
    /* Bind the events and tasks */
    TEST_ESP_OK(esp_etm_channel_connect(etm_handles.etm_neg_handle, etm_handles.cmpr_neg_evt, etm_handles.gptimer_start_task));
    TEST_ESP_OK(esp_etm_channel_connect(etm_handles.etm_pos_handle, etm_handles.cmpr_pos_evt, etm_handles.gptimer_stop_task));
    /* Enable the ETM channels */
    TEST_ESP_OK(esp_etm_channel_enable(etm_handles.etm_pos_handle));
    TEST_ESP_OK(esp_etm_channel_enable(etm_handles.etm_neg_handle));

    return etm_handles;
}

static void test_ana_cmpr_deinit_etm_chain_v1(test_ana_cmpr_etm_handles_t handles)
{
    TEST_ESP_OK(esp_etm_channel_disable(handles.etm_pos_handle));
    TEST_ESP_OK(esp_etm_channel_disable(handles.etm_neg_handle));

    TEST_ESP_OK(esp_etm_del_task(handles.gptimer_start_task));
    TEST_ESP_OK(esp_etm_del_task(handles.gptimer_stop_task));

    TEST_ESP_OK(esp_etm_del_event(handles.cmpr_pos_evt));
    TEST_ESP_OK(esp_etm_del_event(handles.cmpr_neg_evt));

    TEST_ESP_OK(esp_etm_del_channel(handles.etm_pos_handle));
    TEST_ESP_OK(esp_etm_del_channel(handles.etm_neg_handle));
}

TEST_CASE("ana_cmpr etm cross event", "[ana_cmpr][etm]")
{
    gptimer_handle_t gptimer = test_ana_cmpr_gptimer_init();
    ana_cmpr_handle_t cmpr = test_ana_cmpr_init();
    gpio_num_t src_gpio = test_init_src_chan_gpio(cmpr, 0, 1);
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    // Trigger a scan to make sure the initial level is sampled
    TEST_ESP_OK(ana_cmpr_trigger_scan(cmpr));
#endif

    test_ana_cmpr_etm_handles_t handles = test_ana_cmpr_init_etm_chain_v1(cmpr, gptimer);

    // triggers a negative pulse, whose duration is ~TEST_TIME_US
    // negedge triggers the gptimer to start task
    // posedge triggers the gptimer to stop task
    // gptimer will record the time between the negedge and posedge
    gpio_set_level(src_gpio, 0);
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    TEST_ESP_OK(ana_cmpr_trigger_scan(cmpr));
#endif
    esp_rom_delay_us(TEST_TIME_US);
    gpio_set_level(src_gpio, 1);
#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1
    TEST_ESP_OK(ana_cmpr_trigger_scan(cmpr));
#endif

    // the gptimer should already stopped, so delay any time here is ok
    vTaskDelay(10);

    uint64_t cnt_us = 0;
    TEST_ESP_OK(gptimer_get_raw_count(gptimer, &cnt_us));
    printf("Count: %" PRIu64 "\n", cnt_us);
    // gptimer timer should stopped
    uint64_t cnt_us_again = 0;
    TEST_ESP_OK(gptimer_get_raw_count(gptimer, &cnt_us_again));
    TEST_ASSERT_EQUAL(cnt_us, cnt_us_again);

    test_ana_cmpr_deinit_etm_chain_v1(handles);
    test_ana_cmpr_deinit(cmpr);
    test_ana_cmpr_gptimer_deinit(gptimer);

    TEST_ASSERT_UINT_WITHIN(TEST_TIME_US * 0.1, TEST_TIME_US, cnt_us);
}

#if ANALOG_CMPR_LL_GET(IP_VERSION) > 1

TEST_CASE("ana_cmpr etm event delete keeps sibling source event working", "[ana_cmpr][etm]")
{
    gptimer_handle_t gptimer = test_ana_cmpr_gptimer_init();
    ana_cmpr_handle_t cmpr = test_ana_cmpr_init();
    gpio_num_t src_gpio = test_init_src_chan_gpio(cmpr, 0, 1);
    // Trigger a scan to make sure the initial level is sampled
    TEST_ESP_OK(ana_cmpr_trigger_scan(cmpr));

    ana_cmpr_etm_event_config_t evt_cfg = {
        .src_chan_id = 0,
        .event_type = ANA_CMPR_EVENT_POS_CROSS,
    };
    esp_etm_event_handle_t pos_evt = NULL;
    esp_etm_event_handle_t neg_evt = NULL;
    TEST_ESP_OK(ana_cmpr_new_etm_event(cmpr, &evt_cfg, &pos_evt));
    evt_cfg.event_type = ANA_CMPR_EVENT_NEG_CROSS;
    TEST_ESP_OK(ana_cmpr_new_etm_event(cmpr, &evt_cfg, &neg_evt));

    gptimer_etm_task_config_t gptimer_task_cfg = {
        .task_type = GPTIMER_ETM_TASK_START_COUNT,
    };
    esp_etm_task_handle_t start_task = NULL;
    TEST_ESP_OK(gptimer_new_etm_task(gptimer, &gptimer_task_cfg, &start_task));

    esp_etm_channel_config_t channel_cfg = {};
    esp_etm_channel_handle_t neg_handle = NULL;
    TEST_ESP_OK(esp_etm_new_channel(&channel_cfg, &neg_handle));
    TEST_ESP_OK(esp_etm_channel_connect(neg_handle, neg_evt, start_task));
    TEST_ESP_OK(esp_etm_channel_enable(neg_handle));

    // Removing the positive-cross event must not disable ETM routing for the
    // remaining negative-cross event on the same source channel.
    TEST_ESP_OK(esp_etm_del_event(pos_evt));

    TEST_ESP_OK(gptimer_set_raw_count(gptimer, 0));
    TEST_ESP_OK(gpio_set_level(src_gpio, 0));
    TEST_ESP_OK(ana_cmpr_trigger_scan(cmpr));
    esp_rom_delay_us(50);

    uint64_t cnt_us = 0;
    TEST_ESP_OK(gptimer_get_raw_count(gptimer, &cnt_us));

    TEST_ESP_OK(esp_etm_channel_disable(neg_handle));
    TEST_ESP_OK(esp_etm_del_channel(neg_handle));
    TEST_ESP_OK(esp_etm_del_task(start_task));
    TEST_ESP_OK(esp_etm_del_event(neg_evt));
    test_ana_cmpr_deinit(cmpr);
    test_ana_cmpr_gptimer_deinit(gptimer);

    TEST_ASSERT_GREATER_THAN_UINT32(20, (uint32_t)cnt_us);
}

#define TEST_ETM_MONITOR_GPIO    4
#define TEST_ETM_SCAN_PERIOD_US  50
#define TEST_ETM_WAIT_TIMEOUT_US 2000

typedef struct {
    esp_etm_event_handle_t gptimer_alarm_evt;
    esp_etm_event_handle_t cmpr_pos_evt;
    esp_etm_event_handle_t cmpr_neg_evt;
    esp_etm_task_handle_t gptimer_en_alarm_task;
    esp_etm_task_handle_t cmpr_start_task;
    esp_etm_task_handle_t gpio_set_task;
    esp_etm_task_handle_t gpio_clr_task;
    esp_etm_channel_handle_t etm_reload_handle;
    esp_etm_channel_handle_t etm_scan_handle;
    esp_etm_channel_handle_t etm_pos_handle;
    esp_etm_channel_handle_t etm_neg_handle;
} test_ana_cmpr_etm_task_handles_t;

static void test_ana_cmpr_wait_output_level(ana_cmpr_handle_t cmpr, int src_chan_id, bool expect_level)
{
    bool out_level = !expect_level;
    for (int i = 0; i < TEST_ETM_WAIT_TIMEOUT_US; i++) {
        TEST_ESP_OK(ana_cmpr_get_output_level(cmpr, src_chan_id, &out_level));
        if (out_level == expect_level) {
            return;
        }
        esp_rom_delay_us(1);
    }
    TEST_ASSERT_EQUAL(expect_level, out_level);
}

/*
 * ETM chain v2: gptimer alarm events periodically trigger ana_cmpr scans, and the
 * resulting comparator crossings drive a monitor GPIO.
 *
 *   gptimer ALARM_MATCH event --ETM--> GPTIMER_ETM_TASK_EN_ALARM
 *                           |
 *                           +-------> ANA_CMPR_TASK_START
 *                                         |
 *                                         v
 *                                  ana_cmpr samples src_gpio
 *                                         |
 *                    +--------------------+--------------------+
 *                    |                                         |
 *                    v                                         v
 *   ana_cmpr POS_CROSS event --ETM--> GPIO_ETM_TASK_SET   ana_cmpr NEG_CROSS event --ETM--> GPIO_ETM_TASK_CLR
 *                                                                                                          |
 *                                                                                                          v
 *                                                                                                 monitor GPIO level
 *
 * Result: the monitor GPIO mirrors the comparator output, proving the full
 * gptimer -> ana_cmpr task -> ana_cmpr event -> gpio task ETM chain works.
 */
static test_ana_cmpr_etm_task_handles_t test_ana_cmpr_init_etm_chain_v2(ana_cmpr_handle_t cmpr, gptimer_handle_t gptimer)
{
    test_ana_cmpr_etm_task_handles_t etm_handles = {};

    gptimer_etm_event_config_t gptimer_evt_cfg = {
        .event_type = GPTIMER_ETM_EVENT_ALARM_MATCH,
    };
    TEST_ESP_OK(gptimer_new_etm_event(gptimer, &gptimer_evt_cfg, &etm_handles.gptimer_alarm_evt));

    gptimer_etm_task_config_t gptimer_task_cfg = {
        .task_type = GPTIMER_ETM_TASK_EN_ALARM,
    };
    TEST_ESP_OK(gptimer_new_etm_task(gptimer, &gptimer_task_cfg, &etm_handles.gptimer_en_alarm_task));

    ana_cmpr_etm_task_config_t cmpr_task_cfg = {
        .task_type = ANA_CMPR_TASK_START,
    };
    TEST_ESP_OK(ana_cmpr_new_etm_task(cmpr, &cmpr_task_cfg, &etm_handles.cmpr_start_task));

    ana_cmpr_etm_event_config_t cmpr_evt_cfg = {
        .event_type = ANA_CMPR_EVENT_POS_CROSS,
    };
    TEST_ESP_OK(ana_cmpr_new_etm_event(cmpr, &cmpr_evt_cfg, &etm_handles.cmpr_pos_evt));
    cmpr_evt_cfg.event_type = ANA_CMPR_EVENT_NEG_CROSS;
    TEST_ESP_OK(ana_cmpr_new_etm_event(cmpr, &cmpr_evt_cfg, &etm_handles.cmpr_neg_evt));

    gpio_etm_task_config_t gpio_task_cfg = {};
    gpio_task_cfg.actions[0] = GPIO_ETM_TASK_ACTION_SET;
    gpio_task_cfg.actions[1] = GPIO_ETM_TASK_ACTION_CLR;
    TEST_ESP_OK(gpio_new_etm_task(&gpio_task_cfg, &etm_handles.gpio_set_task, &etm_handles.gpio_clr_task));
    TEST_ESP_OK(gpio_etm_task_add_gpio(etm_handles.gpio_set_task, TEST_ETM_MONITOR_GPIO));
    TEST_ESP_OK(gpio_etm_task_add_gpio(etm_handles.gpio_clr_task, TEST_ETM_MONITOR_GPIO));

    esp_etm_channel_config_t etm_cfg = {};
    TEST_ESP_OK(esp_etm_new_channel(&etm_cfg, &etm_handles.etm_reload_handle));
    TEST_ESP_OK(esp_etm_new_channel(&etm_cfg, &etm_handles.etm_scan_handle));
    TEST_ESP_OK(esp_etm_new_channel(&etm_cfg, &etm_handles.etm_pos_handle));
    TEST_ESP_OK(esp_etm_new_channel(&etm_cfg, &etm_handles.etm_neg_handle));

    TEST_ESP_OK(esp_etm_channel_connect(etm_handles.etm_reload_handle, etm_handles.gptimer_alarm_evt, etm_handles.gptimer_en_alarm_task));
    TEST_ESP_OK(esp_etm_channel_connect(etm_handles.etm_scan_handle, etm_handles.gptimer_alarm_evt, etm_handles.cmpr_start_task));
    TEST_ESP_OK(esp_etm_channel_connect(etm_handles.etm_pos_handle, etm_handles.cmpr_pos_evt, etm_handles.gpio_set_task));
    TEST_ESP_OK(esp_etm_channel_connect(etm_handles.etm_neg_handle, etm_handles.cmpr_neg_evt, etm_handles.gpio_clr_task));

    TEST_ESP_OK(esp_etm_channel_enable(etm_handles.etm_reload_handle));
    TEST_ESP_OK(esp_etm_channel_enable(etm_handles.etm_scan_handle));
    TEST_ESP_OK(esp_etm_channel_enable(etm_handles.etm_pos_handle));
    TEST_ESP_OK(esp_etm_channel_enable(etm_handles.etm_neg_handle));

    return etm_handles;
}

static void test_ana_cmpr_deinit_etm_chain_v2(test_ana_cmpr_etm_task_handles_t handles)
{
    TEST_ESP_OK(esp_etm_channel_disable(handles.etm_reload_handle));
    TEST_ESP_OK(esp_etm_channel_disable(handles.etm_scan_handle));
    TEST_ESP_OK(esp_etm_channel_disable(handles.etm_pos_handle));
    TEST_ESP_OK(esp_etm_channel_disable(handles.etm_neg_handle));

    TEST_ESP_OK(gpio_etm_task_rm_gpio(handles.gpio_set_task, TEST_ETM_MONITOR_GPIO));
    TEST_ESP_OK(gpio_etm_task_rm_gpio(handles.gpio_clr_task, TEST_ETM_MONITOR_GPIO));

    TEST_ESP_OK(esp_etm_del_task(handles.gpio_set_task));
    TEST_ESP_OK(esp_etm_del_task(handles.gpio_clr_task));
    TEST_ESP_OK(esp_etm_del_task(handles.cmpr_start_task));
    TEST_ESP_OK(esp_etm_del_task(handles.gptimer_en_alarm_task));

    TEST_ESP_OK(esp_etm_del_event(handles.gptimer_alarm_evt));
    TEST_ESP_OK(esp_etm_del_event(handles.cmpr_pos_evt));
    TEST_ESP_OK(esp_etm_del_event(handles.cmpr_neg_evt));

    TEST_ESP_OK(esp_etm_del_channel(handles.etm_reload_handle));
    TEST_ESP_OK(esp_etm_del_channel(handles.etm_scan_handle));
    TEST_ESP_OK(esp_etm_del_channel(handles.etm_pos_handle));
    TEST_ESP_OK(esp_etm_del_channel(handles.etm_neg_handle));
}

TEST_CASE("ana_cmpr etm task periodic scan", "[ana_cmpr][etm]")
{
    gptimer_handle_t gptimer = test_ana_cmpr_gptimer_init();
    ana_cmpr_handle_t cmpr = test_ana_cmpr_init();
    gpio_num_t src_gpio = test_init_src_chan_gpio(cmpr, 0, 0);

    gpio_config_t monitor_gpio_cfg = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pin_bit_mask = 1ULL << TEST_ETM_MONITOR_GPIO,
    };
    TEST_ESP_OK(gpio_config(&monitor_gpio_cfg));
    TEST_ESP_OK(gpio_set_level(TEST_ETM_MONITOR_GPIO, 0));

    // Prime the comparator with a known initial low level before ETM-driven scans begin.
    TEST_ESP_OK(gpio_set_level(src_gpio, 0));
    vTaskDelay(pdMS_TO_TICKS(10));
    TEST_ESP_OK(ana_cmpr_trigger_scan(cmpr));
    vTaskDelay(pdMS_TO_TICKS(10));
    test_ana_cmpr_wait_output_level(cmpr, 0, false);

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = TEST_ETM_SCAN_PERIOD_US,
        .flags.auto_reload_on_alarm = true,
    };
    TEST_ESP_OK(gptimer_set_alarm_action(gptimer, &alarm_config));

    test_ana_cmpr_etm_task_handles_t handles = test_ana_cmpr_init_etm_chain_v2(cmpr, gptimer);

    // start the gptimer, which will trigger periodic scans of the comparator via ETM.
    // The test will monitor the output GPIO to verify that it changes level in response to the input signal,
    // demonstrating that the ETM-driven scans are working correctly.
    TEST_ESP_OK(gptimer_start(gptimer));

    TEST_ASSERT_EQUAL(0, gpio_get_level(TEST_ETM_MONITOR_GPIO));

    TEST_ESP_OK(gpio_set_level(src_gpio, 1));
    vTaskDelay(pdMS_TO_TICKS(1));
    test_ana_cmpr_wait_output_level(cmpr, 0, true);
    TEST_ASSERT_EQUAL(1, gpio_get_level(TEST_ETM_MONITOR_GPIO));

    TEST_ESP_OK(gpio_set_level(src_gpio, 0));
    vTaskDelay(pdMS_TO_TICKS(1));
    test_ana_cmpr_wait_output_level(cmpr, 0, false);
    TEST_ASSERT_EQUAL(0, gpio_get_level(TEST_ETM_MONITOR_GPIO));

    TEST_ESP_OK(gptimer_stop(gptimer));
    TEST_ESP_OK(gpio_reset_pin(TEST_ETM_MONITOR_GPIO));
    test_ana_cmpr_deinit_etm_chain_v2(handles);
    test_ana_cmpr_deinit(cmpr);
    test_ana_cmpr_gptimer_deinit(gptimer);
}
#endif
