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

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mcpwm_dev_t *mcpwm_soc_handle_t; // MCPWM SOC layer handle

/**
 * @brief HAL layer configuration
 */
typedef struct {
    int group_id; // Indicate the MCPWM hardware group
} mcpwm_hal_init_config_t;

/**
 * Context that should be maintained by both the driver and the HAL
 */
typedef struct {
    mcpwm_soc_handle_t dev; // MCPWM SOC layer handle
} mcpwm_hal_context_t;

/**
 * @brief Initialize the internal state of the HAL.
 *
 * @param hal Context of the HAL layer.
 * @param init_config Configuration for the HAL to be used only once.
 */
void mcpwm_hal_init(mcpwm_hal_context_t *hal, const mcpwm_hal_init_config_t *init_config);

/**
 * @brief Deinitialize the HAL driver.
 *
 * @param hal Context of the HAL layer.
 */
void mcpwm_hal_deinit(mcpwm_hal_context_t *hal);

/**
 * @brief Reset MCPWM timer
 *
 * @param hal Context of the HAL layer.
 * @param timer_id Timer ID
 */
void mcpwm_hal_timer_reset(mcpwm_hal_context_t *hal, int timer_id);

/**
 * @brief Reset MCPWM operator
 *
 * @param hal Context of the HAL layer.
 * @param oper_id Operator ID
 */
void mcpwm_hal_operator_reset(mcpwm_hal_context_t *hal, int oper_id);

/**
 * @brief Reset MCPWM generator
 *
 * @param hal Context of the HAL layer.
 * @param oper_id Operator ID
 * @param gen_id Generator ID
 */
void mcpwm_hal_generator_reset(mcpwm_hal_context_t *hal, int oper_id, int gen_id);

#ifdef __cplusplus
}
#endif
