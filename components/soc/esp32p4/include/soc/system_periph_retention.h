/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/regdma.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Provide access to interrupt matrix configuration registers retention
 * context definition.
 *
 * This is an internal function of the sleep retention driver, and is not
 * useful for external use.
 */
#define INT_MTX_RETENTION_LINK_LEN   2
extern const regdma_entries_config_t intr_matrix_regs_retention[INT_MTX_RETENTION_LINK_LEN];

/**
 * @brief Provide access to l2_cache configuration registers retention
 * context definition.
 *
 * This is an internal function of the sleep retention driver, and is not
 * useful for external use.
 */
#define L2_CACHE_RETENTION_LINK_LEN 1
extern const regdma_entries_config_t l2_cache_regs_retention[L2_CACHE_RETENTION_LINK_LEN];

/**
 * @brief Provide access to hp_system configuration registers retention
 * context definition.
 *
 * This is an internal function of the sleep retention driver, and is not
 * useful for external use.
 */
#define HP_SYSTEM_RETENTION_LINK_LEN 1
extern const regdma_entries_config_t hp_system_regs_retention[HP_SYSTEM_RETENTION_LINK_LEN];

/**
 * @brief Provide access to timer group configuration registers retention
 * context definition.
 *
 * This is an internal function of the sleep retention driver, and is not
 * useful for external use.
 */
#define TIMG_RETENTION_LINK_LEN     8
extern const regdma_entries_config_t tg_regs_retention[TIMG_RETENTION_LINK_LEN];

/**
 * @brief Provide access to IOMUX configuration registers retention
 * context definition.
 *
 * This is an internal function of the sleep retention driver, and is not
 * useful for external use.
 */
#define IOMUX_RETENTION_LINK_LEN    3
extern const regdma_entries_config_t iomux_regs_retention[IOMUX_RETENTION_LINK_LEN];

/**
 * @brief Provide access to FLASH spimem configuration registers retention
 * context definition.
 *
 * This is an internal function of the sleep retention driver, and is not
 * useful for external use.
 */
#define SPIMEM_FLASH_RETENTION_LINK_LEN   8
extern const regdma_entries_config_t flash_spimem_regs_retention[SPIMEM_FLASH_RETENTION_LINK_LEN];

/**
 * @brief Provide access to PSRAM spimem configuration registers retention
 * context definition.
 *
 * This is an internal function of the sleep retention driver, and is not
 * useful for external use.
 */
#define SPIMEM_PSRAM_RETENTION_LINK_LEN   14
extern const regdma_entries_config_t psram_spimem_regs_retention[SPIMEM_PSRAM_RETENTION_LINK_LEN];

/**
 * @brief Provide access to systimer configuration registers retention
 * context definition.
 *
 * This is an internal function of the sleep retention driver, and is not
 * useful for external use.
 */
#define SYSTIMER_RETENTION_LINK_LEN   19
extern const regdma_entries_config_t systimer_regs_retention[SYSTIMER_RETENTION_LINK_LEN];

/**
 * @brief Provide access to pau configuration registers retention
 * context definition.
 *
 * This is an internal function of the sleep retention driver, and is not
 * useful for external use.
 */
#define PAU_RETENTION_LINK_LEN 1
extern const regdma_entries_config_t pau_regs_retention[L2_CACHE_RETENTION_LINK_LEN];

#ifdef __cplusplus
}
#endif
