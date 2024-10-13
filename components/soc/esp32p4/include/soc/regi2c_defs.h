/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "esp_bit_defs.h"

/* Analog function control register */
#define ANA_CONFIG_REG  0x5012401C
#define ANA_CONFIG_M    (0x3FF)

#define ANA_I2C_SAR_FORCE_PD BIT(18)


#define ANA_CONFIG2_REG  0x50124020
#define ANA_CONFIG2_M    BIT(18)

#define ANA_I2C_SAR_FORCE_PU BIT(16)
