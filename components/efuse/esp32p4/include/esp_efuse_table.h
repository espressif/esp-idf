/*
 * SPDX-FileCopyrightText: 2017-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "sdkconfig.h"

#if !CONFIG_ESP32P4_SELECTS_REV_LESS_V3
#include "esp_efuse_table_v3.0.h"
#else
#include "esp_efuse_table_v0.0_v2.0.h"
#endif

#ifdef __cplusplus
}
#endif
