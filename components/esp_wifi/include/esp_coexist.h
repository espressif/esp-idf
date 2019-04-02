// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
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

#ifndef __ESP_COEXIST_H__
#define __ESP_COEXIST_H__

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief coex prefer value
 */
typedef enum {
    ESP_COEX_PREFER_WIFI = 0,       /*!< Prefer to WiFi, WiFi will have more opportunity to use RF */
    ESP_COEX_PREFER_BT,             /*!< Prefer to bluetooth, bluetooth will have more opportunity to use RF */
    ESP_COEX_PREFER_BALANCE,        /*!< Do balance of WiFi and bluetooth */
    ESP_COEX_PREFER_NUM,            /*!< Prefer value numbers */
} esp_coex_prefer_t;

/**
 * @brief Get software coexist version string
 *
 * @return : version string
 */
const char *esp_coex_version_get(void);

/**
 * @brief Set coexist preference of performance
 *  For example, if prefer to bluetooth, then it will make A2DP(play audio via classic bt)
 *  more smooth while wifi is runnning something.
 *  If prefer to wifi, it will do similar things as prefer to bluetooth.
 *  Default, it prefer to balance.
 *
 *  @param prefer : the prefer enumeration value
 *  @return : ESP_OK - success, other - failed
 */
esp_err_t esp_coex_preference_set(esp_coex_prefer_t prefer);

#ifdef __cplusplus
}
#endif


#endif /* __ESP_COEXIST_H__ */
