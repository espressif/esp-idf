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


#ifndef _MEDIA_HAL_H_
#define _MEDIA_HAL_H_

#include "driver/i2s.h"
#include "driver/i2c.h"
#include "codec_common.h"

/// Codec mode type
typedef enum codec_mode {
    CODEC_MODE_UNKNOWN,
    CODEC_MODE_ENCODE,
    CODEC_MODE_DECODE,
    CODEC_MODE_LINE_IN,
    CODEC_MODE_DECODE_ENCODE,
    CODEC_MODE_MAX,
} codec_mode_t;

/// Codec mute type
typedef enum codec_mute {
    CODEC_MUTE_DISABLE,
    CODEC_MUTE_ENABLE,
} codec_mute_t;

/// Media HAL state
typedef enum media_hal_state {
    MEDIA_HAL_STATE_UNKNOWN,
    MEDIA_HAL_STATE_INIT,
    MEDIA_HAL_STATE_MAX,
} media_hal_state_t;

/// Media Hal configurations
typedef struct {
    codec_role_t role;
    i2s_port_t i2s_port;
    i2s_pin_config_t i2s_pin;
    i2s_config_t i2s_cfg;
    i2c_port_t i2c_port;
    i2c_config_t i2c_cfg;
    dac_output_t dac_output;
    adc_input_t adc_input;
} media_hal_config_t;

/// Media Hal operation functions of CODEC chip
typedef struct media_hal_codec_ops {
    int (*codec_init)(const media_hal_config_t *cfg);
    void (*codec_uninit)(const media_hal_config_t *cfg);
    int (*codec_start)(int mode);
    int (*codec_stop)(int mode);
    int (*codec_config_fmt)(int mode, int fmt);
    int (*codec_set_bit)(int mode, int bit_per_sample);
    int (*codec_set_adc_input)(int input);
    int (*codec_set_vol)(int volume);
    int (*codec_get_vol)(int *volume);
    int (*codec_set_mute)(int en);
    int (*codec_get_mute)(int *mute);
} media_hal_codec_ops_t;

/**
 * @brief  Initialize media codec driver.
 *
 * @praram codec_ops : operation functions of codec
 *
 * @return ESP_OK - success, others - fail
 */
esp_err_t media_hal_init(const media_hal_codec_ops_t *codec_ops);

/**
 * @brief  De-initialize media codec driver.
 *
 * @praram void
 */
void media_hal_deinit(void);

/**
 * @brief         Start codec driver.
 *
 * @param  mode : Refer to enum codec_mode_t.
 *
 * @return        ESP_OK - success, others - fail
 */
esp_err_t media_hal_start(codec_mode_t mode);

/**
 * @brief         Stop codec driver.
 *
 * @param  mode : Refer to enum codec_mode_t.
 *
 * @return        ESP_OK - success, others - fail
 */
esp_err_t media_hal_stop(codec_mode_t mode);

/**
 * @brief         Get the codec working mode.
 *
 * @param  mode : Current working mode will be return.
 *
 * @return        ESP_OK - success, others - fail
 */
esp_err_t media_hal_get_current_mode(codec_mode_t *mode);

/**
 * @brief          Set voice volume.
 *
 * @param  volume: Value will be setup.
 *
 * @return         ESP_OK - success, others - fail
 */
esp_err_t media_hal_set_volume(int volume);

/**
 * @brief          Get voice volume.
 *
 * @param  volume: Current volume will be return.
 *
 * @return         ESP_OK - success, others - fail
 */
esp_err_t media_hal_get_volume(int *volume);

/**
 * @brief         Set the volume amplifier. This function is called when external ADC can not change volume or
 *                when users want to amplify the max volume.
 *
 * @param  scale: scale the voice volume, such as 0.5, or 1.3
 *
 * @return         ESP_OK - success, others - fail
 */
esp_err_t media_hal_set_volume_amplify(float scale);

/**
 * @brief         Get the volume amplifier
 *
 * @return        int, a fixed point (1<<8) value
 */
int media_hal_get_volume_amplify();

/**
 * @brief         Set codec driver mute status.
 *
 * @param  mute:  1--Enable mute; 0-- Disable mute.
 *
 * @return        ESP_OK - success, others - fail
 */
esp_err_t media_hal_set_mute(codec_mute_t mute);

/**
 * @brief      Get codec driver mute status.
 *
 * @param      void
 *
 * @return     int, 0 - Unmute, 1 - Mute, <0 - fail
 */
int media_hal_get_mute(void);

/**
 * @brief                Set codec Bits.
 *
 * @param  bitPerSample: see structure bits_length_t
 *
 * @return               ESP_OK - success, others - fail
 */
esp_err_t media_hal_set_bits(int bit_per_sample);

/**
 * @brief          Set clock & bit width used for I2S RX and TX.
 *
 * @param rate:    I2S sample rate (ex: 8000, 44100...)
 * @param bits:    I2S bit width (16, 24, 32)
 * @param ch:      I2S channel  (I2S_CHANNEL_MONO, I2S_CHANNEL_STEREO)
 *
 * @return         ESP_OK - success, others - fail
 */
esp_err_t media_hal_set_clk(uint32_t rate, uint8_t bits, uint32_t ch);

/**
 * @brief          Get i2s configuration.
 *
 * @param info:    i2s_config_t information.
 *
 * @return         ESP_OK - success, others - fail
 */
esp_err_t media_hal_get_i2s_config(void *info);

/**
 * @brief          Write audio data to I2S.
 *
 * @param data:    point to audio data.
 * @param data_len:audio data length as input
 *
 * @return         the length which is actually written to i2s, <0 means error.
 */
int media_hal_write_data(const uint8_t *data, int data_len);

/**
 * @brief          Write audio data all zero to I2S.
 *
 * @return         ESP_OK - success, others - fail
 */
esp_err_t media_hal_clear_data(void);

/**
 * @brief  Get i2s number
 *
 * @param  None
 *
 * @return int, i2s number
 */
int media_hal_get_i2s_num(void);

/**
 * @brief  Get i2s bits
 *
 * @param  None
 *
 * @return int, i2s bits
 */
int media_hal_get_i2s_bits(void);

/**
 * @brief    Get source-music bits for re-bits feature, but only for 16bits to 32bits
 *
 * @param    None
 *
 * @return   0 =- disabled,  16 - enabled
 */
int media_hal_get_src_bits(void);

/**
 * @brief   Get whether i2s is in DAC mode
 *
 * @param   None
 *
 * @return  0 - is DAC mode, 1 - is DAC mode
 */
int media_hal_get_i2s_dac_mode(void);

/**
 * @brief   Get whether i2s is in ADC mode
 *
 * @param   None
 *
 * @return  0 - is not in ADC mode, 1 - is ADC mode
 */
int media_hal_get_i2s_adc_mode(void);

/**
 * @brief      Power on or not for PA.
 *
 * @param  en: 0 - disable, other - enable; 
 *
 * @return     ESP_OK - success, others - fail
 */
esp_err_t media_hal_pa_pwr(int en);

/**
 * @brief         Get MediaHal state.
 *
 * @param  state: media_hal_state_t enum
 *
 * @return        ESP_OK - success, others - fail
 */
esp_err_t media_hal_get_state(media_hal_state_t *state);

#endif  //__MEDIA_HAL_H__
