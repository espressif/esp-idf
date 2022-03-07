/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "esp_bit_defs.h"
#include "esp_efuse.h"
#include "esp_efuse_rtc_table.h"

esp_err_t esp_efuse_rtc_calib_get_tsens_val(float* tsens_cal)
{
    uint32_t version = esp_efuse_rtc_table_read_calib_version();
    if ((version != 1) && (version != 2)) {
        *tsens_cal = 0.0;
        return ESP_ERR_NOT_SUPPORTED;
    }
    *tsens_cal = esp_efuse_rtc_table_get_parsed_efuse_value(RTCCALIB_IDX_TMPSENSOR, false);

    return ESP_OK;
}
