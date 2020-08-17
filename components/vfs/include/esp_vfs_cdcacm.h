// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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
#include "esp_vfs.h"
#include "esp_vfs_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief add /dev/cdcacm virtual filesystem driver
 *
 * This function is called from startup code to enable console output
 */
esp_err_t esp_vfs_dev_cdcacm_register(void);

/**
 * @brief Set the line endings expected to be received
 *
 * This specifies the conversion between line endings received and
 * newlines ('\n', LF) passed into stdin:
 *
 * - ESP_LINE_ENDINGS_CRLF: convert CRLF to LF
 * - ESP_LINE_ENDINGS_CR: convert CR to LF
 * - ESP_LINE_ENDINGS_LF: no modification
 *
 * @note this function is not thread safe w.r.t. reading
 *
 * @param mode line endings expected
 */
void esp_vfs_dev_cdcacm_set_rx_line_endings(esp_line_endings_t mode);

/**
 * @brief Set the line endings to sent
 *
 * This specifies the conversion between newlines ('\n', LF) on stdout and line
 * endings sent:
 *
 * - ESP_LINE_ENDINGS_CRLF: convert LF to CRLF
 * - ESP_LINE_ENDINGS_CR: convert LF to CR
 * - ESP_LINE_ENDINGS_LF: no modification
 *
 * @note this function is not thread safe w.r.t. writing
 *
 * @param mode line endings to send
 */
void esp_vfs_dev_cdcacm_set_tx_line_endings(esp_line_endings_t mode);

#ifdef __cplusplus
}
#endif
