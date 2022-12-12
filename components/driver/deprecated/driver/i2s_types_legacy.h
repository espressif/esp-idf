/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * This file is for the backward compatible to the deprecated I2S APIs,
 * The deprecated APIs will no longer supported in the future
 * Please refer to "hal/i2s_types.h" for the latest I2S driver types
 * Note that only one set of I2S APIs is allowed to be used at the same time
 */
#pragma once

#include "hal/i2s_types.h"
#include "driver/i2s_types.h"

#ifdef __cplusplus
extern "C" {
#endif
/******************** Deprecated Types **********************/
#define I2S_PIN_NO_CHANGE (-1) /*!< Used in i2s_pin_config_t for pins which should not be changed */

/**
 * @brief I2S bit width per sample.
 */
typedef enum {
    I2S_BITS_PER_SAMPLE_8BIT    = 8,            /*!< data bit-width: 8 */
    I2S_BITS_PER_SAMPLE_16BIT   = 16,           /*!< data bit-width: 16 */
    I2S_BITS_PER_SAMPLE_24BIT   = 24,           /*!< data bit-width: 24 */
    I2S_BITS_PER_SAMPLE_32BIT   = 32,           /*!< data bit-width: 32 */
} i2s_bits_per_sample_t;

/**
 * @brief I2S bit width per chan.
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
 */
typedef enum {
    I2S_CHANNEL_MONO        = 1,  /*!< I2S channel (mono), one channel activated. In this mode, you only need to send one channel data but the fifo will copy same data for the other unactivated channels automatically, then both channels will transmit same data. */
    I2S_CHANNEL_STEREO      = 2,  /*!< I2S channel (stereo), two (or more) channels activated. In this mode, these channels will transmit different data. */
#if SOC_I2S_SUPPORTS_TDM
    // Bit map of activated chan.
    // There are 16 channels in TDM mode.
    // For TX module, only the activated channel send the audio data, the unactivated channel send a constant(configurable) or will be skiped if 'skip_msk' is set.
    // For RX module, only receive the audio data in activated channels, the data in unactivated channels will be ignored.
    // the bit map of activated channel can not exceed the maximum enabled channel number (i.e. 0x10000 << total_chan_num).
    // e.g: active_chan_mask = (I2S_TDM_ACTIVE_CH0 | I2S_TDM_ACTIVE_CH3), here the active_chan_number is 2 and total_chan_num is not supposed to be smaller than 4.
    I2S_TDM_ACTIVE_CH0  = (0x1 << 16),               /*!< I2S channel 0 activated */
    I2S_TDM_ACTIVE_CH1  = (0x1 << 17),               /*!< I2S channel 1 activated */
    I2S_TDM_ACTIVE_CH2  = (0x1 << 18),               /*!< I2S channel 2 activated */
    I2S_TDM_ACTIVE_CH3  = (0x1 << 19),               /*!< I2S channel 3 activated */
    I2S_TDM_ACTIVE_CH4  = (0x1 << 20),               /*!< I2S channel 4 activated */
    I2S_TDM_ACTIVE_CH5  = (0x1 << 21),               /*!< I2S channel 5 activated */
    I2S_TDM_ACTIVE_CH6  = (0x1 << 22),               /*!< I2S channel 6 activated */
    I2S_TDM_ACTIVE_CH7  = (0x1 << 23),               /*!< I2S channel 7 activated */
    I2S_TDM_ACTIVE_CH8  = (0x1 << 24),               /*!< I2S channel 8 activated */
    I2S_TDM_ACTIVE_CH9  = (0x1 << 25),               /*!< I2S channel 9 activated */
    I2S_TDM_ACTIVE_CH10 = (0x1 << 26),              /*!< I2S channel 10 activated */
    I2S_TDM_ACTIVE_CH11 = (0x1 << 27),              /*!< I2S channel 11 activated */
    I2S_TDM_ACTIVE_CH12 = (0x1 << 28),              /*!< I2S channel 12 activated */
    I2S_TDM_ACTIVE_CH13 = (0x1 << 29),              /*!< I2S channel 13 activated */
    I2S_TDM_ACTIVE_CH14 = (0x1 << 30),              /*!< I2S channel 14 activated */
    I2S_TDM_ACTIVE_CH15 = (0x1 << 31),              /*!< I2S channel 15 activated */
#endif
} i2s_channel_t;

/**
 * @brief I2S communication standard format
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

#if SOC_I2S_SUPPORTS_DAC
/**
 * @brief I2S DAC mode for i2s_set_dac_mode.
 *
 * @note Built-in DAC functions are only supported on I2S0 for current ESP32 chip.
 */
typedef enum {
    I2S_DAC_CHANNEL_DISABLE  = 0,    /*!< Disable I2S built-in DAC signals*/
    I2S_DAC_CHANNEL_RIGHT_EN = 1,    /*!< Enable I2S built-in DAC right channel, maps to DAC channel 0 on GPIO25*/
    I2S_DAC_CHANNEL_LEFT_EN  = 2,    /*!< Enable I2S built-in DAC left  channel, maps to DAC channel 1 on GPIO26*/
    I2S_DAC_CHANNEL_BOTH_EN  = 0x3,  /*!< Enable both of the I2S built-in DAC channels.*/
    I2S_DAC_CHANNEL_MAX      = 0x4,  /*!< I2S built-in DAC mode max index*/
} i2s_dac_mode_t;
#endif //SOC_I2S_SUPPORTS_DAC

/**
 * @brief I2S event queue types
 */
typedef enum {
    I2S_EVENT_DMA_ERROR,   /*!< I2S DMA has no next descriptor for sending or receiving */
    I2S_EVENT_TX_DONE,     /*!< I2S DMA finished sending one DMA buffer */
    I2S_EVENT_RX_DONE,     /*!< I2S DMA finished receiving one DMA buffer */
    I2S_EVENT_TX_Q_OVF,    /*!< I2S DMA sending queue overflowed, the oldest data has been overwritten by the new data in the DMA buffer */
    I2S_EVENT_RX_Q_OVF,    /*!< I2S DMA receive queue overflowed, the oldest data has been overwritten by the new data in the DMA buffer  */
} i2s_event_type_t;

/**
 * @brief Event structure used in I2S event queue
 */
typedef struct {
    i2s_event_type_t    type;   /*!< I2S event type */
    size_t              size;   /*!< I2S data size for I2S_DATA event*/
} i2s_event_t;

/**
 * @brief I2S GPIO pins configuration
 */
typedef struct {
    int mck_io_num;         /*!< MCK pin, output */
    int bck_io_num;         /*!< BCK pin, input in slave role, output in master role */
    int ws_io_num;          /*!< WS pin, input in slave role, output in master role */
    int data_out_num;       /*!< DATA pin, output */
    int data_in_num;        /*!< DATA pin, input */
} i2s_pin_config_t;

#if SOC_I2S_SUPPORTS_PCM
/**
 * @brief I2S PCM configuration
 *
 */
typedef struct {
    i2s_pcm_compress_t  pcm_type;       /*!< I2S PCM a/u-law decompress or compress type */
} i2s_pcm_cfg_t;
#endif

#if SOC_I2S_SUPPORTS_PDM_TX
/**
 * @brief Default I2S PDM Up-Sampling Rate configuration
 */
#define I2S_PDM_DEFAULT_UPSAMPLE_CONFIG(rate) { \
        .sample_rate = rate,                    \
        .fp = 960,                              \
        .fs = (rate) / 100,                     \
    }

