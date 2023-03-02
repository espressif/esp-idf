/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "esp_sleep.h"
#include "sdkconfig.h"
#include "driver/rtc_io.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef CONFIG_EXAMPLE_TOUCH_WAKEUP
#include "driver/touch_pad.h"

#if CONFIG_IDF_TARGET_ESP32
#define TOUCH_THRESH_NO_USE 0

static void calibrate_touch_pad(touch_pad_t pad)
{
    int avg = 0;
    const size_t calibration_count = 128;
    for (int i = 0; i < calibration_count; ++i) {
        uint16_t val;
        touch_pad_read(pad, &val);
        avg += val;
    }
    avg /= calibration_count;
    const int min_reading = 300;
    if (avg < min_reading) {
        printf("Touch pad #%d average reading is too low: %d (expecting at least %d). "
               "Not using for deep sleep wakeup.\n", pad, avg, min_reading);
        touch_pad_config(pad, 0);
    } else {
        int threshold = avg - 100;
        printf("Touch pad #%d average: %d, wakeup threshold set to %d.\n", pad, avg, threshold);
        touch_pad_config(pad, threshold);
    }
}
#endif

void example_deep_sleep_register_touch_wakeup(void)
{
#if CONFIG_IDF_TARGET_ESP32
    // Initialize touch pad peripheral.
    // The default fsm mode is software trigger mode.
    ESP_ERROR_CHECK(touch_pad_init());
    // If use touch pad wake up, should set touch sensor FSM mode at 'TOUCH_FSM_MODE_TIMER'.
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    // Set reference voltage for charging/discharging
    // In this case, the high reference valtage will be 2.4V - 1V = 1.4V
    // The low reference voltage will be 0.5
    // The larger the range, the larger the pulse count value.
    touch_pad_set_voltage(TOUCH_HVOLT_2V4, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
    //init RTC IO and mode for touch pad.
    touch_pad_config(TOUCH_PAD_NUM8, TOUCH_THRESH_NO_USE);
    touch_pad_config(TOUCH_PAD_NUM9, TOUCH_THRESH_NO_USE);
    calibrate_touch_pad(TOUCH_PAD_NUM8);
    calibrate_touch_pad(TOUCH_PAD_NUM9);
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
    /* Initialize touch pad peripheral. */
    touch_pad_init();
    /* Only support one touch channel in sleep mode. */
    touch_pad_config(TOUCH_PAD_NUM9);
    /* Denoise setting at TouchSensor 0. */
    touch_pad_denoise_t denoise = {
        /* The bits to be cancelled are determined according to the noise level. */
        .grade = TOUCH_PAD_DENOISE_BIT4,
        .cap_level = TOUCH_PAD_DENOISE_CAP_L4,
    };
    touch_pad_denoise_set_config(&denoise);
    touch_pad_denoise_enable();
    printf("Denoise function init\n");
    /* Filter setting */
    touch_filter_config_t filter_info = {
        .mode = TOUCH_PAD_FILTER_IIR_16,
        .debounce_cnt = 1,      // 1 time count.
        .noise_thr = 0,         // 50%
        .jitter_step = 4,       // use for jitter mode.
        .smh_lvl = TOUCH_PAD_SMOOTH_IIR_2,
    };
    touch_pad_filter_set_config(&filter_info);
    touch_pad_filter_enable();
    printf("touch pad filter init %d\n", TOUCH_PAD_FILTER_IIR_8);
    /* Set sleep touch pad. */
    touch_pad_sleep_channel_enable(TOUCH_PAD_NUM9, true);
    touch_pad_sleep_channel_enable_proximity(TOUCH_PAD_NUM9, false);
    /* Reducing the operating frequency can effectively reduce power consumption. */
    touch_pad_sleep_channel_set_work_time(1000, TOUCH_PAD_MEASURE_CYCLE_DEFAULT);
    /* Enable touch sensor clock. Work mode is "timer trigger". */
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    touch_pad_fsm_start();
    vTaskDelay(100 / portTICK_PERIOD_MS);

    /* set touchpad wakeup threshold */
    uint32_t touch_value, wake_threshold;
    touch_pad_sleep_channel_read_smooth(TOUCH_PAD_NUM9, &touch_value);
    wake_threshold = touch_value * 0.1; // wakeup when touch sensor crosses 10% of background level
    touch_pad_sleep_set_threshold(TOUCH_PAD_NUM9, wake_threshold);
    printf("Touch pad #%d average: %"PRIu32", wakeup threshold set to %"PRIu32"\n",
        TOUCH_PAD_NUM9, touch_value, (uint32_t)(touch_value * 0.1));
#endif
    printf("Enabling touch pad wakeup\n");
    ESP_ERROR_CHECK(esp_sleep_enable_touchpad_wakeup());
    ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON));
}
#endif // CONFIG_EXAMPLE_TOUCH_WAKEUP
