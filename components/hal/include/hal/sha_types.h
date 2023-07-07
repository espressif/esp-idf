/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

/* Use enum from rom for backwards compatibility */
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/sha.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/sha.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/sha.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/sha.h"
#elif CONFIG_IDF_TARGET_ESP32C2
#include "esp32c2/rom/sha.h"
#elif CONFIG_IDF_TARGET_ESP32C6
#include "esp32c6/rom/sha.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rom/sha.h"
#elif CONFIG_IDF_TARGET_ESP32P4
#include "esp32p4/rom/sha.h"
#endif

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
