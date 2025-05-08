/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#pragma once

#include "sdkconfig.h"

/* I2S DMA configuration */
#define EXAMPLE_I2S_DMA_DESC_NUM        8
#define EXAMPLE_I2S_DMA_FRAME_NUM       96
#define EXAMPLE_I2S_DMA_TOTAL_BUFFER_SIZE    (EXAMPLE_I2S_DMA_DESC_NUM * (EXAMPLE_I2S_DMA_FRAME_NUM * EXAMPLE_I2S_DMA_DESC_NUM * EXAMPLE_AUDIO_BIT_WIDTH / 8))

/* I2S frequency tuning configuration */
#define EXAMPLE_DYNAMIC_TUNING_EN       CONFIG_EXAMPLE_DYNAMIC_TUNING_EN
#define EXAMPLE_I2S_MAX_DEVIATED_PPM    (200)   /* Maximum deviation compare to initial freuqency, in PPM (Parts Per Million) */
#define EXAMPLE_I2S_MIN_DEVIATED_PPM    (-200)  /* Minimum deviation compare to initial freuqency, in PPM (Parts Per Million) */
#define EXAMPLE_I2S_TUNING_STEP_PPM     (10)    /* Step of frequency deviation, in PPM (Parts Per Million).
                                                 * The tuning might take no effect when the step is too small,
                                                 * especially when the I2S clock source is lower than 100MHz */
#define EXAMPLE_PPM_TO_MCLK(curr_mclk_hz, ppm)      (ppm * (curr_mclk_hz / 1000000))    /* Convert PPM into MCLK ticks */
#define EXAMPLE_I2S_DEFAULT_TUNING_CFG(curr_mclk_hz, is_increase) { \
    .tune_mode = I2S_TUNING_MODE_ADDSUB,  \
    .tune_mclk_val = is_increase ? EXAMPLE_PPM_TO_MCLK(curr_mclk_hz, EXAMPLE_I2S_TUNING_STEP_PPM) : \
                     -EXAMPLE_PPM_TO_MCLK(curr_mclk_hz, EXAMPLE_I2S_TUNING_STEP_PPM),  \
    .max_delta_mclk = EXAMPLE_PPM_TO_MCLK(curr_mclk_hz, EXAMPLE_I2S_MAX_DEVIATED_PPM),  \
    .min_delta_mclk = EXAMPLE_PPM_TO_MCLK(curr_mclk_hz, EXAMPLE_I2S_MIN_DEVIATED_PPM),  \
}

/* I2C port and GPIOs */
#define EXAMPLE_I2C_NUM                 (0)
#define EXAMPLE_I2C_SCL_IO              CONFIG_EXAMPLE_I2C_SCL_IO
#define EXAMPLE_I2C_SDA_IO              CONFIG_EXAMPLE_I2C_SDA_IO

/* I2S port and GPIOs */
#define EXAMPLE_I2S_NUM                 (0)
#define EXAMPLE_I2S_MCK_IO              CONFIG_EXAMPLE_I2S_MCK_IO
#define EXAMPLE_I2S_BCK_IO              CONFIG_EXAMPLE_I2S_BCK_IO
#define EXAMPLE_I2S_WS_IO               CONFIG_EXAMPLE_I2S_WS_IO
#define EXAMPLE_I2S_DO_IO               CONFIG_EXAMPLE_I2S_DO_IO
#define EXAMPLE_I2S_DI_IO               CONFIG_EXAMPLE_I2S_DI_IO

/* Audio configuration */
#define EXAMPLE_AUDIO_SAMPLE_RATE       CONFIG_UAC_SAMPLE_RATE
#define EXAMPLE_AUDIO_BIT_WIDTH         CONFIG_EXAMPLE_AUDIO_BIT_WIDTH

/* ES8311 configuration */
#define EXAMPLE_CODEC_MCLK_MULTIPLE     (EXAMPLE_AUDIO_BIT_WIDTH == 24 ? 384 : 256)
#define EXAMPLE_CODEC_MCLK_FREQ_HZ      (EXAMPLE_CODEC_MCLK_MULTIPLE * EXAMPLE_AUDIO_SAMPLE_RATE)
#define EXAMPLE_CODEC_VOLUME            CONFIG_EXAMPLE_CODEC_VOLUME
#define EXAMPLE_CODEC_MIC_GAIN          CONFIG_EXAMPLE_CODEC_MIC_GAIN

/* PA control GPIO */
#define EXAMPLE_PA_CTRL_IO              CONFIG_EXAMPLE_PA_CTRL_IO
