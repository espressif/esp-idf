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
 * @brief Configuration ADC1 capture width.
 *
 * The configuration is in effect for all channels of ADC1
 *
 * @param  width_bit ADC1
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t adc1_config_width(adc_bits_width_t width_bit);

/**
 * @brief Configuration ADC1 capture attenuation of channels.
 *
 * @param  channel the ADC1 channel
 * @param  atten attenuation
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t adc1_config_channel_atten(adc1_channel_t channel, adc_atten_t atten);

/**
 * @brief ADC1 get the value of the voltage.
 *
 * @param  channel the ADC1 channel
 *
 * @return
 *     - -1 Parameter error
 *     -  Other the value of ADC1 channel
 */
int adc1_get_voltage(adc1_channel_t channel);

/**
 * @brief Hall  Sensor output value.
 *        @note
 *        The Hall Sensor uses Channel_0 and Channel_3 of ADC1.
 *        So, firstly: please configure ADC1 module by calling adc1_config_width before calling hall_sensor_read.
                 We recommend that the WIDTH ADC1 be configured as 12Bit, because the values of hall_sensor_read are small and almost the same if WIDTH ADC1 is configured as 9Bit, 10Bit or 11Bit.
 *            secondly: when you use the hall sensor, please do not use Channel_0 and Channel_3 of ADC1 as
 *                   ADC channels.
 *
 * @return the value of hall sensor
 */
int hall_sensor_read();

/**
 *----------EXAMPLE TO USE ADC1------------ *
 * @code{c}
 *     adc1_config_width(ADC_WIDTH_12Bit);//config adc1 width
 *     adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_0db);//config channel0 attenuation
 *     int val=adc1_get_voltage(ADC1_CHANNEL_0);//get the  val of channel0
 * @endcode
 **/

/**
 *----------EXAMPLE TO USE HALL SENSOR------------ *
 * @code{c}
 *     adc1_config_width(ADC_WIDTH_12Bit);//config adc1 width
 *     int val=hall_sensor_read();
 * @endcode
 **/

#ifdef __cplusplus
}
#endif

#endif  /*_DRIVER_ADC_H_*/

