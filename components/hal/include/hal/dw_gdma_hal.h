/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DW_GDMA SOC layer representation
 */
typedef struct dw_gdma_dev_t *dw_gdma_soc_handle_t;

/**
 * @brief DW_GDMA HAL driver context
 */
typedef struct {
    dw_gdma_soc_handle_t dev;  /*!< Pointer to the DW_GDMA registers */
} dw_gdma_hal_context_t;

/**
 * @brief DW_GDMA HAL driver configuration
 */
typedef struct {
} dw_gdma_hal_config_t;

/**
 * @brief DW_GDMA HAL driver initialization
 *
 * @note Caller should malloc the memory for the hal context
 *
 * @param hal Pointer to the HAL driver context
 * @param config Pointer to the HAL driver configuration
 */
void dw_gdma_hal_init(dw_gdma_hal_context_t *hal, const dw_gdma_hal_config_t *config);

#ifdef __cplusplus
}
#endif
