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

#if CONFIG_PARLIO_ISR_IRAM_SAFE
#define TEST_PARLIO_CALLBACK_ATTR   IRAM_ATTR
#define TEST_PARLIO_MEM_ALLOC_CAPS  (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define TEST_PARLIO_CALLBACK_ATTR
#define TEST_PARLIO_MEM_ALLOC_CAPS  MALLOC_CAP_DEFAULT
#endif

#if CONFIG_IDF_TARGET_ESP32C6
#define TEST_CLK_GPIO       10
#define TEST_VALID_GPIO     11
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
#define TEST_VALID_GPIO     11
#define TEST_DATA0_GPIO     0
#define TEST_DATA1_GPIO     1
#define TEST_DATA2_GPIO     2
#define TEST_DATA3_GPIO     3
#define TEST_DATA4_GPIO     4
#define TEST_DATA5_GPIO     5
#define TEST_DATA6_GPIO     8
#define TEST_DATA7_GPIO     9
#elif CONFIG_IDF_TARGET_ESP32P4
#define TEST_CLK_GPIO       20
#define TEST_DATA0_GPIO     21
#define TEST_DATA1_GPIO     22
#define TEST_DATA2_GPIO     34
#define TEST_DATA3_GPIO     35
#define TEST_DATA4_GPIO     48
#define TEST_DATA5_GPIO     49
#define TEST_DATA6_GPIO     10
#define TEST_DATA7_GPIO     11
#else
#error "Unsupported target"
#endif

#ifdef __cplusplus
}
#endif
