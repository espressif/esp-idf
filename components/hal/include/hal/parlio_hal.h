/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct parl_io_dev_t *parlio_soc_handle_t; // Parallel IO SOC layer handle

/**
 * @brief HAL context type of Parallel IO driver
 */
typedef struct {
    parlio_soc_handle_t regs; /*!< Parallel IO Register base address */
} parlio_hal_context_t;

/**
 * @brief Initialize the Parallel IO HAL driver
 *
 * @param hal: Parallel IO HAL context
 */
void parlio_hal_init(parlio_hal_context_t *hal);

/**
 * @brief Deinitialize the Parallel IO HAL driver
 *
 * @param hal: Parallel IO HAL context
 */
void parlio_hal_deinit(parlio_hal_context_t *hal);

#ifdef __cplusplus
}
#endif
