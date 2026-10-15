/*
 * SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// DO NOT USE THESE APIS IN ANY APPLICATIONS

#pragma once
#include "esp_err.h"
#include "hal/adc_types.h"
#include "soc/soc_caps.h"
#if SOC_ADC_SUPPORTED
#include "hal/adc_ll.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------
            ADC IOs
---------------------------------------------------------------*/
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
esp_err_t adc_io_to_channel(int io_num, adc_unit_t * const unit_id, adc_channel_t * const channel);

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
esp_err_t adc_channel_to_io(adc_unit_t unit_id, adc_channel_t channel, int * const io_num);

/*---------------------------------------------------------------
            ADC Oneshot Read API ISR Version
---------------------------------------------------------------*/
typedef struct adc_oneshot_unit_ctx_t *adc_oneshot_unit_handle_t;
/**
 * @brief ISR version to get one ADC conversion raw result
 *
 * @note This API only provide atomic register settings, without hardware resources protection. When other drivers are using
 *       SAR-ADCs, calling this API may get wrong ADC result.
 * @note This API can be called in an ISR context.
 * @note Strongly suggest using this function when there's no concurrent hardware usage to the ADC. You can refer to ADC Oneshot
 *       Programming Guide to know ADC Hardware Limitations
 *
 * @param[in] handle    ADC handle
 * @param[in] chan      ADC channel
 * @param[out] out_raw  ADC conversion raw result
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_ARG:   Invalid arguments
 *        - ESP_ERR_INVALID_STATE: Invalid state, the ADC result is invalid
 */
esp_err_t adc_oneshot_read_isr(adc_oneshot_unit_handle_t handle, adc_channel_t chan, int *out_raw);

#if ADC_LL_SW_WEIGHT_CALIBRATION_SUPPORTED
/**
 * @brief Run software-weight calibration for one ADC unit
 *
 * Claims the APB SARADC clock if needed, enables the conversion clock and analog
 * power, samples the internal calibration muxes, computes weights, then restores
 * the ADC mux/trigger and releases those resources.
 *
 * Subsequent calls on an already-calibrated unit skip sampling and only restore
 * the working-mode flags.
 */
void adc_cal_sw_weight(adc_unit_t adc_n, adc_oneshot_clk_src_t clk_src);
#endif

#ifdef __cplusplus
}
#endif
