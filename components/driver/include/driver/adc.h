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
#include <stdbool.h>
#include "esp_err.h"
#include "driver/gpio.h"
#include "soc/adc_channel.h"

typedef enum {
    ADC_ATTEN_DB_0   = 0,  /*!<The input voltage of ADC will be reduced to about 1/1 */
    ADC_ATTEN_DB_2_5 = 1,  /*!<The input voltage of ADC will be reduced to about 1/1.34 */
    ADC_ATTEN_DB_6   = 2,  /*!<The input voltage of ADC will be reduced to about 1/2 */
    ADC_ATTEN_DB_11  = 3,  /*!<The input voltage of ADC will be reduced to about 1/3.6*/
    ADC_ATTEN_MAX,
} adc_atten_t;

typedef enum {
    ADC_WIDTH_BIT_9  = 0, /*!< ADC capture width is 9Bit*/
    ADC_WIDTH_BIT_10 = 1, /*!< ADC capture width is 10Bit*/
    ADC_WIDTH_BIT_11 = 2, /*!< ADC capture width is 11Bit*/
    ADC_WIDTH_BIT_12 = 3, /*!< ADC capture width is 12Bit*/
    ADC_WIDTH_MAX,
} adc_bits_width_t;

//this definitions are only for being back-compatible
#define ADC_ATTEN_0db   ADC_ATTEN_DB_0
#define ADC_ATTEN_2_5db ADC_ATTEN_DB_2_5
#define ADC_ATTEN_6db   ADC_ATTEN_DB_6
#define ADC_ATTEN_11db  ADC_ATTEN_DB_11
//this definitions are only for being back-compatible
#define ADC_WIDTH_9Bit  ADC_WIDTH_BIT_9
#define ADC_WIDTH_10Bit ADC_WIDTH_BIT_10
#define ADC_WIDTH_11Bit ADC_WIDTH_BIT_11
#define ADC_WIDTH_12Bit ADC_WIDTH_BIT_12

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

typedef enum {
    ADC2_CHANNEL_0 = 0, /*!< ADC2 channel 0 is GPIO4 */
    ADC2_CHANNEL_1,     /*!< ADC2 channel 1 is GPIO0 */
    ADC2_CHANNEL_2,     /*!< ADC2 channel 2 is GPIO2 */
    ADC2_CHANNEL_3,     /*!< ADC2 channel 3 is GPIO15 */
    ADC2_CHANNEL_4,     /*!< ADC2 channel 4 is GPIO13 */
    ADC2_CHANNEL_5,     /*!< ADC2 channel 5 is GPIO12 */
    ADC2_CHANNEL_6,     /*!< ADC2 channel 6 is GPIO14 */
    ADC2_CHANNEL_7,     /*!< ADC2 channel 7 is GPIO27 */
    ADC2_CHANNEL_8,     /*!< ADC2 channel 8 is GPIO25 */
    ADC2_CHANNEL_9,     /*!< ADC2 channel 9 is GPIO26 */
    ADC2_CHANNEL_MAX,
} adc2_channel_t;

typedef enum {
    ADC_CHANNEL_0 = 0, /*!< ADC channel */
    ADC_CHANNEL_1,     /*!< ADC channel */
    ADC_CHANNEL_2,     /*!< ADC channel */
    ADC_CHANNEL_3,     /*!< ADC channel */
    ADC_CHANNEL_4,     /*!< ADC channel */
    ADC_CHANNEL_5,     /*!< ADC channel */
    ADC_CHANNEL_6,     /*!< ADC channel */
    ADC_CHANNEL_7,     /*!< ADC channel */
    ADC_CHANNEL_8,     /*!< ADC channel */
    ADC_CHANNEL_9,     /*!< ADC channel */
    ADC_CHANNEL_MAX,
} adc_channel_t;

