/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include "hal/cordic_hal.h"
#include "hal/cordic_ll.h"

// algorithm allowable scale, format [min, max]
uint16_t cordic_hal_algorithm_allowable_scale[ESP_CORDIC_FUNC_MAX][2] = {
    {0, 0},
    {0, 0},
    {0, 15},
    {0, 15},
    {0, 15},
    {1, 1},
    {1, 1},
    {1, 1},
    {1, 1},
    {0, 4},
};
