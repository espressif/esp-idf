// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
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
#ifndef ESP_DBG_STUBS_H_
#define ESP_DBG_STUBS_H_

#include "esp_err.h"

/**
 * Debug stubs entries IDs
 */
typedef enum {
    ESP_DBG_STUB_MAGIC_NUM,
    ESP_DBG_STUB_TABLE_SIZE,
    ESP_DBG_STUB_CONTROL_DATA,   ///< stubs descriptor entry
    ESP_DBG_STUB_ENTRY_FIRST,
    ESP_DBG_STUB_ENTRY_GCOV	///< GCOV entry
        = ESP_DBG_STUB_ENTRY_FIRST,
    ESP_DBG_STUB_ENTRY_CAPABILITIES,
    ESP_DBG_STUB_ENTRY_MAX
} esp_dbg_stub_id_t;

#define ESP_DBG_STUB_MAGIC_NUM_VAL      0xFEEDBEEF
#define ESP_DBG_STUB_CAP_GCOV_TASK      (1 << 0)

/**
 * @brief  Initializes debug stubs.
 *
 * @note   Must be called after esp_apptrace_init() if app tracing is enabled.
 */
void esp_dbg_stubs_init(void);

/**
 * @brief  Initializes application tracing module.
 *
 * @note   Should be called before any esp_apptrace_xxx call.
 *
 * @param id 	Stub ID.
 * @param entry Stub entry. Usually it is stub entry function address,
 *              but can be any value meaningfull for OpenOCD command/code
 *              such as capabilities
 * @return ESP_OK on success, otherwise see esp_err_t
 */
esp_err_t esp_dbg_stub_entry_set(esp_dbg_stub_id_t id, uint32_t entry);

/**
 * @brief   Retrives the corresponding stub entry
 *
 * @param id 	Stub ID.
 * @param entry Stub entry. Usually it is stub entry function address,
 *              but can be any value meaningfull for OpenOCD command/code
 *              such as capabilities
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
esp_err_t esp_dbg_stub_entry_get(esp_dbg_stub_id_t id, uint32_t *entry);

#endif //ESP_DBG_STUBS_H_
