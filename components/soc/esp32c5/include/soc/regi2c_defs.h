/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_bit_defs.h"

/* Analog function control register */
// I2C_ANA_MST_ANA_CONF0_REG
#define I2C_MST_BBPLL_STOP_FORCE_HIGH  (BIT(2))
#define I2C_MST_BBPLL_STOP_FORCE_LOW   (BIT(3))
#define I2C_MST_BBPLL_CAL_DONE         (BIT(24))
