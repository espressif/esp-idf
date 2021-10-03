// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
#include <stdbool.h>
#include <stddef.h>
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief I2S bit width per sample.
 *
 */
typedef enum {
    I2S_BITS_PER_SAMPLE_8BIT    = 8,            /*!< data bit-width: 8 */
    I2S_BITS_PER_SAMPLE_16BIT   = 16,           /*!< data bit-width: 16 */
    I2S_BITS_PER_SAMPLE_24BIT   = 24,           /*!< data bit-width: 24 */
    I2S_BITS_PER_SAMPLE_32BIT   = 32,           /*!< data bit-width: 32 */
} i2s_bits_per_sample_t;

/**
 * @brief I2S bit width per chan.
 *
 */
typedef enum {
    I2S_BITS_PER_CHAN_DEFAULT        = (0),      /*!< channel bit-width equals to data bit-width */
    I2S_BITS_PER_CHAN_8BIT           = (8),      /*!< channel bit-width: 8 */
    I2S_BITS_PER_CHAN_16BIT          = (16),     /*!< channel bit-width: 16 */
    I2S_BITS_PER_CHAN_24BIT          = (24),     /*!< channel bit-width: 24 */
    I2S_BITS_PER_CHAN_32BIT          = (32),     /*!< channel bit-width: 32 */
} i2s_bits_per_chan_t;

/**
 * @brief I2S channel.
 *
 */
typedef enum {
    I2S_CHANNEL_MONO        = (0x01 << 31) | 0x03,  /*!< I2S channel (mono), two channel enabled. In this mode, you only need to send one channel data but the fifo will copy same data for another channel automatically, then both channels will transmit same data. The highest bit is for differentiating I2S_CHANNEL_STEREO since they both use two channels */
    I2S_CHANNEL_STEREO      = 0x03,                 /*!< I2S channel (stereo), two channel enabled. In this mode, two channels will transmit different data. */
#if SOC_I2S_SUPPORTS_TDM
    // Bit map of active chan.
    // There are 16 channels in TDM mode.
    // For TX module, only the active channel send the audio data, the inactive channel send a constant(configurable) or will be skiped if 'skip_msk' is set.
    // For RX module, only receive the audio data in active channels, the data in inactive channels will be ignored.
    // the bit map of active channel can not exceed (0x1<<total_chan_num).
    // e.g: active_chan_mask = (I2S_TDM_ACTIVE_CH0 | I2S_TDM_ACTIVE_CH3), here the active_chan_number is 2 and total_chan_num is not supposed to be smaller than 4.
    I2S_TDM_ACTIVE_CH0  = (0x1 << 0),               /*!< I2S channel 0 enabled */
    I2S_TDM_ACTIVE_CH1  = (0x1 << 1),               /*!< I2S channel 1 enabled */
    I2S_TDM_ACTIVE_CH2  = (0x1 << 2),               /*!< I2S channel 2 enabled */
    I2S_TDM_ACTIVE_CH3  = (0x1 << 3),               /*!< I2S channel 3 enabled */
    I2S_TDM_ACTIVE_CH4  = (0x1 << 4),               /*!< I2S channel 4 enabled */
    I2S_TDM_ACTIVE_CH5  = (0x1 << 5),               /*!< I2S channel 5 enabled */
    I2S_TDM_ACTIVE_CH6  = (0x1 << 6),               /*!< I2S channel 6 enabled */
    I2S_TDM_ACTIVE_CH7  = (0x1 << 7),               /*!< I2S channel 7 enabled */
    I2S_TDM_ACTIVE_CH8  = (0x1 << 8),               /*!< I2S channel 8 enabled */
    I2S_TDM_ACTIVE_CH9  = (0x1 << 9),               /*!< I2S channel 9 enabled */
    I2S_TDM_ACTIVE_CH10 = (0x1 << 10),              /*!< I2S channel 10 enabled */
    I2S_TDM_ACTIVE_CH11 = (0x1 << 11),              /*!< I2S channel 11 enabled */
    I2S_TDM_ACTIVE_CH12 = (0x1 << 12),              /*!< I2S channel 12 enabled */
    I2S_TDM_ACTIVE_CH13 = (0x1 << 13),              /*!< I2S channel 13 enabled */
    I2S_TDM_ACTIVE_CH14 = (0x1 << 14),              /*!< I2S channel 14 enabled */
    I2S_TDM_ACTIVE_CH15 = (0x1 << 15),              /*!< I2S channel 15 enabled */
#endif
} i2s_channel_t;

/**
 * @brief I2S communication standard format
 *
 */
typedef enum {
    I2S_COMM_FORMAT_STAND_I2S        = 0X01, /*!< I2S communication I2S Philips standard, data launch at second BCK*/
    I2S_COMM_FORMAT_STAND_MSB        = 0X02, /*!< I2S communication MSB alignment standard, data launch at first BCK*/
    I2S_COMM_FORMAT_STAND_PCM_SHORT  = 0x04, /*!< PCM Short standard, also known as DSP mode. The period of synchronization signal (WS) is 1 bck cycle.*/
    I2S_COMM_FORMAT_STAND_PCM_LONG   = 0x0C, /*!< PCM Long standard. The period of synchronization signal (WS) is channel_bit*bck cycles.*/
    I2S_COMM_FORMAT_STAND_MAX,               /*!< standard max*/

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
    I2S_CHANNEL_FMT_RIGHT_LEFT,         /*!< Separated left and right channel */
    I2S_CHANNEL_FMT_ALL_RIGHT,          /*!< Load right channel data in both two channels */
    I2S_CHANNEL_FMT_ALL_LEFT,           /*!< Load left channel data in both two channels */
    I2S_CHANNEL_FMT_ONLY_RIGHT,         /*!< Only load data in right channel (mono mode) */
    I2S_CHANNEL_FMT_ONLY_LEFT,          /*!< Only load data in left channel (mono mode) */
#if SOC_I2S_SUPPORTS_TDM
    // Multiple channels are available with TDM feature
    I2S_CHANNEL_FMT_MULTIPLE,           /*!< More than two channels are used */
#endif
}  i2s_channel_fmt_t;

