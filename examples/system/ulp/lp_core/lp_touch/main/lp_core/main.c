/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * LP touch sensor example
 * This code runs on LP coprocessor
 */

#include <stdint.h>
#include "soc/soc_caps.h"
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_print.h"
#include "ulp_lp_core_touch.h"

/* This variable will be exported as a public symbol, visible from main CPU: */
uint32_t touch_data = 0;

/* Touch pad threshold */
#define TOUCH_PAD_THRESHOLD                 100U
#define EXAMPLE_TOUCH_CHANNEL_NUM           4

static int s_channel_id[EXAMPLE_TOUCH_CHANNEL_NUM] = {
    SOC_TOUCH_MIN_CHAN_ID,
    SOC_TOUCH_MIN_CHAN_ID + 1,
    SOC_TOUCH_MIN_CHAN_ID + 2,
    SOC_TOUCH_MIN_CHAN_ID + 3,
};

int main (void)
{
    esp_err_t err = ESP_OK;
    uint32_t benchmark = 0;
    uint32_t smooth = 0;
    uint32_t touch_value = 0;

    for (int i = 0; i < EXAMPLE_TOUCH_CHANNEL_NUM; i++) {
        /* Read the benchmark and the filtered (smooth) touch data */
        if (lp_core_touch_pad_read_benchmark(s_channel_id[i], &benchmark) != ESP_OK) {
            /* Skip channel this time */
            continue;
        }
        if (lp_core_touch_pad_filter_read_smooth(s_channel_id[i], &smooth) != ESP_OK) {
            /* Skip channel this time */
            continue;
        }

        /* The difference between the benchmark and the smooth touch data
         * is used to determine an active touch pad.
         * Check if the touch value breaches the wakeup threshold.
         */
        if (smooth > benchmark) {
            if ((smooth - benchmark) > TOUCH_PAD_THRESHOLD) {
                touch_value |= (1 << s_channel_id[i]);
            }
        }
    }

    /* Wake up the main CPU by LP core if a touch pad is active */
    if (touch_value) {
        touch_data = touch_value;
        ulp_lp_core_wakeup_main_processor();
        touch_value = 0;
    }

    /* lp_core_halt() is called automatically when main exits */
    return 0;
}
