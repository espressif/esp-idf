// Copyright 2018-2018 Espressif Systems (Shanghai) PTE LTD
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

#ifndef __COEXIST_INTERNAL_H__
#define __COEXIST_INTERNAL_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    COEX_PREFER_WIFI = 0,
    COEX_PREFER_BT,
    COEX_PREFER_BALANCE,
    COEX_PREFER_NUM,
} coex_prefer_t;

/**
 * @brief Init software coexist
 *        extern function for internal use.
 *
 * @return Init ok or failed.
 */
esp_err_t coex_init(void);

/**
 * @brief De-init software coexist
 *        extern function for internal use.
 */
void coex_deinit(void);

/**
 * @brief Pause software coexist
 *        extern function for internal use.
 */
void coex_pause(void);

/**
 * @brief Resume software coexist
 *        extern function for internal use.
 */
void coex_resume(void);

/**
 * @brief Get software coexist version string
 *        extern function for internal use.
 * @return : version string
 */
const char *coex_version_get(void);

/**
 * @brief Coexist performance preference set from libbt.a
 *        extern function for internal use.
 *
 *  @param prefer : the prefer enumeration value
 *  @return : ESP_OK - success, other - failed
 */
esp_err_t coex_preference_set(coex_prefer_t prefer);

#ifdef __cplusplus
}
#endif

#endif /* __COEXIST_INTERNAL_H__ */
