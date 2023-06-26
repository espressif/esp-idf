/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_IDF_TARGET_ESP32C6
#define TEST_CLK_GPIO       10
#define TEST_DATA0_GPIO     0
#define TEST_DATA1_GPIO     1
#define TEST_DATA2_GPIO     2
#define TEST_DATA3_GPIO     3
#define TEST_DATA4_GPIO     4
#define TEST_DATA5_GPIO     5
#define TEST_DATA6_GPIO     6
#define TEST_DATA7_GPIO     7
#elif CONFIG_IDF_TARGET_ESP32H2
#define TEST_CLK_GPIO       10
#define TEST_DATA0_GPIO     0
#define TEST_DATA1_GPIO     1
#define TEST_DATA2_GPIO     2
#define TEST_DATA3_GPIO     3
#define TEST_DATA4_GPIO     4
#define TEST_DATA5_GPIO     5
#define TEST_DATA6_GPIO     8
#define TEST_DATA7_GPIO     9
#else
#error "Unsupported target"
#endif

#ifdef __cplusplus
}
#endif
