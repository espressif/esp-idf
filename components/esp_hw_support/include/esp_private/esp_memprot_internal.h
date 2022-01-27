/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#if CONFIG_ESP_SYSTEM_MEMPROT_FEATURE || CONFIG_ESP_SYSTEM_MEMPROT_TEST

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "esp_memprot_err.h"
#include "hal/memprot_types.h"
#include "soc_memprot_types.h"
#include "esp_memprot_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief Convert Memprot low level errors to esp_err_t
*/
esp_err_t esp_mprot_ll_err_to_esp_err(const memprot_ll_err_t err);

/**
 * @brief Convert Memprot low level PMS World IDs to esp_mprot_pms_world_t
 */
esp_mprot_pms_world_t esp_mprot_ll_world_to_hl_world(const memprot_ll_world_t world);

/**
 * @brief Converts operation type to string, no combination of operations allowed
 *
 * @param oper_type PMS operation type
 */
const char *esp_mprot_oper_type_to_str(const uint32_t oper_type);

/**
 * @brief Converts PMS World type to string
 *
 * @param area_type PMS World type
 */
const char *esp_mprot_pms_world_to_str(const esp_mprot_pms_world_t world_type);

/**
 * @brief Sets splitting address for given line type in the target Memory type
 *
 * @param mem_type memory type
 * @param line_type split address type
 * @param line_addr target address from a memory range relevant to given line_addr
 *
 * @return ESP_OK on success
 *         ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID on invalid mem_type
 *         ESP_ERR_MEMPROT_SPLIT_ADDR_INVALID on invalid line_type
 *         ESP_ERR_MEMPROT_SPLIT_ADDR_OUT_OF_RANGE on splitting line out of given memory-type range
 *         ESP_ERR_MEMPROT_SPLIT_ADDR_UNALIGNED on splitting line not aligned to PMS-required boundaries
 */
esp_err_t esp_mprot_set_split_addr(const esp_mprot_mem_t mem_type, const esp_mprot_split_addr_t line_type, const void *line_addr);

/**
 * @brief Gets PMS splitting address for given split_addr type
 *
 * The value is read from the PMS configuration registers
 *
 * @param mem_type memory type
 * @param line_type Split line type (see esp_mprot_split_addr_t enum)
 * @param[out] line_addr Split line address from the configuration register
 *
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_ARG on line_addr is pointer
 *         ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID on invalid mem_type
 *         ESP_ERR_MEMPROT_SPLIT_ADDR_INVALID on invalid line_type
 */
esp_err_t esp_mprot_get_split_addr(const esp_mprot_mem_t mem_type, const esp_mprot_split_addr_t line_type, void **line_addr);

/**
 * @brief Returns default main I/D splitting address for given Memory type
 *
 * @param mem_type memory type
 * @param[out] def_split_addr Main I/D splitting address of required mem_type
 *
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_ARG on invalid def_split_addr pointer
 *         ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID on invalid mem_type
 */
esp_err_t esp_mprot_get_default_main_split_addr(const esp_mprot_mem_t mem_type, void **def_split_addr);

/**
 * @brief Sets a lock for the main IRAM/DRAM splitting addresses
 * Locks can be unlocked only by digital system reset
 *
 * @param mem_type memory type
 *
 * @return ESP_OK on success
 *         ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID on invalid mem_type
 */
esp_err_t esp_mprot_set_split_addr_lock(const esp_mprot_mem_t mem_type);

/**
 * @brief Gets a lock status for the splitting address configuration of given Memory type
 *
 * @param mem_type memory type
 * @param[out] locked mem_type related lock status
 *
 * @return ESP_OK on success
 *         ESP_ERR_INVALID_ARGUMENT on invalid locked pointer
 *         ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID on invalid mem_type
 */
esp_err_t esp_mprot_get_split_addr_lock(const esp_mprot_mem_t mem_type, bool *locked);

/**
 * @brief Sets a lock for PMS Area settings of required Memory type
 * Locks can be unlocked only by digital system reset
 *
 * @param mem_type memory type
 *
 * @return ESP_OK on success
 *         ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID on invalid mem_type
 */
esp_err_t esp_mprot_set_pms_lock(const esp_mprot_mem_t mem_type);

/**
 * @brief Gets a lock status for PMS Area settings of required Memory type
 *
 * @param mem_type memory type
 * @param[out] locked mem_type related lock status
 *
 * @return ESP_OK on success
 *         ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID on invalid mem_type
 *         ESP_ERR_INVALID_ARGUMENT on invalid locked pointer
 */
esp_err_t esp_mprot_get_pms_lock(const esp_mprot_mem_t mem_type, bool *locked);

/**
 * @brief Sets permissions for given PMS Area
 *
 * @param area_type PMS area type
 * @param flags combination of MEMPROT_OP_* defines
 *
 * @return ESP_OK on success
 *         ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID on invalid mem_type
 */
esp_err_t esp_mprot_set_pms_area(const esp_mprot_pms_area_t area_type, const uint32_t flags);

/**
 * @brief Gets current permissions for given PMS Area
 *
 * @param area_type PMS area type
 * @param[out] flags combination of MEMPROT_OP_* defines
 *
 * @return ESP_OK on success
 *         ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID on invalid mem_type
 *         ESP_ERR_INVALID_ARG on invalid flags pointer
 */
esp_err_t esp_mprot_get_pms_area(const esp_mprot_pms_area_t area_type, uint32_t *flags);

/**
 * @brief Sets a lock for PMS interrupt monitor settings of required Memory type
 *
 * Locks can be unlocked only by digital system reset
 *
 * @param mem_type memory type (see esp_mprot_mem_t enum)
 *
 * @return ESP_OK on success
 *         ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID on invalid mem_type
 */
esp_err_t esp_mprot_set_monitor_lock(const esp_mprot_mem_t mem_type);

/**
 * @brief Gets a lock status for PMS interrupt monitor settings of required Memory type
 *
 * @param mem_type memory type
 * @param[out] locked mem_type related lock status
 *
 * @return ESP_OK on success
 *         ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID on invalid mem_type
 *         ESP_ERR_INVALID_ARG on invalid locked pointer
 */
esp_err_t esp_mprot_get_monitor_lock(const esp_mprot_mem_t mem_type, bool *locked);

/**
 * @brief Enable PMS violation interrupt monitoring of required Memory type
 *
 * @param mem_type memory type
 * @param enable enable/disable violation interrupt monitoring
 *
 * @return ESP_OK on success
 *         ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID on invalid mem_type
 */
esp_err_t esp_mprot_set_monitor_en(const esp_mprot_mem_t mem_type, const bool enable);

#ifdef __cplusplus
}
#endif

#endif //CONFIG_ESP_SYSTEM_MEMPROT_FEATURE || CONFIG_ESP_SYSTEM_MEMPROT_TEST
