/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_DBG_STUBS_H_
#define ESP_DBG_STUBS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"

/**
 * Debug stubs entries IDs
 */
typedef enum {
    ESP_DBG_STUB_MAGIC_NUM,
    ESP_DBG_STUB_TABLE_SIZE,
    ESP_DBG_STUB_CONTROL_DATA,   ///< stubs descriptor entry
    ESP_DBG_STUB_ENTRY_FIRST,
    ESP_DBG_STUB_ENTRY_GCOV ///< GCOV entry
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
 * @param id    Stub ID.
 * @param entry Stub entry. Usually it is stub entry function address,
 *              but can be any value meaningfull for OpenOCD command/code
 *              such as capabilities
 * @return ESP_OK on success, otherwise see esp_err_t
 */
esp_err_t esp_dbg_stub_entry_set(esp_dbg_stub_id_t id, uint32_t entry);

/**
 * @brief   Retrives the corresponding stub entry
 *
 * @param id    Stub ID.
 * @param entry Stub entry. Usually it is stub entry function address,
 *              but can be any value meaningfull for OpenOCD command/code
 *              such as capabilities
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
esp_err_t esp_dbg_stub_entry_get(esp_dbg_stub_id_t id, uint32_t *entry);

#ifdef __cplusplus
}
#endif

#endif // ESP_DBG_STUBS_H_
