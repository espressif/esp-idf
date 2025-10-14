/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#pragma once

#include "sdkconfig.h"

/* Example configurations */
#define EXAMPLE_RECV_BUF_SIZE   (2400)
#define EXAMPLE_SAMPLE_RATE     (16000)
#define EXAMPLE_MCLK_MULTIPLE   (384) // If not using 24-bit data width, 256 should be enough
#define EXAMPLE_MCLK_FREQ_HZ    (EXAMPLE_SAMPLE_RATE * EXAMPLE_MCLK_MULTIPLE)
#define EXAMPLE_VOICE_VOLUME    CONFIG_EXAMPLE_VOICE_VOLUME
#define EXAMPLE_PA_CTRL_IO      CONFIG_EXAMPLE_PA_CTRL_IO
#if CONFIG_EXAMPLE_MODE_ECHO
#define EXAMPLE_MIC_GAIN        CONFIG_EXAMPLE_MIC_GAIN
#endif

/* I2C port and GPIOs */
#define I2C_NUM         (0)
#define I2C_SCL_IO      CONFIG_EXAMPLE_I2C_SCL_IO
#define I2C_SDA_IO      CONFIG_EXAMPLE_I2C_SDA_IO

/* I2S port and GPIOs */
#define I2S_NUM         (0)
#define I2S_MCK_IO      CONFIG_EXAMPLE_I2S_MCLK_IO
#define I2S_BCK_IO      CONFIG_EXAMPLE_I2S_BCLK_IO
#define I2S_WS_IO       CONFIG_EXAMPLE_I2S_WS_IO
#define I2S_DO_IO       CONFIG_EXAMPLE_I2S_DOUT_IO
#define I2S_DI_IO       CONFIG_EXAMPLE_I2S_DIN_IO
