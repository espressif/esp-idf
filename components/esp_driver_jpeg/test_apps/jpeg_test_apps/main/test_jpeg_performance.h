/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

// Decoder performance lists
#define IDF_PERFORMANCE_MIN_JPEG_DECODE_1080P_2_RGB565_PERFORMANCE                 40 // 40 fps for 1080p decoder.

// Encoder performance lists
#define IDF_PERFORMANCE_MIN_JPEG_ENCODE_RGB888_2_480P_PERFORMANCE                  160 // 160 fps for 480p encoder.

#ifdef __cplusplus
}
#endif
