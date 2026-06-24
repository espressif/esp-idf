/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_LP_I2S_SUPPORTED
typedef struct lp_i2s_dev_t *lp_i2s_soc_handle_t; // LP I2S SOC layer handle
#else
typedef uint32_t *lp_i2s_soc_handle_t; // LP I2S SOC layer handle
#endif

/**
 * Context that should be maintained by both the driver and the HAL
 */
typedef struct {
    lp_i2s_soc_handle_t dev; // LP I2S SOC layer handle
} lp_i2s_hal_context_t;

/**
 * @brief Init the LP I2S hal and set the LP I2S to the default configuration.
 * @note This function should be called first before other hal layer function is called.
 *
 * @param hal Context of the HAL layer
 * @param group_id LP I2S group ID
 */
void lp_i2s_hal_init(lp_i2s_hal_context_t *hal, int group_id);

#ifdef __cplusplus
}
#endif
