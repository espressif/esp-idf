/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "hal/adc_types.h"
#include "adc_cali.h"
#include "adc_cali_scheme.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of ADC unit handle for oneshot mode
 */
typedef struct adc_oneshot_unit_ctx_t *adc_oneshot_unit_handle_t;

/**
 * @brief ADC oneshot driver initial configurations
 */
typedef struct {
    adc_unit_t unit_id;             ///< ADC unit
    adc_oneshot_clk_src_t clk_src;  ///< Clock source
    adc_ulp_mode_t ulp_mode;        ///< ADC controlled by ULP, see `adc_ulp_mode_t`
} adc_oneshot_unit_init_cfg_t;

/**
 * @brief ADC channel configurations
 */
typedef struct {
    adc_atten_t atten;              ///< ADC attenuation
    adc_bitwidth_t bitwidth;        ///< ADC conversion result bits
} adc_oneshot_chan_cfg_t;

/**
 * @brief Create a handle to a specific ADC unit
 *
 * @note This API is thread-safe. For more details, see ADC programming guide
 *
 * @param[in]  init_config    Driver initial configurations
 * @param[out] ret_unit       ADC unit handle
 *
 * @return
 *        - ESP_OK:              On success
 *        - ESP_ERR_INVALID_ARG: Invalid arguments
 *        - ESP_ERR_NO_MEM:      No memory
 *        - ESP_ERR_NOT_FOUND:   The ADC peripheral to be claimed is already in use
 *        - ESP_FAIL:            Clock source isn't initialised correctly
 */
esp_err_t adc_oneshot_new_unit(const adc_oneshot_unit_init_cfg_t *init_config, adc_oneshot_unit_handle_t *ret_unit);

/**
 * @brief Set ADC oneshot mode required configurations
 *
 * @note This API is thread-safe. For more details, see ADC programming guide
 *
 * @param[in] handle    ADC handle
 * @param[in] channel   ADC channel to be configured
 * @param[in] config    ADC configurations
 *
 * @return
 *        - ESP_OK:              On success
 *        - ESP_ERR_INVALID_ARG: Invalid arguments
 */
esp_err_t adc_oneshot_config_channel(adc_oneshot_unit_handle_t handle, adc_channel_t channel, const adc_oneshot_chan_cfg_t *config);

/**
 * @brief Get one ADC conversion raw result
 *
 * @note This API is thread-safe. For more details, see ADC programming guide
 * @note This API should NOT be called in an ISR context
 *
 * @param[in] handle    ADC handle
 * @param[in] chan      ADC channel
 * @param[out] out_raw  ADC conversion raw result
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_ARG:   Invalid arguments
 *        - ESP_ERR_TIMEOUT:       Timeout, the ADC result is invalid
 */
esp_err_t adc_oneshot_read(adc_oneshot_unit_handle_t handle, adc_channel_t chan, int *out_raw);

/**
 * @brief Delete the ADC unit handle
 *
 * @note This API is thread-safe. For more details, see ADC programming guide
 *
 * @param[in] handle    ADC handle
 *
 * @return
 *        - ESP_OK:              On success
 *        - ESP_ERR_INVALID_ARG: Invalid arguments
 *        - ESP_ERR_NOT_FOUND:   The ADC peripheral to be disclaimed isn't in use
 */
esp_err_t adc_oneshot_del_unit(adc_oneshot_unit_handle_t handle);

/**
 * @brief Get ADC channel from the given GPIO number
 *
 * @param[in]  io_num     GPIO number
 * @param[out] unit_id    ADC unit
 * @param[out] channel    ADC channel
 *
 * @return
 *        - ESP_OK:              On success
 *        - ESP_ERR_INVALID_ARG: Invalid argument
 *        - ESP_ERR_NOT_FOUND:   The IO is not a valid ADC pad
 */
esp_err_t adc_oneshot_io_to_channel(int io_num, adc_unit_t *unit_id, adc_channel_t *channel);

/**
 * @brief Get GPIO number from the given ADC channel
 *
 * @param[in]  unit_id    ADC unit
 * @param[in]  channel    ADC channel
 * @param[out] io_num     GPIO number
 *
 * @param
 *       - ESP_OK:              On success
 *       - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t adc_oneshot_channel_to_io(adc_unit_t unit_id, adc_channel_t channel, int *io_num);

/**
 * @brief Convenience function to get ADC calibrated result
 *
 * This is an all-in-one function which does:
 * - oneshot read ADC raw result
 * - calibrate the raw result and convert it into calibrated result (in mV)
 *
 * @param[in]  handle       ADC oneshot handle, you should call adc_oneshot_new_unit() to get this handle
 * @param[in]  cali_handle  ADC calibration handle, you should call adc_cali_create_scheme_x() in adc_cali_scheme.h to create a handle
 * @param[in]  chan         ADC channel
 * @param[out] cali_result  Calibrated ADC result (in mV)
 *
 * @return
 *        - ESP_OK
 *        Other return errors from adc_oneshot_read() and adc_cali_raw_to_voltage()
 */
esp_err_t adc_oneshot_get_calibrated_result(adc_oneshot_unit_handle_t handle, adc_cali_handle_t cali_handle, adc_channel_t chan, int *cali_result);

#ifdef __cplusplus
}
#endif
