/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "soc/soc.h"
#include "soc/rtc.h"
#include "hal/efuse_hal.h"
#include "hal/efuse_ll.h"
#include "regi2c_ctrl.h"
#include "esp_hw_log.h"


static const char *TAG = "ocode_init";

static void set_ocode_by_efuse(int ocode_scheme_ver)
{
    // TODO: [ESP32C5] IDF-8702
}

static void calibrate_ocode(void)
{
    // TODO: [ESP32C5] IDF-8702
}

void esp_ocode_calib_init(void)
{
    // TODO: [ESP32C5] IDF-8702
    ESP_HW_LOGW(TAG, "esp_ocode_calib_init() has not implemented yet");
    if (efuse_hal_blk_version() >= 1) {
        set_ocode_by_efuse(1);
    } else {
        calibrate_ocode();
    }
}
