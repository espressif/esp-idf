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

// The HAL layer for UHCI.
// There is no parameter check in the hal layer, so the caller must ensure the correctness of the parameters.

#pragma once

#include "hal/uhci_types.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_UHCI_SUPPORTED
typedef struct uhci_dev_t *uhci_soc_handle_t; // UHCI SOC layer handle

/**
 * Context that should be maintained by both the driver and the HAL
 */
typedef struct {
    uhci_soc_handle_t dev;
} uhci_hal_context_t;

/**
 * @brief  Init UHCI hardware instance.
 *
 * @param  hal Context of the HAL layer
 * @param  uhci_num UHCI number
 *
 * @return None
 */
void uhci_hal_init(uhci_hal_context_t *hal, int uhci_num);

/**
 * @brief  Deinit UHCI hardware instance.
 *
 * @param  hal Context of the HAL layer
 * @param  uhci_num UHCI number
 *
 * @return None
 */
void uhci_hal_deinit(uhci_hal_context_t *hal);

#endif

#ifdef __cplusplus
}
#endif
