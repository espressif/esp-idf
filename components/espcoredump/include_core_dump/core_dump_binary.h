// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef ESP_CORE_DUMP_BINARY_H_
#define ESP_CORE_DUMP_BINARY_H_

#include "esp_core_dump_types.h"

/**
 * @brief Initiate the binary core dump generation.
 *
 * @param info      Exception frame info generated when the panic occured.
 * @param write_cfg Structure containing the callbacks that will be called to
 *                  write the generated core dump file.
 *
 * @return ESP_OK on success, otherwise \see esp_err_t.
 */
esp_err_t esp_core_dump_write_binary(core_dump_write_config_t *write_cfg);

#endif
