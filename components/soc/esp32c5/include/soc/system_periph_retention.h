/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
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
#define INT_MTX_RETENTION_LINK_LEN   1
extern const regdma_entries_config_t intr_matrix_regs_retention[INT_MTX_RETENTION_LINK_LEN];

/**
 * @brief Provide access to hp_system configuration registers retention
 * context definition.
 *
 * This is an internal function of the sleep retention driver, and is not
 * useful for external use.
 */
#define HP_SYSTEM_RETENTION_LINK_LEN 3
extern const regdma_entries_config_t hp_system_regs_retention[HP_SYSTEM_RETENTION_LINK_LEN];

/**
 * @brief Provide access to TEE_APM configuration registers retention
 * context definition.
 *
 * This is an internal function of the sleep retention driver, and is not
 * useful for external use.
 */
#define TEE_APM_RETENTION_LINK_LEN   2
extern const regdma_entries_config_t tee_apm_regs_retention[TEE_APM_RETENTION_LINK_LEN];
#define TEE_APM_HIGH_PRI_RETENTION_LINK_LEN   1
extern const regdma_entries_config_t tee_apm_highpri_regs_retention[TEE_APM_HIGH_PRI_RETENTION_LINK_LEN];

/**
 * @brief Provide access to IOMUX configuration registers retention
 * context definition.
 *
 * This is an internal function of the sleep retention driver, and is not
 * useful for external use.
 */
#define IOMUX_RETENTION_LINK_LEN    5
extern const regdma_entries_config_t iomux_regs_retention[IOMUX_RETENTION_LINK_LEN];

/**
 * @brief Provide access to spimem configuration registers retention
 * context definition.
 *
 * This is an internal function of the sleep retention driver, and is not
 * useful for external use.
 */
#define SPIMEM_RETENTION_LINK_LEN   8
extern const regdma_entries_config_t flash_spimem_regs_retention[SPIMEM_RETENTION_LINK_LEN];

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
 * @brief Provide access to pvt configuration registers retention
 * context definition.
 *
 * This is an internal function of the sleep retention driver, and is not
 * useful for external use.
 */
#define PVT_RETENTION_LINK_LEN   1
extern const regdma_entries_config_t pvt_regs_retention[PVT_RETENTION_LINK_LEN];

#ifdef __cplusplus
}
#endif
