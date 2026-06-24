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

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pcnt_dev_t *pcnt_soc_handle_t; // PCNT SOC layer handle

/**
 * Context that should be maintained by both the driver and the HAL
 */
typedef struct {
    pcnt_soc_handle_t dev; // PCNT SOC layer handle
} pcnt_hal_context_t;

/**
 * @brief Init the PCNT hal and set the PCNT to the default configuration.
 * @note This function should be called first before other hal layer function is called.
 *
 * @param hal Context of the HAL layer
 * @param group_id PCNT group ID
 */
void pcnt_hal_init(pcnt_hal_context_t *hal, int group_id);

#ifdef __cplusplus
}
#endif
