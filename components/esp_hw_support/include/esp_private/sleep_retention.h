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

/**
 * @file sleep_retention.h
 *
 * This file contains declarations of sleep retention related functions, it
 * includes sleep retention list creation, destruction and debugging interfaces.
 */

typedef enum sleep_retention_module {
    SLEEP_RETENTION_MODULE_MIN          = 0,
    /* clock module, which includes system and modem */
    SLEEP_RETENTION_MODULE_CLOCK_SYSTEM = 1,
    SLEEP_RETENTION_MODULE_CLOCK_MODEM  = 2,

    /* modem module, which includes WiFi, BLE and 802.15.4 */
    SLEEP_RETENTION_MODULE_WIFI_MAC     = 10,
    SLEEP_RETENTION_MODULE_WIFI_BB      = 11,
    SLEEP_RETENTION_MODULE_BLE_MAC      = 12,
    SLEEP_RETENTION_MODULE_BT_BB        = 13,
    SLEEP_RETENTION_MODULE_802154_MAC   = 14,

    /* digital peripheral module, which includes Interrupt Matrix, HP_SYSTEM,
     * TEE, APM, UART, Timer Group, IOMUX, SPIMEM, SysTimer, etc.. */
    SLEEP_RETENTION_MODULE_SYS_PERIPH   = 16,

    SLEEP_RETENTION_MODULE_ADC          = 17,

    SLEEP_RETENTION_MODULE_GDMA_CH0     = 24,
    SLEEP_RETENTION_MODULE_GDMA_CH1     = 25,
    SLEEP_RETENTION_MODULE_GDMA_CH2     = 26,

    SLEEP_RETENTION_MODULE_MAX          = 31
} sleep_retention_module_t;

typedef enum sleep_retention_module_bitmap {
    /* clock module, which includes system and modem */
    SLEEP_RETENTION_MODULE_BM_CLOCK_SYSTEM = BIT(SLEEP_RETENTION_MODULE_CLOCK_SYSTEM),
    SLEEP_RETENTION_MODULE_BM_CLOCK_MODEM  = BIT(SLEEP_RETENTION_MODULE_CLOCK_MODEM),

    /* modem module, which includes WiFi, BLE and 802.15.4 */
    SLEEP_RETENTION_MODULE_BM_WIFI_MAC     = BIT(SLEEP_RETENTION_MODULE_WIFI_MAC),
    SLEEP_RETENTION_MODULE_BM_WIFI_BB      = BIT(SLEEP_RETENTION_MODULE_WIFI_BB),
    SLEEP_RETENTION_MODULE_BM_BLE_MAC      = BIT(SLEEP_RETENTION_MODULE_BLE_MAC),
    SLEEP_RETENTION_MODULE_BM_BT_BB        = BIT(SLEEP_RETENTION_MODULE_BT_BB),
    SLEEP_RETENTION_MODULE_BM_802154_MAC   = BIT(SLEEP_RETENTION_MODULE_802154_MAC),

    /* digital peripheral module, which includes Interrupt Matrix, HP_SYSTEM,
     * TEE, APM, UART, Timer Group, IOMUX, SPIMEM, SysTimer, etc.. */
    SLEEP_RETENTION_MODULE_BM_SYS_PERIPH   = BIT(SLEEP_RETENTION_MODULE_SYS_PERIPH),

    SLEEP_RETENTION_MODULE_BM_ADC          = BIT(SLEEP_RETENTION_MODULE_ADC),

    SLEEP_RETENTION_MODULE_BM_GDMA_CH0     = BIT(SLEEP_RETENTION_MODULE_GDMA_CH0),
    SLEEP_RETENTION_MODULE_BM_GDMA_CH1     = BIT(SLEEP_RETENTION_MODULE_GDMA_CH1),
    SLEEP_RETENTION_MODULE_BM_GDMA_CH2     = BIT(SLEEP_RETENTION_MODULE_GDMA_CH2),

    SLEEP_RETENTION_MODULE_BM_ALL          = (uint32_t)-1
} sleep_retention_module_bitmap_t;

typedef regdma_entry_buf_t sleep_retention_entries_t;

typedef struct {
    regdma_link_config_t    config;
    uint32_t                owner;  /**< Indicates which regdma entries the current node will insert into */
} sleep_retention_entries_config_t;

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
 * @brief Get all initialized modules that require sleep retention
 *
 * This is an unprotected interface for getting a bitmap of all modules that
 * require sleep retention.
 *
 * It can only be called by the sleep procedure.
 *
 * @return the bitmap for all modules that require sleep retention
 */
uint32_t sleep_retention_get_inited_modules(void);

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
uint32_t sleep_retention_get_created_modules(void);

#if SOC_PM_RETENTION_HAS_CLOCK_BUG
/**
 * @brief Software trigger REGDMA to do extra linked list retention
 *
 * @param backup_or_restore true for backup register context to memory
 *                          or false for restore to register from memory
 */
void sleep_retention_do_extra_retention(bool backup_or_restore);
#endif

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
