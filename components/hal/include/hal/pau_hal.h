/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for PAU (Power Assist Unit)

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "soc/soc_caps.h"
#if SOC_PAU_SUPPORTED
#include "hal/pau_ll.h"
#include "hal/pau_types.h"


typedef struct {
    pau_dev_t *dev;
} pau_hal_context_t;

/**
 * @brief Set regdma entry link address
 *
 * @param hal           regdma hal context
 * @param link_addr     entry link address value
 */
void pau_hal_set_regdma_entry_link_addr(pau_hal_context_t *hal, pau_regdma_link_addr_t *link_addr);

#if SOC_PM_SUPPORT_PMU_MODEM_STATE
/**
 * @brief Set regdma modem link address
 *
 * @param hal           regdma hal context
 * @param link_addr     modem link address value
 */
#define pau_hal_set_regdma_modem_link_addr(hal, addr)   pau_ll_set_regdma_wifimac_link_addr((hal)->dev, (addr))

/**
 * @brief Start transmission on regdma modem link
 *
 * @param hal           regdma hal context
 * @param backup_or_restore        false:restore true:backup
 */
void pau_hal_start_regdma_modem_link(pau_hal_context_t *hal, bool backup_or_restore);

/**
 * @brief Stop transmission on regdma modem link
 *
 * @param hal           regdma hal context
 */
void pau_hal_stop_regdma_modem_link(pau_hal_context_t *hal);
#endif

#if SOC_PM_RETENTION_SW_TRIGGER_REGDMA
/**
 * @brief Set regdma system link address
 *
 * @param hal           regdma hal context
 * @param link_addr     main link address value
 */
#define pau_hal_set_regdma_system_link_addr(hal, addr)   pau_ll_set_regdma_link0_addr(hal->dev, (addr))

/**
 * @brief Start transmission on regdma system link
 *
 * @param hal           regdma hal context
 * @param backup_or_restore        false:restore true:backup
 */
void pau_hal_start_regdma_system_link(pau_hal_context_t *hal, bool backup_or_restore);
/**
 * @brief Stop transmission on regdma system link
 *
 * @param hal           regdma hal context
 */
void pau_hal_stop_regdma_system_link(pau_hal_context_t *hal);
#endif

/**
 * @brief Set regdma extra link address
 *
 * @param hal           regdma hal context
 * @param link_addr     extra link address value
 */
#define pau_hal_set_regdma_extra_link_addr(hal, addr)   pau_ll_set_regdma_link3_addr(hal->dev, (addr))

/**
 * @brief Start transmission on regdma extra link
 *
 * @param hal           regdma hal context
 * @param backup_or_restore        false:restore true:backup
 */
void pau_hal_start_regdma_extra_link(pau_hal_context_t *hal, bool backup_or_restore);

/**
 * @brief Stop transmission on regdma extra link
 *
 * @param hal           regdma hal context
 */
void pau_hal_stop_regdma_extra_link(pau_hal_context_t *hal);

#if SOC_PM_RETENTION_SW_TRIGGER_REGDMA
/**
 * @brief Enable or disable PAU module clock
 *
 * @param hal           regdma hal context
 */
void pau_hal_regdma_clock_configure(pau_hal_context_t *hal, bool enable);
#endif

#endif

#ifdef __cplusplus
}
#endif
