/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/** Major version number (X.x.x) */
#define ESP_IDF_VERSION_MAJOR   5
/** Minor version number (x.X.x) */
#define ESP_IDF_VERSION_MINOR   4
/** Patch version number (x.x.X) */
#define ESP_IDF_VERSION_PATCH   1

/**
 * Macro to convert IDF version number into an integer
 *
 * To be used in comparisons, such as ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)
 */
#define ESP_IDF_VERSION_VAL(major, minor, patch) ((major << 16) | (minor << 8) | (patch))

/**
 * Current IDF version, as an integer
 *
 * To be used in comparisons, such as ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)
 */
#define ESP_IDF_VERSION  ESP_IDF_VERSION_VAL(ESP_IDF_VERSION_MAJOR, \
                                             ESP_IDF_VERSION_MINOR, \
                                             ESP_IDF_VERSION_PATCH)

#ifndef __ASSEMBLER__

/**
 * Return full IDF version string, same as 'git describe' output.
 *
 * @note If you are printing the ESP-IDF version in a log file or other information,
 * this function provides more information than using the numerical version macros.
 * For example, numerical version macros don't differentiate between development,
 * pre-release and release versions, but the output of this function does.
 *
 * @return constant string from IDF_VER
 */
const char* esp_get_idf_version(void);

#endif

#ifdef __cplusplus
}
#endif
