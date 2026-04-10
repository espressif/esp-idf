/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
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

typedef struct sdmmc_dev_t *sdmmc_soc_handle_t;

/**
 * @brief Context of the HAL
 */
typedef struct {
    sdmmc_soc_handle_t dev;   // SDMMC SOC layer handle (i.e. register base address)
} sdmmc_hal_context_t;

/**
 * @brief Init the sdmmc hal context.
 *
 * @param hal Context of the HAL
 */
void sdmmc_hal_init(sdmmc_hal_context_t *hal);

#ifdef __cplusplus
}
#endif
