// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief I2S port number, the max port number is (I2S_NUM_MAX -1).
 */
typedef enum {
    I2S_NUM_0 = 0,                 /*!< I2S port 0 */
#if SOC_I2S_NUM > 1
    I2S_NUM_1 = 1,                 /*!< I2S port 1 */
#endif
    I2S_NUM_MAX,                   /*!< I2S port max */
} i2s_port_t;

/**
 * @brief I2S bit width per sample.
 *
 */
typedef enum {
    I2S_BITS_PER_SAMPLE_8BIT    = 8,        /*!< I2S bits per sample: 8-bits*/
    I2S_BITS_PER_SAMPLE_16BIT   = 16,       /*!< I2S bits per sample: 16-bits*/
    I2S_BITS_PER_SAMPLE_24BIT   = 24,       /*!< I2S bits per sample: 24-bits*/
    I2S_BITS_PER_SAMPLE_32BIT   = 32,       /*!< I2S bits per sample: 32-bits*/
} i2s_bits_per_sample_t;

/**
 * @brief I2S channel.
 *
 */
typedef enum {
    I2S_CHANNEL_MONO        = 1,            /*!< I2S 1 channel (mono)*/
    I2S_CHANNEL_STEREO      = 2             /*!< I2S 2 channel (stereo)*/
} i2s_channel_t;

/**
 * @brief I2S communication standard format
 *
 */
typedef enum {
    // In order to keep compatibility, remain the old definitions and introduce new definitions,
    I2S_COMM_FORMAT_STAND_I2S   = 0X01, /*!< I2S communication I2S Philips standard, data launch at second BCK*/
    I2S_COMM_FORMAT_STAND_MSB   = 0X03, /*!< I2S communication MSB alignment standard, data launch at first BCK*/
    I2S_COMM_FORMAT_STAND_PCM_SHORT  = 0x04, /*!< PCM Short standard*/
    I2S_COMM_FORMAT_STAND_PCM_LONG   = 0x0C, /*!< PCM Long standard*/
    I2S_COMM_FORMAT_STAND_MAX, /*!< standard max*/

    //old definition will be removed in the future.
    I2S_COMM_FORMAT_I2S       __attribute__((deprecated)) = 0x01, /*!< I2S communication format I2S, correspond to `I2S_COMM_FORMAT_STAND_I2S`*/
    I2S_COMM_FORMAT_I2S_MSB   __attribute__((deprecated)) = 0x01, /*!< I2S format MSB, (I2S_COMM_FORMAT_I2S |I2S_COMM_FORMAT_I2S_MSB) correspond to `I2S_COMM_FORMAT_STAND_I2S`*/
    I2S_COMM_FORMAT_I2S_LSB   __attribute__((deprecated)) = 0x02, /*!< I2S format LSB, (I2S_COMM_FORMAT_I2S |I2S_COMM_FORMAT_I2S_LSB) correspond to `I2S_COMM_FORMAT_STAND_MSB`*/
    I2S_COMM_FORMAT_PCM       __attribute__((deprecated)) = 0x04, /*!< I2S communication format PCM, correspond to `I2S_COMM_FORMAT_STAND_PCM_SHORT`*/
    I2S_COMM_FORMAT_PCM_SHORT __attribute__((deprecated)) = 0x04, /*!< PCM Short, (I2S_COMM_FORMAT_PCM | I2S_COMM_FORMAT_PCM_SHORT) correspond to `I2S_COMM_FORMAT_STAND_PCM_SHORT`*/
    I2S_COMM_FORMAT_PCM_LONG  __attribute__((deprecated)) = 0x08, /*!< PCM Long, (I2S_COMM_FORMAT_PCM | I2S_COMM_FORMAT_PCM_LONG) correspond to `I2S_COMM_FORMAT_STAND_PCM_LONG`*/
} i2s_comm_format_t;

/**
 * @brief I2S channel format type
 */
typedef enum {
    I2S_CHANNEL_FMT_RIGHT_LEFT = 0x00,
    I2S_CHANNEL_FMT_ALL_RIGHT,
    I2S_CHANNEL_FMT_ALL_LEFT,
    I2S_CHANNEL_FMT_ONLY_RIGHT,
    I2S_CHANNEL_FMT_ONLY_LEFT,
} i2s_channel_fmt_t;

/**
 * @brief I2S Mode, defaut is I2S_MODE_MASTER | I2S_MODE_TX
 *
 * @note PDM and built-in DAC functions are only supported on I2S0 for current ESP32 chip.
 *
 */
