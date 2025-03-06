/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

/* Use enum from rom for backwards compatibility */
#include "rom/sha.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Use enum from rom for backwards compatibility */
#if CONFIG_IDF_TARGET_ESP32
typedef enum SHA_TYPE esp_sha_type;
#else
typedef SHA_TYPE esp_sha_type;
#endif

#ifdef __cplusplus
}
#endif