/**
 * @brief I2S PDM up-sample rate configuration
 * @note  TX PDM can only be set to the following two up-sampling rate configurations:
 *        1: fp = 960, fs = sample_rate / 100, in this case, Fpdm = 128*48000
 *        2: fp = 960, fs = 480, in this case, Fpdm = 128*Fpcm = 128*sample_rate
 *        If the pdm receiver do not care the pdm serial clock, it's recommended set Fpdm = 128*48000.
 *        Otherwise, the second configuration should be applied.
 */
typedef struct  {
    int sample_rate;                    /*!< I2S PDM sample rate */
    int fp;                             /*!< I2S PDM TX up-sampling parameter. Normally it should be set to 960 */
    int fs;                             /*!< I2S PDM TX up-sampling parameter. When it is set to 480, the pdm clock frequency Fpdm = 128 * sample_rate, when it is set to sample_rate / 100， Fpdm will be fixed to 128*48000 */
} i2s_pdm_tx_upsample_cfg_t;
#endif


/**
 * @brief I2S driver configuration parameters
 *
 */
typedef struct {

    i2s_mode_t              mode;                       /*!< I2S work mode */
    uint32_t                sample_rate;                /*!< I2S sample rate */
    i2s_bits_per_sample_t   bits_per_sample;            /*!< I2S sample bits in one channel */
    i2s_channel_fmt_t       channel_format;             /*!< I2S channel format.*/
    i2s_comm_format_t       communication_format;       /*!< I2S communication format */
    int                     intr_alloc_flags;           /*!< Flags used to allocate the interrupt. One or multiple (ORred) ESP_INTR_FLAG_* values. See esp_intr_alloc.h for more info */
    union {
        int dma_desc_num;                               /*!< The total number of descriptors used by I2S DMA to receive/transmit data */
        int dma_buf_count __attribute__((deprecated));  /*!< This is an alias to 'dma_desc_num' for backward compatibility */
    };
    union {
        int dma_frame_num;                              /*!< Frame number for one-time sampling. Frame here means the total data from all the channels in a WS cycle */
        int dma_buf_len __attribute__((deprecated));    /*!< This is an alias to 'dma_frame_num' for backward compatibility */
    };
    bool                    use_apll;                   /*!< I2S using APLL as main I2S clock, enable it to get accurate clock */
    bool                    tx_desc_auto_clear;         /*!< I2S auto clear tx descriptor if there is underflow condition (helps in avoiding noise in case of data unavailability) */
    int                     fixed_mclk;                 /*!< I2S using fixed MCLK output. If use_apll = true and fixed_mclk > 0, then the clock output for i2s is fixed and equal to the fixed_mclk value. If fixed_mclk set, mclk_multiple won't take effect */
    i2s_mclk_multiple_t     mclk_multiple;              /*!< The multiple of I2S master clock(MCLK) to sample rate */
    i2s_bits_per_chan_t     bits_per_chan;              /*!< I2S total bits in one channel， only take effect when larger than 'bits_per_sample', default '0' means equal to 'bits_per_sample' */

#if SOC_I2S_SUPPORTS_TDM
    i2s_channel_t           chan_mask;                  /*!< I2S active channel bit mask, set value in `i2s_channel_t` to enable specific channel, the bit map of active channel can not exceed (0x1<<total_chan). */
    uint32_t                total_chan;                 /*!< I2S Total number of channels. If it is smaller than the biggest active channel number, it will be set to this number automatically. */
    bool                    left_align;                 /*!< Set to enable left alignment */
    bool                    big_edin;                   /*!< Set to enable big endian */
    bool                    bit_order_msb;              /*!< Set to enable msb order */
    bool                    skip_msk;                   /*!< Set to enable skip mask. If it is enabled, only the data of the enabled channels will be sent, otherwise all data stored in DMA TX buffer will be sent */
#endif // SOC_I2S_SUPPORTS_TDM

} i2s_driver_config_t;

typedef i2s_driver_config_t i2s_config_t;       // for backward compatible

#ifdef __cplusplus
}
#endif
