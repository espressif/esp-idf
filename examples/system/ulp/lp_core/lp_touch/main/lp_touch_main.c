/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/* ULP RISC-V touch sensor example */

#include <stdio.h>
#include "esp_sleep.h"
#include "ulp_lp_core.h"
#include "lp_core_main.h"
#include "driver/touch_sens.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "touch_sens_example_config.h"
#include "esp_sleep.h"

extern const uint8_t lp_core_main_bin_start[] asm("_binary_lp_core_main_bin_start");
extern const uint8_t lp_core_main_bin_end[]   asm("_binary_lp_core_main_bin_end");


// Touch version 3 supports multiple sample configurations (i.e. supports frequency hopping),
// others only have one set of sample configurations.
#define EXAMPLE_TOUCH_SAMPLE_CFG_NUM    TOUCH_SAMPLE_CFG_NUM  // Up to 'TOUCH_SAMPLE_CFG_NUM'
#define EXAMPLE_TOUCH_CHANNEL_NUM           4
#define EXAMPLE_TOUCH_CHAN_INIT_SCAN_TIMES  3

ESP_STATIC_ASSERT(EXAMPLE_TOUCH_SAMPLE_CFG_NUM <= TOUCH_SAMPLE_CFG_NUM, "sample configuration number exceed the supported number");
ESP_STATIC_ASSERT(EXAMPLE_TOUCH_CHANNEL_NUM <= (TOUCH_MAX_CHAN_ID - TOUCH_MIN_CHAN_ID + 1), "touch channel number exceed the max supported number ");

// Active threshold to benchmark ratio. (i.e., touch will be activated when data >= benchmark * (1 + ratio))
static float s_thresh2bm_ratio[EXAMPLE_TOUCH_CHANNEL_NUM] = {
    [0 ... EXAMPLE_TOUCH_CHANNEL_NUM - 1] = 0.015f,  // 1.5%
};
// The touch channel IDs that used in this example
// For the corresponding GPIOs of these channel, please refer to 'touch_sensor_channel.h'
static int s_channel_id[EXAMPLE_TOUCH_CHANNEL_NUM] = {
    TOUCH_MIN_CHAN_ID,
    TOUCH_MIN_CHAN_ID + 1,
    TOUCH_MIN_CHAN_ID + 2,
    TOUCH_MIN_CHAN_ID + 3,
};

static void example_touch_do_initial_scanning(touch_sensor_handle_t sens_handle, touch_channel_handle_t chan_handle[])
{
    /* Enable the touch sensor to do the initial scanning, so that to initialize the channel data */
    ESP_ERROR_CHECK(touch_sensor_enable(sens_handle));

    /* Scan the enabled touch channels for several times, to make sure the initial channel data is stable */
    for (int i = 0; i < EXAMPLE_TOUCH_CHAN_INIT_SCAN_TIMES; i++) {
        ESP_ERROR_CHECK(touch_sensor_trigger_oneshot_scanning(sens_handle, 2000));
    }

    /* Disable the touch channel to rollback the state */
    ESP_ERROR_CHECK(touch_sensor_disable(sens_handle));

    /* (Optional) Read the initial channel benchmark and reconfig the channel active threshold accordingly */
    printf("Initial benchmark and new threshold are:\n");
    for (int i = 0; i < EXAMPLE_TOUCH_CHANNEL_NUM; i++) {
        /* Read the initial benchmark of the touch channel */
        uint32_t benchmark[EXAMPLE_TOUCH_SAMPLE_CFG_NUM] = {};
        ESP_ERROR_CHECK(touch_channel_read_data(chan_handle[i], TOUCH_CHAN_DATA_TYPE_BENCHMARK, benchmark));

        /* Calculate the proper active thresholds regarding the initial benchmark */
        printf("Touch [CH %d]", s_channel_id[i]);
        /* Generate the default channel configuration and then update the active threshold based on the real benchmark */
        touch_channel_config_t chan_cfg = EXAMPLE_TOUCH_CHAN_CFG_DEFAULT();
        for (int j = 0; j < EXAMPLE_TOUCH_SAMPLE_CFG_NUM; j++) {
            chan_cfg.active_thresh[j] = (uint32_t)(benchmark[j] * s_thresh2bm_ratio[i]);
            printf(" %d: %"PRIu32", %"PRIu32"\t", j, benchmark[j], chan_cfg.active_thresh[j]);
        }
        printf("\n");
        /* Update the channel configuration */
        ESP_ERROR_CHECK(touch_sensor_reconfig_channel(chan_handle[i], &chan_cfg));
    }
}

