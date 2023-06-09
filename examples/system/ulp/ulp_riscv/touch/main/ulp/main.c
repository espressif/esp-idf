/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * ULP-RISC-V touch sensor example
 * This code runs on ULP-RISC-V  coprocessor
 */

#include <stdint.h>
#include "ulp_riscv.h"
#include "ulp_riscv_utils.h"
#include "ulp_riscv_touch_ulp_core.h"

/* This variable will be exported as a public symbol, visible from main CPU: */
uint32_t touch_data = 0;

/* Touch pad threshold */
#define TOUCH_PAD_THRESHOLD 1000U
#define TOUCH_BUTTON_NUM    14U

static const touch_pad_t button[TOUCH_BUTTON_NUM] = {
    TOUCH_PAD_NUM1,
    TOUCH_PAD_NUM2,
    TOUCH_PAD_NUM3,
    TOUCH_PAD_NUM4,
    TOUCH_PAD_NUM5,
    TOUCH_PAD_NUM6,
    TOUCH_PAD_NUM7,
    TOUCH_PAD_NUM8,
    TOUCH_PAD_NUM9,
    TOUCH_PAD_NUM10,
    TOUCH_PAD_NUM11,
    TOUCH_PAD_NUM12,
    TOUCH_PAD_NUM13,
    TOUCH_PAD_NUM14
};

int main (void)
{
    esp_err_t err = ESP_OK;
    uint32_t benchmark = 0;
    uint32_t smooth = 0;
    uint32_t touch_value = 0;

    for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
        /* Read the benchmark and the filtered (smooth) touch data */
        err = ulp_riscv_touch_pad_read_benchmark(button[i], &benchmark);
        err = ulp_riscv_touch_pad_filter_read_smooth(button[i], &smooth);
        if (err != ESP_OK) {
            /* Skip channel this time */
            continue;
        }

        /* The difference between the benchmark and the smooth touch data
         * is used to determine an active touch pad.
         * Check if the touch value breaches the wakeup threshold.
         */
        if (smooth > benchmark) {
            if ((smooth - benchmark) > TOUCH_PAD_THRESHOLD) {
                touch_value |= (1 << button[i]);
            }
        }
    }

    if (touch_value) {
        touch_data = touch_value;
        ulp_riscv_wakeup_main_processor();
        touch_value = 0;
    }

    /* ulp_riscv_halt() is called automatically when main exits */
    return 0;
}
