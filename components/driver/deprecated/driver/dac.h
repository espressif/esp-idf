/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "driver/dac_types_legacy.h"

#if !CONFIG_DAC_SUPPRESS_DEPRECATE_WARN
#warning "The legacy DAC driver is deprecated, please use `driver/dac_oneshot.h`, `driver/dac_cosine.h` or `driver/dac_continuous.h` instead"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the GPIO number of a specific DAC channel.
 *
 * @param channel Channel to get the gpio number
 * @param gpio_num output buffer to hold the gpio number
 * @return
 *   - ESP_OK if success
 */
esp_err_t dac_pad_get_io_num(dac_channel_t channel, gpio_num_t *gpio_num);

/**
 * @brief Set DAC output voltage.
 *        DAC output is 8-bit. Maximum (255) corresponds to VDD3P3_RTC.
 *
 * @note Need to configure DAC pad before calling this function.
 *       DAC channel 0 is attached to GPIO25, DAC channel 1 is attached to GPIO26
 * @param channel DAC channel
 * @param dac_value DAC output value
 *
 * @return
 *     - ESP_OK success
 */
esp_err_t dac_output_voltage(dac_channel_t channel, uint8_t dac_value);

/**
 * @brief DAC pad output enable
 *
 * @param channel DAC channel
 * @note DAC channel 0 is attached to GPIO25, DAC channel 1 is attached to GPIO26
 *       I2S left channel will be mapped to DAC channel 1
 *       I2S right channel will be mapped to DAC channel 0
 */
esp_err_t dac_output_enable(dac_channel_t channel);

/**
 * @brief DAC pad output disable
 *
 * @param channel DAC channel
 * @note DAC channel 0 is attached to GPIO25, DAC channel 1 is attached to GPIO26
 * @return
 *     - ESP_OK success
 */
esp_err_t dac_output_disable(dac_channel_t channel);

/**
 * @brief Enable cosine wave generator output.
 *
 * @return
 *     - ESP_OK success
 */
esp_err_t dac_cw_generator_enable(void);

/**
 * @brief Disable cosine wave generator output.
 *
 * @return
 *     - ESP_OK success
 */
esp_err_t dac_cw_generator_disable(void);

/**
 * @brief Config the cosine wave generator function in DAC module.
 *
 * @param cw Configuration.
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG The parameter is NULL.
 */
esp_err_t dac_cw_generator_config(dac_cw_config_t *cw);

/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/
#if CONFIG_IDF_TARGET_ESP32
/**
 * @brief Enable DAC output data from I2S
 *
 * @return
 *     - ESP_OK success
 */
esp_err_t dac_i2s_enable(void);

/**
 * @brief Disable DAC output data from I2S
 *
 * @return
 *     - ESP_OK success
 */
esp_err_t dac_i2s_disable(void);
#endif // CONFIG_IDF_TARGET_ESP32

#if CONFIG_IDF_TARGET_ESP32S2
/**
 * @brief DAC digital controller initialization.
 * @return
 *     - ESP_OK success
 */
esp_err_t dac_digi_init(void);

/**
 * @brief DAC digital controller deinitialization.
 * @return
 *     - ESP_OK success
 */
esp_err_t dac_digi_deinit(void);

/**
 * @brief Setting the DAC digital controller.
 *
 * @param cfg Pointer to digital controller paramter. See ``dac_digi_config_t``.
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t dac_digi_controller_config(const dac_digi_config_t *cfg);

/**
 * @brief DAC digital controller start output voltage.
 * @return
 *     - ESP_OK success
 */
esp_err_t dac_digi_start(void);

/**
 * @brief DAC digital controller stop output voltage.
 * @return
 *     - ESP_OK success
 */
esp_err_t dac_digi_stop(void);

/**
 * @brief Reset DAC digital controller FIFO.
 * @return
 *     - ESP_OK success
 */
esp_err_t dac_digi_fifo_reset(void);

/**
 * @brief Reset DAC digital controller.
 * @return
 *     - ESP_OK success
 */
esp_err_t dac_digi_reset(void);
#endif // CONFIG_IDF_TARGET_ESP32S2

#ifdef __cplusplus
}
#endif
