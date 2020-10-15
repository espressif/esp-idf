// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/** Major version number (X.x.x) */
#define ESP_IDF_VERSION_MAJOR   4
/** Minor version number (x.X.x) */
#define ESP_IDF_VERSION_MINOR   3
/** Patch version number (x.x.X) */
#define ESP_IDF_VERSION_PATCH   0

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
