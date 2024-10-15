/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SAMPLE_RATE     (48000)
#define SAMPLE_BITS     (16)

#if CONFIG_IDF_TARGET_ESP32
#define MASTER_MCK_IO 0
#define MASTER_BCK_IO 4
#define MASTER_WS_IO 5
#define SLAVE_BCK_IO 21
#define SLAVE_WS_IO 22
#define DATA_IN_IO 19
#define DATA_OUT_IO 18
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3
#define MASTER_MCK_IO 0
#define MASTER_BCK_IO 4
#define MASTER_WS_IO 5
#define SLAVE_BCK_IO 14
#define SLAVE_WS_IO 15
#define DATA_IN_IO 19
#define DATA_OUT_IO 18
#elif CONFIG_IDF_TARGET_ESP32P4
#define MASTER_MCK_IO 51
#define MASTER_BCK_IO 45
#define MASTER_WS_IO 46
#define SLAVE_BCK_IO 49
#define SLAVE_WS_IO 50
#define DATA_IN_IO 47
#define DATA_OUT_IO 48
#elif CONFIG_IDF_TARGET_ESP32C5
#define MASTER_MCK_IO 6
#define MASTER_BCK_IO 8
#define MASTER_WS_IO 9
#define SLAVE_BCK_IO 23
#define SLAVE_WS_IO 24
#define DATA_IN_IO 10
#define DATA_OUT_IO 25
#else
#define MASTER_MCK_IO 0
#define MASTER_BCK_IO 4
#define MASTER_WS_IO 5
#define SLAVE_BCK_IO 2
#define SLAVE_WS_IO 3
#define DATA_IN_IO 6
#define DATA_OUT_IO 7
#endif

#define I2S_TEST_MASTER_DEFAULT_PIN { \
        .mclk = MASTER_MCK_IO,  \
        .bclk = MASTER_BCK_IO,  \
        .ws = MASTER_WS_IO,     \
        .dout = DATA_OUT_IO,    \
        .din = DATA_IN_IO,      \
        .invert_flags = {       \
            .mclk_inv = false,  \
            .bclk_inv = false,  \
            .ws_inv = false,    \
        },                      \
    }

#define I2S_TEST_SLAVE_DEFAULT_PIN { \
        .mclk = -1,             \
        .bclk = SLAVE_BCK_IO,   \
        .ws = SLAVE_WS_IO,      \
        .dout = DATA_OUT_IO,    \
        .din = DATA_IN_IO,      \
        .invert_flags = {       \
            .mclk_inv = false,  \
            .bclk_inv = false,  \
            .ws_inv = false,    \
        },                      \
    }

#ifdef __cplusplus
}
#endif
