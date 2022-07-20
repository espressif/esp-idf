/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for sigma delta modulator.
// There is no parameter check in the hal layer, so the caller must ensure the correctness of the parameters.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gpio_sd_dev_t *sdm_soc_handle_t; // Sigma-Delta SOC layer handle

/**
 * HAL context type of Sigma-Delta driver
 */
typedef struct {
    sdm_soc_handle_t dev;
} sdm_hal_context_t;

/**
 * @brief Initialize Sigma-Delta hal driver
 *
 * @param hal Context of the HAL layer
 * @param group_id Sigma-Delta group number
 */
void sdm_hal_init(sdm_hal_context_t *hal, int group_id);

#ifdef __cplusplus
}
#endif
