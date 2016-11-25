// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Init software coexist
 *
 * @return Init ok or failed.
 */
esp_err_t coex_init(void);

/**
 * @brief Get software coexist enable or not
 *
 * @return software coexist enable status.
 */
bool coexist_get_enable(void);

/**
 * @brief Set software coexist enable or not
 *
 * @param enable software coexist or disable it
 *
 * @return Void.
 */
void coexist_set_enable(bool enable);

#ifdef __cplusplus
}
#endif
