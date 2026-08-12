/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_idf_version.h"

#ifdef __cplusplus
extern "C" {
#endif

/* NOTE: Keep in sync with components/esp_tee/version.cmake (checked in CI) */

/** Major version number (X.x.x) */
#define ESP_TEE_VERSION_MAJOR   1
/** Minor version number (x.X.x) */
#define ESP_TEE_VERSION_MINOR   2
/** Patch version number (x.x.X) */
#define ESP_TEE_VERSION_PATCH   0

/**
 * Macro to convert the TEE version number into an integer
 *
 * To be used in comparisons, such as ESP_TEE_VERSION >= ESP_TEE_VERSION_VAL(1, 0, 0)
 */
#define ESP_TEE_VERSION_VAL(major, minor, patch) (((major) << 16) | ((minor) << 8) | (patch))

/**
 * Current TEE version, as an integer
 *
 * To be used in comparisons, such as ESP_TEE_VERSION >= ESP_TEE_VERSION_VAL(1, 0, 0)
 */
#define ESP_TEE_VERSION  ESP_TEE_VERSION_VAL(ESP_TEE_VERSION_MAJOR, \
                                             ESP_TEE_VERSION_MINOR, \
                                             ESP_TEE_VERSION_PATCH)

/** @cond */
#define __ESP_TEE_STR(x)   #x
#define __ESP_TEE_XSTR(x)  __ESP_TEE_STR(x)
/** @endcond */

/**
 * Current TEE version string, as stored in the TEE app descriptor
 *
 * Format: `<major>.<minor>.<patch>~v<IDF major>.<IDF minor>.<IDF patch>` (e.g., "1.2.0~v6.2.0"), where the
 * numeric part identifies the TEE source and the suffix the ESP-IDF release it was built with
 */
#define ESP_TEE_VERSION_STR  __ESP_TEE_XSTR(ESP_TEE_VERSION_MAJOR) "." \
                             __ESP_TEE_XSTR(ESP_TEE_VERSION_MINOR) "." \
                             __ESP_TEE_XSTR(ESP_TEE_VERSION_PATCH) "~v" \
                             __ESP_TEE_XSTR(ESP_IDF_VERSION_MAJOR) "." \
                             __ESP_TEE_XSTR(ESP_IDF_VERSION_MINOR) "." \
                             __ESP_TEE_XSTR(ESP_IDF_VERSION_PATCH)

#ifdef __cplusplus
}
#endif
