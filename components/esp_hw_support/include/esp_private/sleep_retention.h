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

#define SLEEP_RETENTION_MODULE_BITMAP_SZ    ((SLEEP_RETENTION_MODULE_MAX >> 5) + 1)

/**
 * @file sleep_retention.h
 *
 * This file contains declarations of sleep retention related functions, it
 * includes sleep retention list creation, destruction and debugging interfaces.
 */
typedef periph_retention_module_t           sleep_retention_module_t;
typedef struct {
#define RETENTION_MODULE_BITMAP_INIT(module) { .bitmap[(SLEEP_RETENTION_MODULE_ ## module) >> 5] = BIT((SLEEP_RETENTION_MODULE_ ## module) % 32) }
    uint32_t bitmap[SLEEP_RETENTION_MODULE_BITMAP_SZ];
} sleep_retention_module_bitmap_t;
typedef regdma_entry_buf_t                  sleep_retention_entries_t;
typedef regdma_entries_config_t             sleep_retention_entries_config_t;

typedef esp_err_t (*sleep_retention_callback_t)(void *args);

typedef struct {
    sleep_retention_callback_t handle;
    void *arg;
} sleep_retention_create_callback_t;

typedef struct {
    sleep_retention_create_callback_t create;  /*!< A function handle is used to register the implementation of creating a sleep retention linked list and is executed when the corresponding module is created */
} sleep_retention_module_callbacks_t;

typedef enum {
    SLEEP_RETENTION_MODULE_ATTR_PASSIVE = 0x1
} sleep_retention_module_attribute_t;

/**
 * @brief Create a runtime sleep retention linked list
 *
 * @param retent   sleep retention linked list node configuration table
 * @param num      the total number of sleep retention linked list configuration
 *                 items
 * @param priority the priority of the created sleep retention linked list
 * @param module   the number of the module to which the created sleep retention
 *                 linked list belongs
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM not enough memory for sleep retention
 *      - ESP_ERR_INVALID_ARG if either of the arguments is out of range
 */
esp_err_t sleep_retention_entries_create(const sleep_retention_entries_config_t retent[], int num, regdma_link_priority_t priority, sleep_retention_module_t module);

/**
 * @brief Dump the initialization status of all modules.
*/
void sleep_retention_dump_modules(FILE *out);

/**
 * @brief Dump all runtime sleep retention linked lists
 */
void sleep_retention_dump_entries(FILE *out);

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

typedef struct sleep_retention_module_init_param {
    sleep_retention_module_callbacks_t  cbs;        /*!< The callbacks list of the initialize module */
    sleep_retention_module_attribute_t  attribute;  /*!< A bitmap indicating attribute of the initialize module */
    sleep_retention_module_bitmap_t     depends;    /*!< A bitmap identifying all modules that the current module depends on */
} sleep_retention_module_init_param_t;

/**
 * @brief sleep retention initialization for the module
 *
 * @param module   the module number that needs initialization
 * @param param    the initialize parameters for module sleep retention initialization
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM not enough memory for sleep retention
 *      - ESP_ERR_INVALID_ARG if either of the arguments is out of range
 *      - ESP_ERR_INVALID_STATE if the retention context of module already been allocated
 */
esp_err_t sleep_retention_module_init(sleep_retention_module_t module, sleep_retention_module_init_param_t *param);

/**
 * @brief sleep retention de-initialization for the module
 *
 * @param module   the module number that needs de-initialization
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if either of the arguments is out of range
 *      - ESP_ERR_INVALID_STATE if the retention context of module already been allocated
 */
esp_err_t sleep_retention_module_deinit(sleep_retention_module_t module);

/**
 * @brief Allocate the sleep retention context for the module
 *
 * @param module   the module number that need to allocating sleep retention context
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM not enough memory for sleep retention
 *      - ESP_ERR_INVALID_ARG if either of the arguments is out of range
 *      - ESP_ERR_INVALID_STATE if the module is de-initialized
 *      - ESP_ERR_NOT_ALLOWED if the attribute of module is set to SLEEP_RETENTION_MODULE_ATTR_PASSIVE
 */
esp_err_t sleep_retention_module_allocate(sleep_retention_module_t module);

