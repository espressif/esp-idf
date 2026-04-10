/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include "hal/cordic_types.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_CORDIC_SUPPORTED

/**
 * @brief Algorithm allowable scale, format [min, max]
 *
 * @param 1: function type
 * @param 2: [min, max] scale
 */
extern uint16_t cordic_hal_algorithm_allowable_scale[ESP_CORDIC_FUNC_MAX][2];

#endif // SOC_CORDIC_SUPPORTED

#ifdef __cplusplus
}
#endif
