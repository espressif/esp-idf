// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _DRIVER_ADC_H_
#define _DRIVER_ADC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "esp_err.h"

typedef enum {
    ADC_ATTEN_0db   = 0,  /*!<The input voltage of ADC will be reduced to about 1/1 */
    ADC_ATTEN_2_5db = 1,  /*!<The input voltage of ADC will be reduced to about 1/1.34 */
    ADC_ATTEN_6db   = 2,  /*!<The input voltage of ADC will be reduced to about 1/2 */
    ADC_ATTEN_11db  = 3,  /*!<The input voltage of ADC will be reduced to about 1/3.6*/
} adc_atten_t;

typedef enum {
    ADC_WIDTH_9Bit  = 0, /*!< ADC capture width is 9Bit*/
    ADC_WIDTH_10Bit = 1, /*!< ADC capture width is 10Bit*/
    ADC_WIDTH_11Bit = 2, /*!< ADC capture width is 11Bit*/
    ADC_WIDTH_12Bit = 3, /*!< ADC capture width is 12Bit*/
} adc_bits_width_t;

typedef enum {
    ADC1_CHANNEL_0 = 0, /*!< ADC1 channel 0 is GPIO36 */
    ADC1_CHANNEL_1,     /*!< ADC1 channel 1 is GPIO37 */
    ADC1_CHANNEL_2,     /*!< ADC1 channel 2 is GPIO38 */
    ADC1_CHANNEL_3,     /*!< ADC1 channel 3 is GPIO39 */
    ADC1_CHANNEL_4,     /*!< ADC1 channel 4 is GPIO32 */
    ADC1_CHANNEL_5,     /*!< ADC1 channel 5 is GPIO33 */
    ADC1_CHANNEL_6,     /*!< ADC1 channel 6 is GPIO34 */
    ADC1_CHANNEL_7,     /*!< ADC1 channel 7 is GPIO35 */
    ADC1_CHANNEL_MAX,
} adc1_channel_t;

/**
 * @brief Configure ADC1 capture width.
 *
 * The configuration is for all channels of ADC1
 *
 * @param width_bit Bit capture width for ADC1
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t adc1_config_width(adc_bits_width_t width_bit);

/**
 * @brief Configure the ADC1 channel, including setting attenuation.
 *
 * @note This function also configures the input GPIO pin mux to
 * connect it to the ADC1 channel. It must be called before calling
 * adc1_get_voltage() for this channel.
 *
 * The default ADC full-scale voltage is 1.1V. To read higher voltages (up to the pin maximum voltage,
 * usually 3.3V) requires setting >0dB signal attenuation for that ADC channel.
 *
 * When VDD_A is 3.3V:
 *
 * - 0dB attenuaton (ADC_ATTEN_0db) gives full-scale voltage 1.1V
 * - 2.5dB attenuation (ADC_ATTEN_2_5db) gives full-scale voltage 1.5V
 * - 6dB attenuation (ADC_ATTEN_6db) gives full-scale voltage 2.2V
 * - 11dB attenuation (ADC_ATTEN_11db) gives full-scale voltage 3.9V (see note below)
 *
 * @note The full-scale voltage is the voltage corresponding to a maximum reading (depending on ADC1 configured
 * bit width, this value is: 4095 for 12-bits, 2047 for 11-bits, 1023 for 10-bits, 511 for 9 bits.)
 *
 * @note At 11dB attenuation the maximum voltage is limited by VDD_A, not the full scale voltage.
 *
 * @param channel ADC1 channel to configure
 * @param atten  Attenuation level
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t adc1_config_channel_atten(adc1_channel_t channel, adc_atten_t atten);

/**
 * @brief Take an ADC1 reading on a single channel
 *
 * @note Call adc1_config_width() before the first time this
 * function is called.
 *
 * @note For a given channel, adc1_config_channel_atten(channel)
 * must be called before the first time this function is called.
 *
 * @param  channel ADC1 channel to read
 *
 * @return
 *     - -1: Parameter error
 *     -  Other: ADC1 channel reading.
 */
int adc1_get_voltage(adc1_channel_t channel);

/**
 * @brief Read Hall Sensor
 *
 * @note The Hall Sensor uses channels 0 and 3 of ADC1. Do not configure
 * these channels for use as ADC channels.
 *
 * @note The ADC1 module must be enabled by calling
 *       adc1_config_width() before calling hall_sensor_read(). ADC1
 *       should be configured for 12 bit readings, as the hall sensor
 *       readings are low values and do not cover the full range of the
 *       ADC.
 *
 * @return The hall sensor reading.
 */
int hall_sensor_read();

#ifdef __cplusplus
}
#endif

#endif  /*_DRIVER_ADC_H_*/