typedef enum {
    ADC_UNIT_1 = 1,          /*!< SAR ADC 1*/
    ADC_UNIT_2 = 2,          /*!< SAR ADC 2, not supported yet*/
    ADC_UNIT_BOTH = 3,       /*!< SAR ADC 1 and 2, not supported yet */
    ADC_UNIT_ALTER = 7,      /*!< SAR ADC 1 and 2 alternative mode, not supported yet */
    ADC_UNIT_MAX,
} adc_unit_t;

typedef enum {
    ADC_ENCODE_12BIT,        /*!< ADC to I2S data format, [15:12]-channel [11:0]-12 bits ADC data */
    ADC_ENCODE_11BIT,        /*!< ADC to I2S data format, [15]-1 [14:11]-channel [10:0]-11 bits ADC data */
    ADC_ENCODE_MAX,
} adc_i2s_encode_t;

typedef enum {
    ADC_I2S_DATA_SRC_IO_SIG = 0, /*!< I2S data from GPIO matrix signal  */
    ADC_I2S_DATA_SRC_ADC = 1,    /*!< I2S data from ADC */
    ADC_I2S_DATA_SRC_MAX,
} adc_i2s_source_t;

/**
 * @brief Get the gpio number of a specific ADC1 channel.
 * 
 * @param channel Channel to get the gpio number
 * 
 * @param gpio_num output buffer to hold the gpio number
 * 
 * @return 
 *   - ESP_OK if success
 *   - ESP_ERR_INVALID_ARG if channal not valid 
 */
esp_err_t adc1_pad_get_io_num(adc1_channel_t channel, gpio_num_t *gpio_num);

/**
 * @brief Configure ADC1 capture width, meanwhile enable output invert for ADC1.
 * The configuration is for all channels of ADC1
 * @param width_bit Bit capture width for ADC1
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t adc1_config_width(adc_bits_width_t width_bit);

/**
 * @brief Configure ADC capture width.
 * @param adc_unit ADC unit index
 * @param width_bit Bit capture width for ADC unit.
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t adc_set_data_width(adc_unit_t adc_unit, adc_bits_width_t width_bit);

/**
 * @brief Configure the ADC1 channel, including setting attenuation.
 *
 * @note This function also configures the input GPIO pin mux to
 * connect it to the ADC1 channel. It must be called before calling
 * adc1_get_raw() for this channel.
 *
 * The default ADC full-scale voltage is 1.1V. To read higher voltages (up to the pin maximum voltage,
 * usually 3.3V) requires setting >0dB signal attenuation for that ADC channel.
 *
 * When VDD_A is 3.3V:
 *
 * - 0dB attenuaton (ADC_ATTEN_DB_0) gives full-scale voltage 1.1V
 * - 2.5dB attenuation (ADC_ATTEN_DB_2_5) gives full-scale voltage 1.5V
 * - 6dB attenuation (ADC_ATTEN_DB_6) gives full-scale voltage 2.2V
 * - 11dB attenuation (ADC_ATTEN_DB_11) gives full-scale voltage 3.9V (see note below)
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
int adc1_get_raw(adc1_channel_t channel);

/** @cond */    //Doxygen command to hide deprecated function from API Reference
/*
 * @deprecated This function returns an ADC1 reading but is deprecated due to
 * a misleading name and has been changed to directly call the new function.
 * Use the new function adc1_get_raw() instead
 */
int adc1_get_voltage(adc1_channel_t channel) __attribute__((deprecated));
/** @endcond */

/**
 * @brief Enable ADC power
 */
void adc_power_on();

/**
 * @brief Power off SAR ADC
 * This function will force power down for ADC
 */
void adc_power_off();

/**
 * @brief Initialize ADC pad
 * @param adc_unit ADC unit index
 * @param channel ADC channel index
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t adc_gpio_init(adc_unit_t adc_unit, adc_channel_t channel);

/**
 * @brief Set ADC data invert
 * @param adc_unit ADC unit index
 * @param inv_en whether enable data invert
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t adc_set_data_inv(adc_unit_t adc_unit, bool inv_en);

/**
 * @brief Set ADC source clock
 * @param clk_div ADC clock divider, ADC clock is divided from APB clock
 * @return
 *     - ESP_OK success
 */