typedef enum {
    I2S_MODE_MASTER = 1,              /*!< Master mode*/
    I2S_MODE_SLAVE = 2,               /*!< Slave mode*/
    I2S_MODE_TX = 4,                  /*!< TX mode*/
    I2S_MODE_RX = 8,                  /*!< RX mode*/
#if SOC_I2S_SUPPORTS_ADC_DAC
    I2S_MODE_DAC_BUILT_IN = 16,       /*!< Output I2S data to built-in DAC, no matter the data format is 16bit or 32 bit, the DAC module will only take the 8bits from MSB*/
    I2S_MODE_ADC_BUILT_IN = 32,       /*!< Input I2S data from built-in ADC, each data can be 12-bit width at most*/
#endif
#if SOC_I2S_SUPPORTS_PDM
    I2S_MODE_PDM = 64,                /*!< PDM mode*/
#endif
} i2s_mode_t;

/**
 * @brief I2S source clock
 *
 */
typedef enum {
    I2S_CLK_D2CLK = 0,               /*!< Clock from PLL_D2_CLK(160M)*/
    I2S_CLK_APLL,                    /*!< Clock from APLL*/
} i2s_clock_src_t;


/**
 * @brief I2S configuration parameters for i2s_param_config function
 *
 */
typedef struct {
    i2s_mode_t              mode;                   /*!< I2S work mode*/
    int                     sample_rate;            /*!< I2S sample rate*/
    i2s_bits_per_sample_t   bits_per_sample;        /*!< I2S bits per sample*/
    i2s_channel_fmt_t       channel_format;         /*!< I2S channel format */
    i2s_comm_format_t       communication_format;   /*!< I2S communication format */
    int                     intr_alloc_flags;       /*!< Flags used to allocate the interrupt. One or multiple (ORred) ESP_INTR_FLAG_* values. See esp_intr_alloc.h for more info */
    int                     dma_buf_count;          /*!< I2S DMA Buffer Count */
    int                     dma_buf_len;            /*!< I2S DMA Buffer Length */
    bool                    use_apll;              /*!< I2S using APLL as main I2S clock, enable it to get accurate clock */
    bool                    tx_desc_auto_clear;     /*!< I2S auto clear tx descriptor if there is underflow condition (helps in avoiding noise in case of data unavailability) */
    int                     fixed_mclk;             /*!< I2S using fixed MCLK output. If use_apll = true and fixed_mclk > 0, then the clock output for i2s is fixed and equal to the fixed_mclk value.*/
} i2s_config_t;

/**
 * @brief I2S event types
 *
 */
typedef enum {
    I2S_EVENT_DMA_ERROR,
    I2S_EVENT_TX_DONE,     /*!< I2S DMA finish sent 1 buffer*/
    I2S_EVENT_RX_DONE,     /*!< I2S DMA finish received 1 buffer*/
    I2S_EVENT_MAX,         /*!< I2S event max index*/
} i2s_event_type_t;

/**
 * @brief I2S DAC mode for i2s_set_dac_mode.
 *
 * @note PDM and built-in DAC functions are only supported on I2S0 for current ESP32 chip.
 */
typedef enum {
    I2S_DAC_CHANNEL_DISABLE  = 0,    /*!< Disable I2S built-in DAC signals*/
    I2S_DAC_CHANNEL_RIGHT_EN = 1,    /*!< Enable I2S built-in DAC right channel, maps to DAC channel 1 on GPIO25*/
    I2S_DAC_CHANNEL_LEFT_EN  = 2,    /*!< Enable I2S built-in DAC left  channel, maps to DAC channel 2 on GPIO26*/
    I2S_DAC_CHANNEL_BOTH_EN  = 0x3,  /*!< Enable both of the I2S built-in DAC channels.*/
    I2S_DAC_CHANNEL_MAX      = 0x4,  /*!< I2S built-in DAC mode max index*/
} i2s_dac_mode_t;

/**
 * @brief Event structure used in I2S event queue
 *
 */
typedef struct {
    i2s_event_type_t    type;   /*!< I2S event type */
    size_t              size;   /*!< I2S data size for I2S_DATA event*/
} i2s_event_t;

/**
 * @brief I2S pin number for i2s_set_pin
 *
 */
typedef struct {
    int bck_io_num;     /*!< BCK in out pin*/
    int ws_io_num;      /*!< WS in out pin*/
    int data_out_num;   /*!< DATA out pin*/
    int data_in_num;    /*!< DATA in pin*/
} i2s_pin_config_t;

#if SOC_I2S_SUPPORTS_PDM
/**
 * @brief I2S PDM RX downsample mode
 */
typedef enum {
    I2S_PDM_DSR_8S = 0,  /*!< downsampling number is 8 for PDM RX mode*/
    I2S_PDM_DSR_16S,     /*!< downsampling number is 16 for PDM RX mode*/
    I2S_PDM_DSR_MAX,
} i2s_pdm_dsr_t;

/**
 * @brief PDM PCM convter enable/disable.
 *
 */
typedef enum {
    PDM_PCM_CONV_ENABLE,     /*!< Enable PDM PCM convert*/
    PDM_PCM_CONV_DISABLE,    /*!< Disable PDM PCM convert*/
} pdm_pcm_conv_t;
#endif


#ifdef __cplusplus
}
#endif