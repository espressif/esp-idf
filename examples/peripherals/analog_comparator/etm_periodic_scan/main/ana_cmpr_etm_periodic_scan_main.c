/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_etm.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "driver/ana_cmpr.h"
#include "driver/gpio_etm.h"
#include "driver/gptimer_etm.h"
#include "driver/ana_cmpr_etm.h"

#define EXAMPLE_ANA_CMPR_UNIT          (0)
#define EXAMPLE_MONITOR_GPIO_NUM       CONFIG_EXAMPLE_MONITOR_GPIO_NUM
#define EXAMPLE_SCAN_PERIOD_US         CONFIG_EXAMPLE_SCAN_PERIOD_US
#define EXAMPLE_ANA_CMPR_SRC_GPIO_NUM  CONFIG_EXAMPLE_SRC_GPIO_NUM

#define TAG "example"

typedef struct {
    /* ETM events are produced by peripherals and can trigger ETM tasks without CPU intervention. */
    esp_etm_event_handle_t gptimer_alarm_evt;
    esp_etm_event_handle_t cmpr_pos_evt;
    esp_etm_event_handle_t cmpr_neg_evt;
    /* ETM tasks are the hardware actions we want to execute when an event happens. */
    esp_etm_task_handle_t gptimer_en_alarm_task;
    esp_etm_task_handle_t cmpr_start_task;
    esp_etm_task_handle_t gpio_set_task;
    esp_etm_task_handle_t gpio_clr_task;
    /* One ETM channel connects exactly one event source to one task target. */
    esp_etm_channel_handle_t etm_realarm_handle;
    esp_etm_channel_handle_t etm_scan_handle;
    esp_etm_channel_handle_t etm_pos_handle;
    esp_etm_channel_handle_t etm_neg_handle;
} example_etm_handles_t;

static void example_init_monitor_gpio(void)
{
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pin_bit_mask = 1ULL << EXAMPLE_MONITOR_GPIO_NUM,
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    ESP_ERROR_CHECK(gpio_set_level(EXAMPLE_MONITOR_GPIO_NUM, 0));
    ESP_LOGI(TAG, "Monitor GPIO %d", EXAMPLE_MONITOR_GPIO_NUM);
}

static ana_cmpr_handle_t example_init_ana_cmpr(void)
{
    gpio_num_t src_gpio = -1;
    ana_cmpr_handle_t cmpr = NULL;
    ana_cmpr_config_t config = {
        .unit = EXAMPLE_ANA_CMPR_UNIT,
        .clk_src = ANA_CMPR_CLK_SRC_DEFAULT,
        .ref_src = ANA_CMPR_REF_SRC_INTERNAL,
        .cross_type = ANA_CMPR_CROSS_ANY,
        .src_chan0_gpio = EXAMPLE_ANA_CMPR_SRC_GPIO_NUM,
        /* Require several consistent samples before the scan result is updated.
         * This makes the output more stable when the input is noisy. */
        .resample_limit = 3,
    };
    ESP_ERROR_CHECK(ana_cmpr_new_unit(&config, &cmpr));
    ESP_ERROR_CHECK(ana_cmpr_get_channel_gpio(cmpr, ANA_CMPR_SOURCE_CHAN, 0, &src_gpio));

    ana_cmpr_internal_ref_config_t ref_cfg = {
        .ref_volt = ANA_CMPR_REF_VOLT_50_PCT_VDD,
        .ref_hys_level = ANA_CMPR_REF_HYS_LEVEL0,
    };
    /* Compare the input signal against an internal reference set to 50% of VDD. */
    ESP_ERROR_CHECK(ana_cmpr_set_internal_reference(cmpr, &ref_cfg));

    ana_cmpr_scan_config_t scan_cfg = {
        .scan_mode = ANA_CMPR_SCAN_MODE_FULL,
        /* Stay on each source channel for a short time before moving to the next one. */
        .poll_period_us = 2,
    };
    /* Scan parameters control how the hardware walks through the enabled source channels. */
    ESP_ERROR_CHECK(ana_cmpr_set_scan_config(cmpr, &scan_cfg));

    ESP_LOGI(TAG, "Analog comparator source GPIO %d", src_gpio);
    ESP_LOGI(TAG, "Analog comparator internal reference 50%% VDD");

    return cmpr;
}

static gptimer_handle_t example_init_gptimer(void)
{
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1 * 1000 * 1000, // 1 MHz, which means the timer count value will increase by 1 every microsecond
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));
    ESP_ERROR_CHECK(gptimer_set_raw_count(gptimer, 0));
    ESP_ERROR_CHECK(gptimer_enable(gptimer));

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        /* The alarm period is the high-level scan period of this example.
         * Every alarm will trigger one comparator scan through ETM. */
        .alarm_count = EXAMPLE_SCAN_PERIOD_US,
        .flags.auto_reload_on_alarm = true,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
    ESP_LOGI(TAG, "GPTimer scan period %d us", EXAMPLE_SCAN_PERIOD_US);

    return gptimer;
}

