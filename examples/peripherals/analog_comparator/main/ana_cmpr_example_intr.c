/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ana_cmpr.h"
#include "esp_log.h"
#include "ana_cmpr_example_main.h"

static const char *TAG = "ana_cmpr_example";

static bool example_ana_cmpr_on_cross_callback(ana_cmpr_handle_t cmpr,
                                               const ana_cmpr_cross_event_data_t *edata,
                                               void *user_ctx)
{
#if CONFIG_EXAMPLE_HYSTERESIS_COMPARATOR
    static ana_cmpr_internal_ref_config_t ref_cfg = {
        .ref_volt = ANA_CMPR_REF_VOLT_70_PCT_VDD,
    };
    bool is_70p = ref_cfg.ref_volt == ANA_CMPR_REF_VOLT_70_PCT_VDD;
    /* Toggle the GPIO, monitor the gpio on an oscilloscope. */
    gpio_set_level(EXAMPLE_MONITOR_GPIO_NUM, is_70p);
    /* Set the internal reference voltage to 30% VDD and 70 %VDD alternately */
    ana_cmpr_set_internal_reference(cmpr, &ref_cfg);
    ref_cfg.ref_volt = is_70p ? ANA_CMPR_REF_VOLT_30_PCT_VDD : ANA_CMPR_REF_VOLT_70_PCT_VDD;
#else
    static int lvl = 0;
    /* Toggle the GPIO, monitor the gpio on a oscilloscope. */
    gpio_set_level(EXAMPLE_MONITOR_GPIO_NUM, lvl);
    lvl = !lvl;
#endif
    return false;
}

void example_init_analog_comparator_intr(void)
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
#if CONFIG_EXAMPLE_HYSTERESIS_COMPARATOR
        /* Set the initial internal reference voltage to 70% VDD, it will be updated in the callback every time the interrupt triggered */
        .ref_volt = ANA_CMPR_REF_VOLT_70_PCT_VDD
#else
        .ref_volt = ANA_CMPR_REF_VOLT_50_PCT_VDD,
#endif
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
#endif

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

    /* Step 3: Register the event callbacks */
    ana_cmpr_event_callbacks_t cbs = {
        .on_cross = example_ana_cmpr_on_cross_callback,
    };
    ESP_ERROR_CHECK(ana_cmpr_register_event_callbacks(cmpr, &cbs, NULL));

    /* Step 4: Enable the analog comparator unit */
    ESP_ERROR_CHECK(ana_cmpr_enable(cmpr));

#if CONFIG_EXAMPLE_INTERNAL_REF
    ESP_LOGI(TAG, "Analog comparator enabled, reference voltage: %d%% * VDD", (int)ref_cfg.ref_volt * 10);
#else
    ESP_LOGI(TAG, "Analog comparator enabled, external reference selected");
#endif
}

void example_analog_comparator_intr_app(void)
{
    /* Initialize GPIO to monitor the comparator interrupt */
    example_init_monitor_gpio();
    /* Initialize Analog Comparator */
    example_init_analog_comparator_intr();
}
