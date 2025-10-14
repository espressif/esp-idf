/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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
 * @brief Configuration for the Sigma-Delta HAL layer initialization
 */
typedef struct {
    int group_id; // Group ID, index from 0
} sdm_hal_init_config_t;

/**
 * @brief Initialize Sigma-Delta hal driver
 *
 * @param hal Context of the HAL layer
 * @param config Configuration for the HAL layer initialization
 */
void sdm_hal_init(sdm_hal_context_t *hal, const sdm_hal_init_config_t *config);

/**
 * @brief Deinitialize Sigma-Delta hal driver
 *
 * @param hal Context of the HAL layer
 */
void sdm_hal_deinit(sdm_hal_context_t *hal);

#ifdef __cplusplus
}
#endif
