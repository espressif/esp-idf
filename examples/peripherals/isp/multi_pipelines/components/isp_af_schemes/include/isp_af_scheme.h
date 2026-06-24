/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of ISP AF scheme handle
 */
typedef struct isp_af_scheme_t *isp_af_scheme_handle_t;

/**
 * @brief ISP AF process, which is used to calculate definition threshold and luminance threshold for AF environment detector to detect environment change
 *
 * @param[in]  scheme                 AF scheme handle
 * @param[out] out_definition_thresh  Calculated definition threshold
 * @param[out] out_luminance_thresh   Calculated luminance threshold
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t isp_af_process(isp_af_scheme_handle_t scheme, int *out_definition_thresh, int *out_luminance_thresh);

#ifdef __cplusplus
}
#endif