static void init_touch_pad(void)
{
    touch_sensor_handle_t sens_handle = NULL;
    touch_channel_handle_t chan_handle[EXAMPLE_TOUCH_CHANNEL_NUM];

    /* Step 1: Create a new touch sensor controller handle with default sample configuration */
    touch_sensor_sample_config_t sample_cfg[TOUCH_SAMPLE_CFG_NUM] = {
        TOUCH_SENSOR_V3_DEFAULT_SAMPLE_CONFIG2(3, 29, 8, 3),
        TOUCH_SENSOR_V3_DEFAULT_SAMPLE_CONFIG2(2, 88, 31, 7),
        TOUCH_SENSOR_V3_DEFAULT_SAMPLE_CONFIG2(3, 10, 31, 7)
    };
    touch_sensor_config_t sens_cfg = TOUCH_SENSOR_DEFAULT_BASIC_CONFIG(EXAMPLE_TOUCH_SAMPLE_CFG_NUM, sample_cfg);
    ESP_ERROR_CHECK(touch_sensor_new_controller(&sens_cfg, &sens_handle));
    /* Step 2: Create and enable the new touch channel handles with default configurations */
    touch_channel_config_t chan_cfg = EXAMPLE_TOUCH_CHAN_CFG_DEFAULT();
    /* Allocate new touch channel on the touch controller */
    for (int i = 0; i < EXAMPLE_TOUCH_CHANNEL_NUM; i++) {
        ESP_ERROR_CHECK(touch_sensor_new_channel(sens_handle, s_channel_id[i], &chan_cfg, &chan_handle[i]));
        /* Display the touch channel corresponding GPIO number, you can also know from `touch_sensor_channel.h` */
        touch_chan_info_t chan_info = {};
        ESP_ERROR_CHECK(touch_sensor_get_channel_info(chan_handle[i], &chan_info));
        printf("Touch [CH %d] enabled on GPIO%d\n", s_channel_id[i], chan_info.chan_gpio);
    }
    printf("=================================\n");
    /* Step 3: Confiture the default filter for the touch sensor (Note: Touch V1 uses software filter) */
    touch_sensor_filter_config_t filter_cfg = TOUCH_SENSOR_DEFAULT_FILTER_CONFIG();
    ESP_ERROR_CHECK(touch_sensor_config_filter(sens_handle, &filter_cfg));

    /* Step 4: Keep the RTC_PERIPH domain power on to ensure the touch sensor works during the deep sleep */
    /* Note: Here we don't need to enable the Touch Sensor wake-up by calling `touch_sensor_config_sleep_wakeup`
     *       because we want to decide whether to wake up in the LP program, instead of by the touch sensor hardware directly */
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);

    /* Step 5: Do the initial scanning to initialize the touch channel data
     * Without this step, the channel data in the first read will be invalid
     */
    example_touch_do_initial_scanning(sens_handle, chan_handle);

    /* Step 6: Enable the touch sensor */
    ESP_ERROR_CHECK(touch_sensor_enable(sens_handle));

    /* Step 7: Start continuous scanning, you can also trigger oneshot scanning manually */
    ESP_ERROR_CHECK(touch_sensor_start_continuous_scanning(sens_handle));
}

static void lp_core_init(void)
{
    esp_err_t ret = ESP_OK;

    /* Set the time to wake up the LP core periodically */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER,
        .lp_timer_sleep_duration_us = 300*1000,  // 300 ms
    };

    ret = ulp_lp_core_load_binary(lp_core_main_bin_start, (lp_core_main_bin_end - lp_core_main_bin_start));
    if (ret != ESP_OK) {
        printf("LP Core load failed\n");
        abort();
    }

    ret = ulp_lp_core_run(&cfg);
    if (ret != ESP_OK) {
        printf("LP Core run failed\n");
        abort();
    }
    printf("LP core loaded with firmware successfully\n");
}

void app_main(void)
{
    /* If user is using USB-serial-jtag then idf monitor needs some time to
    *  re-connect to the USB port. We wait 1 sec here to allow for it to make the reconnection
    *  before we print anything. Otherwise the chip will go back to sleep again before the user
    *  has time to monitor any output.
    */
    vTaskDelay(pdMS_TO_TICKS(1000));

    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    /* Woke up by LP core, i.e., wake up the main CPU in the LP program */
    if (cause == ESP_SLEEP_WAKEUP_ULP) {
        printf("LP core woke up the main CPU\n");
        // We can access the LP core global variable by adding `ulp_` prefix
        uint32_t touch_data = ulp_touch_data;
        for (int i = 0; i < EXAMPLE_TOUCH_CHANNEL_NUM; i++) {
            if ((touch_data >> s_channel_id[i]) & 0x1) {
                printf("T%d touched\n", s_channel_id[i]);
            }
        }
        printf("\n");
    }
    /* Woke up by other source, like power_on */
    else {
        printf("Not an LP core wakeup. Cause = %d\n", cause);
        printf("Initializing...\n");

        /* Initialize Touch sensor from the main processor */
        init_touch_pad();
        /* Load LP Core binary and start the coprocessor */
        lp_core_init();
    }

    /* Go to sleep, only the ULP RISC-V will run */
    printf("Entering in deep sleep\n\n");

    /* Small delay to ensure the messages are printed */
    vTaskDelay(100);

    ESP_ERROR_CHECK(esp_sleep_enable_ulp_wakeup());
    esp_deep_sleep_start();
}
