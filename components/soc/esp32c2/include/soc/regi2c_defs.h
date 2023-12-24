/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#pragma once

#include "esp_bit_defs.h"

/* Analog function control register */
#define I2C_MST_ANA_CONF0_REG  0x6004E840
#define I2C_MST_BBPLL_STOP_FORCE_HIGH  (BIT(2))
#define I2C_MST_BBPLL_STOP_FORCE_LOW  (BIT(3))
#define I2C_MST_BBPLL_CAL_DONE         (BIT(24))

#define ANA_CONFIG_REG  0x6004E844
#define ANA_CONFIG_S    (8)
#define ANA_CONFIG_M    (0x3FF)

#define ANA_I2C_SAR_FORCE_PD BIT(18)
#define ANA_I2C_BBPLL_M      BIT(17) /* Clear to enable BBPLL */


#define ANA_CONFIG2_REG  0x6004E848
#define ANA_CONFIG2_M    BIT(18)

#define ANA_I2C_SAR_FORCE_PU BIT(16)

/**
 * Restore regi2c analog calibration related configuration registers.
 * This is a workaround for calibration error when waking up from light sleep
 */
#define REGI2C_ANA_CALI_PD_WORKAROUND    1
#define REGI2C_ANA_CALI_BYTE_NUM         8