/**
 * @brief Free the sleep retention context for the module
 *
 * @param module   the module number that need to free sleep retention context
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if either of the arguments is out of range
 *      - ESP_ERR_INVALID_STATE if the module is de-initialized
 *      - ESP_ERR_NOT_ALLOWED if the attribute of module is set to SLEEP_RETENTION_MODULE_ATTR_PASSIVE
 */
esp_err_t sleep_retention_module_free(sleep_retention_module_t module);

/**
 * @brief Force take the power lock so that during sleep the power domain won't be powered off.
 *
 * @return
 *      - ESP_OK if success
 *      - other value when the internal `sleep_retention_module_init` fails.
*/
esp_err_t sleep_retention_power_lock_acquire(void);

/**
 * @brief Release the power lock so that the peripherals' power domain can be powered off.
 *        Please note that there is an internal reference counter and the power domain will be kept on until same number
 *        of `sleep_retention_power_lock_release` is called as `sleep_retention_power_lock_acquire`.
 * @return always ESP_OK
*/
esp_err_t sleep_retention_power_lock_release(void);

/**
 * @brief Get all initialized modules that require sleep retention
 *
 * This is an unprotected interface for getting a bitmap of all modules that
 * require sleep retention.
 *
 * It can only be called by the sleep procedure.
 *
 * @return the bitmap for all modules that require sleep retention
 */
sleep_retention_module_bitmap_t sleep_retention_get_inited_modules(void);

/**
 * @brief Get all created modules that require sleep retention
 *
 * This is an unprotected interface for getting a bitmap of all modules that
 * require sleep retention.
 *
 * It can only be called by the sleep procedure.
 *
 * @return the bitmap for all modules that have successfully created a sleep
 * retention context
 */
sleep_retention_module_bitmap_t sleep_retention_get_created_modules(void);

/**
 * @brief Get the initialization state of module
 *
 * @param module   module number
 *
 * @return false if the module is not initialized or the module number is
 * invalid, otherwise return true
 */
bool sleep_retention_is_module_inited(sleep_retention_module_t module);

/**
 * @brief Get the creation state of module
 *
 * @param module   module number
 *
 * @return false if the module is not created or the module number is
 * invalid, otherwise return true
 */
bool sleep_retention_is_module_created(sleep_retention_module_t module);

/**
 * @brief Calculates the bitwise logical and of the module bitmap and return results
 *
 * This is an unprotected interface. It can only be called by the sleep procedure.
 *
 * @param op0   module bitmap operator 0
 * @param op1   module bitmap operator 1
 *
 * @return the bitwise logical and result of module bitmap
 */
sleep_retention_module_bitmap_t sleep_retention_module_bitmap_and(sleep_retention_module_bitmap_t op0, sleep_retention_module_bitmap_t op1);

/**
 * @brief Calculates the bitwise logical or of the module bitmap and return results
 *
 * This is an unprotected interface. It can only be called by the sleep procedure.
 *
 * @param op0   module bitmap operator 0
 * @param op1   module bitmap operator 1
 *
 * @return the bitwise logical or result of module bitmap
 */
sleep_retention_module_bitmap_t sleep_retention_module_bitmap_or(sleep_retention_module_bitmap_t op0, sleep_retention_module_bitmap_t op1);

/**
 * @brief Calculates the bitwise logical not of the module bitmap and return results
 *
 * This is an unprotected interface. It can only be called by the sleep procedure.
 *
 * @param op0   module bitmap operator
 *
 * @return the bitwise logical not result of module bitmap
 */
sleep_retention_module_bitmap_t sleep_retention_module_bitmap_not(sleep_retention_module_bitmap_t op);

/**
 * @brief Compares the module bitmap values are equal and return results
 *
 * This is an unprotected interface. It can only be called by the sleep procedure.
 *
 * @param op0   module bitmap operator 0
 * @param op1   module bitmap operator 1
 *
 * @return If the module bitmap values are equal then return true, otherwise return false
 */
bool sleep_retention_module_bitmap_eq(sleep_retention_module_bitmap_t op0, sleep_retention_module_bitmap_t op1);

/**
 * @brief Software trigger REGDMA to do extra linked list retention
 *
 * @param backup_or_restore true for backup register context to memory
 *                          or false for restore to register from memory
 */
void sleep_retention_do_extra_retention(bool backup_or_restore);

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
