/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
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

typedef struct soc_etm_dev_t *etm_soc_handle_t; // ETM SOC layer handle

/**
 * @brief HAL context type of ETM driver
 */
typedef struct {
    etm_soc_handle_t regs; /*!< ETM Register base address */
} etm_hal_context_t;

/**
 * @brief Initialize the ETM HAL driver
 *
 * @param hal: ETM HAL context
 */
void etm_hal_init(etm_hal_context_t *hal);

/**
 * @brief Deinitialize the ETM HAL driver
 *
 * @param hal: ETM HAL context
 */
void etm_hal_deinit(etm_hal_context_t *hal);

#ifdef __cplusplus
}
#endif
