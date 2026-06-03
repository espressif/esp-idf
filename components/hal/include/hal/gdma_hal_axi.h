/*
 * SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "hal/gdma_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

void gdma_axi_hal_init(gdma_hal_context_t *hal, const gdma_hal_config_t *config);

#ifdef __cplusplus
}
#endif
