/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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

typedef enum sleep_retention_module_bitmap {
    /* clock module, which includes system and modem */
    SLEEP_RETENTION_MODULE_CLOCK_SYSTEM = BIT(1),
    SLEEP_RETENTION_MODULE_CLOCK_MODEM  = BIT(2),

    /* modem module, which includes WiFi, BLE and 802.15.4 */
    SLEEP_RETENTION_MODULE_WIFI_MAC     = BIT(10),
    SLEEP_RETENTION_MODULE_WIFI_BB      = BIT(11),
    SLEEP_RETENTION_MODULE_BLE_MAC      = BIT(12),
    SLEEP_RETENTION_MODULE_BT_BB        = BIT(13),
    SLEEP_RETENTION_MODULE_802154_MAC   = BIT(14),

    /* digital peripheral module, which includes Interrupt Matrix, HP_SYSTEM,
     * TEE, APM, UART, Timer Group, IOMUX, SPIMEM, SysTimer, etc.. */
    SLEEP_RETENTION_MODULE_INTR_MATRIX  = BIT(16),
    SLEEP_RETENTION_MODULE_HP_SYSTEM    = BIT(17),
    SLEEP_RETENTION_MODULE_TEE_APM      = BIT(18),
    SLEEP_RETENTION_MODULE_UART0        = BIT(19),
    SLEEP_RETENTION_MODULE_TG0          = BIT(20),
    SLEEP_RETENTION_MODULE_IOMUX        = BIT(21),
    SLEEP_RETENTION_MODULE_SPIMEM       = BIT(22),
    SLEEP_RETENTION_MODULE_SYSTIMER     = BIT(23),
    SLEEP_RETENTION_MODULE_GDMA_CH0     = BIT(24),
    SLEEP_RETENTION_MODULE_GDMA_CH1     = BIT(25),
    SLEEP_RETENTION_MODULE_GDMA_CH2     = BIT(26),


    SLEEP_RETENTION_MODULE_ALL          = (uint32_t)-1
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
