/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_IDF_TARGET_ESP32H4_BETA_VERSION_1
#define GPIO_MATRIX_CONST_ONE_INPUT   (0x38)
#define GPIO_MATRIX_CONST_ZERO_INPUT  (0x3C)
#elif CONFIG_IDF_TARGET_ESP32H4_BETA_VERSION_2
#define GPIO_MATRIX_CONST_ONE_INPUT   (0x1E)
#define GPIO_MATRIX_CONST_ZERO_INPUT  (0x1F)
#endif

#ifdef __cplusplus
}
#endif
