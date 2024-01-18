/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_PAU_SUPPORTED
#include "esp_regdma.h"
#include "soc/retention_periph_defs.h"

/**
 * @file sleep_retention.h
 *
 * This file contains declarations of sleep retention related functions, it
 * includes sleep retention list creation, destruction and debugging interfaces.
 */
typedef periph_retention_module_bitmap_t    sleep_retention_module_bitmap_t;
typedef regdma_entry_buf_t                  sleep_retention_entries_t;
typedef regdma_entries_config_t             sleep_retention_entries_config_t;

/**
 * @brief Create a runtime sleep retention linked list
 *
 * @param retent   sleep retention linked list node configuration table
 * @param num      the total number of sleep retention linked list configuration
 *                 items
 * @param priority the priority of the created sleep retention linked list
 * @param module   the bitmap of the module to which the created sleep retention
 *                 linked list belongs
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM not enough memory for sleep retention
 *      - ESP_ERR_INVALID_ARG if either of the arguments is out of range
 */
esp_err_t sleep_retention_entries_create(const sleep_retention_entries_config_t retent[], int num, regdma_link_priority_t priority, int module);

/**
 * @brief Destroy a runtime sleep retention linked list
 *
 * @param module   the bitmap of the module to be destroyed
 */
void sleep_retention_entries_destroy(int module);

/**
 * @brief Print all runtime sleep retention linked lists
 */
void sleep_retention_entries_show_memories(void);

/**
 * @brief Find the linked list node with the unique id
 *
 * @param  id the unique identifier of specified linked list node
 *
 * @return NULL or the address of the linked list node found
 */
void * sleep_retention_find_link_by_id(int id);

/**
 * @brief Get the head pointer of all entry linked list of REGDMA
 *
 * @param  entries buffer for getting results
 */
void sleep_retention_entries_get(sleep_retention_entries_t *entries);

#if SOC_PM_RETENTION_HAS_CLOCK_BUG
/**
 * @brief Software trigger REGDMA to do extra linked list retention
 *
 * @param backup_or_restore true for backup register context to memory
 *                          or false for restore to register from memory
 */
void sleep_retention_do_extra_retention(bool backup_or_restore);
#endif

/**
 * @brief Get all registered modules that require sleep retention
 *
 * This is an unprotected interface for getting a bitmap of all modules that
 * require sleep retention.
 *
 * It can only be called by the sleep procedure.
 *
 * @return the bitmap of all modules requiring sleep retention
 */
uint32_t sleep_retention_get_modules(void);

#if SOC_PM_RETENTION_SW_TRIGGER_REGDMA
/**
 * @brief Software trigger REGDMA to do system linked list retention
 *
 * @param backup_or_restore true for backup register context to memory
 *                          or false for restore to register from memory
 */
void sleep_retention_do_system_retention(bool backup_or_restore);
#endif

#endif // SOC_PAU_SUPPORTED

#ifdef __cplusplus
}
#endif
