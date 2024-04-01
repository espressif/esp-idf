/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "isp_af_scheme.h"
#include "driver/isp_af.h"

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------
        ISP AF Step Approximation Scheme (SA Scheme)
---------------------------------------------------------------*/
typedef struct {
    int first_step_val;        ///< Step of the camera sensor focus value for first stage approximation
    int first_approx_cycles;   ///< First stage approximation cycles
    int second_step_val;       ///< Step of the camera sensor focus value for second stage approximation
    int second_approx_cycles;  ///< Second stage approximation cycles
} isp_af_sa_scheme_config_t;

typedef struct {
    int focus_val_max;         ///< Max camera sensor focus value
} isp_af_sa_scheme_sensor_info_t;

typedef struct {
    /**
     * @brief Sensor driver API to set sensor focus value
     *
     * @param[in] focus_val  Camera sensor focus value
     */
    esp_err_t (*af_sensor_set_focus)(int focus_val);

} isp_af_sa_scheme_sensor_drv_t;

/**
 * @brief Create an AF step approximation scheme
 *
 * @param[in]  af_ctrlr    AF controller handle
 * @param[in]  config      AF SA scheme configurations, see `isp_af_sa_scheme_config_t`
 * @param[out] ret_scheme  AF scheme handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid
 *         - ESP_ERR_INVALID_STATE Invalid state
 *         - ESP_ERR_NO_MEM        If out of memory
 */
esp_err_t isp_af_create_sa_scheme(isp_af_ctrlr_t af_ctrlr, const isp_af_sa_scheme_config_t *config, isp_af_scheme_handle_t *ret_scheme);

/**
 * @brief Delete an AF step approximation scheme
 *
 * @param[in] scheme  AF scheme handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid
 *         - ESP_ERR_INVALID_STATE Invalid state
 *         - ESP_ERR_NO_MEM        If out of memory
 */
esp_err_t isp_af_delete_sa_scheme(isp_af_scheme_handle_t scheme);

/**
 * @brief Register camera sensor driver to the SA scheme
 *
 * @param[in] scheme      AF scheme handle
 * @param[in] sensor_drv  Sensor driver, see `isp_af_sa_scheme_sensor_drv_t`
 * @param[in] info        Sensor info
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid
 *         - ESP_ERR_INVALID_STATE Invalid state
 */
esp_err_t isp_af_sa_scheme_register_sensor_driver(isp_af_scheme_handle_t scheme, const isp_af_sa_scheme_sensor_drv_t *sensor_drv, const isp_af_sa_scheme_sensor_info_t *info);

#ifdef __cplusplus
}
#endif
