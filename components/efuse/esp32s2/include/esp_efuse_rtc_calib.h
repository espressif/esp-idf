/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_types.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Get the temperature sensor calibration number delta_T stored in the efuse.
 *
 * @param tsens_cal Pointer of the specification of temperature sensor calibration number in efuse.
 *
 * @return ESP_OK if get the calibration value successfully.
 *         ESP_ERR_INVALID_ARG if can't get the calibration value.
 */
esp_err_t esp_efuse_rtc_calib_get_tsens_val(float* tsens_cal);

#ifdef __cplusplus
}
#endif
