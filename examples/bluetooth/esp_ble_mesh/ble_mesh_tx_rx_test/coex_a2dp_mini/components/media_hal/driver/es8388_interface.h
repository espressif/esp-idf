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


#ifndef __ES8388_INTERFACE_H__
#define __ES8388_INTERFACE_H__

#include "codec_common.h"
#include "esp_types.h"
#include "driver/i2c.h"
#include "media_hal.h"

/* ES8388 address */
#define ES8388_ADDR             0x20  // 0x22:CE=1;0x20:CE=0

/* ES8388 register */
#define ES8388_CONTROL1         0x00
#define ES8388_CONTROL2         0x01

#define ES8388_CHIPPOWER        0x02

#define ES8388_ADCPOWER         0x03
#define ES8388_DACPOWER         0x04

#define ES8388_CHIPLOPOW1       0x05
#define ES8388_CHIPLOPOW2       0x06

#define ES8388_ANAVOLMANAG      0x07

#define ES8388_MASTERMODE       0x08
/* ADC */
#define ES8388_ADCCONTROL1      0x09
#define ES8388_ADCCONTROL2      0x0a
#define ES8388_ADCCONTROL3      0x0b
#define ES8388_ADCCONTROL4      0x0c
#define ES8388_ADCCONTROL5      0x0d
#define ES8388_ADCCONTROL6      0x0e
#define ES8388_ADCCONTROL7      0x0f
#define ES8388_ADCCONTROL8      0x10
#define ES8388_ADCCONTROL9      0x11
#define ES8388_ADCCONTROL10     0x12
#define ES8388_ADCCONTROL11     0x13
#define ES8388_ADCCONTROL12     0x14
#define ES8388_ADCCONTROL13     0x15
#define ES8388_ADCCONTROL14     0x16
/* DAC */
#define ES8388_DACCONTROL1      0x17
#define ES8388_DACCONTROL2      0x18
#define ES8388_DACCONTROL3      0x19
#define ES8388_DACCONTROL4      0x1a
#define ES8388_DACCONTROL5      0x1b
#define ES8388_DACCONTROL6      0x1c
#define ES8388_DACCONTROL7      0x1d
#define ES8388_DACCONTROL8      0x1e
#define ES8388_DACCONTROL9      0x1f
#define ES8388_DACCONTROL10     0x20
#define ES8388_DACCONTROL11     0x21
#define ES8388_DACCONTROL12     0x22
#define ES8388_DACCONTROL13     0x23
#define ES8388_DACCONTROL14     0x24
#define ES8388_DACCONTROL15     0x25
#define ES8388_DACCONTROL16     0x26
#define ES8388_DACCONTROL17     0x27
#define ES8388_DACCONTROL18     0x28
#define ES8388_DACCONTROL19     0x29
#define ES8388_DACCONTROL20     0x2a
#define ES8388_DACCONTROL21     0x2b
#define ES8388_DACCONTROL22     0x2c
#define ES8388_DACCONTROL23     0x2d
#define ES8388_DACCONTROL24     0x2e
#define ES8388_DACCONTROL25     0x2f
#define ES8388_DACCONTROL26     0x30
#define ES8388_DACCONTROL27     0x31
#define ES8388_DACCONTROL28     0x32
#define ES8388_DACCONTROL29     0x33
#define ES8388_DACCONTROL30     0x34

#define CODEC_OPS_ESP8388() (media_hal_codec_ops_t) {     \
    .codec_init = es8388_init,                            \
    .codec_uninit = es8388_deinit,                        \
    .codec_start = es8388_start,                          \
    .codec_stop = es8388_stop,                            \
    .codec_config_fmt = es8388_config_fmt,                \
    .codec_set_bit = es8388_set_bits_per_sample,          \
    .codec_set_adc_input = es8388_config_adc_input,       \
    .codec_set_vol = es8388_set_voice_volume,             \
    .codec_get_vol = es8388_get_voice_volume,             \
    .codec_set_mute = es8388_set_voice_mute,              \
    .codec_get_mute = es8388_get_voice_mute,              \
}

