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
    SLEEP_RETENTION_MODULE_INTR_MATRIX  = 16,
    SLEEP_RETENTION_MODULE_HP_SYSTEM    = 17,
    SLEEP_RETENTION_MODULE_TEE_APM      = 18,
    SLEEP_RETENTION_MODULE_UART0        = 19,
    SLEEP_RETENTION_MODULE_TG0          = 20,
    SLEEP_RETENTION_MODULE_IOMUX        = 21,
    SLEEP_RETENTION_MODULE_SPIMEM       = 22,
    SLEEP_RETENTION_MODULE_SYSTIMER     = 23,
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
    SLEEP_RETENTION_MODULE_BM_INTR_MATRIX  = BIT(SLEEP_RETENTION_MODULE_INTR_MATRIX),
    SLEEP_RETENTION_MODULE_BM_HP_SYSTEM    = BIT(SLEEP_RETENTION_MODULE_HP_SYSTEM),
    SLEEP_RETENTION_MODULE_BM_TEE_APM      = BIT(SLEEP_RETENTION_MODULE_TEE_APM),
    SLEEP_RETENTION_MODULE_BM_UART0        = BIT(SLEEP_RETENTION_MODULE_UART0),
    SLEEP_RETENTION_MODULE_BM_TG0          = BIT(SLEEP_RETENTION_MODULE_TG0),
    SLEEP_RETENTION_MODULE_BM_IOMUX        = BIT(SLEEP_RETENTION_MODULE_IOMUX),
    SLEEP_RETENTION_MODULE_BM_SPIMEM       = BIT(SLEEP_RETENTION_MODULE_SPIMEM),
    SLEEP_RETENTION_MODULE_BM_SYSTIMER     = BIT(SLEEP_RETENTION_MODULE_SYSTIMER),
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
 * @brief Destroy a runtime sleep retention linked list
 *
 * @param module   the bitmap of the module to be destroyed
 */
void sleep_retention_entries_destroy(sleep_retention_module_t module);

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

/**
 * @brief Get all created modules that require sleep retention
 *
 * This is an unprotected interface for getting a bitmap of all modules that
 * require sleep retention.
 *
 * It can only be called by the sleep procedure.
 *
 * @return the bitmap of all modules requiring sleep retention
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
