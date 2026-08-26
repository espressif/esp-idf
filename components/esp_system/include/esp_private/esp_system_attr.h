/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_attr.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_ESP_SYSTEM_IN_IRAM
#define ESP_SYSTEM_IRAM_ATTR IRAM_ATTR
#else
#define ESP_SYSTEM_IRAM_ATTR
#endif

#ifdef __cplusplus
}
#endif