static void example_init_etm(ana_cmpr_handle_t cmpr, gptimer_handle_t gptimer)
{
    example_etm_handles_t handles = {};

    /* Step 1: create the ETM event generated when the timer alarm fires. */
    gptimer_etm_event_config_t gptimer_evt_cfg = {
        .event_type = GPTIMER_ETM_EVENT_ALARM_MATCH,
    };
    ESP_ERROR_CHECK(gptimer_new_etm_event(gptimer, &gptimer_evt_cfg, &handles.gptimer_alarm_evt));

    /* Step 2: create the timer task that rearms the alarm after each trigger. */
    gptimer_etm_task_config_t gptimer_task_cfg = {
        .task_type = GPTIMER_ETM_TASK_EN_ALARM,
    };
    ESP_ERROR_CHECK(gptimer_new_etm_task(gptimer, &gptimer_task_cfg, &handles.gptimer_en_alarm_task));

    /* Step 3: create the comparator task that starts one scan sequence. */
    ana_cmpr_etm_task_config_t cmpr_task_cfg = {
        .task_type = ANA_CMPR_TASK_START,
    };
    ESP_ERROR_CHECK(ana_cmpr_new_etm_task(cmpr, &cmpr_task_cfg, &handles.cmpr_start_task));

    /* Step 4: create comparator events for positive and negative threshold crossings. */
    ana_cmpr_etm_event_config_t cmpr_evt_cfg = {
        .event_type = ANA_CMPR_EVENT_POS_CROSS,
    };
    ESP_ERROR_CHECK(ana_cmpr_new_etm_event(cmpr, &cmpr_evt_cfg, &handles.cmpr_pos_evt));
    cmpr_evt_cfg.event_type = ANA_CMPR_EVENT_NEG_CROSS;
    ESP_ERROR_CHECK(ana_cmpr_new_etm_event(cmpr, &cmpr_evt_cfg, &handles.cmpr_neg_evt));

    /* Step 5: create GPIO ETM tasks so the comparator result is visible on a normal output pin.
     * Positive crossing drives the monitor GPIO high, negative crossing drives it low. */
    gpio_etm_task_config_t gpio_task_cfg = {};
    gpio_task_cfg.actions[0] = GPIO_ETM_TASK_ACTION_SET;
    gpio_task_cfg.actions[1] = GPIO_ETM_TASK_ACTION_CLR;
    ESP_ERROR_CHECK(gpio_new_etm_task(&gpio_task_cfg, &handles.gpio_set_task, &handles.gpio_clr_task));
    ESP_ERROR_CHECK(gpio_etm_task_add_gpio(handles.gpio_set_task, EXAMPLE_MONITOR_GPIO_NUM));
    ESP_ERROR_CHECK(gpio_etm_task_add_gpio(handles.gpio_clr_task, EXAMPLE_MONITOR_GPIO_NUM));

    /* Step 6: allocate ETM channels. Each channel is an event-to-task connection. */
    esp_etm_channel_config_t etm_cfg = {};
    ESP_ERROR_CHECK(esp_etm_new_channel(&etm_cfg, &handles.etm_realarm_handle));
    ESP_ERROR_CHECK(esp_etm_new_channel(&etm_cfg, &handles.etm_scan_handle));
    ESP_ERROR_CHECK(esp_etm_new_channel(&etm_cfg, &handles.etm_pos_handle));
    ESP_ERROR_CHECK(esp_etm_new_channel(&etm_cfg, &handles.etm_neg_handle));

    /* Step 7: wire the ETM graph:
     * - timer alarm event -> timer rearm task
     * - timer alarm event -> comparator start task
     * - comparator positive event -> GPIO set task
     * - comparator negative event -> GPIO clear task */
    ESP_ERROR_CHECK(esp_etm_channel_connect(handles.etm_realarm_handle, handles.gptimer_alarm_evt, handles.gptimer_en_alarm_task));
    ESP_ERROR_CHECK(esp_etm_channel_connect(handles.etm_scan_handle, handles.gptimer_alarm_evt, handles.cmpr_start_task));
    ESP_ERROR_CHECK(esp_etm_channel_connect(handles.etm_pos_handle, handles.cmpr_pos_evt, handles.gpio_set_task));
    ESP_ERROR_CHECK(esp_etm_channel_connect(handles.etm_neg_handle, handles.cmpr_neg_evt, handles.gpio_clr_task));

    /* Step 8: enable the channels so the hardware pipeline becomes active. */
    ESP_ERROR_CHECK(esp_etm_channel_enable(handles.etm_realarm_handle));
    ESP_ERROR_CHECK(esp_etm_channel_enable(handles.etm_scan_handle));
    ESP_ERROR_CHECK(esp_etm_channel_enable(handles.etm_pos_handle));
    ESP_ERROR_CHECK(esp_etm_channel_enable(handles.etm_neg_handle));
}

void app_main(void)
{
    example_init_monitor_gpio();
    ana_cmpr_handle_t cmpr = example_init_ana_cmpr();
    gptimer_handle_t gptimer = example_init_gptimer();
    example_init_etm(cmpr, gptimer);

    ESP_ERROR_CHECK(ana_cmpr_enable(cmpr));
    /* After the timer starts, future scans are launched by ETM rather than by CPU code. */
    ESP_ERROR_CHECK(gptimer_start(gptimer));
    ESP_LOGI(TAG, "Periodic ETM-driven comparator scan started");
}
