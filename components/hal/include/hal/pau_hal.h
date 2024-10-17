/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
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
 * @brief Enable the bus clock of REGDMA module
 * @param hal    regdma hal context
 * @param enable enable or disable the module clock
 */
#define pau_hal_enable_bus_clock(enable) pau_ll_enable_bus_clock(enable)

/**
 * @brief Set regdma entry link address
 *
 * @param hal           regdma hal context
 * @param link_addr     entry link address value
 */
void pau_hal_set_regdma_entry_link_addr(pau_hal_context_t *hal, pau_regdma_link_addr_t *link_addr);

#if SOC_PM_SUPPORT_PMU_MODEM_STATE
#if SOC_PM_PAU_REGDMA_LINK_WIFIMAC
/**
 * @brief Set regdma modem link address
 *
 * @param hal           regdma hal context
 * @param link_addr     modem link address value
 */
#define pau_hal_set_regdma_modem_link_addr(hal, addr)   pau_ll_set_regdma_wifimac_link_addr((hal)->dev, (addr))
#endif

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
#if SOC_PM_PAU_REGDMA_LINK_MULTI_ADDR
/**
 * @brief Set regdma system link address
 *
 * @param hal           regdma hal context
 * @param link_addr     main link address value
 */
#define pau_hal_set_regdma_system_link_addr(hal, addr)   pau_ll_set_regdma_link0_addr(hal->dev, (addr))
#endif

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

#if SOC_PM_PAU_REGDMA_LINK_MULTI_ADDR
/**
 * @brief Set regdma extra link address
 *
 * @param hal           regdma hal context
 * @param link_addr     extra link address value
 */
#define pau_hal_set_regdma_extra_link_addr(hal, addr)   pau_ll_set_regdma_link3_addr(hal->dev, (addr))
#endif

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

#if SOC_PM_PAU_REGDMA_LINK_CONFIGURABLE
/**
 * @brief Enable or disable PAU module clock
 *
 * @param hal           regdma hal context
 * @param count         the maximum number of regdma linked list
 */
void pau_hal_regdma_link_count_config(pau_hal_context_t *hal, int count);
#endif

/**
 * @brief Set PAU module link work timeout threshold
 *
 * @param hal           regdma hal context
 * @param loop_num      the maximum number of regdma link loop num
 * @param count         the maximum number of register access timeout
 */
void pau_hal_set_regdma_work_timeout(pau_hal_context_t *hal, uint32_t loop_num, uint32_t count);

/**
 * @brief Set regdma link wait timeout, include wait retry count and register read interval
 *
 * @param hal           regdma hal context
 * @param count         the maximum number of regdma wait retry count
 * @param interval      the interval of regdma wait link to read register
 */
void pau_hal_set_regdma_wait_timeout(pau_hal_context_t *hal, int count, int interval);

#if SOC_PAU_IN_TOP_DOMAIN
/**
 * If PAU is in TOP power domain, configuration will be lost after sleep, it is necessary
 * to use LP_SYS_BACKUP_DMA_CFG2_REG to override restore link address, do related logic
 * initialization by this function.
 */
void pau_hal_lp_sys_initialize(void);
#endif

#endif

#ifdef __cplusplus
}
#endif
