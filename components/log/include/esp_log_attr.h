/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "esp_attr.h"
#include "esp_private/log_attr.h"
#include "esp_log_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if ESP_LOG_MODE_BINARY_EN
#define ESP_LOG_ATTR_STR(str)           (__builtin_constant_p(str) ? ESP_LOG_NOLOAD_STR(str) : str)
#define ESP_LOG_ATTR_DRAM_STR(str)      ESP_LOG_NOLOAD_STR(str)
#else
#define ESP_LOG_ATTR_STR(str)           (str)
#define ESP_LOG_ATTR_DRAM_STR(str)      DRAM_STR(str)
#endif

#ifdef __cplusplus
}
#endif
