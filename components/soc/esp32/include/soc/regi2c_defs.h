/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_bit_defs.h"

/* Analog function control register */
#define ANA_CONFIG_REG  0x6000E044
#define ANA_CONFIG_S    (8)
#define ANA_CONFIG_M    (0x3FF)
/* Clear to enable APLL */
#define I2C_APLL_M      (BIT(14))
/* Clear to enable BBPLL */
#define I2C_BBPLL_M     (BIT(17))
