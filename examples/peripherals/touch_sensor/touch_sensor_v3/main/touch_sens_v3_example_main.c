/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/touch_sens.h"
#include "esp_check.h"

// Touch version 3 supports multiple sample configurations
#define EXAMPLE_TOUCH_SAMPLE_CFG_NUM        1  // Up to 'TOUCH_SAMPLE_CFG_NUM'
#define EXAMPLE_TOUCH_CHANNEL_NUM           4
#define EXAMPLE_TOUCH_CHAN_INIT_SCAN_TIMES  3

static touch_sensor_handle_t  s_sens_handle = NULL;
static touch_channel_handle_t s_chan_handle[EXAMPLE_TOUCH_CHANNEL_NUM] = {};
// Active threshold to benchmark ratio. (i.e., touch will be activated when data >= benchmark * (1 + ratio))
static float s_thresh2bm_ratio[EXAMPLE_TOUCH_CHANNEL_NUM] = {
    [0 ... EXAMPLE_TOUCH_CHANNEL_NUM - 1] = 0.015f,  // 1.5%
};

bool example_touch_on_active_callback(touch_sensor_handle_t sens_handle, const touch_active_event_data_t *event, void *user_ctx)
{
    ESP_EARLY_LOGI("touch_callback", "[CH %d] active", (int)event->chan_id);
    return false;
}

bool example_touch_on_inactive_callback(touch_sensor_handle_t sens_handle, const touch_inactive_event_data_t *event, void *user_ctx)
{
    ESP_EARLY_LOGI("touch_callback", "[CH %d] inactive", (int)event->chan_id);
    return false;
}

static void example_touch_do_initial_scanning(void)
{
    /* Enable the touch sensor to do the initial scanning, so that to initialize the channel data */
    ESP_ERROR_CHECK(touch_sensor_enable(s_sens_handle));

    /* Scan the enabled touch channels for several times, to make sure the initial channel data is stable */
    for (int i = 0; i < EXAMPLE_TOUCH_CHAN_INIT_SCAN_TIMES; i++) {
        ESP_ERROR_CHECK(touch_sensor_trigger_oneshot_scanning(s_sens_handle, 2000));
    }

    /* Disable the touch channel to rollback the state */
    ESP_ERROR_CHECK(touch_sensor_disable(s_sens_handle));

    /* (Optional) Read the initial channel benchmark and reconfig the channel active threshold accordingly */
    printf("Initial benchmark and new threshold are:\n");
    for (int i = 0; i < EXAMPLE_TOUCH_CHANNEL_NUM; i++) {
        /* Read the initial benchmark of the touch channel */
        uint32_t benchmark[EXAMPLE_TOUCH_SAMPLE_CFG_NUM] = {};
        ESP_ERROR_CHECK(touch_channel_read_data(s_chan_handle[i], TOUCH_CHAN_DATA_TYPE_BENCHMARK, benchmark));
        /* Calculate the proper active thresholds regarding the initial benchmark */
        printf("[CH %d]", i);
        touch_channel_config_t chan_cfg = {};
        for (int j = 0; j < EXAMPLE_TOUCH_SAMPLE_CFG_NUM; j++) {
            chan_cfg.active_thresh[j] = (uint32_t)(benchmark[j] * s_thresh2bm_ratio[i]);
            printf(" %d: %"PRIu32", %"PRIu32"\t", j, benchmark[j], chan_cfg.active_thresh[j]);
        }
        printf("\n");
        /* Update the channel configuration */
        ESP_ERROR_CHECK(touch_sensor_reconfig_channel(s_chan_handle[i], &chan_cfg));
    }
}

