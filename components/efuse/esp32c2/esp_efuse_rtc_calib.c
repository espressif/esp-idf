/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_bit_defs.h>
#include "esp_efuse.h"
#include "esp_efuse_table.h"

int esp_efuse_rtc_calib_get_ver(void)
{
    uint32_t result = 0;
    esp_efuse_read_field_blob(ESP_EFUSE_BLK_VERSION_MINOR, &result, ESP_EFUSE_BLK_VERSION_MINOR[0]->bit_count); // IDF-5366
    return result;
}

esp_err_t esp_efuse_rtc_calib_get_tsens_val(float* tsens_cal)
{
    // Currently calibration is not supported on ESP32-C2, IDF-5236
    *tsens_cal = 0.0;
    return ESP_OK;
}
