/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "driver/isp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ISP DPC dynamic correction method
 *
 * Method 1 uses absolute thresholds around the minimum and maximum of the
 * eight neighboring pixels. Method 2 uses ratios and a second adaptive test
 * against the neighboring-pixel estimate.
 */
typedef enum {
    ESP_ISP_DPC_DYNAMIC_METHOD_1 = 0, ///< Detects a pixel outside [min8 - low_threshold, max8 + high_threshold]
    ESP_ISP_DPC_DYNAMIC_METHOD_2 = 1, ///< Applies ratio screening followed by an estimate-based adaptive threshold test
} esp_isp_dpc_dynamic_method_t;

/**
 * @brief ISP DPC dynamic correction configurations
 *
 * @note Both methods examine the eight same-color neighboring pixels and replace a
 *       detected dead pixel with their median value. Method 1 directly compares the center
 *       pixel with the neighboring minimum and maximum using absolute thresholds. Method 2
 *       first keeps pixels within a max8-based ratio range. For pixels outside that range,
 *       it compares the center pixel with the neighboring-pixel estimate using adaptive
 *       bright and dark thresholds.
 */
typedef struct {
    esp_isp_dpc_dynamic_method_t method; ///< Dynamic correction method
    union {
        struct {
            uint8_t high_threshold; ///< A pixel above max8 + high_threshold is a bright dead-pixel candidate (0-255)
            uint8_t low_threshold;  ///< A pixel below min8 - low_threshold is a dark dead-pixel candidate (0-255)
        } method_1;
        struct {
            isp_dpc_ratio_t first_stage_upper_ratio;             /*!< Upper bound of the first-stage normal-pixel range.
                                                                   *   Value = integer + decimal / ISP_DPC_RATIO_MAX. Range 0 ~ 1.
                                                                   *   Fractional values use integer 0 and decimal 0 ~ (ISP_DPC_RATIO_MAX - 1);
                                                                   *   1.0 uses integer 1 and decimal 0. Must be greater than first_stage_lower_ratio.
                                                                   */
            isp_dpc_ratio_t first_stage_lower_ratio;             /*!< Lower bound of the first-stage normal-pixel range.
                                                                   *   Value = integer + decimal / ISP_DPC_RATIO_MAX. Range 0 ~ 1.
                                                                   *   Fractional values use integer 0 and decimal 0 ~ (ISP_DPC_RATIO_MAX - 1);
                                                                   *   1.0 uses integer 1 and decimal 0.
                                                                   */
            isp_dpc_deviation_factor_t bright_deviation_factor;  /*!< Second-stage bright-pixel sensitivity.
                                                                   *   Value = integer + decimal / ISP_DPC_DEVIATION_FACTOR_MAX. Range 0 ~ 1.
                                                                   *   Fractional values use integer 0 and decimal 0 ~ (ISP_DPC_DEVIATION_FACTOR_MAX - 1);
                                                                   *   1.0 uses integer 1 and decimal 0. A smaller value corrects bright pixels more aggressively.
                                                                   */
            isp_dpc_deviation_factor_t dark_deviation_factor;    /*!< Second-stage dark-pixel sensitivity.
                                                                   *   Value = integer + decimal / ISP_DPC_DEVIATION_FACTOR_MAX. Range 0 ~ 1.
                                                                   *   Fractional values use integer 0 and decimal 0 ~ (ISP_DPC_DEVIATION_FACTOR_MAX - 1);
                                                                   *   1.0 uses integer 1 and decimal 0. A smaller value corrects dark pixels more aggressively.
                                                                   */
        } method_2;
    };
} esp_isp_dpc_dynamic_config_t;

/**
 * @brief Configure dynamic DPC correction
 *
 * @note Call this while DPC is disabled. This function enables dynamic correction but
 *       does not enable the DPC module; call esp_isp_dpc_configure() and then
 *       esp_isp_dpc_enable() afterwards. Static and dynamic correction can be
 *       configured together.
 *
 * @param[in] isp_proc Processor handle
 * @param[in] config Dynamic correction configuration
 *
 * @return
 *        - ESP_OK                 On success
 *        - ESP_ERR_INVALID_ARG    Invalid argument or dynamic parameters
 *        - ESP_ERR_INVALID_STATE  DPC is enabled
 *        - ESP_ERR_NOT_SUPPORTED  Not supported
 */
esp_err_t esp_isp_dpc_dynamic_configure(isp_proc_handle_t isp_proc, const esp_isp_dpc_dynamic_config_t *config);

#ifdef __cplusplus
}
#endif
