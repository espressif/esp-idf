/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/readme.md
 ******************************************************************************/

// The HAL layer for I3C master

#pragma once

#include "soc/soc_caps.h"
#if SOC_I3C_MASTER_SUPPORTED
#include "hal/i3c_master_types.h"
#include "hal/i3c_master_ll.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_I3C_MASTER_SUPPORTED

typedef struct i3c_mst_dev_t *i3c_master_soc_handle_t; // I3C master SOC layer handle

/**
 * @brief I3C master hal Context definition
 */
typedef struct {
    i3c_master_soc_handle_t dev;
} i3c_master_hal_context_t;

/**
 * @brief Initialize the I3C master HAL context
 *
 * @param hal HAL context
 * @param i3c_port I3C port number
 */
void i3c_master_hal_init(i3c_master_hal_context_t *hal, int i3c_port);

/**
 * @brief Deinitialize the I3C master HAL context
 *
 * @param hal HAL context
 */
void i3c_master_hal_deinit(i3c_master_hal_context_t *hal);

#endif

#ifdef __cplusplus
}
#endif
