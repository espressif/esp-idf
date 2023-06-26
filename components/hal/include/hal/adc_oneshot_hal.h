/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "hal/adc_types.h"
#include "hal/adc_hal_common.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct sens_dev_t *adc_oneshot_soc_handle_t;

typedef struct adc_oneshot_hal_cfg_t {
    adc_unit_t unit;                 ///< ADC unit
    adc_hal_work_mode_t work_mode;   ///< ADC work mode
    adc_oneshot_clk_src_t clk_src;   ///< Clock source
    uint32_t clk_src_freq_hz;        ///< Clock source frequency in hz
} adc_oneshot_hal_cfg_t;

/**
 * ADC channel configuration
 */
typedef struct adc_oneshot_hal_chan_cfg_t {
    adc_atten_t atten;               ///< ADC attenuation
    adc_bitwidth_t bitwidth;         ///< ADC conversion result bits
} adc_oneshot_hal_chan_cfg_t;

/**
 * Context of the ADC unit, should be maintained by both the driver and the HAL.
 */
typedef struct adc_oneshot_hal_ctx_t {
    /* These should be configured by driver, dou't modify these directly in hal*/
    adc_oneshot_soc_handle_t dev;    ///< ADC SoC layer handle
    adc_unit_t unit;                 ///< ADC unit
    adc_hal_work_mode_t work_mode;   ///< ADC work mode
    adc_oneshot_hal_chan_cfg_t chan_configs[SOC_ADC_MAX_CHANNEL_NUM];    ///< ADC configurations per channel
    adc_oneshot_clk_src_t clk_src;   ///< Clock source
    uint32_t clk_src_freq_hz;        ///< Clock source frequency in hz
} adc_oneshot_hal_ctx_t;

/**
 * Initialise the context
 *
 * @param hal    ADC Oneshot Hal context
 * @param config ADC Oneshot Hal init config
 */
void adc_oneshot_hal_init(adc_oneshot_hal_ctx_t *hal, const adc_oneshot_hal_cfg_t *config);

/**
 * Prepare ADC Oneshot hal context
 *
 * @param hal    ADC Oneshot Hal context
 * @param config ADC Oneshot Hal configuration
 * @param chan   ADC Channel
 */
void adc_oneshot_hal_channel_config(adc_oneshot_hal_ctx_t *hal, const adc_oneshot_hal_chan_cfg_t *config, adc_channel_t chan);

/**
 * Set ADC Oneshot mode required registers
 *
 * @param hal     ADC Oneshot Hal context
 * @param channel ADC Channel
 */
void adc_oneshot_hal_setup(adc_oneshot_hal_ctx_t *hal, adc_channel_t channel);

/**
 * Start ADC conversion in Oneshot mode and get the raw result
 *
 * @param hal          ADC Oneshot Hal context
 * @param[out] out_raw ADC oneshot conversion raw result
 *
 * @return
 *        - true: ADC raw result is valid
 *        - false: ADC raw result is invalid
 */
bool adc_oneshot_hal_convert(adc_oneshot_hal_ctx_t *hal, int *out_raw);


#ifdef __cplusplus
}
#endif
