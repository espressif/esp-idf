/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The HAL is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct jpeg_dev_t *jpeg_soc_handle_t; // JPEG SOC layer handle

/**
 * Context that should be maintained by both the driver and the HAL
 */
typedef struct {
    jpeg_soc_handle_t dev; // JPEG SOC layer handle (i.e. register base address)
} jpeg_hal_context_t;

/**
 * @brief Initialize the JPEG codec HAL driver
 *
 * @param hal: JPEG codec HAL context
 */
void jpeg_hal_init(jpeg_hal_context_t *hal);

/**
 * @brief Deinitialize the JPEG codec HAL driver
 *
 * @param hal: JPEG codec HAL context
 */
void jpeg_hal_deinit(jpeg_hal_context_t *hal);

#ifdef __cplusplus
}
#endif
