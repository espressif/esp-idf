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
    I2S_BITS_PER_SAMPLE_8BIT    = 8,
    I2S_BITS_PER_SAMPLE_16BIT   = 16,
    I2S_BITS_PER_SAMPLE_24BIT   = 24,
    I2S_BITS_PER_SAMPLE_32BIT   = 32,
} i2s_bits_per_sample_t;

/**
 * @brief I2S bit width per slot.
 *
 */
typedef enum {
    I2S_BITS_PER_SLOT_8BIT           = (8),      /*!< slot bit 8*/
    I2S_BITS_PER_SLOT_16BIT          = (16),     /*!< slot bit 16*/
    I2S_BITS_PER_SLOT_24BIT          = (24),     /*!< slot bit 24*/
    I2S_BITS_PER_SLOT_32BIT          = (32),     /*!< slot bit 32*/
    I2S_BITS_PER_SLOT_SAMPLE_BIT_EQU = (0),      /*!< slot bit equals to data bit*/
} i2s_bits_per_slot_t;

#define SLOT_BIT_SHIFT   (16)  //slot bit shift
#define SLOT_CH_SHIFT    (16)  //slot channel shift

/**
 * @brief I2S slot bit configuration paramater. The low 16bit is the audio_data_bit_width and the high 16bit is the slot_bit_width.
 *       e.g.: If set to (I2S_BITS_PER_SLOT_24BIT << SLOT_BIT_SHIFT) | I2S_BITS_PER_SAMPLE_16BIT, the audio data bit is 16bit and the slot bit is 24bit.
 *
 *
 * @note: If the slot_bit_width is set to `I2S_BITS_PER_SLOT_SAMPLE_BIT_EQU`, then the slot_bit_width equals to audio_data_bit_width.
 *
 */
typedef uint32_t i2s_slot_bits_cfg_t;      /*!< slot bit configuration*/

/**
 * @brief I2S slot channel configuration paramater. The low 16bit is the active_slot_number and the high 16bit is the total_slot_num.
 *        The audio data only launch in active slot, otherwise, launch 0 or single data in inactive slot.
 *        e.g.: If set to (4 << SLOT_CH_SHIFT) | 2, the active_slot_number is 2 and the total_slot_num 4.
 *
 * @note: If the total_slot_num is set to 0, then the total_slot_num equals to active_slot_number.
 *
 */
typedef uint32_t i2s_slot_channel_cfg_t;   /*!< slot channel configuration*/

/**
 * @brief I2S channel.
 *
 */
typedef enum {
    I2S_CHANNEL_MONO        = 1,            /*!< I2S 1 channel (mono)*/
    I2S_CHANNEL_STEREO      = 2             /*!< I2S 2 channel (stereo)*/
} i2s_channel_t;


#if SOC_I2S_SUPPORTS_TDM
/**
 * @brief Bit map of active slot.
 *        For TX module, only the active slot send the audio data, the inactive slot send a constant(configurable).
 *        For RX module, only receive the audio data in active slot, the data in inactive slot will be ignored.
 *
 * @note the bit map of active slot can not exceed (0x1<<total_slot_num).
 *        e.g: active_slot_mask = (I2S_TDM_ACTIVE_CH0 | I2S_TDM_ACTIVE_CH1), total_slot_num = 4, active_slot_number = 2.
 */
typedef enum {
    I2S_TDM_ACTIVE_CH0  = (0x1 << 0),
    I2S_TDM_ACTIVE_CH1  = (0x1 << 1),
    I2S_TDM_ACTIVE_CH2  = (0x1 << 2),
    I2S_TDM_ACTIVE_CH3  = (0x1 << 3),
    I2S_TDM_ACTIVE_CH4  = (0x1 << 4),
    I2S_TDM_ACTIVE_CH5  = (0x1 << 5),
    I2S_TDM_ACTIVE_CH6  = (0x1 << 6),
    I2S_TDM_ACTIVE_CH7  = (0x1 << 7),
    I2S_TDM_ACTIVE_CH8  = (0x1 << 8),
    I2S_TDM_ACTIVE_CH9  = (0x1 << 9),
    I2S_TDM_ACTIVE_CH10 = (0x1 << 10),
    I2S_TDM_ACTIVE_CH11 = (0x1 << 11),
    I2S_TDM_ACTIVE_CH12 = (0x1 << 12),
    I2S_TDM_ACTIVE_CH13 = (0x1 << 13),
    I2S_TDM_ACTIVE_CH14 = (0x1 << 14),
    I2S_TDM_ACTIVE_CH15 = (0x1 << 15),
} i2s_tdm_active_slot_t;
#endif

