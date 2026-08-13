/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"
#include "driver/isp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_ISP_DPC_MAX_DEAD_PIXELS 512 ///< Maximum number of dead pixels supported by the static DPC LUT

/**
 * @brief DPC pixel coordinate
 */
typedef struct {
    uint16_t x; ///< Horizontal coordinate
    uint16_t y; ///< Vertical coordinate
} esp_isp_dpc_pixel_coord_t;

/**
 * @brief ISP DPC static correction configurations
 */
typedef struct {
    const esp_isp_dpc_pixel_coord_t *dead_pixel_coords; ///< Unique coordinates in ascending y/x order. Coordinates must be inside the input frame. The driver reads this array only during esp_isp_dpc_static_configure().
    uint32_t dead_pixel_count;                           ///< Number of dead pixels in the array (0 to ESP_ISP_DPC_MAX_DEAD_PIXELS)
} esp_isp_dpc_static_config_t;

/**
 * @brief Configure static DPC correction
 *
 * @note Call this while DPC is disabled. This function writes the coordinates to the
 *       hardware LUT and enables static correction. It does not enable the DPC module;
 *       call esp_isp_dpc_configure() and then esp_isp_dpc_enable() afterwards. The
 *       driver reads dead_pixel_coords only during this call and does not retain the
 *       array after it returns.
 *
 * @param[in] isp_proc Processor handle
 * @param[in] config Static correction configuration
 *
 * @return
 *        - ESP_OK                 On success
 *        - ESP_ERR_INVALID_STATE  DPC is enabled
 *        - ESP_ERR_INVALID_ARG    Invalid argument or coordinates
 *        - ESP_ERR_NOT_SUPPORTED  Not supported
 */
esp_err_t esp_isp_dpc_static_configure(isp_proc_handle_t isp_proc, const esp_isp_dpc_static_config_t *config);

/**
 * @brief ISP DPC static calibration configuration
 */
typedef struct {
    uint8_t threshold;     ///< Dead-pixel threshold. For a white image, lower pixels are considered dead pixels; for a black image, higher pixels are considered dead pixels
    bool enable_output;    ///< Whether to output the calibration frame. When false, DPC records detected coordinates without outputting image data.
} esp_isp_dpc_calibration_config_t;

/**
 * @brief DPC calibration image type
 */
typedef enum {
    ESP_ISP_DPC_CALIBRATION_IMAGE_WHITE, ///< Uniform white frame, used to detect dark dead pixels
    ESP_ISP_DPC_CALIBRATION_IMAGE_BLACK, ///< Uniform black frame, used to detect bright dead pixels
} esp_isp_dpc_calibration_image_t;

/**
 * @brief ISP DPC calibration coordinate reference
 */
typedef struct {
    esp_isp_dpc_pixel_coord_t dead_pixel_coords[ESP_ISP_DPC_MAX_DEAD_PIXELS]; ///< Dead pixel coordinates
    uint32_t dead_pixel_count;                                             ///< Number of valid coordinates in dead_pixel_coords
} esp_isp_dpc_calibration_ref_t;

/**
 * @brief Start DPC static calibration onetime
 *
 * @note After calling this function, input a uniform frame matching image_type,
 *       then call esp_isp_dpc_calibration_read_result to wait for completion and
 *       read the results.
 *       Static correction is enabled and dynamic correction is disabled while
 *       calibration check mode is enabled.
 *       DPC must not be enabled before calling this function.
 *
 * @param[in] isp_proc    Processor handle
 * @param[in] image_type  Calibration image type
 * @param[in] config      Calibration configuration
 *
 * @return
 *        - ESP_OK                 On success
 *        - ESP_ERR_INVALID_STATE  Not allowed to be called under current state
 *        - ESP_ERR_INVALID_ARG    If the combination of arguments is invalid
 *        - ESP_ERR_NOT_SUPPORTED  Not supported
 */
esp_err_t esp_isp_dpc_static_calibration_start_once(isp_proc_handle_t isp_proc,
                                                    esp_isp_dpc_calibration_image_t image_type,
                                                    const esp_isp_dpc_calibration_config_t *config);

/**
 * @brief Read DPC static calibration results
 *
 * @note This function waits for calibration completion, then disables check mode and
 *       DPC before reading the LUT. The result storage is supplied by the caller.
 *
 * @param[in] isp_proc    Processor handle
 * @param[in] timeout_ms  Maximum wait time in milliseconds; zero polls once
 * @param[out] result     Calibration coordinate reference
 *
 * @return
 *        - ESP_OK                 On success
 *        - ESP_ERR_INVALID_ARG    If the combination of arguments is invalid
 *        - ESP_ERR_INVALID_STATE  Calibration is not running
 *        - ESP_ERR_TIMEOUT        Calibration did not complete within the timeout
 */
esp_err_t esp_isp_dpc_calibration_read_result(isp_proc_handle_t isp_proc, uint32_t timeout_ms,
                                              esp_isp_dpc_calibration_ref_t *result);

/**
 * @brief Merge static calibration coordinate references
 *
 * @note This is a software-only utility. It does not access an ISP processor or any
 *       hardware resource. It combines the coordinates from ref_count references, sorts
 *       them by y/x, removes duplicates, and keeps at most ESP_ISP_DPC_MAX_DEAD_PIXELS
 *       coordinates. The output reference must not also appear in refs.
 *
 * @param[in] refs Array of pointers to the coordinate references to merge
 * @param[in] ref_count Number of entries in refs, must be greater than zero
 * @param[out] merged_ref Merged coordinate reference
 *
 * @return
 *        - ESP_OK                 On success
 *        - ESP_ERR_INVALID_ARG    If the combination of arguments is invalid
 */
esp_err_t esp_isp_dpc_calibration_merge_result(const esp_isp_dpc_calibration_ref_t *const refs[],
                                               size_t ref_count,
                                               esp_isp_dpc_calibration_ref_t *merged_ref);

#ifdef __cplusplus
}
#endif
