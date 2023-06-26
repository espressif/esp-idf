/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_bit_defs.h>
#include "esp_efuse.h"
#include "esp_efuse_table.h"

esp_err_t esp_efuse_rtc_calib_get_tsens_val(float* tsens_cal)
{
    // Currently calibration is not supported on ESP32-H2, IDF-5236
    *tsens_cal = 0;
    return ESP_OK;
}
