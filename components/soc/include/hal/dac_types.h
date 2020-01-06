#pragma once

#include "soc/dac_caps.h"
#include "sdkconfig.h"

typedef enum {
    DAC_CHANNEL_1 = 0,    /*!< DAC channel 1 is GPIO25(ESP32) / GPIO17(ESP32S2BETA) */
    DAC_CHANNEL_2 = 1,    /*!< DAC channel 2 is GPIO26(ESP32) / GPIO18(ESP32S2BETA) */
    DAC_CHANNEL_MAX,
} dac_channel_t;

/**
 * The multiple of the amplitude of the cosine wave generator. The max amplitude is VDD3P3_RTC.
 */
typedef enum {
    DAC_CW_SCALE_1 = 0x0,   /*!< 1/1. Default. */
    DAC_CW_SCALE_2 = 0x1,   /*!< 1/2. */
    DAC_CW_SCALE_4 = 0x2,   /*!< 1/4. */
    DAC_CW_SCALE_8 = 0x3,   /*!< 1/8. */
} dac_cw_scale_t;

/**
 * Set the phase of the cosine wave generator output.
 */
typedef enum {
    DAC_CW_PHASE_0   = 0x2, /*!< Phase shift +0° */
    DAC_CW_PHASE_180 = 0x3, /*!< Phase shift +180° */
} dac_cw_phase_t;

/**
 * Config the cosine wave generator function in DAC module.
 */
typedef struct {
    dac_channel_t en_ch;    /*!< Enable the cosine wave generator of DAC channel. */
    dac_cw_scale_t scale;   /*!< Set the amplitude of the cosine wave generator output. */
    dac_cw_phase_t phase;   /*!< Set the phase of the cosine wave generator output. */
    uint32_t freq;          /*!< Set frequency of cosine wave generator output. Range: 130(130Hz) ~ 55000(100KHz). */
    int8_t offset;          /*!< Set the voltage value of the DC component of the cosine wave generator output. 
                                 Note: Unreasonable settings can cause waveform to be oversaturated. Range: -128 ~ 127. */
} dac_cw_config_t;