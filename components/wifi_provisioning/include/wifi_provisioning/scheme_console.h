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

#include <protocomm.h>
#include <protocomm_console.h>

#include "wifi_provisioning/manager.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Scheme that can be used by manager for provisioning
 *          over console (Serial UART)
 */
extern const wifi_prov_scheme_t wifi_prov_scheme_console;

#ifdef __cplusplus
}
#endif