esp_err_t adc_set_clk_div(uint8_t clk_div);

/**
 * @brief Set I2S data source
 * @param src I2S DMA data source, I2S DMA can get data from digital signals or from ADC.
 * @return
 *     - ESP_OK success
 */
esp_err_t adc_set_i2s_data_source(adc_i2s_source_t src);

/**
 * @brief Initialize I2S ADC mode
 * @param adc_unit ADC unit index
 * @param channel ADC channel index
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t adc_i2s_mode_init(adc_unit_t adc_unit, adc_channel_t channel);

/**
 * @brief Configure ADC1 to be usable by the ULP
 *
 * This function reconfigures ADC1 to be controlled by the ULP.
 * Effect of this function can be reverted using adc1_get_raw function.
 *
 * Note that adc1_config_channel_atten, adc1_config_width functions need
 * to be called to configure ADC1 channels, before ADC1 is used by the ULP.
 */
void adc1_ulp_enable();

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

/**
 * @brief Get the gpio number of a specific ADC2 channel.
 * 
 * @param channel Channel to get the gpio number
 * 
 * @param gpio_num output buffer to hold the gpio number
 * 
 * @return 
 *   - ESP_OK if success
 *   - ESP_ERR_INVALID_ARG if channal not valid 
 */
esp_err_t adc2_pad_get_io_num(adc2_channel_t channel, gpio_num_t *gpio_num);

/**
 * @brief Configure the ADC2 channel, including setting attenuation.
 *
 * @note This function also configures the input GPIO pin mux to
 * connect it to the ADC2 channel. It must be called before calling
 * ``adc2_get_raw()`` for this channel.
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
 * @note The full-scale voltage is the voltage corresponding to a maximum reading 
 * (depending on ADC2 configured bit width, this value is: 4095 for 12-bits, 2047 
 * for 11-bits, 1023 for 10-bits, 511 for 9 bits.)
 *
 * @note At 11dB attenuation the maximum voltage is limited by VDD_A, not the full scale voltage.
 *
 * @param channel ADC2 channel to configure
 * @param atten  Attenuation level
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t adc2_config_channel_atten(adc2_channel_t channel, adc_atten_t atten);

/**
 * @brief Take an ADC2 reading on a single channel
 *
 * @note For a given channel, ``adc2_config_channel_atten()``
 * must be called before the first time this function is called. If Wi-Fi is started via ``esp_wifi_start()``, this
 * function will always fail with ``ESP_ERR_TIMEOUT``.
 *
 * @param  channel ADC2 channel to read
 * 
 * @param width_bit Bit capture width for ADC2
 * 
 * @param raw_out the variable to hold the output data.
 *
 * @return
 *     - ESP_OK if success
 *     - ESP_ERR_TIMEOUT the WIFI is started, using the ADC2
 */
esp_err_t adc2_get_raw(adc2_channel_t channel, adc_bits_width_t width_bit, int* raw_out);

/**
 *  @brief Output ADC2 reference voltage to gpio 25 or 26 or 27
 *
 *  This function utilizes the testing mux exclusive to ADC 2 to route the
 *  reference voltage one of ADC2's channels. Supported gpios are gpios
 *  25, 26, and 27. This refernce voltage can be manually read from the pin
 *  and used in the esp_adc_cal component.
 *
 *  @param[in]  gpio    GPIO number (gpios 25,26,27 supported)
 *
 *  @return
 *                  - ESP_OK: v_ref successfully routed to selected gpio
 *                  - ESP_ERR_INVALID_ARG: Unsupported gpio
 */
esp_err_t adc2_vref_to_gpio(gpio_num_t gpio);

#ifdef __cplusplus
}
#endif

#endif  /*_DRIVER_ADC_H_*/