/**
 * @brief I2S Mode
 */
typedef enum {
    I2S_MODE_MASTER       = (0x1 << 0),       /*!< Master mode*/
    I2S_MODE_SLAVE        = (0x1 << 1),       /*!< Slave mode*/
    I2S_MODE_TX           = (0x1 << 2),       /*!< TX mode*/
    I2S_MODE_RX           = (0x1 << 3),       /*!< RX mode*/
#if SOC_I2S_SUPPORTS_DAC
    //built-in DAC functions are only supported on I2S0 for ESP32 chip.
    I2S_MODE_DAC_BUILT_IN = (0x1 << 4),       /*!< Output I2S data to built-in DAC, no matter the data format is 16bit or 32 bit, the DAC module will only take the 8bits from MSB*/
#endif // SOC_I2S_SUPPORTS_DAC
#if SOC_I2S_SUPPORTS_ADC
    I2S_MODE_ADC_BUILT_IN = (0x1 << 5),       /*!< Input I2S data from built-in ADC, each data can be 12-bit width at most*/
#endif // SOC_I2S_SUPPORTS_ADC
    // PDM functions are only supported on I2S0 (all chips).
    I2S_MODE_PDM          = (0x1 << 6),       /*!< I2S PDM mode*/
} i2s_mode_t;

/**
 * @brief I2S source clock
 */
typedef enum {
    I2S_CLK_D2CLK = 0,               /*!< Clock from PLL_D2_CLK(160M)*/
#if SOC_I2S_SUPPORTS_APLL
    I2S_CLK_APLL,                    /*!< Clock from APLL*/
#endif
} i2s_clock_src_t;

/**
 * @brief The multiple of mclk to sample rate
 */
typedef enum {
    I2S_MCLK_MULTIPLE_DEFAULT   = 0,       /*!< Default value. mclk = sample_rate * 256 */
    I2S_MCLK_MULTIPLE_128       = 128,     /*!< mclk = sample_rate * 128 */
    I2S_MCLK_MULTIPLE_256       = 256,     /*!< mclk = sample_rate * 256 */
    I2S_MCLK_MULTIPLE_384       = 384,     /*!< mclk = sample_rate * 384 */
} i2s_mclk_multiple_t;

#if SOC_I2S_SUPPORTS_DAC
/**
 * @brief I2S DAC mode for i2s_set_dac_mode.
 *
 * @note Built-in DAC functions are only supported on I2S0 for current ESP32 chip.
 */
typedef enum {
    I2S_DAC_CHANNEL_DISABLE  = 0,    /*!< Disable I2S built-in DAC signals*/
    I2S_DAC_CHANNEL_RIGHT_EN = 1,    /*!< Enable I2S built-in DAC right channel, maps to DAC channel 1 on GPIO25*/
    I2S_DAC_CHANNEL_LEFT_EN  = 2,    /*!< Enable I2S built-in DAC left  channel, maps to DAC channel 2 on GPIO26*/
    I2S_DAC_CHANNEL_BOTH_EN  = 0x3,  /*!< Enable both of the I2S built-in DAC channels.*/
    I2S_DAC_CHANNEL_MAX      = 0x4,  /*!< I2S built-in DAC mode max index*/
} i2s_dac_mode_t;
#endif //SOC_I2S_SUPPORTS_DAC

#if SOC_I2S_SUPPORTS_PCM
/**
 * @brief A/U-law decompress or compress configuration.
 *
 */
typedef enum {
    I2S_PCM_DISABLE = 0,      /*!< Disable A/U law decopress or compress*/
    I2S_PCM_A_DECOMPRESS,     /*!< A-law decompress*/
    I2S_PCM_A_COMPRESS,       /*!< A-law compress*/
    I2S_PCM_U_DECOMPRESS,     /*!< U-law decompress*/
    I2S_PCM_U_COMPRESS,       /*!< U-law compress*/
} i2s_pcm_compress_t;
#endif

#if SOC_I2S_SUPPORTS_PDM_RX
/**
 * @brief I2S PDM RX downsample mode
 */
typedef enum {
    I2S_PDM_DSR_8S = 0,  /*!< downsampling number is 8 for PDM RX mode*/
    I2S_PDM_DSR_16S,     /*!< downsampling number is 16 for PDM RX mode*/
    I2S_PDM_DSR_MAX,
} i2s_pdm_dsr_t;
#endif

#if SOC_I2S_SUPPORTS_PDM_TX
typedef enum {
    I2S_PDM_SIG_SCALING_DIV_2 = 0,   /*!< I2S TX PDM sigmadelta signal scaling: /2 */
    I2S_PDM_SIG_SCALING_MUL_1 = 1,   /*!< I2S TX PDM sigmadelta signal scaling: x1 */
    I2S_PDM_SIG_SCALING_MUL_2 = 2,   /*!< I2S TX PDM sigmadelta signal scaling: x2 */
    I2S_PDM_SIG_SCALING_MUL_4 = 3,   /*!< I2S TX PDM sigmadelta signal scaling: x4 */
} i2s_pdm_sig_scale_t;
#endif

#ifdef __cplusplus
}
#endif