/**
 * @brief I2S communication standard format
 *
 */
typedef enum {
    I2S_COMM_FORMAT_STAND_I2S   = 0X01, /*!< I2S communication I2S Philips standard, data launch at second BCK*/
    I2S_COMM_FORMAT_STAND_MSB   = 0X02, /*!< I2S communication MSB alignment standard, data launch at first BCK*/
    I2S_COMM_FORMAT_STAND_PCM_SHORT  = 0x04, /*!< PCM Short standard, also known as DSP mode. The period of synchronization signal (WS) is 1 bck cycle.*/
    I2S_COMM_FORMAT_STAND_PCM_LONG   = 0x0C, /*!< PCM Long standard. The period of synchronization signal (WS) is channel_bit*bck cycles.*/
    I2S_COMM_FORMAT_STAND_MAX, /*!< standard max*/
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
 * @brief I2S Mode
 */
typedef enum {
    I2S_MODE_MASTER = 1,              /*!< Master mode*/
    I2S_MODE_SLAVE = 2,               /*!< Slave mode*/
    I2S_MODE_TX = 4,                  /*!< TX mode*/
    I2S_MODE_RX = 8,                  /*!< RX mode*/
#if SOC_I2S_SUPPORTS_ADC_DAC
    //built-in DAC functions are only supported on I2S0 for ESP32 chip.
    I2S_MODE_DAC_BUILT_IN = 16,       /*!< Output I2S data to built-in DAC, no matter the data format is 16bit or 32 bit, the DAC module will only take the 8bits from MSB*/
    I2S_MODE_ADC_BUILT_IN = 32,       /*!< Input I2S data from built-in ADC, each data can be 12-bit width at most*/
#endif
#if SOC_I2S_SUPPORTS_PDM
    //PDM functions are only supported on I2S0.
    I2S_MODE_PDM = 64,               /*!< I2S PDM mode*/
#endif
} i2s_mode_t;

/**
 * @brief I2S source clock
 */
typedef enum {
    I2S_CLK_D2CLK = 0,               /*!< Clock from PLL_D2_CLK(160M)*/
    I2S_CLK_APLL,                    /*!< Clock from APLL*/
} i2s_clock_src_t;

/**
 * @brief I2S bit width per sample.
 *
 * @note: The chip of ESP32 and ESP32S2 only needs to initialize the fields out side the `SOC_I2S_SUPPORTS_TDM` macro.
 *        The chip of ESP32-S3, ESP32-C3 and the later chip, all this fields should be initialized.
 */
typedef struct {
    i2s_mode_t              mode;                   /*!< I2S work mode, using ored mask of `i2s_mode_t`*/
    uint32_t                sample_rate;            /*!< I2S sample rate*/
    i2s_slot_bits_cfg_t     slot_bits_cfg;          /*!< slot bit configuration, low 16bit is the audio data bit; high 16bit is the slot bit, if set to 0, total slot bit equals to audio data bit*/
    i2s_channel_fmt_t       channel_format;         /*!< I2S channel format*/
    i2s_comm_format_t       communication_format;   /*!< I2S communication format */
#if SOC_I2S_SUPPORTS_TDM
    i2s_slot_channel_cfg_t  slot_channel_cfg;       /*!< slot number configuration, low 16bit is the valid slot number; high 16bit is the total slot number, if set to 0, total slot number equals to valid slot number*/
    uint32_t                active_slot_mask;       /*!< active slot bit mask, using the ored mask of `i2s_tdm_active_slot_t`*/
    bool                    left_align_en;          /*!< Set to enable left aligment*/
    bool                    big_edin_en;            /*!< Set to enable big edin*/
    bool                    bit_order_msb_en;       /*!< Set to enable msb order*/
#endif
} i2s_config_param_t;

#if SOC_I2S_SUPPORTS_ADC_DAC
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
#endif //SOC_I2S_SUPPORTS_ADC_DAC

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

#if SOC_I2S_SUPPORTS_PCM
/**
 * @brief A/U-law decompress or compress configuration.
 *
 */
typedef enum {
    I2S_PCM_A_DECOMPRESS=0,   /*!< A-law decompress*/
    I2S_PCM_A_COMPRESS,       /*!< A-law compress*/
    I2S_PCM_U_DECOMPRESS,     /*!< U-law decompress*/
    I2S_PCM_U_COMPRESS,       /*!< U-law compress*/
    I2S_PCM_DISABLE,          /*!< Disable A/U law decopress or compress*/
} i2s_pcm_cfg_t;
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

#if SOC_I2S_SUPPORTS_PDM
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
