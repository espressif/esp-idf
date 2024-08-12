/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
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
#if CONFIG_EXAMPLE_MODE_ECHO
#define EXAMPLE_MIC_GAIN        CONFIG_EXAMPLE_MIC_GAIN
#endif

#if !defined(CONFIG_EXAMPLE_BSP)

/* I2C port and GPIOs */
#define I2C_NUM         (0)
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
#define I2C_SCL_IO      (GPIO_NUM_16)
#define I2C_SDA_IO      (GPIO_NUM_17)
#elif CONFIG_IDF_TARGET_ESP32H2
#define I2C_SCL_IO      (GPIO_NUM_8)
#define I2C_SDA_IO      (GPIO_NUM_9)
#elif CONFIG_IDF_TARGET_ESP32P4
#define I2C_SCL_IO      (GPIO_NUM_8)
#define I2C_SDA_IO      (GPIO_NUM_7)
#else
#define I2C_SCL_IO      (GPIO_NUM_6)
#define I2C_SDA_IO      (GPIO_NUM_7)
#endif

/* I2S port and GPIOs */
#define I2S_NUM         (0)
#if CONFIG_IDF_TARGET_ESP32P4
#define I2S_MCK_IO      (GPIO_NUM_13)
#define I2S_BCK_IO      (GPIO_NUM_12)
#define I2S_WS_IO       (GPIO_NUM_10)
#define I2S_DO_IO       (GPIO_NUM_9)
#define I2S_DI_IO       (GPIO_NUM_11)
#else
#define I2S_MCK_IO      (GPIO_NUM_0)
#define I2S_BCK_IO      (GPIO_NUM_4)
#define I2S_WS_IO       (GPIO_NUM_5)
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
#define I2S_DO_IO       (GPIO_NUM_18)
#define I2S_DI_IO       (GPIO_NUM_19)
#else
#define I2S_DO_IO       (GPIO_NUM_2)
#define I2S_DI_IO       (GPIO_NUM_3)
#endif
#endif

#else // CONFIG_EXAMPLE_BSP
#include "bsp/esp-bsp.h"
#define I2C_NUM BSP_I2C_NUM

#endif // CONFIG_EXAMPLE_BSP
