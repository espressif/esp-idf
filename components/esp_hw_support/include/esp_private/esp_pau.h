/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "soc/soc.h"
#include "soc/soc_caps.h"

#if SOC_PAU_SUPPORTED
#include "hal/pau_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set the addresses of all REGDMA Links
 * @param link_entries all linked lists addresses
 */
void pau_regdma_set_entry_link_addr(pau_regdma_link_addr_t *link_entries);

#if SOC_PM_SUPPORT_PMU_MODEM_STATE
/**
 * @brief Set the address of WiFi MAC REGDMA Link in modem state
 * @param link_addr linked lists address
 */
void pau_regdma_set_modem_link_addr(void *link_addr);

/**
 * @brief Software trigger regdma to perform modem link backup
 */
void pau_regdma_trigger_modem_link_backup(void);

/**
 * @brief Software trigger regdma to perform modem link restore
 */
void pau_regdma_trigger_modem_link_restore(void);
#endif

#if SOC_PM_RETENTION_SW_TRIGGER_REGDMA
/**
 * @brief Set the address of system REGDMA Link in active state
 * @param link_addr linked lists address
 */
void pau_regdma_set_system_link_addr(void *link_addr);

/**
 * @brief Software trigger regdma to perform system link backup
 */
void pau_regdma_trigger_system_link_backup(void);

/**
 * @brief Software trigger regdma to perform system link restore
 */
void pau_regdma_trigger_system_link_restore(void);
#endif

/**
 * @brief Set the address of extra REGDMA Link in active state
 * @param link_addr linked lists address
 */
void pau_regdma_set_extra_link_addr(void *link_addr);

/**
 * @brief Software trigger regdma to perform extra link backup
 */
void pau_regdma_trigger_extra_link_backup(void);

/**
 * @brief Software trigger regdma to perform extra link restore
 */
void pau_regdma_trigger_extra_link_restore(void);

#ifdef __cplusplus
}
#endif

#endif //SOC_PAU_SUPPORTED
