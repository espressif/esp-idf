/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The HAL is not public api, don't use in application code.
 * See readme.md in soc/README.md
 ******************************************************************************/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ppa_dev_t *ppa_soc_handle_t; // PPA SOC layer handle

/**
 * Context that should be maintained by both the driver and the HAL
 */
typedef struct {
    ppa_soc_handle_t dev; // PPA SOC layer handle (i.e. register base address)
} ppa_hal_context_t;

/**
 * @brief Init the PPA hal. This function should be called first before other hal layer function is called
 *
 * @param hal Context of the HAL layer
 */
void ppa_hal_init(ppa_hal_context_t *hal);

/**
 * @brief De-init the PPA hal
 *
 * @param hal Context of the HAL layer
 */
void ppa_hal_deinit(ppa_hal_context_t *hal);

#ifdef __cplusplus
}
#endif