void app_main(void)
{
    /* Use the default sample configurations */
    touch_sensor_sample_config_t sample_cfg[EXAMPLE_TOUCH_SAMPLE_CFG_NUM] = {
        TOUCH_SENSOR_V3_DEFAULT_SAMPLE_CONFIG(1, 1, 1),
#if EXAMPLE_TOUCH_SAMPLE_CFG_NUM > 1
        TOUCH_SENSOR_V3_DEFAULT_SAMPLE_CONFIG(2, 1, 1),
#endif
#if EXAMPLE_TOUCH_SAMPLE_CFG_NUM > 2
        TOUCH_SENSOR_V3_DEFAULT_SAMPLE_CONFIG(4, 1, 1),
#endif
    };
    /* Allocate new touch controller handle */
    touch_sensor_config_t sens_cfg = TOUCH_SENSOR_DEFAULT_BASIC_CONFIG(EXAMPLE_TOUCH_SAMPLE_CFG_NUM, sample_cfg);
    ESP_ERROR_CHECK(touch_sensor_new_controller(&sens_cfg, &s_sens_handle));

    /* Configure the touch sensor filter */
    touch_sensor_filter_config_t filter_cfg = TOUCH_SENSOR_DEFAULT_FILTER_CONFIG();
    ESP_ERROR_CHECK(touch_sensor_config_filter(s_sens_handle, &filter_cfg));

    /* Allocate new touch channel on the touch controller */
    touch_channel_config_t chan_cfg = {
        /** Set the touch channel active threshold of each sample configuration.
         *
         *  @How to Determine:
         *  As the actual threshold is affected by various factors in real application,
         *  we need to run the touch app first to get the `benchmark` and the `smooth_data` that being touched.
         *
         *  @Formula:
         *  threshold = benchmark * coeff, (coeff for example, 0.1%~20%)
         *  Please adjust the coeff to guarantee the threshold < smooth_data - benchmark
         *
         *  @Typical Practice:
         *  Normally, we can't determine a fixed threshold at the beginning,
         *  but we can give them estimated values first and update them after an initial scanning (like this example),
         *  Step1: set an estimated value for each sample configuration first. (i.e., here)
         *  Step2: then reconfig the threshold after the initial scanning.(see `example_touch_do_initial_scanning`)
         *  Step3: adjust the `s_thresh2bm_ratio` to a proper value to trigger the active callback
         */
        .active_thresh = {
            1000,       // estimated active threshold of sample configuration 0
#if EXAMPLE_TOUCH_SAMPLE_CFG_NUM > 1
            2500,       // estimated active threshold of sample configuration 1
#endif
#if EXAMPLE_TOUCH_SAMPLE_CFG_NUM > 2
            5000,       // estimated active threshold of sample configuration 2
#endif
        },
    };
    for (int i = 0; i < EXAMPLE_TOUCH_CHANNEL_NUM; i++) {
        ESP_ERROR_CHECK(touch_sensor_new_channel(s_sens_handle, i, &chan_cfg, &s_chan_handle[i]));
    }

    /* Do the initial scanning to initialize the touch channel data
     * Without this step, the channel data in the first read will be invalid
     */
    example_touch_do_initial_scanning();

    /* Register the touch sensor callbacks, here only take `active` and `deactivate` event for example */
    touch_event_callbacks_t callbacks = {
        .on_active = example_touch_on_active_callback,
        .on_inactive = example_touch_on_inactive_callback,
        .on_measure_done = NULL,
        .on_scan_done = NULL,
        .on_timeout = NULL,
        .on_proximity_meas_done = NULL,
    };
    ESP_ERROR_CHECK(touch_sensor_register_callbacks(s_sens_handle, &callbacks, NULL));

    /* Enable the touch sensor */
    ESP_ERROR_CHECK(touch_sensor_enable(s_sens_handle));

    /* Start continuous scanning, you can also trigger oneshot scanning manually */
    ESP_ERROR_CHECK(touch_sensor_start_continuous_scanning(s_sens_handle));

    uint32_t benchmark[EXAMPLE_TOUCH_SAMPLE_CFG_NUM] = {};
    uint32_t chan_data[EXAMPLE_TOUCH_SAMPLE_CFG_NUM] = {};
    while (1) {
        printf("=================================\n");
        for (int i = 0; i < EXAMPLE_TOUCH_CHANNEL_NUM; i++) {
            /* Read and print the benchmark of each sample configuration */
            ESP_ERROR_CHECK(touch_channel_read_data(s_chan_handle[i], TOUCH_CHAN_DATA_TYPE_BENCHMARK, benchmark));
            printf("benchmark [CH %d]:", i);
            for (int j = 0; j < EXAMPLE_TOUCH_SAMPLE_CFG_NUM; j++) {
                printf(" %"PRIu32, benchmark[j]);
            }
            printf("\n");
            /* Read and print the channel data of each sample configuration */
            ESP_ERROR_CHECK(touch_channel_read_data(s_chan_handle[i], TOUCH_CHAN_DATA_TYPE_SMOOTH, chan_data));
            printf("chan_data [CH %d]:", i);
            for (int j = 0; j < EXAMPLE_TOUCH_SAMPLE_CFG_NUM; j++) {
                printf(" %"PRIu32, chan_data[j]);
            }
            printf("\n\n");
        }
        /* Read and display the data every 300 ms */
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}
