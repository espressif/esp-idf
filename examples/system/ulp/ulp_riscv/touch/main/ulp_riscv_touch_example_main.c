/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/* ULP RISC-V touch sensor example */

#include <stdio.h>
#include "esp_sleep.h"
#include "ulp_riscv.h"
#include "ulp_main.h"
#include "driver/touch_sens.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "touch_sens_example_config.h"

extern const uint8_t _binary_ulp_main_bin_start[];
extern const uint8_t _binary_ulp_main_bin_end[];

#define TOUCH_BUTTON_NUM    14U

static const int s_channel_id[TOUCH_BUTTON_NUM] = {
    TOUCH_MIN_CHAN_ID,
    TOUCH_MIN_CHAN_ID + 1,
    TOUCH_MIN_CHAN_ID + 2,
    TOUCH_MIN_CHAN_ID + 3,
    TOUCH_MIN_CHAN_ID + 4,
    TOUCH_MIN_CHAN_ID + 5,
    TOUCH_MIN_CHAN_ID + 6,
    TOUCH_MIN_CHAN_ID + 7,
    TOUCH_MIN_CHAN_ID + 8,
    TOUCH_MIN_CHAN_ID + 9,
    TOUCH_MIN_CHAN_ID + 10,
    TOUCH_MIN_CHAN_ID + 11,
    TOUCH_MIN_CHAN_ID + 12,
    TOUCH_MIN_CHAN_ID + 13,
};

/* Handles of touch sensor and its channels*/
static touch_sensor_handle_t s_sens_handle = NULL;
static touch_channel_handle_t s_chan_handle[TOUCH_BUTTON_NUM];

static void init_touch_sensor(void)
{
    /* Create a new touch sensor controller handle with default sample configuration */
    touch_sensor_sample_config_t sample_cfg[TOUCH_SAMPLE_CFG_NUM] = EXAMPLE_TOUCH_SAMPLE_CFG_DEFAULT();
    touch_sensor_config_t sens_cfg = TOUCH_SENSOR_DEFAULT_BASIC_CONFIG(1, sample_cfg);
    ESP_ERROR_CHECK(touch_sensor_new_controller(&sens_cfg, &s_sens_handle));

    /* Create and enable the new touch channel handles with default configurations */
    touch_channel_config_t chan_cfg = EXAMPLE_TOUCH_CHAN_CFG_DEFAULT();
    /* Allocate new touch channel on the touch controller */
    for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
        ESP_ERROR_CHECK(touch_sensor_new_channel(s_sens_handle, s_channel_id[i], &chan_cfg, &s_chan_handle[i]));
    }

    /* Confiture the default filter for the touch sensor (Note: Touch V1 uses software filter) */
    touch_sensor_filter_config_t filter_cfg = TOUCH_SENSOR_DEFAULT_FILTER_CONFIG();
    ESP_ERROR_CHECK(touch_sensor_config_filter(s_sens_handle, &filter_cfg));

    /* Enable the touch sensor */
    ESP_ERROR_CHECK(touch_sensor_enable(s_sens_handle));

    /* Start continuous scanning, you can also trigger oneshot scanning manually */
    ESP_ERROR_CHECK(touch_sensor_start_continuous_scanning(s_sens_handle));
}

static void init_ulp_program(void)
{
    esp_err_t err = ulp_riscv_load_binary(_binary_ulp_main_bin_start, (_binary_ulp_main_bin_end - _binary_ulp_main_bin_start));
    ESP_ERROR_CHECK(err);

    /* The first argument is the period index, which is not used by the ULP-RISC-V timer
     * The second argument is the period in microseconds, which gives a wakeup time period of: 20ms
     */
    ulp_set_wakeup_period(0, 20000);

    /* Start the ULP program */
    err = ulp_riscv_run();
    ESP_ERROR_CHECK(err);
}

void app_main(void)
{
    /* If user is using USB-serial-jtag then idf monitor needs some time to
    *  re-connect to the USB port. We wait 1 sec here to allow for it to make the reconnection
    *  before we print anything. Otherwise the chip will go back to sleep again before the user
    *  has time to monitor any output.
    */
    vTaskDelay(pdMS_TO_TICKS(1000));

    if (esp_sleep_get_wakeup_causes() & BIT(ESP_SLEEP_WAKEUP_ULP)) {
        /* ULP RISC-V detected a touch input */
        printf("ULP-RISC-V woke up the main CPU! \n");
        uint32_t touch_data = ulp_touch_data;
        for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
            if ((touch_data >> s_channel_id[i]) & 0x1) {
                printf("T%d touched\n", s_channel_id[i]);
            }
        }
        printf("\n");
    } else {
        /* not a wakeup from ULP, load the firmware */
        printf("Not a ULP-RISC-V wakeup, initializing ...\n");
        /* Initialize Touch peripheral */
        init_touch_sensor();

        /* Initialize ULP core */
        init_ulp_program();
    }

    /* Go to sleep, only the ULP RISC-V will run */
    printf("Entering in deep sleep\n\n");

    /* Small delay to ensure the messages are printed */
    vTaskDelay(100);

    /* Keep RTC_PERIPH powered on to keep all touch pad channels active */
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);

    ESP_ERROR_CHECK(esp_sleep_enable_ulp_wakeup());
    esp_deep_sleep_start();
}
