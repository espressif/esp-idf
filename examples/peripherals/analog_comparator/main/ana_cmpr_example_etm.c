/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ana_cmpr.h"
#include "driver/ana_cmpr_etm.h"
#include "driver/gpio_etm.h"
#include "esp_etm.h"
#include "esp_log.h"
#include "ana_cmpr_example_main.h"

static const char *TAG = "ana_cmpr_example";

static void example_etm_bind_ana_cmpr_event_with_gpio_task(ana_cmpr_handle_t cmpr)
{
    /* Allocate the analog comparator positive & negative cross events */
    esp_etm_event_handle_t cmpr_pos_evt = NULL;
    esp_etm_event_handle_t cmpr_neg_evt = NULL;
    ana_cmpr_etm_event_config_t evt_cfg = {
        .event_type = ANA_CMPR_EVENT_POS_CROSS,
    };
    ESP_ERROR_CHECK(ana_cmpr_new_etm_event(cmpr, &evt_cfg, &cmpr_pos_evt));
    evt_cfg.event_type = ANA_CMPR_EVENT_NEG_CROSS;
    ESP_ERROR_CHECK(ana_cmpr_new_etm_event(cmpr, &evt_cfg, &cmpr_neg_evt));

    /* Allocate the GPIO set & clear tasks */
    esp_etm_task_handle_t gpio_set_task = NULL;
    esp_etm_task_handle_t gpio_clr_task = NULL;
    gpio_etm_task_config_t task_cfg = {};
    task_cfg.actions[0] = GPIO_ETM_TASK_ACTION_SET;
    task_cfg.actions[1] = GPIO_ETM_TASK_ACTION_CLR;
    ESP_ERROR_CHECK(gpio_new_etm_task(&task_cfg, &gpio_set_task, &gpio_clr_task));
    /* Add task to the monitor GPIO */
    ESP_ERROR_CHECK(gpio_etm_task_add_gpio(gpio_set_task, EXAMPLE_MONITOR_GPIO_NUM));
    ESP_ERROR_CHECK(gpio_etm_task_add_gpio(gpio_clr_task, EXAMPLE_MONITOR_GPIO_NUM));

    /* Allocate the Event Task Matrix channels */
    esp_etm_channel_handle_t etm_pos_handle;
    esp_etm_channel_handle_t etm_neg_handle;
    esp_etm_channel_config_t etm_cfg = {};
    ESP_ERROR_CHECK(esp_etm_new_channel(&etm_cfg, &etm_pos_handle));
    ESP_ERROR_CHECK(esp_etm_new_channel(&etm_cfg, &etm_neg_handle));
    /* Bind the events and tasks */
    ESP_ERROR_CHECK(esp_etm_channel_connect(etm_pos_handle, cmpr_pos_evt, gpio_set_task));
    ESP_ERROR_CHECK(esp_etm_channel_connect(etm_neg_handle, cmpr_neg_evt, gpio_clr_task));
    /* Enable the ETM channels */
    ESP_ERROR_CHECK(esp_etm_channel_enable(etm_pos_handle));
    ESP_ERROR_CHECK(esp_etm_channel_enable(etm_neg_handle));
}

ana_cmpr_handle_t example_init_analog_comparator_etm(void)
{
    /* Step 0: Show the source channel and reference channel GPIO */
    int src_gpio =  -1;
    int ext_ref_gpio = -1;
    ESP_ERROR_CHECK(ana_cmpr_get_gpio(EXAMPLE_ANA_CMPR_UNIT, ANA_CMPR_SOURCE_CHAN, &src_gpio));
    ESP_ERROR_CHECK(ana_cmpr_get_gpio(EXAMPLE_ANA_CMPR_UNIT, ANA_CMPR_EXT_REF_CHAN, &ext_ref_gpio));
    ESP_LOGI(TAG, "Analog Comparator source gpio %d, external reference gpio %d", src_gpio, ext_ref_gpio);

    ana_cmpr_handle_t cmpr = NULL;

#if CONFIG_EXAMPLE_INTERNAL_REF
    /* Step 1: Allocate the new analog comparator unit */
    ana_cmpr_config_t config = {
        .unit = EXAMPLE_ANA_CMPR_UNIT,
        .clk_src = ANA_CMPR_CLK_SRC_DEFAULT,
        .ref_src = ANA_CMPR_REF_SRC_INTERNAL,
        .cross_type = ANA_CMPR_CROSS_ANY,
    };
    ESP_ERROR_CHECK(ana_cmpr_new_unit(&config, &cmpr));
    ESP_LOGI(TAG, "Allocate Analog Comparator with internal reference");

    /* Step 1.1: As we are using the internal reference source, we need to configure the internal reference */
    ana_cmpr_internal_ref_config_t ref_cfg = {
        .ref_volt = ANA_CMPR_REF_VOLT_50_PCT_VDD,
    };
    ESP_ERROR_CHECK(ana_cmpr_set_internal_reference(cmpr, &ref_cfg));
#else
    /* Step 1: Allocate the new analog comparator unit */
    ana_cmpr_config_t config = {
        .unit = EXAMPLE_ANA_CMPR_UNIT,
        .clk_src = ANA_CMPR_CLK_SRC_DEFAULT,
        .ref_src = ANA_CMPR_REF_SRC_EXTERNAL,
        .cross_type = ANA_CMPR_CROSS_ANY,
    };
    ESP_ERROR_CHECK(ana_cmpr_new_unit(&config, &cmpr));
    ESP_LOGI(TAG, "Allocate Analog Comparator with external reference");
#endif  // CONFIG_EXAMPLE_INTERNAL_REF

    /* Step 2: (Optional) Set the debounce configuration
     * It's an optional configuration, if the wait time is set in debounce configuration,
     * the cross interrupt will be disabled temporary after it is triggered, and it will be enabled
     * automatically enabled after `wait_us`, so that the duplicate interrupts
     * can be suppressed while the source signal crossing the reference signal. */
    ana_cmpr_debounce_config_t dbc_cfg = {
        /* Normally the `wait_us` is related to the relative frequency between the source and reference signal
         * comparing to another one. This example adopts an approximate frequency as the relative signal
         * frequency, and set the default wait time to EXAMPLE_WAIT_TIME_PROP of the relative signal period.
         * We need to estimate an appropriate `freq_approx` and EXAMPLE_WAIT_TIME_PROP
         * to make sure the interrupt neither triggers duplicate interrupts, nor misses the next crossing interrupt.
         * Here we take 1 KHz for example */
        .wait_us = EXAMPLE_WAITE_TIME_US(1000),
    };
    ESP_ERROR_CHECK(ana_cmpr_set_debounce(cmpr, &dbc_cfg));

    /* Step 3: Enable the analog comparator unit */
    ESP_ERROR_CHECK(ana_cmpr_enable(cmpr));

#if CONFIG_EXAMPLE_INTERNAL_REF
    ESP_LOGI(TAG, "Analog comparator enabled, reference voltage: %d%% * VDD", (int)ref_cfg.ref_volt * 10);
#else
    ESP_LOGI(TAG, "Analog comparator enabled, external reference selected");
#endif
    return cmpr;
}

void example_analog_comparator_etm_app(void)
{
    /* Initialize GPIO to monitor the comparator interrupt */
    example_init_monitor_gpio();
    /* Initialize Analog Comparator */
    ana_cmpr_handle_t cmpr = example_init_analog_comparator_etm();
    /* Connect the analog comparator events and gpio tasks via ETM channels */
    example_etm_bind_ana_cmpr_event_with_gpio_task(cmpr);
}