/**
 * @brief Initialise ES8388 codec chip
 *
 * @param cfg       configuration of ES8388 codec chip
 *
 * @return          ESP_OK - Success, Others - Error
 */
esp_err_t es8388_init(const media_hal_config_t *cfg);

/**
 * @brief De-initialise ES8388 codec chip
 *
 * @param cfg       configuration of ES8388 codec chip
 *
 * @return          ESP_OK - Success, Others - Error
 */
void es8388_deinit(const media_hal_config_t *cfg);

/**
 * @brief Configure ES8388 I2S format
 *
 * @param mode:           set ADC or DAC or all
 * @param bitPerSample:   see Es8388I2sFmt
 *
 * @return          ESP_OK - Success, Others - Error
 */
esp_err_t es8388_config_fmt(codec_module_t mode, codec_i2s_fmt_t fmt);

/**
 * @brief Config I2s clock in MSATER mode
 *
 * @param cfg.sclk_div:      generate SCLK by dividing MCLK in MSATER mode
 * @param cfg.lclk_div:      generate LCLK by dividing MCLK in MSATER mode
 *
 * @return          ESP_OK - Success, Others - Error
 */
esp_err_t es8388_i2s_config_clock(codec_i2s_clk_t cfg);

/**
 * @brief Configure ES8388 data sample bits
 *
 * @param mode:           set ADC or DAC or all
 * @param bitPerSample:   see bits_length_t
 *
 * @return          ESP_OK - Success, Others - Error
 */
esp_err_t es8388_set_bits_per_sample(codec_module_t mode, bits_length_t bit_per_sample);

/**
 * @brief Power Management
 *
 * @param mod:      if ES_POWER_CHIP, the whole chip including ADC and DAC is enabled
 * @param enable:   false to disable true to enable
 *
 * @return          ESP_OK - Success, Others - Error
 */
esp_err_t es8388_start(codec_module_t mode);

/**
 * @brief Power Management
 *
 * @param mod:      if ES_POWER_CHIP, the whole chip including ADC and DAC is enabled
 * @param enable:   false to disable true to enable
 *
 * @return          ESP_OK - Success, Others - Error
 */
esp_err_t es8388_stop(codec_module_t mode);

/**
 * @brief    Set voice volume
 *
 * @param    volume: 0 ~ 100
 *
 * @return   ESP_OK - Success, Others - Error
 */
esp_err_t es8388_set_voice_volume(int volume);

/**
 * @brief    Get voice volume
 *
 * @param    point to volume value
 *
 * @return   ESP_OK - Success, Others - Error
 */
esp_err_t es8388_get_voice_volume(int *volume);

/**
 * @brief           Configure ES8388 DAC mute or not.
 *                  Basicly you can use this function to mute the output or don't
 *
 * @param enable:   enable or disable
 *
 * @return          ESP_OK - Success, Others - Error
 */
esp_err_t es8388_set_voice_mute(int enable);

/**
 * @brief           Get ES8388 DAC mute status.
 *
 * @param mute:     point to mute value to indicate it mute or not
 *
 * @return          ESP_OK - Success, Others - Error
 */
esp_err_t es8388_get_voice_mute(int *mute);

/**
 * @brief       Set MIC gain
 * @param gain: The gain value. Reference to mic_gain_t
 *
 * @return          ESP_OK - Success, Others - Error
 */
esp_err_t es8388_set_mic_gain(mic_gain_t gain);

/**
 * @brief           Config ADC Input
 * 
 * @param input:    input capability
 *
 * @return          ESP_OK - Success, Others - Error
 */
int es8388_config_adc_input(adc_input_t input);

/**
 * @brief           Config DAC Output
 *
 * @param output:   output capability, Ex: (DAC_OUTPUT_LOUT1 | DAC_OUTPUT_LOUT2 | DAC_OUTPUT_ROUT1 | DAC_OUTPUT_ROUT2)
 * 
 * @return          ESP_OK - Success, Others - Error
 */
esp_err_t es8388_config_dac_output(dac_output_t output);

#endif //__ES8388_INTERFACE_H__
