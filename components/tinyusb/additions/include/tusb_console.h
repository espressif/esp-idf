// Copyright 2020 Espressif Systems (Shanghai) Co. Ltd.
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

#include "esp_err.h"

/**
 * @brief Redirect output to the USB serial
 * @param cdc_intf - interface number of TinyUSB's CDC
 *
 * @return esp_err_t - ESP_OK, ESP_FAIL or an error code
 */
esp_err_t esp_tusb_init_console(int cdc_intf);

/**
 * @brief Switch log to the default output
 * @param cdc_intf - interface number of TinyUSB's CDC
 *
 * @return esp_err_t
 */
esp_err_t esp_tusb_deinit_console(int cdc_intf);
